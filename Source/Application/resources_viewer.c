/*
 * System Resources Viewer
 * Display embedded resources and configuration
 */
#include "systemconfig.h"
#include "lcdif.h"
#include "keypad.h"
#include "utils.h"
#include "rgu.h"

/* External LCD functions */
extern void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg);

/* Resources viewer */
void Action_Resources(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    
    LCD_DrawString(40, 10, "SYSTEM RESOURCES", 0xFFFF, 0x0000);
    
    // Firmware info
    LCD_DrawString(20, 50, "FIRMWARE:", 0x07E0, 0x0000);
    LCD_DrawString(20, 70, "  VERSION: 1.0", 0x7BEF, 0x0000);
    LCD_DrawString(20, 85, "  BUILD: CUSTOM", 0x7BEF, 0x0000);
    
    // Embedded resources
    LCD_DrawString(20, 110, "RESOURCES:", 0x07E0, 0x0000);
    LCD_DrawString(20, 130, "  FONTS: 5X7 ASCII", 0x7BEF, 0x0000);
    LCD_DrawString(20, 145, "  GAMES: 6", 0x7BEF, 0x0000);
    LCD_DrawString(20, 160, "  APPS: 4", 0x7BEF, 0x0000);
    
    // Configuration
    LCD_DrawString(20, 185, "CONFIG:", 0x07E0, 0x0000);
    LCD_DrawString(20, 205, "  LCD: 240X320", 0x7BEF, 0x0000);
    LCD_DrawString(20, 220, "  KEYS: 20", 0x7BEF, 0x0000);
    LCD_DrawString(20, 235, "  BACKLIGHT: AUTO", 0x7BEF, 0x0000);
    
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while (1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}
