/*
 * This file is part of the DZ09 project.
 *
 * Copyright (C) 2022 AJScorp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */
#include "systemconfig.h"
#include "kp.h"

#include "pctl.h"

/* Backlight wake function */
extern void KeepBacklightOn(void);

/* Key mapping table */
static const TKEY KeyMap[] = {
    KEY_NONE,   // 0x00
    KEY_1,      // 0x01
    KEY_2,      // 0x02
    KEY_3,      // 0x03
    KEY_4,      // 0x04
    KEY_5,      // 0x05
    KEY_6,      // 0x06
    KEY_7,      // 0x07
    KEY_8,      // 0x08
    KEY_9,      // 0x09
    KEY_STAR,   // 0x0A
    KEY_0,      // 0x0B
    KEY_HASH,   // 0x0C
    KEY_UP,     // 0x0D
    KEY_DOWN,   // 0x0E
    KEY_LEFT,   // 0x0F
    KEY_RIGHT,  // 0x10
    KEY_OK,     // 0x11
    KEY_CALL,   // 0x12
    KEY_END,    // 0x13
    KEY_BACK    // 0x14
};

/* Software Keypad Scanning Implementation */
/* Rows: GPIO18-21 (Output) */
/* Cols: GPIO12-16 (Input with Pull-up) */

/* Row Pins */
#define KROW0 GPIO21
#define KROW1 GPIO20
#define KROW2 GPIO19
#define KROW3 GPIO18

/* Col Pins */
#define KCOL0 GPIO16
#define KCOL1 GPIO15
#define KCOL2 GPIO14
#define KCOL3 GPIO13
#define KCOL4 GPIO12

void KP_Initialize(void)
{
    /* Disable Hardware Keypad Controller to avoid conflict */
    KP_EN = 0;

    /* Enable ALL Peripheral Clocks (Shotgun approach to fix Keypad Clock) */
    /* BUT Disable UART1/2 to prevent GPIO conflict with Keypad Cols */
    CNFG_PDN_CON0_CLR = 0xFFFF;
    CNFG_PDN_CON1_CLR = 0xFFFF;
    CNFG_PDN_CON2_CLR = 0xFFFF;
    
    PCTL_PowerDown(PD_UART1); // Critical: GPIO14/15 are UART1
    PCTL_PowerDown(PD_UART2); // Critical: GPIO12/17 are UART2

    /* Configure Rows as Outputs (Default High) */
    /* Set Mode to GPIO (0) */
    uint32_t mode2 = GPIO_MODE2;
    /* Clear 16 (0-3), 18 (8-11), 19 (12-15), 20 (16-19), 21 (20-23) */
    /* Mask: 00FF FF0F */
    mode2 &= ~(0x00FFFF0F);
    GPIO_MODE2 = mode2; // Set to Mode 0 (IO)

    uint32_t mode1 = GPIO_MODE1;
    mode1 &= ~(0xFFFF0000); // Clear 12-15
    GPIO_MODE1 = mode1; // Set to Mode 0 (IO)

    /* Set Rows to Output */
    GPIO_SETDIROUT(KROW0);
    GPIO_SETDIROUT(KROW1);
    GPIO_SETDIROUT(KROW2);
    GPIO_SETDIROUT(KROW3);

    /* Set Rows High (Inactive) */
    GPIO_DATAOUT(KROW0, 1);
    GPIO_DATAOUT(KROW1, 1);
    GPIO_DATAOUT(KROW2, 1);
    GPIO_DATAOUT(KROW3, 1);

    /* Configure Cols as Inputs with Pull-ups */
    GPIO_SETDIRIN(KCOL0);
    GPIO_SETDIRIN(KCOL1);
    GPIO_SETDIRIN(KCOL2);
    GPIO_SETDIRIN(KCOL3);
    GPIO_SETDIRIN(KCOL4);

    /* CRITICAL: Enable Input Buffers (IES) */
    /* Without this, pins might always read 0! */
    GPIO_SETINPUTEN(KCOL0);
    GPIO_SETINPUTEN(KCOL1);
    GPIO_SETINPUTEN(KCOL2);
    GPIO_SETINPUTEN(KCOL3);
    GPIO_SETINPUTEN(KCOL4);

    /* Enable Pull-ups */
    GPIO_SETPULLUP(KCOL0);
    GPIO_SETPULLUP(KCOL1);
    GPIO_SETPULLUP(KCOL2);
    GPIO_SETPULLUP(KCOL3);
    GPIO_SETPULLUP(KCOL4);

    DebugPrint("Keypad initialized (Software Scan Mode)\r\n");
    
    /* PIN TEST: Verify if pins can be driven High/Low */
    /* This helps identify stuck pins or shorts */
    
    uint32_t pins[] = {KROW0, KROW1, KROW2, KROW3, KCOL0, KCOL1, KCOL2, KCOL3, KCOL4};
    const char* names[] = {"R0", "R1", "R2", "R3", "C0", "C1", "C2", "C3", "C4"};
    
    DebugPrint("--- PIN TEST START ---\r\n");
    for(int i=0; i<9; i++) {
        uint32_t p = pins[i];
        
        /* Set to Output */
        GPIO_SETDIROUT(p);
        
        /* Drive Low */
        GPIO_DATAOUT(p, 0);
        for(volatile int k=0; k<1000; k++);
        boolean valLow = GPIO_DATAIN(p);
        
        /* Drive High */
        GPIO_DATAOUT(p, 1);
        for(volatile int k=0; k<1000; k++);
        boolean valHigh = GPIO_DATAIN(p);
        
        DebugPrint("Pin %s (GPIO%d): Write 0->Read %d, Write 1->Read %d\r\n", 
            names[i], p, valLow, valHigh);
            
        /* Restore to default (Rows High, Cols Input) */
        if (i < 4) { // Rows
             GPIO_SETDIROUT(p);
             GPIO_DATAOUT(p, 1);
        } else { // Cols
             GPIO_SETDIRIN(p);
             GPIO_SETPULLUP(p);
        }
    }
    DebugPrint("--- PIN TEST END ---\r\n");
}

