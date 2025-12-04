
/* ========== GAMES IMPLEMENTATIONS ========== */

static void Action_Tetris(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(70, 100, "TETRIS", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_Pong(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(80, 100, "PONG", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_TicTacToe(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 100, "TIC TAC TOE", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_MemoryGame(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 100, "MEMORY GAME", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

/* ========== SETTINGS IMPLEMENTATIONS ========== */

static void Action_BrightnessSettings(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(50, 100, "BRIGHTNESS", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_TimeSettings(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 100, "TIME SETTINGS", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_LanguageSettings(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(50, 100, "LANGUAGE", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_SoundSettings(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 100, "SOUND", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

/* ========== APPS IMPLEMENTATIONS ========== */

static void Action_Calculator(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 100, "CALCULATOR", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_Clock(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(70, 100, "CLOCK", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_Calendar(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 100, "CALENDAR", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_Notes(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(70, 100, "NOTES", 0xFFFF, 0x0000);
    LCD_DrawString(30, 140, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK", 0xFFE0, 0x0000);
    
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}
