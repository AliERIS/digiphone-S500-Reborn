/*
 * SD Card Protocol Driver
 */
#include "systemconfig.h"
#include "sdcard.h"
#include "msdc.h"
#include "debug.h"
#include <stdio.h>

/* SD Commands */
#define CMD0    0
#define CMD2    2
#define CMD3    3
#define CMD7    7
#define CMD8    8
#define CMD16   16
#define CMD17   17
#define CMD55   55
#define ACMD6   6
#define ACMD41  41

/* Response Types */
#define RSP_R1  1
#define RSP_R2  2
#define RSP_R3  3
#define RSP_R6  6
#define RSP_R7  1

static uint32_t g_rca = 0;

static boolean SendCmd(TMSDC Index, uint32_t Cmd, uint32_t Arg, uint32_t *Resp)
{
    return MSDC_SendCommand(Index, Cmd, Arg, Resp);
}

#include "lcdif.h"

#include "rgu.h"

static boolean SD_PerformInitSequence(TMSDC Index, boolean use4Bit, uint32_t finalSpeed, boolean verbose)
{
    uint32_t resp[4];
    
    RGU_RestartWDT();
    
    if (verbose) {
        char dbgBuf[40];
        sprintf(dbgBuf, "TRY: %s %dMHz", use4Bit ? "4BIT" : "1BIT", finalSpeed/1000000);
        LCD_DrawString(20, 100, dbgBuf, 0xFFE0, 0x0000);
    }

    /* 1. Initialize Controller (sets 400kHz default) */
    MSDC_Init(Index);
    USC_Pause_us(50000);
    
    if (verbose) LCD_DrawString(20, 120, "CMD0...", 0xFFFF, 0x0000);
    
    /* 2. CMD0: Go Idle State */
    {
        int retry;
        for (retry = 0; retry < 20; retry++) {
            USC_Pause_us(2000);
            if (SendCmd(Index, SDC_CMD(CMD0) | SDC_RSPTYP(SDC_NO_RSP), 0, NULL)) break;
        }
        if (retry == 20) {
            if (verbose) LCD_DrawString(20, 130, "CMD0 FAIL", 0xF800, 0x0000);
            return false;
        }
        if (verbose) LCD_DrawString(20, 130, "CMD0 OK", 0x07E0, 0x0000);
    }
    
    USC_Pause_us(10000);
    
    if (verbose) LCD_DrawString(20, 140, "CMD8...", 0xFFFF, 0x0000);
    
    /* 3. CMD8: Send Interface Condition */
    if (!SendCmd(Index, SDC_CMD(CMD8) | SDC_RSPTYP(SDC_RSP_R1), 0x1AA, resp)) {
        if (verbose) LCD_DrawString(20, 150, "CMD8 FAIL", 0xF800, 0x0000);
        return false;
    }
    if ((resp[0] & 0xFF) != 0xAA) {
        if (verbose) LCD_DrawString(20, 150, "CMD8 BAD RSP", 0xF800, 0x0000);
        return false;
    }
    if (verbose) LCD_DrawString(20, 150, "CMD8 OK", 0x07E0, 0x0000);
    
    USC_Pause_us(5000);
    
    if (verbose) LCD_DrawString(20, 160, "ACMD41...", 0xFFFF, 0x0000);
    
    /* 4. ACMD41: Send Op Cond */
    uint32_t timeout = 2000; // 2s
    do {
        RGU_RestartWDT();
        SendCmd(Index, SDC_CMD(CMD55) | SDC_RSPTYP(SDC_RSP_R1), 0, NULL);
        USC_Pause_us(2000);
        SendCmd(Index, SDC_CMD(ACMD41) | SDC_RSPTYP(SDC_RSP_R3), 0x40FF8000, resp);
        USC_Pause_us(5000);
    } while (!(resp[0] & 0x80000000) && --timeout);
    
    if (timeout == 0) {
        if (verbose) LCD_DrawString(20, 170, "ACMD41 TIMEOUT", 0xF800, 0x0000);
        return false;
    }
    if (verbose) LCD_DrawString(20, 170, "ACMD41 OK", 0x07E0, 0x0000);
    
    /* 5. CMD2: All Send CID */
    if (!SendCmd(Index, SDC_CMD(CMD2) | SDC_RSPTYP(SDC_RSP_R2), 0, resp)) {
        if (verbose) LCD_DrawString(20, 180, "CMD2 FAIL", 0xF800, 0x0000);
        return false;
    }
    
    /* 6. CMD3: Send Relative Address */
    if (!SendCmd(Index, SDC_CMD(CMD3) | SDC_RSPTYP(SDC_RSP_R6), 0, resp)) {
        if (verbose) LCD_DrawString(20, 180, "CMD3 FAIL", 0xF800, 0x0000);
        return false;
    }
    g_rca = resp[0] >> 16;
    
    /* 7. CMD7: Select Card */
    if (!SendCmd(Index, SDC_CMD(CMD7) | SDC_RSPTYP(SDC_RSP_R1B), g_rca << 16, resp)) {
        if (verbose) LCD_DrawString(20, 180, "CMD7 FAIL", 0xF800, 0x0000);
        return false;
    }
    
    if (verbose) LCD_DrawString(20, 180, "CARD SELECT OK", 0x07E0, 0x0000);
    
    /* 8. ACMD6: Set Bus Width */
    if (use4Bit) {
        SendCmd(Index, SDC_CMD(CMD55) | SDC_RSPTYP(SDC_RSP_R1), g_rca << 16, NULL);
        /* 2 = 4-bit bus */
        if (!SendCmd(Index, SDC_CMD(ACMD6) | SDC_RSPTYP(SDC_RSP_R1), 2, NULL)) return false;
        MSDC_SetBusWidth(Index, 4);
    } else {
        SendCmd(Index, SDC_CMD(CMD55) | SDC_RSPTYP(SDC_RSP_R1), g_rca << 16, NULL);
        /* 0 = 1-bit bus */
        if (!SendCmd(Index, SDC_CMD(ACMD6) | SDC_RSPTYP(SDC_RSP_R1), 0, NULL)) return false;
        MSDC_SetBusWidth(Index, 1);
    }
    
    /* 9. CMD16: Set Block Length (512) */
    SendCmd(Index, SDC_CMD(CMD16) | SDC_RSPTYP(SDC_RSP_R1), 512, NULL);
    
    /* Switch to target speed */
    MSDC_SetClock(Index, finalSpeed);
    
    /* VERIFY READ ACCESS */
    {
        uint8_t testBuf[512];
        /* Try to read sector 0 to confirm data lines are working */
        if (!SD_ReadBlock(Index, 0, testBuf)) {
            if (verbose) {
                LCD_DrawString(20, 120, "INIT OK, READ FAIL", 0xF800, 0x0000);
            }
            return false;
        }
    }
    
    if (verbose) {
        LCD_DrawString(20, 120, "INIT & READ SUCCESS", 0x07E0, 0x0000);
    }
    return true;
}

