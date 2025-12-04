/*
 * Boot Logo Display
 * Shows a custom logo at device startup
 */
#include "boot_logo.h"
#include "lcdif.h"
#include "utils.h"

// If you have a custom logo, include it here
// Example: #include "my_logo.h"
// For now, we'll draw a simple custom logo

void ShowBootLogo(void)
{
    // Clear screen to black
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    
    // Draw a simple logo/welcome screen
    // You can replace this with your custom logo data
    
    // Draw border
    LCD_FillRect(0, 0, 240, 3, 0x07E0);      // Top border (green)
    LCD_FillRect(0, 317, 240, 3, 0x07E0);    // Bottom border (green)
    LCD_FillRect(0, 0, 3, 320, 0x07E0);      // Left border (green)
    LCD_FillRect(237, 0, 3, 320, 0x07E0);    // Right border (green)
    
    // Draw title box
    LCD_FillRect(20, 100, 200, 60, 0x001F);  // Blue box
    LCD_FillRect(23, 103, 194, 54, 0x0000);  // Black inner
    
    // Custom firmware title
    LCD_DrawString(45, 115, "DZ09 CUSTOM", 0x07E0, 0x0000);
    LCD_DrawString(50, 135, "FIRMWARE", 0x07E0, 0x0000);
    
    // Version info
    LCD_DrawString(75, 180, "v1.0", 0x7BEF, 0x0000);
    
    // Credits
    LCD_DrawString(45, 220, "Modified by:", 0x7BEF, 0x0000);
    LCD_DrawString(80, 240, "AliERIS", 0xFFE0, 0x0000);
    
    // Bottom text
    LCD_DrawString(55, 290, "Loading...", 0x7BEF, 0x0000);
    
    // Keep logo on screen for 2 seconds
    USC_Pause_us(2000000);
}

/* 
 * To use a custom image instead:
 * 
 * 1. Create your image (240x320 PNG/JPG)
 * 2. Run: python img2c.py your_logo.png boot
 * 3. Move boot_logo.h and boot_logo.c to Source/Application/
 * 4. Include: #include "boot_logo.h" (the generated one, not this file)
 * 5. Replace the code above with:
 *    
 *    void ShowBootLogo(void)
 *    {
 *        // Clear screen
 *        LCD_FillRect(0, 0, 240, 320, 0x0000);
 *        
 *        // Draw logo from array
 *        for (int y = 0; y < LOGO_HEIGHT; y++) {
 *            for (int x = 0; x < LOGO_WIDTH; x++) {
 *                uint16_t color = boot_logo_data[y * LOGO_WIDTH + x];
 *                LCD_DrawPixel(x, y, color);
 *            }
 *        }
 *        
 *        // Keep on screen
 *        USC_Pause_us(2000000);
 *    }
 */