/* Helper to read a specific row */
static uint8_t ScanRow(uint8_t rowPin) {
    /* Drive Row Low */
    GPIO_DATAOUT(rowPin, 0);
    
    /* Small delay for signal settling */
    for(volatile int i=0; i<100; i++);

    /* Read Cols */
    uint8_t colState = 0;
    if (!GPIO_DATAIN(KCOL0)) colState |= (1<<0);
    if (!GPIO_DATAIN(KCOL1)) colState |= (1<<1);
    if (!GPIO_DATAIN(KCOL2)) colState |= (1<<2);
    if (!GPIO_DATAIN(KCOL3)) colState |= (1<<3);
    if (!GPIO_DATAIN(KCOL4)) colState |= (1<<4);

    /* Drive Row High again */
    GPIO_DATAOUT(rowPin, 1);

    return colState;
}

static TKEY LastDetectedKey = KEY_NONE;

boolean KP_IsKeyPressed(void)
{
    /* Quick scan to see if any key is pressed */
    /* Drive all rows low */
    GPIO_DATAOUT(KROW0, 0);
    GPIO_DATAOUT(KROW1, 0);
    GPIO_DATAOUT(KROW2, 0);
    GPIO_DATAOUT(KROW3, 0);
    
    for(volatile int i=0; i<100; i++);

    boolean pressed = false;
    if (!GPIO_DATAIN(KCOL0) || !GPIO_DATAIN(KCOL1) || !GPIO_DATAIN(KCOL2) || !GPIO_DATAIN(KCOL3) || !GPIO_DATAIN(KCOL4)) {
        pressed = true;
        KeepBacklightOn();  /* Wake backlight on any key press */
    }

    /* Restore Rows High */
    GPIO_DATAOUT(KROW0, 1);
    GPIO_DATAOUT(KROW1, 1);
    GPIO_DATAOUT(KROW2, 1);
    GPIO_DATAOUT(KROW3, 1);

    return pressed;
}

TKEY KP_ReadKey(void)
{
    /* Hardware-Specific Signature Matching */
    /* Each key has a unique matrix signature due to hardware quirks */
    
    uint32_t matrix = KP_GetMatrixState();
    
    /* Define base pattern (no keys pressed) */
    const uint32_t BASE_PATTERN = 0xCE739;
    
    /* Key Signatures - Direct matrix values when each key is pressed */
    typedef struct {
        uint32_t signature;  // Exact matrix value for this key
        TKEY key;
    } KeySignature;
    
    const KeySignature signatures[] = {
        /* Number Keys */
        { 0xCE318, KEY_1 },
        { 0x8E631, KEY_2 },
        { 0xCE73D, KEY_3 },
        { 0xC6319, KEY_4 },
        { 0xDE739, KEY_5 },
        { 0xEE739, KEY_6 },
        { 0xC6718, KEY_7 },
        { 0xCEF39, KEY_8 },
        { 0xCF739, KEY_9 },
        { 0xC6338, KEY_STAR },
        { 0xCE779, KEY_0 },
        { 0xCE7B9, KEY_HASH },
        
        /* Navigation Keys */
        { 0xCA529, KEY_UP },
        { 0x8C731, KEY_DOWN },
        { 0x8C639, KEY_RIGHT },
        { 0x4A729, KEY_LEFT },
        { 0xCC631, KEY_OK },
        
        /* Function Keys */
        { 0x4A539, KEY_CALL },
        { 0xCE73B, KEY_BACK },
        { 0x4E529, KEY_END },  // OPTIONS button
    };
    
    /* Check if no key is pressed */
    if (matrix == BASE_PATTERN) {
        return KEY_NONE;
    }
    
    /* Match against known signatures */
    for (int i = 0; i < sizeof(signatures)/sizeof(signatures[0]); i++) {
        if (matrix == signatures[i].signature) {
            KeepBacklightOn();  /* Wake backlight on any key press */
            return signatures[i].key;
        }
    }
    
    /* Unknown key pattern */
    return KEY_NONE;
}

/* Returns 20-bit state: Row0(0-4), Row1(5-9), Row2(10-14), Row3(15-19) */
uint32_t KP_GetMatrixState(void)
{
    uint32_t state = 0;
    
    /* Row 0 */
    uint8_t cols = ScanRow(KROW0);
    state |= (cols & 0x1F) << 0;
    
    /* Row 1 */
    cols = ScanRow(KROW1);
    state |= (cols & 0x1F) << 5;
    
    /* Row 2 */
    cols = ScanRow(KROW2);
    state |= (cols & 0x1F) << 10;
    
    /* Row 3 */
    cols = ScanRow(KROW3);
    state |= (cols & 0x1F) << 15;
    
    return state;
}

void KP_ClearKeyBuffer(void)
{
    /* No buffer in software scan */
}
