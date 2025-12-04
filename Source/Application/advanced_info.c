/*
 * Advanced System Info
 * Detailed hardware status and memory usage
 */
#include "systemconfig.h"
#include "lcdif.h"
#include "keypad.h"
#include "utils.h"
#include "rgu.h"
#include "pmu.h"
#include "rtc.h"

/* External LCD functions */
extern void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg);

/* Advanced System Info */
void Action_AdvancedInfo(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    
    LCD_DrawString(20, 10, "ADVANCED SYSTEM INFO", 0xFFFF, 0x0000);
    LCD_FillRect(0, 30, 240, 1, 0x7BEF);
    
    // Flash memory info
    LCD_DrawString(10, 40, "FLASH MEMORY:", 0x07E0, 0x0000);
    LCD_DrawString(20, 58, "TOTAL: 4MB", 0x7BEF, 0x0000);
    LCD_DrawString(20, 73, "REGION: 0X10000000", 0x7BEF, 0x0000);
    
    // RAM info
    LCD_DrawString(10, 95, "RAM:", 0x07E0, 0x0000);
    LCD_DrawString(20, 113, "TOTAL: 4MB", 0x7BEF, 0x0000);
    LCD_DrawString(20, 128, "REGION: 0X00000000", 0x7BEF, 0x0000);
    
    // CPU info
    LCD_DrawString(10, 150, "CPU:", 0x07E0, 0x0000);
    LCD_DrawString(20, 168, "MT6261DA", 0x7BEF, 0x0000);
    
    // Get real CPU frequency from system
    // MT6261DA clock from PLL_SYS_CON0
    uint32_t pll_con0 = *(volatile uint32_t*)0xA0170100;
    uint32_t div = ((pll_con0 >> 9) & 0x1F) + 1; // Divider
    uint32_t freq_mhz = (26 * 10) / div; // 26MHz ref * 10 / divider
    
    char freq_str[20];
    freq_str[0] = 'C';
    freq_str[1] = 'L';
    freq_str[2] = 'O';
    freq_str[3] = 'C';
    freq_str[4] = 'K';
    freq_str[5] = ':';
    freq_str[6] = ' ';
    
    // Convert frequency to string
    if (freq_mhz >= 100) {
        freq_str[7] = '0' + (freq_mhz / 100);
        freq_str[8] = '0' + ((freq_mhz / 10) % 10);
        freq_str[9] = '0' + (freq_mhz % 10);
        freq_str[10] = 'M';
        freq_str[11] = 'H';
        freq_str[12] = 'Z';
        freq_str[13] = '\0';
    } else {
        freq_str[7] = '0' + (freq_mhz / 10);
        freq_str[8] = '0' + (freq_mhz % 10);
        freq_str[9] = 'M';
        freq_str[10] = 'H';
        freq_str[11] = 'Z';
        freq_str[12] = '\0';
    }
    
    LCD_DrawString(20, 183, freq_str, 0x7BEF, 0x0000);
    
    // Display info
    LCD_DrawString(10, 205, "DISPLAY:", 0x07E0, 0x0000);
    LCD_DrawString(20, 223, "LCD: 240X320", 0x7BEF, 0x0000);
    LCD_DrawString(20, 238, "COLORS: 65K", 0x7BEF, 0x0000);
    
    // Status
    LCD_DrawString(10, 260, "STATUS:", 0x07E0, 0x0000);
    LCD_DrawString(20, 278, "SYSTEM: OK", 0x07E0, 0x0000);
    
    LCD_DrawString(60, 305, "BACK:EXIT", 0xFFE0, 0x0000);
    
    LCD_DrawString(60, 305, "BACK:Exit", 0xFFE0, 0x0000);
    
    while (1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}
