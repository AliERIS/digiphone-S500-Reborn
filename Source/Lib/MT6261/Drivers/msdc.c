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
#include <stdio.h>

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
    Context->MSDC->SDC_CFG  = (Context->MSDC->SDC_CFG & ~SDC_DTOC(-1)) | SDC_DTOC(80) | SDC_SIEN;
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

    LCD_DrawString(20, 100, "MSDC: PMU SET", 0xFFFF, 0x0000);

    /* Enable Power (VMC) */
    PMU_SetVoltageVMC(VMC_VO33V);
    PMU_TurnOnVMC(true);
    USC_Pause_us(10000); // Wait for power stable

    LCD_DrawString(20, 110, "MSDC: GPIO SET", 0xFFFF, 0x0000);

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

    LCD_DrawString(20, 120, "MSDC: PULLUPS", 0xFFFF, 0x0000);

    /* Configure MSDC internal pull-up resistors (per datasheet)
     * CMD: 47k pull-up
     * CLK: 47k pull-up
     * DAT: 47k pull-up
     */
    ctx->MSDC->MSDC_IOCON1 =
        MSDC_PRVAL_CM(MSDC_PRVAL_CM_47K) |   // CMD: 47k
        MSDC_PRCFG_CM_PU |                    // CMD: pull-up
        MSDC_PRVAL_CK(MSDC_PRVAL_CK_47K) |   // CLK: 47k
        MSDC_PRCFG_CK_PU |                    // CLK: pull-up
        MSDC_PRVAL_DA(MSDC_PRVAL_DA_47K) |   // DAT: 47k
        MSDC_PRCFG_DA_PU;                     // DAT: pull-up

    LCD_DrawString(20, 130, "MSDC: RESET", 0xFFFF, 0x0000);

    /* Reset Controller */
    MSDC_Reset(ctx);

    /* Wait after reset for controller to stabilize */
    USC_Pause_us(20000); // 20ms after reset

    LCD_DrawString(20, 140, "MSDC: ENABLE", 0xFFFF, 0x0000);

    /* Enable Controller FIRST (required for clock init to work!) */
    ctx->MSDC->MSDC_CFG |= MSDC_EN;
    USC_Pause_us(10000); // Wait for controller enable

    LCD_DrawString(20, 150, "MSDC: CLOCK", 0xFFFF, 0x0000);

    /* Initialize Clock (controller must be enabled first!) */
    MSDC_ClockInit(ctx);

    /* Set default 400kHz clock for SD card initialization */
    MSDC_SetClock(Index, 400000);

    /* Wait for stable */
    USC_Pause_us(50000); // 50ms for clock to stabilize

    /* Clear any pending status flags */
    ctx->MSDC->SDC_CMDSTA = 0;
    ctx->MSDC->SDC_DATSTA = 0;

    /* Force clear SDC_STA - reset doesn't always clear it! */
    /* Write 0 to clear any stuck flags */
    volatile uint32_t dummy = ctx->MSDC->SDC_STA; // Read to clear
    (void)dummy; // Prevent unused warning

    /* Clear FIFO */
    ctx->MSDC->MSDC_STA |= MSDC_FIFOCLR;
    USC_Pause_us(1000); // Wait for FIFO clear

    /* Wait for CMD and DAT lines to be ready */
    {
        uint32_t timeout = 100000;
        while ((ctx->MSDC->SDC_STA & (SDC_BECMDBUSY | SDC_BEDATBUSY)) && --timeout) {
            /* Try to clear stuck flags */
            ctx->MSDC->SDC_CMDSTA = 0;
            ctx->MSDC->SDC_DATSTA = 0;
            USC_Pause_us(100);
        }
        if (timeout == 0) {
            LCD_DrawString(20, 160, "SDC: LINE BUSY", 0xF800, 0x0000);
        } else {
            LCD_DrawString(20, 160, "SDC: LINES OK", 0x07E0, 0x0000);
        }
    }

    /* Wait for controller-level BUSY to clear */
    {
        uint32_t timeout = 100000;
        while ((ctx->MSDC->MSDC_STA & MSDC_BUSY) && --timeout) {
            USC_Pause_us(100);
        }
        if (timeout == 0) {
            LCD_DrawString(20, 170, "MSDC: INIT BUSY", 0xF800, 0x0000);
        } else {
            LCD_DrawString(20, 170, "MSDC: READY", 0x07E0, 0x0000);
        }
    }

    LCD_DrawString(20, 180, "MSDC: DONE", 0x07E0, 0x0000);
}

