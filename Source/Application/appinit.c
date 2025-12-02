// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
 * This file is part of the DZ09 project.
 *
 * Copyright (C) 2020, 2019 AJScorp
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
#include "appinit.h"
#include "rgu.h"
#include "gpio.h"
#include "pmu.h"
#include "afe.h"
#include "gdifont.h"
#include "Resource/fontlib.h"
#include "kp.h"

/* 5x7 ASCII Font (Minimal Set for "Hello DigiPhone S500 MT6261DA") */
/* 5x7 ASCII Font (Corrected GLCD Values) */
/* Standard 5x7 ASCII Font (Space to Z) */
static const uint8_t Font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, //   0x20
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // ! 0x21
    {0x00, 0x07, 0x00, 0x07, 0x00}, // " 0x22
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // # 0x23
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $ 0x24
    {0x23, 0x13, 0x08, 0x64, 0x62}, // % 0x25
    {0x36, 0x49, 0x55, 0x22, 0x50}, // & 0x26
    {0x00, 0x05, 0x03, 0x00, 0x00}, // ' 0x27
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // ( 0x28
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ) 0x29
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // * 0x2A
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // + 0x2B
    {0x00, 0x50, 0x30, 0x00, 0x00}, // , 0x2C
    {0x08, 0x08, 0x08, 0x08, 0x08}, // - 0x2D
    {0x00, 0x60, 0x60, 0x00, 0x00}, // . 0x2E
    {0x20, 0x10, 0x08, 0x04, 0x02}, // / 0x2F
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0 0x30
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1 0x31
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 0x32
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3 0x33
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4 0x34
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 0x35
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6 0x36
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 0x37
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 0x38
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9 0x39
    {0x00, 0x36, 0x36, 0x00, 0x00}, // : 0x3A
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ; 0x3B
    {0x08, 0x14, 0x22, 0x41, 0x00}, // < 0x3C
    {0x14, 0x14, 0x14, 0x14, 0x14}, // = 0x3D
    {0x00, 0x41, 0x22, 0x14, 0x08}, // > 0x3E
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ? 0x3F
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @ 0x40
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A 0x41
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B 0x42
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C 0x43
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D 0x44
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E 0x45
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F 0x46
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G 0x47
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H 0x48
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I 0x49
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J 0x4A
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K 0x4B
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L 0x4C
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M 0x4D
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N 0x4E
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O 0x4F
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P 0x50
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q 0x51
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R 0x52
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S 0x53
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T 0x54
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U 0x55
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V 0x56
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W 0x57
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X 0x58
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y 0x59
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z 0x5A
};

void LCD_DrawPixel(uint16_t color) {
    LCDIF_WriteData(color >> 8);
    LCDIF_WriteData(color & 0xFF);
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    LCDIF_WriteCommand(0x2A); // Column
    LCDIF_WriteData(x >> 8); LCDIF_WriteData(x & 0xFF);
    LCDIF_WriteData((x+w-1) >> 8); LCDIF_WriteData((x+w-1) & 0xFF);

    LCDIF_WriteCommand(0x2B); // Row
    LCDIF_WriteData(y >> 8); LCDIF_WriteData(y & 0xFF);
    LCDIF_WriteData((y+h-1) >> 8); LCDIF_WriteData((y+h-1) & 0xFF);

    LCDIF_WriteCommand(0x2C); // Write

    for(int i=0; i<w*h; i++) {
        LCD_DrawPixel(color);
    }
}

void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg) {
    if (c < 0x20 || c > 0x5A) c = 0x20; // Bound check (Space to Z)
    const uint8_t *bitmap = Font5x7[c - 0x20];

    LCDIF_WriteCommand(0x2A); // Column
    LCDIF_WriteData(x >> 8); LCDIF_WriteData(x & 0xFF);
    LCDIF_WriteData((x+4) >> 8); LCDIF_WriteData((x+4) & 0xFF);

    LCDIF_WriteCommand(0x2B); // Row
    LCDIF_WriteData(y >> 8); LCDIF_WriteData(y & 0xFF);
    LCDIF_WriteData((y+6) >> 8); LCDIF_WriteData((y+6) & 0xFF);

    LCDIF_WriteCommand(0x2C); // Write

    for(int row=0; row<7; row++) {
        for(int col=0; col<5; col++) {
            uint8_t pixel = (bitmap[col] >> row) & 0x01;
            LCD_DrawPixel(pixel ? color : bg);
        }
    }
}

void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg) {
    while(*str) {
        LCD_DrawChar(x, y, *str, color, bg);
        x += 6;
        str++;
    }
}

typedef struct {
    uint16_t x, y, w, h;
    const char* label;
    TKEY key;
} KeyBtn;

