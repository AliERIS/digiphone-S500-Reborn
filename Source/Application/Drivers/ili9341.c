// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
 * This file is part of the DZ09 project.
 *
 * Copyright (C) 2019 AJScorp
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
#include "appconfig.h"
#include "ili9341.h"
#include "rgu.h"

/* ST7789P3 Initialization Sequence - Clean & Robust */
boolean ILI9341_Initialize(void)
{
    /* 1. Software Reset */
    LCDIF_WriteCommand(ILI9341_SWRESET);
    RGU_RestartWDT();                                                                               // Kick WDT
    USC_Pause_us(50000);                                                                            // Wait 50ms

    /* 2. Sleep Out */
    LCDIF_WriteCommand(ILI9341_SLPOUT);
    RGU_RestartWDT();                                                                               // Kick WDT
    USC_Pause_us(120000);                                                                           // Wait 120ms

    /* 3. Pixel Format Set - 16-bit RGB565 */
    LCDIF_WriteCommand(ILI9341_PIXFMT);
    LCDIF_WriteData(0x55);                                                                          // 16-bit/pixel

    /* 4. Memory Data Access Control */
    /* MT6261D/ST7789P3: MY=1, MX=1 (Flip Vert & Horz), MV=0, ML=0, BGR=0, MH=0 */
    LCDIF_WriteCommand(ILI9341_MADCTL);
    LCDIF_WriteData(0xC0);                                                                          // MY=1, MX=1

    /* 5. Display Inversion ON */
    /* Fixes Black appearing as White */
    LCDIF_WriteCommand(ILI9341_INVON);

    /* 6. Normal Display Mode ON */
    LCDIF_WriteCommand(ILI9341_NORON);
    USC_Pause_us(10000);                                                                            // Wait 10ms

    /* 7. Display ON */
    LCDIF_WriteCommand(ILI9341_DISPON);
    RGU_RestartWDT();                                                                               // Kick WDT
    USC_Pause_us(50000);                                                                            // Wait 50ms

    return true;
}

void ILI9341_SleepLCD(void)
{
    LCDIF_WriteCommand(ILI9341_DISPOFF);
    USC_Pause_us(20000);
    LCDIF_WriteCommand(ILI9341_SLPIN);
    USC_Pause_us(120000);
}

void ILI9341_ResumeLCD(void)
{
    LCDIF_WriteCommand(ILI9341_SLPOUT);
    USC_Pause_us(120000);
    LCDIF_WriteCommand(ILI9341_DISPON);
    USC_Pause_us(20000);
}

uint32_t *ILI9341_SetOutputWindow(pRECT Rct, uint32_t *Count, uint32_t DataAttr, uint32_t CmdAttr)
{
    uint32_t *Data = NULL;

    if ((Rct != NULL) && (Count != NULL))
    {
        uint32_t i = 0;

        Data = malloc(ILI9341_SETWINCMDSIZE * sizeof(uint32_t));
        if (Data != NULL)
        {
            /* Column Address Set */
            Data[i++] = LCDIF_COMM(ILI9341_CASET) | CmdAttr;
            Data[i++] = LCDIF_COMM(CASSH(Rct->l)) | DataAttr;
            Data[i++] = LCDIF_COMM(CASSL(Rct->l)) | DataAttr;
            Data[i++] = LCDIF_COMM(CASEH(Rct->r)) | DataAttr;
            Data[i++] = LCDIF_COMM(CASEL(Rct->r)) | DataAttr;

            /* Row Address Set - Includes Row Shift if defined */
            Data[i++] = LCDIF_COMM(ILI9341_RASET) | CmdAttr;
            Data[i++] = LCDIF_COMM(RASSH(Rct->t + ILI9341_ROWSHIFT)) | DataAttr;
            Data[i++] = LCDIF_COMM(RASSL(Rct->t + ILI9341_ROWSHIFT)) | DataAttr;
            Data[i++] = LCDIF_COMM(RASEH(Rct->b + ILI9341_ROWSHIFT)) | DataAttr;
            Data[i++] = LCDIF_COMM(RASEL(Rct->b + ILI9341_ROWSHIFT)) | DataAttr;

            /* Memory Write */
            Data[i++] = LCDIF_COMM(ILI9341_RAMWR) | CmdAttr;

            *Count = i;
        }
    }
    return Data;
}