boolean SD_Initialize(TMSDC Index)
{
    /* Silent Mode: Try to initialize without LCD output */
    if (SD_PerformInitSequence(Index, true, 25000000, false)) return true;
    if (SD_PerformInitSequence(Index, false, 25000000, false)) return true;
    if (SD_PerformInitSequence(Index, false, 10000000, false)) return true;
    if (SD_PerformInitSequence(Index, false, 400000, false)) return true;
    return false;
}

boolean SD_RunDiagnostic(TMSDC Index)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 20, "SD CARD DIAGNOSTIC", 0x07E0, 0x0000);
    LCD_FillRect(0, 35, 240, 1, 0x07E0);

    LCD_DrawString(20, 50, "Starting...", 0xFFFF, 0x0000);
    
    /* Attempt 1: 4-Bit 25MHz (Standard) */
    if (SD_PerformInitSequence(Index, true, 25000000, true)) {
        LCD_DrawString(20, 140, "MODE: 4BIT 25MHz", 0x07E0, 0x0000);
        goto test_read;
    }
    
    /* Attempt 2: 1-Bit 25MHz (Bad wiring/contact) */
    if (SD_PerformInitSequence(Index, false, 25000000, true)) {
        LCD_DrawString(20, 140, "MODE: 1BIT 25MHz", 0xFFE0, 0x0000);
        goto test_read;
    }

    /* Attempt 3: 1-Bit 10MHz (Noisy environment) */
    if (SD_PerformInitSequence(Index, false, 10000000, true)) {
        LCD_DrawString(20, 140, "MODE: 1BIT 10MHz", 0xFFE0, 0x0000);
        goto test_read;
    }

    /* Attempt 4: 1-Bit 400kHz (Failsafe) */
    if (SD_PerformInitSequence(Index, false, 400000, true)) {
        LCD_DrawString(20, 140, "MODE: 1BIT LO-SPD", 0xF800, 0x0000);
        goto test_read;
    }
    
    LCD_DrawString(20, 160, "ALL MODES FAILED!", 0xF800, 0x0000);
    return false;

test_read:
    USC_Pause_us(100000);
    LCD_DrawString(20, 160, "Reading Sec 0...", 0xFFFF, 0x0000);
    
    uint8_t buf[512];
    if (SD_ReadBlock(Index, 0, buf)) {
        LCD_DrawString(20, 180, "READ: OK", 0x07E0, 0x0000);
        
        char hexBuf[30];
        sprintf(hexBuf, "DATA: %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);
        LCD_DrawString(20, 200, hexBuf, 0x7BEF, 0x0000);
        
        sprintf(hexBuf, "SIG: %02X %02X", buf[510], buf[511]);
        LCD_DrawString(20, 220, hexBuf, 0x7BEF, 0x0000);
        return true;
    } else {
        LCD_DrawString(20, 180, "READ: FAILED", 0xF800, 0x0000);
        return false;
    }
}

boolean SD_ReadBlock(TMSDC Index, uint32_t Sector, void *Buffer)
{
    /* CMD17: Read Single Block */
    if (!SendCmd(Index, SDC_CMD(CMD17) | SDC_RSPTYP(SDC_RSP_R1) | SDC_DTYPE(SDC_DTYPE_SINGLE) | SDC_CMD_READ | SDC_BLKLEN(512), Sector, NULL)) {
        return false;
    }
    
    return MSDC_ReadData(Index, Buffer, 512);
}