static const KeyBtn KeypadGrid[] = {
    {20, 60,  40, 30, "1", KEY_1}, {70, 60,  40, 30, "2", KEY_2}, {120, 60,  40, 30, "3", KEY_3},
    {20, 100, 40, 30, "4", KEY_4}, {70, 100, 40, 30, "5", KEY_5}, {120, 100, 40, 30, "6", KEY_6},
    {20, 140, 40, 30, "7", KEY_7}, {70, 140, 40, 30, "8", KEY_8}, {120, 140, 40, 30, "9", KEY_9},
    {20, 180, 40, 30, "*", KEY_STAR}, {70, 180, 40, 30, "0", KEY_0}, {120, 180, 40, 30, "#", KEY_HASH},
    
    /* Navigation & Control */
    {170, 60,  50, 30, "UP", KEY_UP},
    {170, 100, 50, 30, "DWN", KEY_DOWN},
    {170, 140, 50, 30, "LFT", KEY_LEFT},
    {170, 180, 50, 30, "RGT", KEY_RIGHT},
    
    {20, 220, 50, 30, "CALL", KEY_CALL}, {80, 220, 40, 30, "OK", KEY_OK}, {130, 220, 50, 30, "BACK", KEY_BACK}, {190, 220, 40, 30, "END", KEY_END}
};

void DrawKey(const KeyBtn* btn, boolean pressed) {
    uint16_t bg = pressed ? clGreen : 0x7BEF; // Grayish
    uint16_t fg = pressed ? clBlack : clWhite;
    
    LCD_FillRect(btn->x, btn->y, btn->w, btn->h, bg);
    
    // Center text roughly
    int len = 0; while(btn->label[len]) len++;
    int tx = btn->x + (btn->w - (len*6))/2;
    int ty = btn->y + (btn->h - 7)/2;
    
    LCD_DrawString(tx, ty, btn->label, fg, bg);
}

