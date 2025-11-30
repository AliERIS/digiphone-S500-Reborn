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
#ifndef _ILI9341_H_
#define _ILI9341_H_

/* ST7789P3 LCD Driver - Compatible with ILI9341 interface */
/* Default to 0 offset - adjust only if image is shifted */
#define ILI9341_ROWSHIFT            0
#define ILI9341_SETWINCMDSIZE       11

/* ST7789P3 Command Set */
#define ILI9341_NOP                 0x00
#define ILI9341_SWRESET             0x01
#define ILI9341_RDDID               0x04
#define ILI9341_RDDST               0x09
#define ILI9341_SLPIN               0x10
#define ILI9341_SLPOUT              0x11
#define ILI9341_PTLON               0x12
#define ILI9341_NORON               0x13
#define ILI9341_INVOFF              0x20
#define ILI9341_INVON               0x21
#define ILI9341_DISPOFF             0x28
#define ILI9341_DISPON              0x29
#define ILI9341_CASET               0x2A
#define CASSH(v)                    (((v) & 0xFF00) >> 8)
#define CASSL(v)                    ((v) & 0xFF)
#define CASEH(v)                    (((v) & 0xFF00) >> 8)
#define CASEL(v)                    ((v) & 0xFF)
#define ILI9341_RASET               0x2B
#define RASSH(v)                    (((v) & 0xFF00) >> 8)
#define RASSL(v)                    ((v) & 0xFF)
#define RASEH(v)                    (((v) & 0xFF00) >> 8)
#define RASEL(v)                    ((v) & 0xFF)
#define ILI9341_RAMWR               0x2C
#define ILI9341_MADCTL              0x36
#define MC_MH                       (1 << 2)
#define MC_RGB                      (0 << 3)
#define MC_BGR                      (1 << 3)
#define MC_ML                       (1 << 4)
#define MC_MV                       (1 << 5)
#define MC_MX                       (1 << 6)
#define MC_MY                       (1 << 7)
#define ILI9341_PIXFMT              0x3A
#define DBI(v)                      (((v) & 0x07) << 0)
#define DBI16bit                    0x05
#define DPI(v)                      (((v) & 0x07) << 4)
#define DPI16bit                    0x05

/* Function prototypes */
extern boolean ILI9341_Initialize(void);
extern void ILI9341_SleepLCD(void);
extern void ILI9341_ResumeLCD(void);
extern uint32_t *ILI9341_SetOutputWindow(pRECT Rct, uint32_t *Count, uint32_t DataAttr, uint32_t CmdAttr);

#endif /* _ILI9341_H_ */
