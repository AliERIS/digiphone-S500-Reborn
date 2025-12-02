/*
 * SD Card Protocol Driver
 */
#include "systemconfig.h"
#include "sdcard.h"
#include "msdc.h"
#include "debug.h"

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

boolean SD_Initialize(TMSDC Index)
{
    uint32_t resp[4];
    
    RGU_RestartWDT(); // Restart watchdog at start
    
    LCD_DrawString(20, 120, "SD: INIT CTRL", 0xFFFF, 0x0000);
    
    /* 1. Initialize Controller */
    RGU_RestartWDT(); // Restart watchdog before MSDC init
    MSDC_Init(Index);
    MSDC_SetClock(Index, 400000); // 400kHz for init
    RGU_RestartWDT(); // Restart watchdog after MSDC init
    
    /* Wait for controller and card to stabilize */
    USC_Pause_us(150000); // 150ms delay - increased for better stability
    
    LCD_DrawString(20, 120, "SD: SEND CMD0", 0xFFFF, 0x0000);
    
    /* 2. CMD0: Go Idle State - Send multiple times to ensure card is in idle */
    {
        int retry;
        for (retry = 0; retry < 20; retry++) { // Increased retries to 20
            if (SendCmd(Index, SDC_CMD(CMD0) | SDC_RSPTYP(SDC_NO_RSP), 0, NULL)) {
                USC_Pause_us(20000); // 20ms delay after successful CMD0
                break;
            }
            USC_Pause_us(20000); // 20ms wait before retry
        }
        if (retry == 20) {
            DebugPrint("CMD0 Failed after retries\n");
            LCD_DrawString(20, 140, "SD: CMD0 FAIL", 0xF800, 0x0000);
            return false;
        }
    }
    
    /* Verify CMD0 succeeded by checking if we can proceed */
    USC_Pause_us(50000); // 50ms additional delay before next command
    RGU_RestartWDT(); // Restart watchdog after CMD0
    
    LCD_DrawString(20, 120, "SD: SEND CMD8", 0xFFFF, 0x0000);
    
    /* 3. CMD8: Send Interface Condition */
    RGU_RestartWDT(); // Restart watchdog before CMD8
    if (!SendCmd(Index, SDC_CMD(CMD8) | SDC_RSPTYP(SDC_RSP_R1), 0x1AA, resp)) {
        DebugPrint("CMD8 Failed\n");
        LCD_DrawString(20, 140, "SD: CMD8 FAIL", 0xF800, 0x0000);
        return false;
    }
    if ((resp[0] & 0xFF) != 0xAA) {
        DebugPrint("CMD8 Check Pattern Failed\n");
        LCD_DrawString(20, 140, "SD: CMD8 PAT", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog after CMD8
    USC_Pause_us(10000); // Wait before ACMD41
    
    LCD_DrawString(20, 120, "SD: WAIT ACMD41", 0xFFFF, 0x0000);
    
    /* 4. ACMD41: Send Op Cond */
    uint32_t timeout = 1000; // 1s timeout
    do {
        RGU_RestartWDT(); // Restart watchdog in ACMD41 loop
        SendCmd(Index, SDC_CMD(CMD55) | SDC_RSPTYP(SDC_RSP_R1), 0, NULL);
        USC_Pause_us(5000); // Wait between CMD55 and ACMD41
        SendCmd(Index, SDC_CMD(ACMD41) | SDC_RSPTYP(SDC_RSP_R3), 0x40FF8000, resp); // HCS=1
        USC_Pause_us(10000); // Increased wait time
    } while (!(resp[0] & 0x80000000) && --timeout);
    
    if (timeout == 0) {
        DebugPrint("ACMD41 Timeout\n");
        LCD_DrawString(20, 140, "SD: ACMD41 TO", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog after ACMD41
    LCD_DrawString(20, 120, "SD: SEND CMD2", 0xFFFF, 0x0000);
    
    /* 5. CMD2: All Send CID */
    if (!SendCmd(Index, SDC_CMD(CMD2) | SDC_RSPTYP(SDC_RSP_R2), 0, resp)) {
        DebugPrint("CMD2 Failed\n");
        LCD_DrawString(20, 140, "SD: CMD2 FAIL", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog after CMD2
    LCD_DrawString(20, 120, "SD: SEND CMD3", 0xFFFF, 0x0000);
    
    /* 6. CMD3: Send Relative Address */
    if (!SendCmd(Index, SDC_CMD(CMD3) | SDC_RSPTYP(SDC_RSP_R6), 0, resp)) {
        DebugPrint("CMD3 Failed\n");
        LCD_DrawString(20, 140, "SD: CMD3 FAIL", 0xF800, 0x0000);
        return false;
    }
    g_rca = resp[0] >> 16;
    
    RGU_RestartWDT(); // Restart watchdog after CMD3
    LCD_DrawString(20, 120, "SD: SEND CMD7", 0xFFFF, 0x0000);
    
    /* 7. CMD7: Select Card */
    if (!SendCmd(Index, SDC_CMD(CMD7) | SDC_RSPTYP(SDC_RSP_R1B), g_rca << 16, resp)) {
        DebugPrint("CMD7 Failed\n");
        LCD_DrawString(20, 140, "SD: CMD7 FAIL", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog after CMD7
    
    /* 8. ACMD6: Set Bus Width (4-bit) */
    SendCmd(Index, SDC_CMD(CMD55) | SDC_RSPTYP(SDC_RSP_R1), g_rca << 16, NULL);
    SendCmd(Index, SDC_CMD(ACMD6) | SDC_RSPTYP(SDC_RSP_R1), 2, NULL);
    
    /* 9. CMD16: Set Block Length (512) */
    SendCmd(Index, SDC_CMD(CMD16) | SDC_RSPTYP(SDC_RSP_R1), 512, NULL);
    
    /* Switch to high speed */
    MSDC_SetClock(Index, 25000000); // 25MHz
    
    RGU_RestartWDT(); // Restart watchdog before returning
    
    LCD_DrawString(20, 120, "SD: INIT DONE", 0xFFFF, 0x0000);
    
    return true;
}

boolean SD_ReadBlock(TMSDC Index, uint32_t Sector, void *Buffer)
{
    /* CMD17: Read Single Block */
    if (!SendCmd(Index, SDC_CMD(CMD17) | SDC_RSPTYP(SDC_RSP_R1) | SDC_DTYPE(SDC_DTYPE_SINGLE) | SDC_CMD_READ | SDC_BLKLEN(512), Sector, NULL)) {
        return false;
    }
    
    return MSDC_ReadData(Index, Buffer, 512);
}
