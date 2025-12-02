// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
* This file is part of the DZ09 project.
*
* Copyright (C) 2022 AJScorp
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#include "systemconfig.h"
#include "msdc.h"

static TMSDCCONTEXT MSDCCONTEXT[MSDC_ITFNUM] =
{
    {MSDC0, PD_MSDC},
    {MSDC1, PD_MSDC2}
};

static TMSDCCONTEXT *MSDC_GetModuleContext(TMSDC Index)
{
    if (Index >= MSDC_ITFNUM) return NULL;
    else return &MSDCCONTEXT[Index];
}

static void MSDC_Reset(TMSDCCONTEXT *Context)
{
    Context->MSDC->MSDC_CFG |= MSDC_RST;
    USC_Pause_us(2000);                                                                             // Pause 2 ms
    Context->MSDC->MSDC_CFG &= ~(MSDC_RST | MSDC_EN);
}

static void MSDC_ClockInit(TMSDCCONTEXT *Context)
{
    Context->MSDC->CLK_RED &= ~MSDC_CLKPAD_RED;
    Context->MSDC->SDC_CFG  = (Context->MSDC->SDC_CFG & ~SDC_DTOC(-1)) | SDC_DTOC(80);
}

boolean MSDC_IsMultiLineSupported(TMSDC Index)
{
    return (Index == MSDC_ITF0) ? true : false;
}

#include "pmu.h"
#include "gpio.h"
#include "lcdif.h"

void MSDC_Init(TMSDC Index)
{
    TMSDCCONTEXT *ctx = MSDC_GetModuleContext(Index);
    if (!ctx) return;
    
    LCD_DrawString(20, 160, "MSDC: PMU SET", 0xFFFF, 0x0000);
    
    /* Enable Power (VMC) */
    PMU_SetVoltageVMC(VMC_VO33V);
    PMU_TurnOnVMC(true);
    USC_Pause_us(10000); // Wait for power stable
    
    LCD_DrawString(20, 160, "MSDC: GPIO SET", 0xFFFF, 0x0000);
    
    /* Configure GPIOs for MSDC0 (SD Card) */
    if (Index == MSDC_ITF0) {
        LCD_DrawString(20, 180, "SET GPIO31", 0xFFFF, 0x0000);
        /* CLK: GPIO31 Mode 1 (MCCK) */
        GPIO_Setup(GPIO31, GPMODE(1) | GPDO | GPDIEN | GPPULLEN | GPPUP | GPSMT);
        
        LCD_DrawString(20, 180, "SET GPIO32", 0xFFFF, 0x0000);
        /* CMD: GPIO32 Mode 1 (MCCM0) */
        GPIO_Setup(GPIO32, GPMODE(1) | GPDO | GPDIEN | GPPULLEN | GPPUP | GPSMT);
        
        LCD_DrawString(20, 180, "SET GPIO33", 0xFFFF, 0x0000);
        /* DAT0: GPIO33 Mode 1 (MCDA0) */
        GPIO_Setup(GPIO33, GPMODE(1) | GPDO | GPDIEN | GPPULLEN | GPPUP | GPSMT);
    }
    /* Configure GPIOs for MSDC1 (SD Card - Alternative) */
    else if (Index == MSDC_ITF1) {
        LCD_DrawString(20, 180, "SET GPIO28", 0xFFFF, 0x0000);
        /* CLK: GPIO28 Mode 8 (MC2CK) */
        GPIO_Setup(GPIO28, GPMODE(8) | GPDO | GPDIEN | GPPULLEN | GPPUP | GPSMT);
        
        LCD_DrawString(20, 180, "SET GPIO27", 0xFFFF, 0x0000);
        /* CMD: GPIO27 Mode 8 (MC2CM0) */
        GPIO_Setup(GPIO27, GPMODE(8) | GPDO | GPDIEN | GPPULLEN | GPPUP | GPSMT);
        
        LCD_DrawString(20, 180, "SET GPIO29", 0xFFFF, 0x0000);
        /* DAT0: GPIO29 Mode 8 (MC2DA0) */
        GPIO_Setup(GPIO29, GPMODE(8) | GPDO | GPDIEN | GPPULLEN | GPPUP | GPSMT);
        USC_Pause_us(10000); // Wait for GPIO to stabilize
    }
    
    LCD_DrawString(20, 160, "MSDC: RESET   ", 0xFFFF, 0x0000);
    
    /* Reset Controller */
    MSDC_Reset(ctx);
    
    /* Wait after reset for controller to stabilize */
    USC_Pause_us(20000); // 20ms after reset
    
    LCD_DrawString(20, 160, "MSDC: CLOCK   ", 0xFFFF, 0x0000);
    
    /* Initialize Clock */
    MSDC_ClockInit(ctx);
    
    LCD_DrawString(20, 160, "MSDC: ENABLE  ", 0xFFFF, 0x0000);
    
    /* Enable Controller */
    ctx->MSDC->MSDC_CFG |= MSDC_EN;
    
    /* Wait for stable */
    USC_Pause_us(50000); // Increased to 50ms
    
    /* Clear any pending status flags */
    ctx->MSDC->SDC_CMDSTA = 0;
    ctx->MSDC->SDC_DATSTA = 0;
    
    /* Wait for controller to be ready (not busy) */
    {
        uint32_t timeout = 100000;
        while ((ctx->MSDC->SDC_STA & (SDC_BECMDBUSY | SDC_BEDATBUSY)) && --timeout) {
            USC_Pause_us(100);
        }
        if (timeout == 0) {
            LCD_DrawString(20, 180, "MSDC: BUSY ERR", 0xF800, 0x0000);
        }
    }
    
    LCD_DrawString(20, 160, "MSDC: DONE    ", 0xFFFF, 0x0000);
}

