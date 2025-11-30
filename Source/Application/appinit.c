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

/* 5x7 ASCII Font (Minimal Set for "Hello DigiPhone S500 MT6261DA") */
/* 5x7 ASCII Font (Corrected GLCD Values) */
static const uint8_t Font5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // 0: Space
    0x7F, 0x41, 0x41, 0x22, 0x1C, // 1: D
    0x00, 0x44, 0x7D, 0x40, 0x00, // 2: i
    0x08, 0x54, 0x54, 0x54, 0x3C, // 3: g
    0x7F, 0x08, 0x08, 0x08, 0x7F, // 4: H
    0x38, 0x54, 0x54, 0x54, 0x18, // 5: e
    0x00, 0x41, 0x7F, 0x40, 0x00, // 6: l
    0x38, 0x44, 0x44, 0x44, 0x38, // 7: o
    0x7F, 0x09, 0x09, 0x09, 0x06, // 8: P
    0x7F, 0x08, 0x04, 0x04, 0x78, // 9: h
    0x7C, 0x08, 0x04, 0x04, 0x78, // 10: n
    0x48, 0x54, 0x54, 0x54, 0x20, // 11: S
    0x27, 0x45, 0x45, 0x45, 0x39, // 12: 5
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 13: 0
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // 14: M
    0x01, 0x01, 0x7F, 0x01, 0x01, // 15: T
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 16: 6
    0x42, 0x61, 0x51, 0x49, 0x46, // 17: 2
    0x00, 0x42, 0x7F, 0x40, 0x00, // 18: 1
    0x7E, 0x11, 0x11, 0x11, 0x7E, // 19: A
};

void LCD_DrawPixel(uint16_t color) {
    LCDIF_WriteData(color >> 8);
    LCDIF_WriteData(color & 0xFF);
}

void LCD_DrawChar(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t color, uint16_t bg) {
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

boolean APP_Initialize(void)
{
    pGUIOBJECT MainLayer;

    /* CRITICAL: Enable RTC PowerUp to generate PWRBB signal */
    /* This prevents device shutdown by keeping PMU powered */
    /* MUST UNPROTECT RTC REGISTERS FIRST! */
    RTC_Unprotect();
    RTC_EnablePowerUp();
    
    /* VERIFY: Did the write succeed? */
    uint16_t bbpu_val = RTC_BBPU;
    DebugPrint("APP_Initialize: RTC_BBPU Readback = 0x%X\r\n", bbpu_val);
    
    if (bbpu_val & PWREN) {
        DebugPrint("SUCCESS: PWRBB bit is SET!\r\n");
    } else {
        DebugPrint("FAILURE: PWRBB bit is NOT SET! Protection active?\r\n");
    }
    
    RGU_RestartWDT();

    do
    {
        if (!GUI_Initialize()) break;
        RGU_RestartWDT();

        /* BARE METAL TEXT DRAWING */
        
        /* 1. Clear Screen to BLUE */
        LCDIF_WriteCommand(0x2A);
        LCDIF_WriteData(0); LCDIF_WriteData(0); LCDIF_WriteData(0); LCDIF_WriteData((uint8_t)0xEF);
        LCDIF_WriteCommand(0x2B);
        LCDIF_WriteData(0); LCDIF_WriteData(0); LCDIF_WriteData(0x01); LCDIF_WriteData(0x3F);
        LCDIF_WriteCommand(0x2C);
        
        for(int i=0; i<240*320; i++) {
            LCD_DrawPixel(clBlue);
            if((i%1000)==0) RGU_RestartWDT();
        }

        DebugPrint("Screen Cleared. Drawing Text...\r\n");

        /* 2. Draw "Hello DigiPhone" */
        int cx = 20, cy = 100;
        const uint8_t *chars[] = {
            &Font5x7[4*5], // H
            &Font5x7[5*5], // e
            &Font5x7[6*5], // l
            &Font5x7[6*5], // l
            &Font5x7[7*5], // o
            &Font5x7[0*5], // Space
            &Font5x7[1*5], // D
            &Font5x7[2*5], // i
            &Font5x7[3*5], // g
            &Font5x7[2*5], // i
            &Font5x7[8*5], // P
            &Font5x7[9*5], // h
            &Font5x7[7*5], // o
            &Font5x7[10*5],// n
            &Font5x7[5*5]  // e
        };
        
        for(int i=0; i<15; i++) {
            LCD_DrawChar(cx, cy, chars[i], clWhite, clBlue);
            cx += 6;
        }
        
        /* 3. Draw "S500 MT6261DA" */
        cx = 20; cy += 10;
        const uint8_t *chars2[] = {
            &Font5x7[11*5], // S
            &Font5x7[12*5], // 5
            &Font5x7[13*5], // 0
            &Font5x7[13*5], // 0
            &Font5x7[0*5],  // Space
            &Font5x7[14*5], // M
            &Font5x7[15*5], // T
            &Font5x7[16*5], // 6
            &Font5x7[17*5], // 2
            &Font5x7[16*5], // 6
            &Font5x7[18*5], // 1
            &Font5x7[1*5],  // D
            &Font5x7[19*5]  // A
        };
        
        for(int i=0; i<13; i++) {
            LCD_DrawChar(cx, cy, chars2[i], clWhite, clBlue);
            cx += 6;
        }

        RGU_RestartWDT();

        /* Turn on backlight */
        BL_TurnOn(true);
        RGU_RestartWDT();

        /* Initialize and enable USB */
        USB_Initialize();
        USB_EnableDevice();

        DebugPrint("Speaker Test initialized. Beeping once.\r\n");

        /* Single Startup Beep */
        AFE_TurnOnSpeaker(true);
        RGU_RestartWDT();
        Beep();
        RGU_RestartWDT();
        AFE_TurnOnSpeaker(false);
        RGU_RestartWDT();

        DebugPrint("Initialization Complete. Entering Idle Loop.\r\n");

        /* Idle Loop */
        while(1)
        {
            /* Feed Watchdog to prevent reset */
            RGU_RestartWDT();
            
            /* TODO: Implement Power Hold logic here if needed */
            /* For now, just wait */
            USC_Pause_us(100000); // 100ms
        }

        return true;
    }
    while(0);

    DebugPrint("APP_Initialize failed!\r\n");
    return false;
}
