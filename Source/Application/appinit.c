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
#include "menu.h"
#include "fat32.h"

/* Global Backlight State */
uint32_t g_backlightTimer = 0;
boolean g_backlightOn = true;

void KeepBacklightOn(void) {
    g_backlightTimer = USC_GetCurrentTicks();
    g_backlightOn = true;
    BL_TurnOn(true);  /* Always force backlight ON */
}

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

void LCD_PushPixel(uint16_t color) {
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
        LCD_PushPixel(color);
    }
}

void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg) {
    if (c < 0x20 || c > 0x7E) c = 0x20; // Bound check (Space to ~)
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
            LCD_PushPixel(pixel ? color : bg);
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

void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        LCD_PushPixel(color); // Reuse local helper (was LCD_DrawPixel, locally renamed or used as helper)
        // Wait, I need to call LCD_DrawPixel from lcdif.h or use the local LCD_PushPixel?
        // appinit.c has LCD_PushPixel defined locally. To use it for drawing a single pixel at specific coords,
        // I need to set the address window for each pixel.
        // That is inefficient. Better to use LCD_DrawPixel if available in hardware, or set window 1x1.
        // appinit.c LCD_PushPixel just writes DATA. It assumes COMMAND 0x2C was sent and Window set.
        // So I must set window for each pixel in DrawLine.
        
        LCDIF_WriteCommand(0x2A); LCDIF_WriteData(x0 >> 8); LCDIF_WriteData(x0 & 0xFF); LCDIF_WriteData(x0 >> 8); LCDIF_WriteData(x0 & 0xFF);
        LCDIF_WriteCommand(0x2B); LCDIF_WriteData(y0 >> 8); LCDIF_WriteData(y0 & 0xFF); LCDIF_WriteData(y0 >> 8); LCDIF_WriteData(y0 & 0xFF);
        LCDIF_WriteCommand(0x2C); 
        LCD_PushPixel(color);

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
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

static void PlayBootLog(void) {
    const char* logs[] = {
        "[    0.000000] BOOTING LINUX ON PHYSICAL CPU 0X0",
        "[    0.000000] LINUX VERSION 2.6.35 (ALI@S500) #1",
        "[    0.000000] CPU: ARM926EJ-S [41069265] REVISION 5 (ARMV5TEJ)",
        "[    0.000000] MACHINE: MEDIATEK MT6261DA",
        "[    0.000000] MEMORY POLICY: ECC DISABLED, DATA CACHE WRITEBACK",
        "[    0.000000] BUILT 1 ZONELISTS IN ZONE ORDER, MOBILITY GROUPING ON.",
        "[    0.000000] KERNEL COMMAND LINE: CONSOLE=TTYS0,115200 ROOT=/DEV/MTDBLOCK2",
        "[    0.000125] PID HASH TABLE ENTRIES: 128 (ORDER: -3, 512 BYTES)",
        "[    0.000213] DENTRY CACHE HASH TABLE ENTRIES: 4096 (ORDER: 2, 16384 BYTES)",
        "[    0.000451] INODE-CACHE HASH TABLE ENTRIES: 2048 (ORDER: 1, 8192 BYTES)",
        "[    0.001021] MEMORY: 4MB = 4MB TOTAL",
        "[    0.002000] MEMORY: 3264K/3264K AVAILABLE, 832K RESERVED",
        "[    0.150000] MT6261_PMU_INIT: PMU INITIALIZED",
        "[    0.210000] MT6261_LCD_INIT: ILI9341 FOUND",
        "[    0.350000] CONSOLE: COLOUR DUMMY DEVICE 80X30",
        "[    0.352000] CALIBRATING DELAY LOOP... 104.00 BOGOMIPS (LPJ=52000)",
        "[    0.580000] MOUNT-CACHE HASH TABLE ENTRIES: 512",
        "[    0.601000] NET: REGISTERED PROTOCOL FAMILY 16",
        "[    0.612000] BIO: CREATE SLAB <BIO-0> AT 0",
        "[    0.640000] USBCORE: REGISTERED NEW INTERFACE DRIVER USBFS",
        "[    0.650000] USBCORE: REGISTERED NEW INTERFACE DRIVER HUB",
        "[    0.660000] USBCORE: REGISTERED NEW DEVICE DRIVER USB",
        "[    0.800000] RTC: MT6261-RTC SET TO 2025-12-07 22:37:37",
        "[    1.020000] VFS: MOUNTED ROOT (CRAMFS FILESYSTEM) READONLY.",
        "[    1.150000] FREEING INIT MEMORY: 104K",
        "[    1.450000] STARTING GUI..."
    };
    
    LCD_FillRect(0, 0, 240, 320, 0x0000); // Black background
    
    int y = 5;
    int count = sizeof(logs) / sizeof(logs[0]);
    
    for(int i=0; i<count; i++) {
        LCD_DrawString(2, y, logs[i], 0xFFFF, 0x0000); // White on Black
        y += 12; // Line height
        
        USC_Pause_us(i < 5 ? 10000 : 50000 + (i*2000)); // Varying speed
        RGU_RestartWDT();
        
        if (y > 310) {
            // Simple scroll effect: Clear screen and reset y (or just stop)
            // For realism, let's just clear for now or simple "scroll"
            // Implementing full scroll is complex without a buffer, so just reset top
            LCD_FillRect(0, 0, 240, 320, 0x0000); 
            y = 5;
        }
    }
    
    USC_Pause_us(500000); // Pause at end
}

static void PlayBootAnim(void) {
    /* Simple Penguin Pixel Art (16x16 scaled by 8) */
    const int SCALE = 8;
    const int START_X = (240 - (16 * SCALE)) / 2;
    const int START_Y = (320 - (16 * SCALE)) / 2;
    
    /* 0:Trans, 1:Black, 2:White, 3:Orange */
    const uint8_t penguin[16][16] = {
        {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
        {0,0,0,0,1,1,2,2,2,2,1,1,0,0,0,0},
        {0,0,0,1,2,2,2,2,2,2,2,1,0,0,0,0},
        {0,0,0,1,2,2,1,2,1,2,2,1,0,0,0,0}, // Eyes
        {0,0,0,1,2,2,1,2,1,2,2,1,0,0,0,0},
        {0,0,0,1,2,2,2,3,2,2,2,1,0,0,0,0}, // Beak
        {0,0,0,1,2,2,2,2,2,2,2,1,0,0,0,0},
        {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0}, // Neck
        {0,0,1,1,2,2,1,1,1,1,2,2,1,1,0,0},
        {0,1,2,2,1,1,2,2,2,2,1,1,2,2,1,0},
        {0,1,2,2,1,1,2,2,2,2,1,1,2,2,1,0},
        {0,1,2,2,1,1,2,2,2,2,1,1,2,2,1,0},
        {0,0,1,1,1,1,2,2,2,2,1,1,1,1,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0}, // Feet
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };
    
    LCD_FillRect(0, 0, 240, 320, 0xFFFF); // White background
    RGU_RestartWDT(); // Prevent watchdog reset
    
    /* Draw Penguin */
    for(int y=0; y<16; y++) {
        for(int x=0; x<16; x++) {
            uint16_t color = 0xFFFF;
            switch(penguin[y][x]) {
                case 1: color = 0x0000; break; // Black
                case 2: color = 0xFFFF; break; // White
                case 3: color = 0xFD20; break; // Orange
                default: continue; // Transparent
            }
            LCD_FillRect(START_X + x*SCALE, START_Y + y*SCALE, SCALE, SCALE, color);
        }
        RGU_RestartWDT(); // Keep watchdog happy during drawing
    }
    
    LCD_DrawString(80, START_Y + 16*SCALE + 20, "DIGIPHONE S500 ", 0x0000, 0xFFFF);
    
    /* Split 2 second delay into small chunks to avoid watchdog */
    for(int i=0; i<20; i++) {
        USC_Pause_us(100000); // 100ms x 20 = 2 seconds
        RGU_RestartWDT();
    }
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
        /* Initialize Power Management Unit (Battery, Charging) */
        if (!PMU_Initialize()) break;
        RGU_RestartWDT();

        if (!GUI_Initialize()) break;
        RGU_RestartWDT();

        /* Clear Screen to Black */
        LCD_FillRect(0, 0, 240, 320, (uint16_t)clBlack);

        /* Turn on backlight */
        BL_TurnOn(true);
        RGU_RestartWDT();
        
        /* Linux Boot Log */
        PlayBootLog();
        
        /* Boot Animation */
        PlayBootAnim();

        /* Initialize USB */
        USB_Initialize();
        USB_EnableDevice();

        /* Initialize Keypad */
        KP_Initialize();
        RGU_RestartWDT();
        
        /* Initialize Menu System */
        Menu_Initialize();
        Menu *currentMenu = Menu_GetRoot();
        
        /* === HOME SCREEN === */
        #define HOME_BACKLIGHT_TIMEOUT_MS ((uint32_t)1800000)  // 30 minutes
        {
            boolean onHomeScreen = true;
            uint32_t lastUpdateTime = 0;
            
            /* Draw initial wallpaper - gradient from dark blue to purple */
            for (int y = 0; y < 320; y++) {
                uint8_t r = (y * 8) / 320;       /* 0 to 8 */
                uint8_t g = 0;
                uint8_t b = 31 - (y * 16) / 320; /* 31 to 15 */
                uint16_t color = (r << 11) | (g << 5) | b;
                LCD_FillRect(0, y, 240, 1, color);
            }
            
            /* Draw decorative elements */
            LCD_DrawString(60, 280, "DigiPhone S500", 0x7BEF, 0x0000);
            LCD_DrawString(50, 300, "Press OK for Menu", 0x4A69, 0x0000);
            
            while (onHomeScreen) {
                RGU_RestartWDT();
                
                /* Update clock every second */
                uint32_t now = USC_GetCurrentTicks();
                if (now - lastUpdateTime > 1000000) { /* 1 second */
                    lastUpdateTime = now;
                    
                    /* Get time from RTC */
                    TDATETIME dt = RTC_GetDateTime();
                    
                    /* Draw time in large font (2x scale effect) */
                    char timeBuf[10];
                    sprintf(timeBuf, "%02d:%02d", dt.Time.Hour, dt.Time.Min);
                    
                    /* Clear time area */
                    LCD_FillRect(50, 100, 140, 50, 0x0000);
                    
                    /* Draw each digit larger (shadow effect) */
                    for (int i = 0; i < 5; i++) {
                        char ch[2] = {timeBuf[i], 0};
                        LCD_DrawString(55 + i * 24, 100, ch, 0xFFFF, 0x0000);
                        LCD_DrawString(56 + i * 24, 100, ch, 0xFFFF, 0x0000);
                        LCD_DrawString(55 + i * 24, 101, ch, 0xFFFF, 0x0000);
                        LCD_DrawString(56 + i * 24, 101, ch, 0xFFFF, 0x0000);
                    }
                    
                    /* Draw date */
                    char dateBuf[20];
                    sprintf(dateBuf, "%02d/%02d/%04d", dt.Date.Day, dt.Date.Month, dt.Date.Year);
                    LCD_FillRect(65, 160, 110, 16, 0x0000);
                    LCD_DrawString(70, 160, dateBuf, 0xC618, 0x0000);
                }
                
                /* Handle input */
                if (KP_IsKeyPressed()) {
                    KeepBacklightOn();  /* Wake backlight on any key press */
                    TKEY key = KP_ReadKey();
                    if (key == KEY_OK || key == KEY_CALL) {
                        onHomeScreen = false;
                    }
                }
                
                /* Backlight timer check - DISABLED */
                /*
                if (g_backlightOn) {
                    uint32_t elapsed = (USC_GetCurrentTicks() - g_backlightTimer) / 1000;
                    if (elapsed >= HOME_BACKLIGHT_TIMEOUT_MS) {
                        g_backlightOn = false;
                        BL_TurnOn(false);
                    }
                }
                */
                
                /* Handle Power Button on home screen */
                boolean pwrPressedHome = PMU_IsPowerKeyPressed();
                static boolean pwrWasPressedHome = false;
                static uint32_t pwrStartHome = 0;
                
                if (pwrPressedHome && !pwrWasPressedHome) {
                    pwrStartHome = USC_GetCurrentTicks();
                    pwrWasPressedHome = true;
                } else if (!pwrPressedHome && pwrWasPressedHome) {
                    uint32_t dur = (USC_GetCurrentTicks() - pwrStartHome) / 1000;
                    if (dur < 3000) {
                        g_backlightOn = !g_backlightOn;
                        BL_TurnOn(g_backlightOn);
                    }
                    pwrWasPressedHome = false;
                }
                
                USC_Pause_us(50000);
            }
        }
        
        /* Entering Menu Mode */
        Menu_Render(currentMenu);
        
        /* Backlight Management - GLOBAL so games can reset it */
        extern uint32_t g_backlightTimer;
        extern boolean g_backlightOn;
        g_backlightTimer = USC_GetCurrentTicks();
        g_backlightOn = true;
        #define BACKLIGHT_TIMEOUT_MS ((uint32_t)1800000)  // 30 minutes (30 * 60 * 1000)
        
        /* Power Button Management */
        uint32_t pwrPressStartTime = 0;
        boolean pwrWasPressed = false;
        #define PWR_LONG_PRESS_MS 3000      // 3 seconds for shutdown
        
        /* Main Menu Loop */
        TKEY lastKey = KEY_NONE;
        
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
                uint32_t pressDuration = (USC_GetCurrentTicks() - pwrPressStartTime) / 1000;
                
                if (pressDuration < PWR_LONG_PRESS_MS) {
                    // Short press: Toggle backlight
                    g_backlightOn = !g_backlightOn;
                    BL_TurnOn(g_backlightOn);
                    if (g_backlightOn) {
                        g_backlightTimer = USC_GetCurrentTicks();
                        Menu_Render(currentMenu); // Redraw menu when waking up
                    }
                }
                pwrWasPressed = false;
            }
            else if (pwrPressed && pwrWasPressed) {
                // Power button is being held
                uint32_t pressDuration = (USC_GetCurrentTicks() - pwrPressStartTime) / 1000;
                
                if (pressDuration >= PWR_LONG_PRESS_MS) {
                    // Long press: Shutdown sequence
                    LCD_FillRect(0, 0, 240, 320, (uint16_t)clBlack);
                    LCD_DrawString(50, 150, "SHUTTING DOWN...", (uint16_t)clWhite, (uint16_t)clBlack);
                    
                    // Disable watchdog to prevent reset during shutdown
                    RGU_DisableWDT();
                    
                    USC_Pause_us(500000); // 0.5 second delay
                    
                    // Disable all power systems
                    BL_TurnOn(false);
                    
                    // Disable RTC power-up
                    RTC_Unprotect();
                    RTC_DisablePowerUp();
                    
                    // Clear power hold bit to allow shutdown
                    STRUP_CON0 &= ~THR_HWPDN_EN;
                    
                    // Infinite loop - device should power off
                    while(1);
                }
            }

            /* Backlight Auto-Off Timer - DISABLED */
            /*
            if (g_backlightOn) {
                uint32_t elapsed = (USC_GetCurrentTicks() - g_backlightTimer) / 1000;
                if (elapsed >= BACKLIGHT_TIMEOUT_MS) {
                    g_backlightOn = false;
                    BL_TurnOn(false);
                }
            }
            */

            /* Handle Menu Navigation */
            if (KP_IsKeyPressed())
            {
                // Any key press resets backlight timer and forces it ON
                KeepBacklightOn();

                TKEY currentKey = KP_ReadKey();
                if (currentKey != KEY_NONE) {
                    /* Only process if key changed OR enough time passed for repeat */
                    static uint32_t lastKeyTime = 0;
                    uint32_t currentTime = USC_GetCurrentTicks();
                    
                    if (currentKey != lastKey || (currentTime - lastKeyTime) > 200000) {
                        lastKey = currentKey;
                        lastKeyTime = currentTime;
                        
                        switch(currentKey) {
                            case KEY_UP:
                                Menu_NavigateUp(currentMenu);
                                Menu_Render(currentMenu);
                                break;
                            case KEY_DOWN:
                                Menu_NavigateDown(currentMenu);
                                Menu_Render(currentMenu);
                                break;
                            case KEY_LEFT:
                                Menu_NavigateLeft(currentMenu);
                                Menu_Render(currentMenu);
                                break;
                            case KEY_RIGHT:
                                Menu_NavigateRight(currentMenu);
                                Menu_Render(currentMenu);
                                break;
                            case KEY_OK:
                                currentMenu = Menu_Select(currentMenu);
                                Menu_Render(currentMenu);
                                break;
                            case KEY_BACK:
                                currentMenu = Menu_Back(currentMenu);
                                Menu_Render(currentMenu);
                                break;
                            case KEY_CALL:
                                /* Show sliding options menu */
                                {
                                    int result = ShowOptionsMenu(currentMenu);
                                    if (result == 0) { /* OPT_OPEN */
                                        currentMenu = Menu_Select(currentMenu);
                                    }
                                    Menu_Render(currentMenu);
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            else
            {
                lastKey = KEY_NONE;
            }

            /* Poll PMU Status every 500ms (10 loops x 50ms) */
            static int pmuCounter = 0;
            if (++pmuCounter >= 10) {
                PMU_UpdateStatus();
                pmuCounter = 0;
            }

            USC_Pause_us(50000); // 50ms loop delay
        }

        return true;
    }
    while(0);

    return false;
}



