/*
 * Memory Browser - Hex Viewer for Flash and RAM
 * Read-only memory viewer with hex and ASCII display
 */
#include "systemconfig.h"
#include "lcdif.h"
#include "keypad.h"
#include "utils.h"
#include "rgu.h"

/* External LCD functions */
extern void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg);

#define BYTES_PER_LINE 16
#define LINES_PER_SCREEN 12

static uint32_t g_currentAddress = 0x10000000; // Start at flash

/* Safe memory read with bounds checking */
static uint8_t SafeMemRead(uint32_t addr)
{
    // Flash region: 0x10000000 - 0x10400000 (4MB)
    // RAM region: 0x00000000 - 0x00400000 (4MB)
    
    if ((addr >= 0x10000000 && addr < 0x10400000) ||
        (addr >= 0x00000000 && addr < 0x00400000)) {
        return *(volatile uint8_t*)addr;
    }
    return 0xFF; // Invalid address
}

/* Convert nibble to hex char */
static char NibbleToHex(uint8_t nibble)
{
    nibble &= 0x0F;
    return (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
}

/* Draw hex byte */
static void DrawHexByte(uint16_t x, uint16_t y, uint8_t byte, uint16_t color)
{
    char hex[3];
    hex[0] = NibbleToHex(byte >> 4);
    hex[1] = NibbleToHex(byte);
    hex[2] = '\0';
    LCD_DrawString(x, y, hex, color, 0x0000);
}

/* Draw address */
static void DrawAddress(uint16_t x, uint16_t y, uint32_t addr)
{
    char addrStr[11];
    addrStr[0] = '0';
    addrStr[1] = 'x';
    for (int i = 0; i < 8; i++) {
        addrStr[2 + i] = NibbleToHex((addr >> (28 - i*4)) & 0x0F);
    }
    addrStr[10] = '\0';
    LCD_DrawString(x, y, addrStr, 0x7BEF, 0x0000);
}

/* Memory Browser main function */
void Action_MemoryBrowser(void)
{
    KP_ClearKeyBuffer();
    boolean running = true;
    
    // Draw once initially
    boolean needsRedraw = true;
    
    while (running) {
        RGU_RestartWDT();
        
        if (needsRedraw) {
            // Clear screen
            LCD_FillRect(0, 0, 240, 320, 0x0000);
            
            // Title
            LCD_DrawString(40, 5, "MEMORY BROWSER", 0xFFFF, 0x0000);
            
            // Current address
            LCD_DrawString(5, 25, "ADDR:", 0x7BEF, 0x0000);
            DrawAddress(35, 25, g_currentAddress);
            
            // Memory type indicator
        if (g_currentAddress >= 0x10000000) {
            LCD_DrawString(140, 25, "[FLASH]", 0x07E0, 0x0000);
        } else {
            LCD_DrawString(140, 25, "[RAM]", 0x001F, 0x0000);
        }
        
        // Draw memory dump
        uint16_t yPos = 45;
        uint32_t addr = g_currentAddress;
        
        for (int line = 0; line < LINES_PER_SCREEN; line++) {
            // Draw address for this line
            DrawAddress(5, yPos, addr);
            
            // Draw hex bytes
            uint16_t hexX = 65;
            for (int i = 0; i < BYTES_PER_LINE; i++) {
                uint8_t byte = SafeMemRead(addr + i);
                DrawHexByte(hexX, yPos, byte, 0xFFFF);
                hexX += 18;
                
                // Space every 4 bytes
                if ((i + 1) % 4 == 0) hexX += 6;
            }
            
            // Draw ASCII representation
            uint16_t asciiX = 5;
            yPos += 12;
            for (int i = 0; i < BYTES_PER_LINE; i++) {
                uint8_t byte = SafeMemRead(addr + i);
                char c = (byte >= 32 && byte < 127) ? byte : '.';
                char str[2] = {c, '\0'};
                LCD_DrawString(asciiX, yPos, str, 0x7BEF, 0x0000);
                asciiX += 6;
            }
            
            addr += BYTES_PER_LINE;
            yPos += 8;
        }
        
            // Help text
            LCD_DrawString(5, 285, "UP/DN:SCROLL", 0xFFE0, 0x0000);
            LCD_DrawString(5, 300, "L/R:JUMP BACK:EXIT", 0xFFE0, 0x0000);
            
            needsRedraw = false;
        }
        
        // Wait for input
        USC_Pause_us(50000);
        
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            needsRedraw = true;
            
            switch (key) {
                case KEY_UP:
                    if (g_currentAddress >= BYTES_PER_LINE * LINES_PER_SCREEN) {
                        g_currentAddress -= BYTES_PER_LINE * LINES_PER_SCREEN;
                    }
                    break;
                    
                case KEY_DOWN:
                    g_currentAddress += BYTES_PER_LINE * LINES_PER_SCREEN;
                    // Bounds check
                    if (g_currentAddress >= 0x10400000) {
                        g_currentAddress = 0x10000000;
                    }
                    break;
                    
                case KEY_LEFT:
                    // Jump to RAM
                    g_currentAddress = 0x00000000;
                    break;
                    
                case KEY_RIGHT:
                    // Jump to Flash
                    g_currentAddress = 0x10000000;
                    break;
                    
                case KEY_BACK:
                    running = false;
                    break;
                    
                default:
                    break;
            }
            
            USC_Pause_us(150000); // Debounce
        }
    }
}