boolean APP_Initialize(void)
{
    pGUIOBJECT MainLayer;

    /* CRITICAL: Enable RTC PowerUp to generate PWRBB signal */
    RTC_Unprotect();
    RTC_EnablePowerUp();
    RTC_LockBBPower(); // Add this to ensure BBPU bit is set

    /* FINAL ATTEMPT: Force PWRHOLD via STRUP_CON0 */
    #define THR_HWPDN_EN (1 << 3)
    #define RG_USBDL_EN_STRUP (1 << 4)
    uint16_t strup_orig = STRUP_CON0;
    STRUP_CON0 = (strup_orig | THR_HWPDN_EN) & ~RG_USBDL_EN_STRUP;
    
    RGU_RestartWDT();

    do
    {
        if (!GUI_Initialize()) break;
        RGU_RestartWDT();

        /* Clear Screen to Black */
        LCD_FillRect(0, 0, 240, 320, clBlack);
        
        /* Draw Title */
        LCD_DrawString(80, 20, "KEYPAD TEST", clWhite, clBlack);
        
        /* Draw Initial Grid */
        for(int i=0; i < sizeof(KeypadGrid)/sizeof(KeypadGrid[0]); i++) {
            DrawKey(&KeypadGrid[i], false);
        }

        /* Turn on backlight */
        BL_TurnOn(true);
        RGU_RestartWDT();

        /* Initialize USB */
        USB_Initialize();
        USB_EnableDevice();

        /* Initialize Keypad */
        KP_Initialize();
        RGU_RestartWDT();

        /* Keypad Test Loop */
        TKEY lastKey = KEY_NONE;
        
        /* Backlight Management */
        uint32_t backlightTimer = 0;
        boolean backlightOn = true;
        #define BACKLIGHT_TIMEOUT_MS 30000  // 30 seconds
        
        /* Power Button Management */
        uint32_t pwrPressStartTime = 0;
        boolean pwrWasPressed = false;
        #define PWR_LONG_PRESS_MS 3000      // 3 seconds for shutdown
        
        /* Draw Matrix Grid Labels */
        LCD_DrawString(20, 260, "Raw Matrix (R0-R3)", clWhite, clBlack);
        
        while(1)
        {
            RGU_RestartWDT();

            /* Handle Power Button */
            boolean pwrPressed = PMU_IsPowerKeyPressed();
            
            if (pwrPressed && !pwrWasPressed) {
                // Power button just pressed
                pwrPressStartTime = USC_GetCurrentTicks();
                pwrWasPressed = true;
            }
            else if (!pwrPressed && pwrWasPressed) {
                // Power button just released
                uint32_t pressDuration = (USC_GetCurrentTicks() - pwrPressStartTime) / 1000; // Convert to ms
                
                if (pressDuration < PWR_LONG_PRESS_MS) {
                    // Short press: Toggle backlight
                    backlightOn = !backlightOn;
                    BL_TurnOn(backlightOn);
                    if (backlightOn) {
                        backlightTimer = USC_GetCurrentTicks();
                    }
                }
                // Long press check is done while holding
                pwrWasPressed = false;
            }
            else if (pwrPressed && pwrWasPressed) {
                // Power button is being held
                uint32_t pressDuration = (USC_GetCurrentTicks() - pwrPressStartTime) / 1000;
                
                if (pressDuration >= PWR_LONG_PRESS_MS) {
                    // Long press: Shutdown
                    LCD_FillRect(0, 0, 240, 320, clBlack);
                    LCD_DrawString(60, 150, "SHUTTING DOWN...", clWhite, clBlack);
                    USC_Pause_us(1000000); // Wait 1 second
                    
                    // Actual shutdown
                    BL_TurnOn(false);
                    RTC_DisablePowerUp();
                    while(1); // Wait for power to cut
                }
            }

            /* Backlight Auto-Off Timer */
            if (backlightOn) {
                uint32_t elapsed = (USC_GetCurrentTicks() - backlightTimer) / 1000; // ms
                if (elapsed >= BACKLIGHT_TIMEOUT_MS) {
                    backlightOn = false;
                    BL_TurnOn(false);
                }
            }

            /* 1. Handle Standard Key Logic (Keep existing visual) */
            if (KP_IsKeyPressed())
            {
                // Any key press resets backlight timer
                if (!backlightOn) {
                    backlightOn = true;
                    BL_TurnOn(true);
                }
                backlightTimer = USC_GetCurrentTicks();
                
                TKEY currentKey = KP_ReadKey();
                if (currentKey != lastKey) {
                    if (lastKey != KEY_NONE) {
                        for(int i=0; i < sizeof(KeypadGrid)/sizeof(KeypadGrid[0]); i++) {
                            if (KeypadGrid[i].key == lastKey) {
                                DrawKey(&KeypadGrid[i], false);
                                break;
                            }
                        }
                    }
                    if (currentKey != KEY_NONE) {
                         for(int i=0; i < sizeof(KeypadGrid)/sizeof(KeypadGrid[0]); i++) {
                            if (KeypadGrid[i].key == currentKey) {
                                DrawKey(&KeypadGrid[i], true);
                                break;
                            }
                        }
                    }
                    lastKey = currentKey;
                }
            }
            else
            {
                if (lastKey != KEY_NONE) {
                     for(int i=0; i < sizeof(KeypadGrid)/sizeof(KeypadGrid[0]); i++) {
                        if (KeypadGrid[i].key == lastKey) {
                            DrawKey(&KeypadGrid[i], false);
                            break;
                        }
                    }
                    lastKey = KEY_NONE;
                }
            }
            
            /* 2. Draw Raw Matrix State */
            uint32_t matrix = KP_GetMatrixState();
            
            for(int r=0; r<4; r++) {
                for(int c=0; c<5; c++) {
                    int bitIndex = (r*5) + c;
                    boolean active = (matrix >> bitIndex) & 0x01;
                    
                    uint16_t color = active ? clRed : clGray;
                    // Draw small circle/rect
                    int mx = 20 + (c * 20);
                    int my = 280 + (r * 10);
                    LCD_FillRect(mx, my, 15, 8, color);
                }
            }
            
            /* DEBUG: Show raw matrix value */
            char matrixHex[16];
            uint32_t tempMatrix = matrix;
            for(int i=0; i<5; i++) {
                uint8_t nibble = (tempMatrix >> (16 - i*4)) & 0x0F;
                matrixHex[i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
            }
            matrixHex[5] = 0;
            LCD_DrawString(20, 245, "M:", clYellow, clBlack);
            LCD_DrawString(32, 245, matrixHex, clYellow, clBlack);
            
            /* 3. Draw Power Key Status */
            LCD_DrawString(140, 260, "PWR:", clWhite, clBlack);
            LCD_DrawChar(170, 260, pwrPressed ? '1' : '0', pwrPressed ? clGreen : clRed, clBlack);

            USC_Pause_us(50000); // 50ms
            
            /* Heartbeat */
            static int heartbeat = 0;
            heartbeat++;
            if ((heartbeat % 10) == 0) {
                 int digit = (heartbeat/10) % 5; 
                 const uint8_t *bmp = &Font5x7[13*5];
                 if(digit==1) bmp = &Font5x7[18*5];
                 if(digit==2) bmp = &Font5x7[17*5];
                 if(digit==3) bmp = &Font5x7[12*5];
                 if(digit==4) bmp = &Font5x7[16*5];
                 LCD_DrawChar(200, 10, bmp, clWhite, clRed);
                 
                 /* DEBUG: Show Last Key Code Raw */
                 LCD_DrawChar(160, 30, 'K', clWhite, clBlack);
                 LCD_DrawChar(166, 30, ':', clWhite, clBlack);
                 uint8_t val = (uint8_t)lastKey;
                 uint8_t h = val >> 4;
                 uint8_t l = val & 0x0F;
                 if(h < 10) LCD_DrawChar(172, 30, (char)('0'+h), clWhite, clBlack);
                 else LCD_DrawChar(172, 30, (char)('A'+h-10), clWhite, clBlack);
                 if(l < 10) LCD_DrawChar(178, 30, (char)('0'+l), clWhite, clBlack);
                 else LCD_DrawChar(178, 30, (char)('A'+l-10), clWhite, clBlack);
            }
        }

        return true;
    }
    while(0);

    return false;
}