void MSDC_SetClock(TMSDC Index, uint32_t Frequency)
{
    TMSDCCONTEXT *ctx = MSDC_GetModuleContext(Index);
    if (!ctx) return;
    
    /* Simple clock divider implementation */
    /* Assuming 48MHz source, divider = Source / (4 * Freq) */
    uint32_t divider = 48000000 / (4 * Frequency);
    if (divider > 255) divider = 255;
    if (divider < 1) divider = 1;
    
    ctx->MSDC->MSDC_CFG &= ~MSDC_SCLKF(0xFF);
    ctx->MSDC->MSDC_CFG |= MSDC_SCLKF(divider);
    
    /* Wait for clock to stabilize */
    USC_Pause_us(10000); // 10ms delay after clock change
}

boolean MSDC_SendCommand(TMSDC Index, uint32_t Cmd, uint32_t Arg, uint32_t *Resp)
{
    TMSDCCONTEXT *ctx = MSDC_GetModuleContext(Index);
    if (!ctx) return false;
    
    /* Clear any pending status first */
    ctx->MSDC->SDC_CMDSTA = 0;
    ctx->MSDC->SDC_DATSTA = 0;
    
    /* Wait for command ready - increased timeout and better clearing */
    uint32_t timeout = 200000; // Increased timeout
    uint32_t clearCount = 0;
    
    while ((ctx->MSDC->SDC_STA & SDC_BECMDBUSY) && --timeout) {
        /* Periodically try to clear busy state */
        if ((timeout % 10000) == 0) {
            ctx->MSDC->SDC_CMDSTA = 0;
            ctx->MSDC->SDC_DATSTA = 0;
            clearCount++;
            
            /* If we've tried clearing multiple times, try a soft reset */
            if (clearCount >= 5) {
                /* Disable and re-enable controller to clear stuck state */
                ctx->MSDC->MSDC_CFG &= ~MSDC_EN;
                USC_Pause_us(1000);
                ctx->MSDC->MSDC_CFG |= MSDC_EN;
                USC_Pause_us(5000);
                clearCount = 0;
            }
        }
        USC_Pause_us(10); // Small delay in busy loop
    }
    
    if (timeout == 0) {
        /* Last resort: try to clear busy state more aggressively */
        ctx->MSDC->SDC_CMDSTA = 0;
        ctx->MSDC->SDC_DATSTA = 0;
        
        /* Disable and re-enable controller */
        ctx->MSDC->MSDC_CFG &= ~MSDC_EN;
        USC_Pause_us(2000);
        ctx->MSDC->MSDC_CFG |= MSDC_EN;
        USC_Pause_us(10000);
        
        /* Check one more time */
        timeout = 100000;
        while ((ctx->MSDC->SDC_STA & SDC_BECMDBUSY) && --timeout) {
            USC_Pause_us(10);
        }
        
        if (timeout == 0) {
            LCD_DrawString(20, 200, "CMD: BUSY TO", 0xF800, 0x0000);
            return false;
        }
    }
    
    /* Clear status */
    ctx->MSDC->SDC_CMDSTA = 0;
    
    /* Write Argument */
    ctx->MSDC->SDC_ARG = Arg;
    
    /* Check if this is a NO_RSP command (like CMD0) */
    boolean isNoResponse = ((Cmd & SDC_RSPTYP(0x07)) == SDC_RSPTYP(SDC_NO_RSP));
    
    /* Write Command */
    ctx->MSDC->SDC_CMD = Cmd | SDC_INTC; // Enable interrupt clear
    
    /* For NO_RSP commands, just wait for command to be processed */
    if (isNoResponse) {
        /* NO_RSP commands don't wait for response, just give time for command to be sent */
        USC_Pause_us(5000); // 5ms delay for NO_RSP commands to be processed
        
        /* For NO_RSP, we don't check BUSY or wait for CMDRDY */
        /* The command is considered successful if we got here */
        return true;
    }
    
    /* Wait for completion - increased timeout for response commands */
    timeout = 1000000; // Increased to 1M for better reliability
    while (!(ctx->MSDC->SDC_CMDSTA & (SDC_CMDRDY | SDC_CMDTO | SDC_RSPCRCERR)) && --timeout) {
        USC_Pause_us(1); // Small delay in wait loop
    }
    
    if (timeout == 0) {
        LCD_DrawString(20, 200, "CMD: WAIT TO", 0xF800, 0x0000);
        return false;
    }
    
    if (ctx->MSDC->SDC_CMDSTA & SDC_CMDTO) {
        LCD_DrawString(20, 200, "CMD: TIMEOUT", 0xF800, 0x0000);
        return false;
    }
    
    if (ctx->MSDC->SDC_CMDSTA & SDC_RSPCRCERR) {
        LCD_DrawString(20, 200, "CMD: CRC ERR", 0xF800, 0x0000);
        return false;
    }
    
    /* Read Response */
    if (Resp) {
        Resp[0] = ctx->MSDC->SDC_RESP0;
        Resp[1] = ctx->MSDC->SDC_RESP1;
        Resp[2] = ctx->MSDC->SDC_RESP2;
        Resp[3] = ctx->MSDC->SDC_RESP3;
    }
    
    return true;
}

boolean MSDC_ReadData(TMSDC Index, void *Buffer, uint32_t Length)
{
    TMSDCCONTEXT *ctx = MSDC_GetModuleContext(Index);
    if (!ctx) return false;
    uint32_t *pBuf = (uint32_t*)Buffer;
    
    /* Wait for data ready */
    uint32_t timeout = 100000;
    while (!(ctx->MSDC->MSDC_STA & MSDC_DRQ) && --timeout);
    if (timeout == 0) return false;
    
    /* Read FIFO */
    for (uint32_t i = 0; i < Length / 4; i++) {
        *pBuf++ = ctx->MSDC->MSDC_DAT;
    }
    
    /* Check for errors */
    if (ctx->MSDC->SDC_DATSTA & (SDC_DATTO | SDC_DATCRCERR)) {
        return false;
    }
    
    return true;
}