void MSDC_SetClock(TMSDC Index, uint32_t Frequency)
{
    TMSDCCONTEXT *ctx = MSDC_GetModuleContext(Index);
    if (!ctx) return;

    /* Set clock source to MPLL/7MHz explicitly (datasheet default) */
    ctx->MSDC->MSDC_CFG &= ~MSDC_CLKSRC(3);
    ctx->MSDC->MSDC_CFG |= MSDC_CLKSRC(MSDC_CLKSRC_MPLL_7MHZ);

    /* Calculate divider using datasheet formula: fslave = [1/(4*SCLKF)] * fhost
     * fhost = 7MHz (MPLL/7)
     * For 400kHz: SCLKF = 7000000 / (4 * 400000) = 4.375 ≈ 4
     * For 25MHz: SCLKF = 7000000 / (4 * 25000000) = 0.07 ≈ 1 (minimum)
     */
    uint32_t fhost = 7000000; // MPLL/7MHz clock source
    uint32_t divider = fhost / (4 * Frequency);

    /* Clamp divider to valid range [1, 255] */
    if (divider < 1) divider = 1;
    if (divider > 255) divider = 255;

    ctx->MSDC->MSDC_CFG &= ~MSDC_SCLKF(0xFF);
    ctx->MSDC->MSDC_CFG |= MSDC_SCLKF(divider);

    /* Wait for clock to stabilize */
    USC_Pause_us(10000); // 10ms delay after clock change

    /* Clock change can set BUSY - wait for it to clear! */
    uint32_t timeout = 100000;
    while ((ctx->MSDC->MSDC_STA & MSDC_BUSY) && --timeout) {
        USC_Pause_us(10);
    }
}

boolean MSDC_SendCommand(TMSDC Index, uint32_t Cmd, uint32_t Arg, uint32_t *Resp)
{
    TMSDCCONTEXT *ctx = MSDC_GetModuleContext(Index);
    if (!ctx) return false;

    /* Clear any pending status first */
    ctx->MSDC->SDC_CMDSTA = 0;
    ctx->MSDC->SDC_DATSTA = 0;

    /* Skip BUSY checks - they appear unreliable on this hardware
     * Let command timeout handle real errors instead */

    /* Clear status before command */
    ctx->MSDC->SDC_CMDSTA = 0;

    /* Write Argument */
    ctx->MSDC->SDC_ARG = Arg;

    /* Check if this is a NO_RSP command (like CMD0) */
    boolean isNoResponse = ((Cmd & SDC_RSPTYP(0x07)) == SDC_RSPTYP(SDC_NO_RSP));

    /* Write Command */
    ctx->MSDC->SDC_CMD = Cmd;

    /* For NO_RSP commands, just wait for command to be sent */
    if (isNoResponse) {
        /* NO_RSP commands: wait briefly for command transmission */
        USC_Pause_us(2000); // 2ms for command to be sent

        /* Check if there were any command errors */
        if (ctx->MSDC->SDC_CMDSTA & SDC_CMDTO) {
            return false;
        }

        return true;
    }

    /* Wait for response completion (for commands with response) */
    int timeout = 5000000; // 5 second timeout (was 500ms - trying longer for slow cards)
    while (!(ctx->MSDC->SDC_CMDSTA & (SDC_CMDRDY | SDC_CMDTO | SDC_RSPCRCERR)) && --timeout) {
        /* Restart watchdog periodically to prevent device reset */
        if ((timeout % 10000) == 0) {
            RGU_RestartWDT();
        }
        USC_Pause_us(1);
    }

    if (timeout == 0) {
        /* Debug: Show what SDC_CMDSTA actually contains */
        uint32_t cmdsta = ctx->MSDC->SDC_CMDSTA;
        char buf[32];
        sprintf(buf, "CMDSTA:%08X", (unsigned int)cmdsta);
        LCD_DrawString(20, 220, buf, 0xFFE0, 0x0000);
        
        LCD_DrawString(20, 200, "CMD: WAIT TO", 0xF800, 0x0000);
        return false;
    }

    /* Check for timeout */
    if (ctx->MSDC->SDC_CMDSTA & SDC_CMDTO) {
        uint32_t cmdsta = ctx->MSDC->SDC_CMDSTA;
        char buf[32];
        sprintf(buf, "CMDSTA:%08X", (unsigned int)cmdsta);
        LCD_DrawString(20, 220, buf, 0xFFE0, 0x0000);
        
        LCD_DrawString(20, 200, "CMD: TIMEOUT", 0xF800, 0x0000);
        return false;
    }

    /* Check for CRC error */
    if (ctx->MSDC->SDC_CMDSTA & SDC_RSPCRCERR) {
        uint32_t cmdsta = ctx->MSDC->SDC_CMDSTA;
        char buf[32];
        sprintf(buf, "CMDSTA:%08X", (unsigned int)cmdsta);
        LCD_DrawString(20, 220, buf, 0xFFE0, 0x0000);
        
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

    /* For R1/R1b responses, check card status register (SDC_CSTA) */
    uint32_t rsptyp = (Cmd >> 7) & 0x07;
    if (rsptyp == SDC_RSP_R1 || rsptyp == SDC_RSP_R1B) {
        uint32_t csta = ctx->MSDC->SDC_CSTA;

        /* Check for card errors per datasheet */
        if (csta & (SDC_ILLEGAL_COMMAND | SDC_COM_CRC_ERROR |
                    SDC_ERROR | SDC_CC_ERROR | SDC_CARD_ECC_FAILED)) {
            /* Don't fail on these, but log for debugging */
            if (csta & SDC_ILLEGAL_COMMAND) {
                LCD_DrawString(20, 220, "R1: ILL CMD", 0xFFE0, 0x0000);
            }
            return false;
        }
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

