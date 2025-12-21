/*
 * Simple Menu System Implementation
 */
#include "systemconfig.h"
#include "menu.h"
#include "lcdif.h"
#include "fat32.h"
#include "memory_browser.h"
#include "ram_files.h"
#include "resources_viewer.h"
#include "advanced_info.h"
#include "afe.h"
#include "bluetooth.h"
#include "pmu.h"
#include "lrtimer.h"
#include "dlist.h"
#include "sdcard.h"
#include "msdc.h"

extern void KeepBacklightOn(void);

/* Forward declarations for menu actions */
static void Action_Settings(void);
static void Action_Games(void);
static void Action_BacklightSettings(void);
static void Action_SystemInfo(void);
static void Action_About(void);
static void Action_Snake(void);
static void Action_Racing(void);
static void Action_FileManager(void);
/* New game actions */
static void Action_Tetris(void);
static void Action_Pong(void);
static void Action_TicTacToe(void);
static void Action_MemoryGame(void);
/* Settings actions */
static void Action_BrightnessSettings(void);
static void Action_TimeSettings(void);
static void Action_LanguageSettings(void);
static void Action_LanguageSettings(void);
static void Action_SoundSettings(void);
static void Action_Bluetooth(void);
static void Action_AboutSystem(void);
/* Phone apps */
static void Action_Calculator(void);
static void Action_Clock(void);
static void Action_Calendar(void);
static void Action_Notes(void);
static void Action_BeepTest(void);
static void Action_Terminal(void);
static void Action_Flashlight(void);
static void Action_Flashlight(void);
static void Action_TaskManager(void);
static void Action_SDCardTest(void);
static void Action_3DCube(void);
static void Action_MSDCRegs(void);
static void Action_3DMaze(void);
static void Action_Platformer(void);
static void Action_Freekick(void);

/* Tools Submenu */
static Menu settingsMenu;
static Menu toolsMenu;
static Menu appsMenu;
static MenuItem toolsItems[] = {
    { "BACKLIGHT", MENU_ACTION_FUNCTION, .action.function = Action_BacklightSettings },
    { "SYSTEM INFO", MENU_ACTION_FUNCTION, .action.function = Action_SystemInfo },
    { "FILE MANAGER", MENU_ACTION_FUNCTION, .action.function = Action_FileManager },
    { "MEMORY BROWSER", MENU_ACTION_FUNCTION, .action.function = Action_MemoryBrowser },
    { "RAM FILES", MENU_ACTION_FUNCTION, .action.function = Action_RAMFiles },
    { "RESOURCES", MENU_ACTION_FUNCTION, .action.function = Action_Resources },
    { "ADVANCED INFO", MENU_ACTION_FUNCTION, .action.function = Action_AdvancedInfo },
    { "BEEP TEST", MENU_ACTION_FUNCTION, .action.function = Action_BeepTest },
    { "TERMINAL", MENU_ACTION_FUNCTION, .action.function = Action_Terminal },
    { "FLASHLIGHT", MENU_ACTION_FUNCTION, .action.function = Action_Flashlight },
    { "TASK MANAGER", MENU_ACTION_FUNCTION, .action.function = Action_TaskManager },
    { "SD CARD TEST", MENU_ACTION_FUNCTION, .action.function = Action_SDCardTest },
    { "3D CUBE", MENU_ACTION_FUNCTION, .action.function = Action_3DCube },
    { "MSDC REGS", MENU_ACTION_FUNCTION, .action.function = Action_MSDCRegs },
    { "3D MAZE", MENU_ACTION_FUNCTION, .action.function = Action_3DMaze },
    { "PLATFORMER", MENU_ACTION_FUNCTION, .action.function = Action_Platformer },
    { "FREEKICK", MENU_ACTION_FUNCTION, .action.function = Action_Freekick },
};

static Menu toolsMenu = {
    .title = "Tools",
    .items = toolsItems,
    .itemCount = 17,
    .selectedIndex = 0,
    .parent = NULL
};

/* Forward declarations for new apps */
static void Action_EBookReader(void);
static void Action_Browser(void);
static void Action_Contacts(void);
static void Action_CallLog(void);

/* Apps Submenu */
static MenuItem appsItems[] = {
    { "CALCULATOR", MENU_ACTION_FUNCTION, .action.function = Action_Calculator },
    { "CLOCK", MENU_ACTION_FUNCTION, .action.function = Action_Clock },
    { "CALENDAR", MENU_ACTION_FUNCTION, .action.function = Action_Calendar },
    { "NOTES", MENU_ACTION_FUNCTION, .action.function = Action_Notes },
    { "eBOOK", MENU_ACTION_FUNCTION, .action.function = Action_EBookReader },
    { "BROWSER", MENU_ACTION_FUNCTION, .action.function = Action_Browser },
    { "CONTACTS", MENU_ACTION_FUNCTION, .action.function = Action_Contacts },
    { "CALL LOG", MENU_ACTION_FUNCTION, .action.function = Action_CallLog },
};

static Menu appsMenu = {
    .title = "APPS",
    .items = appsItems,
    .itemCount = 8,
    .selectedIndex = 0,
    .parent = NULL
};

static void Action_Breakout(void);
static void Action_SpaceInvaders(void);
static void Action_Dungeon(void);
static void Action_FlappyBird(void);

/* Games Submenu */
static MenuItem gamesItems[] = {
    { "SNAKE", MENU_ACTION_FUNCTION, .action.function = Action_Snake },
    { "RACING", MENU_ACTION_FUNCTION, .action.function = Action_Racing },
    { "TETRIS", MENU_ACTION_FUNCTION, .action.function = Action_Tetris },
    { "PONG", MENU_ACTION_FUNCTION, .action.function = Action_Pong },
    { "TIC TAC TOE", MENU_ACTION_FUNCTION, .action.function = Action_TicTacToe },
    { "MEMORY GAME", MENU_ACTION_FUNCTION, .action.function = Action_MemoryGame },
    { "FLAPPY BIRD", MENU_ACTION_FUNCTION, .action.function = Action_FlappyBird },
    { "BREAKOUT", MENU_ACTION_FUNCTION, .action.function = Action_Breakout },
    { "SPACE INVADERS", MENU_ACTION_FUNCTION, .action.function = Action_SpaceInvaders },
    { "DUNGEON", MENU_ACTION_FUNCTION, .action.function = Action_Dungeon },
};

static Menu gamesMenu = {
    .title = "GAMES",
    .items = gamesItems,
    .itemCount = 10,
    .selectedIndex = 0,
    .parent = NULL
};



/* Settings Submenu */
static MenuItem settingsItems[] = {
    { "BACKLIGHT", MENU_ACTION_FUNCTION, .action.function = Action_BacklightSettings },
    { "BLUETOOTH", MENU_ACTION_FUNCTION, .action.function = Action_Bluetooth },
    { "TIME DATE", MENU_ACTION_FUNCTION, .action.function = Action_TimeSettings },
    { "SOUND", MENU_ACTION_FUNCTION, .action.function = Action_SoundSettings },
    { "ROI", MENU_ACTION_FUNCTION, .action.function = Action_SystemInfo },
};

static Menu settingsMenu = {
    .title = "SETTINGS",
    .items = settingsItems,
    .itemCount = 5,
    .selectedIndex = 0,
    .parent = NULL
};

static MenuItem mainItems[] = {
    { "APPS", MENU_ACTION_SUBMENU, .action.submenu = &appsMenu },
    { "GAMES", MENU_ACTION_SUBMENU, .action.submenu = &gamesMenu },
    { "SETTINGS", MENU_ACTION_SUBMENU, .action.submenu = &settingsMenu },
    { "TOOLS", MENU_ACTION_SUBMENU, .action.submenu = &toolsMenu },
    { "ABOUT", MENU_ACTION_FUNCTION, .action.function = Action_About },
};

static Menu mainMenu = {
    .title = "MAIN MENU",
    .items = mainItems,
    .itemCount = 5,
    .selectedIndex = 0,
    .parent = NULL
};

/* External drawing functions from appinit.c */
extern void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg);

void Menu_Initialize(void)
{
    /* Set parent pointers */
    appsMenu.parent = &mainMenu;
    gamesMenu.parent = &mainMenu;
    settingsMenu.parent = &mainMenu;
    toolsMenu.parent = &mainMenu;
}

#include "rtc.h"
#include "pmu.h"
#include <stdio.h>

void Menu_InvalidateCache(void);

/* Render cache variables */
static uint8_t g_lastSelectedIndex = 255;
static Menu *g_lastMenu = NULL;

/* Helper to draw status bar */
/* Helper to draw status bar */
static void DrawStatusBar(void)
{
    /* Background */
    LCD_FillRect(0, 0, 240, 20, 0x18E3); // Dark Gray

    /* Time */
    TDATETIME dt = RTC_GetDateTime();
    char timeBuf[10];
    sprintf(timeBuf, "%02d:%02d", dt.Time.Hour, dt.Time.Min);
    LCD_DrawString(5, 2, timeBuf, 0xFFFF, 0x18E3);

    /* Battery - Cached to avoid slow ADC reads on every frame */
    static uint32_t lastBatCheck = 0;
    static uint16_t cachedBatLevel = 0;
    static boolean cachedCharging = false;
    
    uint32_t currentTicks = USC_GetCurrentTicks();
    
    if (lastBatCheck == 0 || (currentTicks - lastBatCheck) > 2000000) { // Update every 2 seconds
        uint16_t batVal = PMU_GetBatteryChargeLevel(); // Slow ADC read
        cachedBatLevel = batVal / 10; // 0-1000 -> 0-100
        if (cachedBatLevel > 100) cachedBatLevel = 100;
        
        cachedCharging = PMU_IsBatteryCharging();
        lastBatCheck = currentTicks;
    }

    char batBuf[16];
    uint16_t batColor = 0xFFFF; // White

    if (cachedCharging) {
        sprintf(batBuf, "CHG %d%%", cachedBatLevel);
        batColor = 0x07E0; // Green
        LCD_DrawString(150, 2, batBuf, batColor, 0x18E3);
    } else {
        sprintf(batBuf, "%d%%", cachedBatLevel);
        if (cachedBatLevel < 20) batColor = 0xF800; // Red
        else batColor = 0xFFFF;
        LCD_DrawString(200, 2, batBuf, batColor, 0x18E3);
    }
}

/* Helper to draw hollow rectangle */
static void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    LCD_FillRect(x, y, w, 1, color);         // Top
    LCD_FillRect(x, y + h - 1, w, 1, color); // Bottom
    LCD_FillRect(x, y, 1, h, color);         // Left
    LCD_FillRect(x + w - 1, y, 1, h, color); // Right
}


#define STATUS_BAR_HEIGHT 20
#define GRID_COLS 3
#define GRID_ROWS 4
#define ITEMS_PER_PAGE (GRID_COLS * GRID_ROWS)
#define CELL_WIDTH  (240 / GRID_COLS)
#define ACTION_BAR_HEIGHT 20
#define CELL_HEIGHT ((320 - STATUS_BAR_HEIGHT - ACTION_BAR_HEIGHT) / GRID_ROWS)

/* Global scroll offset */
static uint16_t g_scrollOffset = 0;

/* Helper to draw bottom action bar */
static void DrawActionBar(void)
{
    LCD_FillRect(0, 300, 240, 20, 0x18E3); // Dark gray background
    LCD_DrawString(10, 304, "OPTIONS", 0xFFFF, 0x18E3);
    LCD_DrawString(180, 304, "BACK", 0xFFFF, 0x18E3);
}

void Menu_Render(Menu *menu)
{
    if (!menu) return;

    /* Calculate new scroll offset */
    uint16_t newScrollOffset = g_scrollOffset;
    if (menu->selectedIndex < newScrollOffset) {
        newScrollOffset = (menu->selectedIndex / GRID_COLS) * GRID_COLS;
    }
    else if (menu->selectedIndex >= newScrollOffset + ITEMS_PER_PAGE) {
        newScrollOffset = ((menu->selectedIndex / GRID_COLS) - (GRID_ROWS - 1)) * GRID_COLS;
    }

    /* Check if full redraw is needed */
    boolean fullRedraw = (g_lastMenu != menu) || (g_scrollOffset != newScrollOffset);
    g_scrollOffset = newScrollOffset;

    if (fullRedraw) {
        LCD_FillRect(0, STATUS_BAR_HEIGHT, 240, 320 - STATUS_BAR_HEIGHT - ACTION_BAR_HEIGHT, 0x0000);
        DrawStatusBar();
        DrawActionBar();
    }

    /* Always update status bar if enough time passed? For now just on interaction */
    // DrawStatusBar();

    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        int itemIndex = g_scrollOffset + i;
        if (itemIndex >= menu->itemCount) break;

        /* Optimization: Only redraw if full redraw OR this item changed state */
        boolean isSelected = (itemIndex == menu->selectedIndex);
        boolean wasSelected = (itemIndex == g_lastSelectedIndex);

        if (!fullRedraw && !isSelected && !wasSelected) {
            continue; // Skip unchanged items
        }

        int col = i % GRID_COLS;
        int row = i / GRID_COLS;

        uint16_t x = col * CELL_WIDTH;
        uint16_t y = STATUS_BAR_HEIGHT + (row * CELL_HEIGHT);

        /* Clear cell background if changing selection state */
        if (!fullRedraw) {
             LCD_FillRect(x, y, CELL_WIDTH, CELL_HEIGHT, 0x0000);
        }

        /* Draw Selection Highlight */
        if (isSelected) {
            LCD_FillRect(x + 2, y + 2, CELL_WIDTH - 4, CELL_HEIGHT - 4, 0x001F); // Blue highlight
            LCD_DrawRect(x + 2, y + 2, CELL_WIDTH - 4, CELL_HEIGHT - 4, 0xFFFF); // White border
        } else {
            LCD_DrawRect(x + 5, y + 5, CELL_WIDTH - 10, CELL_HEIGHT - 10, 0x18E3); // Dim gray border
        }

        /* Draw Item Text (Centered) */
        const char *title = menu->items[itemIndex].title;
        int len = 0; while(title[len]) len++;
        int textX = x + (CELL_WIDTH - (len * 8)) / 2;
        if (textX < x) textX = x + 2;

        int textY = y + (CELL_HEIGHT / 2) - 8;

        uint16_t fgColor = isSelected ? 0xFFFF : 0x7BEF;
        uint16_t bgColor = isSelected ? 0x001F : 0x0000;

        LCD_DrawString(textX, textY, title, fgColor, bgColor);
    }

    /* Draw Scrollbar if needed */
    if (menu->itemCount > ITEMS_PER_PAGE) {
        int barHeight = 320 * ITEMS_PER_PAGE / menu->itemCount;
        int barY = 320 * g_scrollOffset / menu->itemCount;
        LCD_FillRect(236, 0, 4, 320, 0x0000); // Clear scrollbar area
        LCD_FillRect(236, barY, 4, barHeight, 0x07E0); // Green scrollbar
    }

    g_lastMenu = menu;
    g_lastSelectedIndex = menu->selectedIndex;
}

void Menu_InvalidateCache(void)
{
    g_scrollOffset = 0;
    g_lastMenu = NULL;
    g_lastSelectedIndex = 255;
}

/* Sliding Options Menu */
#define OPT_OPEN 0
#define OPT_MOVE 1
#define OPT_DELETE 2
#define OPT_RENAME 3
#define OPT_COUNT 4

static const char* optionLabels[] = {"OPEN", "MOVE", "DELETE", "RENAME"};

int ShowOptionsMenu(Menu *menu)
{
    if (!menu || menu->itemCount == 0) return -1;

    int optSelected = 0;
    int menuHeight = 100;
    int menuY = 320; // Start off-screen
    int targetY = 220;

    /* Slide animation */
    while (menuY > targetY) {
        RGU_RestartWDT();
        menuY -= 20;
        if (menuY < targetY) menuY = targetY;

        /* Draw popup background */
        LCD_FillRect(0, menuY, 240, menuHeight, 0x2104);
        LCD_FillRect(0, menuY, 240, 2, 0x07E0); // Green top border

        /* Draw title */
        LCD_DrawString(90, menuY + 5, "OPTIONS", 0x07E0, 0x2104);

        /* Draw options */
        for(int i=0; i<OPT_COUNT; i++) {
            uint16_t fg = (i == optSelected) ? 0x0000 : 0xFFFF;
            uint16_t bg = (i == optSelected) ? 0x07E0 : 0x2104;
            LCD_FillRect(10, menuY + 25 + i*18, 220, 16, bg);
            LCD_DrawString(20, menuY + 27 + i*18, optionLabels[i], fg, bg);
        }

        USC_Pause_us(20000);
    }

    /* Handle selection */
    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();

        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();

            if (key == KEY_BACK || key == KEY_CALL) {
                /* Slide out animation */
                while (menuY < 320) {
                    RGU_RestartWDT();
                    menuY += 20;
                    LCD_FillRect(0, menuY - 20, 240, 20, 0x0000);
                    if (menuY < 320) {
                        LCD_FillRect(0, menuY, 240, menuHeight, 0x2104);
                    }
                    USC_Pause_us(20000);
                }
                Menu_InvalidateCache();
                return -1;
            }

            if (key == KEY_UP && optSelected > 0) {
                /* Redraw old selection */
                LCD_FillRect(10, menuY + 25 + optSelected*18, 220, 16, 0x2104);
                LCD_DrawString(20, menuY + 27 + optSelected*18, optionLabels[optSelected], 0xFFFF, 0x2104);
                optSelected--;
                /* Draw new selection */
                LCD_FillRect(10, menuY + 25 + optSelected*18, 220, 16, 0x07E0);
                LCD_DrawString(20, menuY + 27 + optSelected*18, optionLabels[optSelected], 0x0000, 0x07E0);
            }

            if (key == KEY_DOWN && optSelected < OPT_COUNT - 1) {
                LCD_FillRect(10, menuY + 25 + optSelected*18, 220, 16, 0x2104);
                LCD_DrawString(20, menuY + 27 + optSelected*18, optionLabels[optSelected], 0xFFFF, 0x2104);
                optSelected++;
                LCD_FillRect(10, menuY + 25 + optSelected*18, 220, 16, 0x07E0);
                LCD_DrawString(20, menuY + 27 + optSelected*18, optionLabels[optSelected], 0x0000, 0x07E0);
            }

            if (key == KEY_OK) {
                /* Execute option */
                int selectedItem = menu->selectedIndex;
                const char* itemName = menu->items[selectedItem].title;

                switch(optSelected) {
                    case OPT_OPEN:
                        /* Slide out and let menu handle open */
                        while (menuY < 320) { RGU_RestartWDT(); menuY += 30; USC_Pause_us(10000); }
                        Menu_InvalidateCache();
                        return OPT_OPEN;

                    case OPT_MOVE:
                        LCD_FillRect(30, 140, 180, 40, 0x001F);
                        LCD_DrawString(50, 155, "MOVE: Not Yet", 0xFFFF, 0x001F);
                        USC_Pause_us(1500000);
                        break;

                    case OPT_DELETE:
                        LCD_FillRect(30, 140, 180, 40, 0xF800);
                        LCD_DrawString(40, 150, "DELETE:", 0xFFFF, 0xF800);
                        LCD_DrawString(40, 165, itemName, 0xFFE0, 0xF800);
                        USC_Pause_us(2000000);
                        break;

                    case OPT_RENAME:
                        LCD_FillRect(30, 140, 180, 40, 0x07E0);
                        LCD_DrawString(40, 155, "RENAME: Not Yet", 0x0000, 0x07E0);
                        USC_Pause_us(1500000);
                        break;
                }

                /* Redraw menu after action */
                while (menuY < 320) { RGU_RestartWDT(); menuY += 30; USC_Pause_us(10000); }
                Menu_InvalidateCache();
                return -1;
            }

            USC_Pause_us(150000);
        }

        USC_Pause_us(30000);
    }
}

void Menu_NavigateUp(Menu *menu)
{
    if (menu->selectedIndex >= GRID_COLS) {
        menu->selectedIndex -= GRID_COLS;
    } else {
        /* Wrap to bottom same column? Or just stop? Symbian stops or wraps to bottom */
        /* Let's wrap to last row same column if possible */
        int lastRowStart = ((menu->itemCount - 1) / GRID_COLS) * GRID_COLS;
        int target = lastRowStart + (menu->selectedIndex % GRID_COLS);
        if (target >= menu->itemCount) target -= GRID_COLS;
        menu->selectedIndex = target;
    }
}

void Menu_NavigateDown(Menu *menu)
{
    if (menu->selectedIndex + GRID_COLS < menu->itemCount) {
        menu->selectedIndex += GRID_COLS;
    } else {
        /* Wrap to top same column */
        menu->selectedIndex = menu->selectedIndex % GRID_COLS;
    }
}

void Menu_NavigateLeft(Menu *menu)
{
    if (menu->selectedIndex > 0) {
        menu->selectedIndex--;
    } else {
        menu->selectedIndex = menu->itemCount - 1; // Wrap to end
    }
}

void Menu_NavigateRight(Menu *menu)
{
    if (menu->selectedIndex < menu->itemCount - 1) {
        menu->selectedIndex++;
    } else {
        menu->selectedIndex = 0; // Wrap to start
    }
}

#include "rgu.h"

Menu* Menu_Select(Menu *menu)
{
    MenuItem *selected = &menu->items[menu->selectedIndex];

    if (selected->actionType == MENU_ACTION_SUBMENU) {
        selected->action.submenu->selectedIndex = 0; // Reset selection
        return selected->action.submenu;
    } else if (selected->actionType == MENU_ACTION_FUNCTION) {
        if (selected->action.function) {
            RGU_RestartWDT(); // Restart watchdog before calling function
            selected->action.function();
            RGU_RestartWDT(); // Restart watchdog after function returns
            Menu_InvalidateCache(); // Force redraw after action
        }
    }

    return menu; // Stay in current menu
}

Menu* Menu_Back(Menu *menu)
{
    if (menu->parent) {
        return menu->parent;
    }
    return menu; // Stay in current menu if at root
}

Menu* Menu_GetRoot(void)
{
    return &mainMenu;
}

/* Menu Action Implementations */
static void Action_Settings(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 150, "SETTINGS", 0xFFFF, 0x0000);
    LCD_DrawString(40, 180, "NOT IMPLEMENTED YET", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK TO RETURN", 0xFFE0, 0x0000);

    /* Wait for BACK key */
    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_Games(void)
{
    /* This is handled by submenu */
}

static void Action_BacklightSettings(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(40, 150, "BACKLIGHT: 30s AUTO-OFF", 0xFFFF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK TO RETURN", 0xFFE0, 0x0000);

    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_SystemInfo(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 80, "SYSTEM INFO", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "DEVICE: S500", 0x7BEF, 0x0000);
    LCD_DrawString(20, 140, "CPU: MT6261", 0x7BEF, 0x0000);
    LCD_DrawString(20, 160, "LCD: 240x320", 0x7BEF, 0x0000);
    LCD_DrawString(20, 180, "KEYS: 20 WORKING!", 0x07E0, 0x0000); // Green
    LCD_DrawString(40, 280, "PRESS BACK TO RETURN", 0xFFE0, 0x0000);

    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_About(void)
{
    KP_ClearKeyBuffer();
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(80, 100, "PHONE", 0xFFFF, 0x0000);
    LCD_DrawString(50, 140, "CUSTOM FIRMWARE", 0x7BEF, 0x0000);
    LCD_DrawString(70, 160, "VERSION 1.0", 0x7BEF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK TO RETURN", 0xFFE0, 0x0000);

    while(1) {
        RGU_RestartWDT();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_Snake(void)
{
    KP_ClearKeyBuffer();

    /* Snake Game Configuration */
    #define GRID_WIDTH 20
    #define GRID_HEIGHT 25
    #define CELL_SIZE 12
    #define MAX_SNAKE_LENGTH 100
    #define GAME_SPEED_MS 150000UL  // 150ms per frame

    /* Data structures */
    typedef struct {
        uint8_t x, y;
    } Point;

    typedef struct {
        Point body[MAX_SNAKE_LENGTH];
        uint8_t length;
        int8_t dx, dy;
    } Snake;

    Snake snake;
    Point food;
    uint16_t score = 0;
    boolean gameOver = false;
    boolean exitGame = false;

    /* Initialize snake at center */
    snake.length = 3;
    snake.dx = 1;
    snake.dy = 0;
    snake.body[0].x = GRID_WIDTH / 2;
    snake.body[0].y = GRID_HEIGHT / 2;
    snake.body[1].x = snake.body[0].x - 1;
    snake.body[1].y = snake.body[0].y;
    snake.body[2].x = snake.body[1].x - 1;
    snake.body[2].y = snake.body[1].y;

    /* Generate initial food */
    food.x = 15;
    food.y = 10;

    /* Draw initial screen */
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(5, 2, "SNAKE", 0xFFFF, 0x0000);
    LCD_DrawString(150, 2, "SCORE:", 0xFFFF, 0x0000);

    /* Draw initial food */
    LCD_FillRect(food.x * CELL_SIZE, food.y * CELL_SIZE + 20, CELL_SIZE, CELL_SIZE, 0xF800); // Red

    /* Draw initial snake */
    for (uint8_t i = 0; i < snake.length; i++) {
        LCD_FillRect(snake.body[i].x * CELL_SIZE, snake.body[i].y * CELL_SIZE + 20, CELL_SIZE, CELL_SIZE, 0x07E0); // Green
    }

    /* Main game loop */
    while(!gameOver && !exitGame) {
        RGU_RestartWDT();

        /* Handle input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();

            switch(key) {
                case KEY_UP:
                    if (snake.dy != 1) { snake.dx = 0; snake.dy = -1; }
                    break;
                case KEY_DOWN:
                    if (snake.dy != -1) { snake.dx = 0; snake.dy = 1; }
                    break;
                case KEY_LEFT:
                    if (snake.dx != 1) { snake.dx = -1; snake.dy = 0; }
                    break;
                case KEY_RIGHT:
                    if (snake.dx != -1) { snake.dx = 1; snake.dy = 0; }
                    break;
                case KEY_BACK:
                    exitGame = true;
                    break;
                default:
                    break;
            }

            USC_Pause_us(50000); // Debounce
        }

        /* Move snake */
        Point newHead;
        newHead.x = snake.body[0].x + snake.dx;
        newHead.y = snake.body[0].y + snake.dy;

        /* Check wall collision */
        if (newHead.x >= GRID_WIDTH || newHead.y >= GRID_HEIGHT) {
            gameOver = true;
            continue;
        }

        /* Check self collision */
        for (uint8_t i = 0; i < snake.length; i++) {
            if (snake.body[i].x == newHead.x && snake.body[i].y == newHead.y) {
                gameOver = true;
                break;
            }
        }

        if (gameOver) continue;

        /* Check food collision */
        boolean ateFood = (newHead.x == food.x && newHead.y == food.y);

        if (ateFood) {
            score++;
            snake.length++;

            /* Generate new food */
            food.x = (USC_GetCurrentTicks() % GRID_WIDTH);
            food.y = ((USC_GetCurrentTicks() / 100) % GRID_HEIGHT);

            /* Draw new food */
            LCD_FillRect(food.x * CELL_SIZE, food.y * CELL_SIZE + 20, CELL_SIZE, CELL_SIZE, 0xF800); // Red

            /* Update score display */
            char scoreStr[10];
            scoreStr[0] = '0' + (score / 10);
            scoreStr[1] = '0' + (score % 10);
            scoreStr[2] = 0;
            LCD_DrawString(200, 2, scoreStr, 0x07E0, 0x0000); // Green
        }

        /* Erase tail if not growing */
        if (!ateFood) {
            Point tail = snake.body[snake.length - 1];
            LCD_FillRect(tail.x * CELL_SIZE, tail.y * CELL_SIZE + 20, CELL_SIZE, CELL_SIZE, 0x0000); // Black
        }

        /* Move body segments */
        for (int i = snake.length - 1; i > 0; i--) {
            snake.body[i] = snake.body[i - 1];
        }

        snake.body[0] = newHead;

        /* Draw new head */
        LCD_FillRect(newHead.x * CELL_SIZE, newHead.y * CELL_SIZE + 20, CELL_SIZE, CELL_SIZE, 0x07E0); // Green

        USC_Pause_us(GAME_SPEED_MS);
    }

    /* Game Over screen */
    if (gameOver) {
        LCD_FillRect(0, 0, 240, 320, 0x0000);
        LCD_DrawString(70, 120, "GAME OVER!", 0xF800, 0x0000); // Red

        char finalScore[20];
        finalScore[0] = 'S'; finalScore[1] = 'C'; finalScore[2] = 'O';
        finalScore[3] = 'R'; finalScore[4] = 'E'; finalScore[5] = ':';
        finalScore[6] = ' ';
        finalScore[7] = '0' + (score / 10);
        finalScore[8] = '0' + (score % 10);
        finalScore[9] = 0;
        LCD_DrawString(70, 150, finalScore, 0xFFFF, 0x0000);

        LCD_DrawString(40, 280, "PRESS BACK TO RETURN", 0xFFE0, 0x0000);

        while(1) {
            RGU_RestartWDT();
            if (KP_IsKeyPressed()) {
                TKEY key = KP_ReadKey();
                if (key == KEY_BACK) break;
            }
            USC_Pause_us(50000);
        }
    }
}

static void Action_Racing(void)
{
    KP_ClearKeyBuffer();

    /* Racing Game Configuration */
    #define ROAD_WIDTH 180
    #define ROAD_X 30
    #define LANE_WIDTH 60
    #define CAR_WIDTH 40
    #define CAR_HEIGHT 60
    #define MAX_OBSTACLES 5

    /* Data Structures */
    typedef struct {
        uint8_t lane;      // 0, 1, or 2
        int16_t y;         // Y position
        boolean active;
    } ObstacleCar;

    uint8_t playerLane = 1;  // Start in middle lane
    ObstacleCar obstacles[MAX_OBSTACLES];
    uint16_t score = 0;
    uint16_t frameCount = 0;
    uint32_t gameSpeed = 100000UL;  // Initial speed (100ms)
    boolean gameOver = false;
    boolean exitGame = false;
    int16_t roadOffset = 0;

    /* Initialize obstacles */
    for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = false;
    }

    /* Draw initial screen */
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(5, 2, "RACING", 0xFFFF, 0x0000);
    LCD_DrawString(150, 2, "SCORE:", 0xFFFF, 0x0000);

    /* Draw static road boundaries */
    LCD_FillRect(ROAD_X - 2, 20, 2, 280, 0xFFFF); // Left edge
    LCD_FillRect(ROAD_X + ROAD_WIDTH, 20, 2, 280, 0xFFFF); // Right edge

    /* Track previous player lane for erasing */
    uint8_t prevPlayerLane = playerLane;

    /* Main game loop */
    while (!gameOver && !exitGame) {
        RGU_RestartWDT();

        /* Handle input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();

            switch(key) {
                case KEY_LEFT:
                    if (playerLane > 0) playerLane--;
                    break;
                case KEY_RIGHT:
                    if (playerLane < 2) playerLane++;
                    break;
                case KEY_BACK:
                    exitGame = true;
                    break;
                default:
                    break;
            }

            USC_Pause_us(50000); // Debounce
        }

        /* Erase old player car if lane changed */
        if (prevPlayerLane != playerLane) {
            uint16_t oldX = ROAD_X + (prevPlayerLane * LANE_WIDTH) + (LANE_WIDTH - CAR_WIDTH) / 2;
            LCD_FillRect(oldX, 240, CAR_WIDTH, CAR_HEIGHT, 0x0000);
            prevPlayerLane = playerLane;
        }

        /* Update road markers with simple animation */
        roadOffset = (roadOffset + 5) % 40;

        /* Draw lane markers at new positions */
        for (int16_t y = 20 - roadOffset; y < 300; y += 40) {
            // Clear old markers
            LCD_FillRect(ROAD_X + LANE_WIDTH - 2, 20, 4, 280, 0x0000);
            LCD_FillRect(ROAD_X + LANE_WIDTH * 2 - 2, 20, 4, 280, 0x0000);

            // Draw new markers
            LCD_FillRect(ROAD_X + LANE_WIDTH - 2, y, 4, 20, 0xFFFF);
            LCD_FillRect(ROAD_X + LANE_WIDTH * 2 - 2, y, 4, 20, 0xFFFF);
        }

        /* Spawn new obstacle */
        frameCount++;
        if (frameCount % 30 == 0) {
            for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
                if (!obstacles[i].active) {
                    obstacles[i].lane = USC_GetCurrentTicks() % 3;
                    obstacles[i].y = 20;
                    obstacles[i].active = true;
                    break;
                }
            }
        }

        /* Update and draw obstacles */
        for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles[i].active) {
                int16_t oldY = obstacles[i].y;

                /* Erase old position */
                if (oldY >= 20 && oldY <= 300) {
                    uint16_t obX = ROAD_X + (obstacles[i].lane * LANE_WIDTH) + (LANE_WIDTH - CAR_WIDTH) / 2;
                    LCD_FillRect(obX, oldY, CAR_WIDTH, CAR_HEIGHT, 0x0000);
                }

                obstacles[i].y += 5;

                /* Check if passed player (score) */
                if (obstacles[i].y > 240 && obstacles[i].y < 245) {
                    score++;

                    /* Update score display */
                    char scoreStr[10];
                    scoreStr[0] = '0' + (score / 10);
                    scoreStr[1] = '0' + (score % 10);
                    scoreStr[2] = 0;
                    LCD_DrawString(200, 2, scoreStr, 0x07E0, 0x0000);

                    /* Increase speed every 10 points */
                    if (score % 10 == 0 && gameSpeed > 50000) {
                        gameSpeed -= 10000;
                    }
                }

                /* Deactivate if off screen */
                if (obstacles[i].y > 300) {
                    obstacles[i].active = false;
                }

                /* Draw obstacle at new position */
                if (obstacles[i].active && obstacles[i].y >= 20 && obstacles[i].y <= 300) {
                    uint16_t obX = ROAD_X + (obstacles[i].lane * LANE_WIDTH) + (LANE_WIDTH - CAR_WIDTH) / 2;
                    LCD_FillRect(obX, obstacles[i].y, CAR_WIDTH, CAR_HEIGHT, 0xF800); // Red

                    /* Check collision with player */
                    if (obstacles[i].lane == playerLane &&
                        obstacles[i].y + CAR_HEIGHT > 240 &&
                        obstacles[i].y < 240 + CAR_HEIGHT) {
                        gameOver = true;
                    }
                }
            }
        }

        /* Draw player car */
        uint16_t playerX = ROAD_X + (playerLane * LANE_WIDTH) + (LANE_WIDTH - CAR_WIDTH) / 2;
        LCD_FillRect(playerX, 240, CAR_WIDTH, CAR_HEIGHT, 0x07E0); // Green

        USC_Pause_us(gameSpeed);
    }

    /* Game Over screen */
    if (gameOver) {
        LCD_FillRect(0, 0, 240, 320, 0x0000);
        LCD_DrawString(70, 120, "GAME OVER!", 0xF800, 0x0000);

        char finalScore[20];
        finalScore[0] = 'S'; finalScore[1] = 'C'; finalScore[2] = 'O';
        finalScore[3] = 'R'; finalScore[4] = 'E'; finalScore[5] = ':';
        finalScore[6] = ' ';
        finalScore[7] = '0' + (score / 10);
        finalScore[8] = '0' + (score % 10);
        finalScore[9] = 0;
        LCD_DrawString(70, 150, finalScore, 0xFFFF, 0x0000);

        LCD_DrawString(40, 280, "PRESS BACK TO RETURN", 0xFFE0, 0x0000);

        while(1) {
            RGU_RestartWDT();
            if (KP_IsKeyPressed()) {
                TKEY key = KP_ReadKey();
                if (key == KEY_BACK) break;
            }
            USC_Pause_us(50000);
        }
    }
}
static void Action_FileManager(void)
{
    RGU_RestartWDT(); // Restart watchdog at very start of function

    KP_ClearKeyBuffer();
    RGU_RestartWDT(); // Restart watchdog after key buffer clear

    FileEntry files[20]; // Max 20 files for now
    int fileCount = 0;
    int selectedIndex = 0;
    boolean exitFM = false;
    boolean initSuccess = false;

    RGU_RestartWDT(); // Restart watchdog before LCD operations

    /* Initial Draw */
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    RGU_RestartWDT(); // Restart watchdog after fill

    LCD_DrawString(60, 10, "FILE MANAGER", 0xFFFF, 0x0000);
    RGU_RestartWDT(); // Restart watchdog after title

    LCD_FillRect(0, 50, 240, 2, 0xFFFF);
    RGU_RestartWDT(); // Restart watchdog after line

    LCD_DrawString(20, 100, "INITIALIZING SD...", 0xFFFF, 0x0000);
    RGU_RestartWDT(); // Restart watchdog after init message

    /* Initialize FAT32 */
    RGU_RestartWDT(); // Restart watchdog before FAT32 init
    if (!FAT32_IsInitialized()) {
        if (FAT32_Init()) {
            initSuccess = true;
        }
    } else {
        initSuccess = true;
    }
    RGU_RestartWDT(); // Restart watchdog after FAT32 init check

    if (initSuccess) {
        RGU_RestartWDT(); // Restart watchdog before reading directory
        LCD_DrawString(20, 100, "READING FILES...", 0xFFFF, 0x0000);

        /* Read Root Directory */
        fileCount = FAT32_ListRoot(files, 20);

        RGU_RestartWDT(); // Restart watchdog after reading directory
        LCD_FillRect(0, 100, 240, 20, 0x0000); // Clear init msg

        if (fileCount == 0) {
            LCD_DrawString(20, 100, "NO FILES FOUND", 0x7BEF, 0x0000);
        } else {
            LCD_DrawString(20, 100, "FILES LOADED", 0x07E0, 0x0000);
        }
    } else {
        LCD_DrawString(20, 100, "SD INIT FAILED!", 0xF800, 0x0000);
        LCD_DrawString(20, 120, "CHECK CARD", 0xF800, 0x0000);
    }

    while(!exitFM) {
        RGU_RestartWDT();

        if (initSuccess && fileCount > 0) {
            /* Draw File List */
            for(int i=0; i<fileCount; i++) {
                uint16_t y = 60 + (i * 25);
                uint16_t color = (i == selectedIndex) ? 0x001F : 0x0000;
                uint16_t textColor = (i == selectedIndex) ? 0xFFFF : 0x7BEF;

                /* Selection Bar */
                if(i == selectedIndex) {
                    LCD_FillRect(0, y-2, 240, 22, color);
                } else {
                    LCD_FillRect(0, y-2, 240, 22, 0x0000);
                }

                /* Icon (Dir/File) */
                LCD_DrawString(10, y, files[i].isDir ? "[D]" : "[F]", 0xFFE0, color);

                /* Name */
                LCD_DrawString(40, y, files[i].name, textColor, color);

                /* Size (if file) */
                if(!files[i].isDir) {
                    char sizeStr[10];
                    uint32_t kb = files[i].size / 1024;
                    if (kb == 0 && files[i].size > 0) kb = 1;

                    sizeStr[0] = '0' + (kb / 100) % 10;
                    sizeStr[1] = '0' + (kb / 10) % 10;
                    sizeStr[2] = '0' + (kb % 10);
                    sizeStr[3] = 'K'; sizeStr[4] = 0;
                    LCD_DrawString(180, y, sizeStr, 0x7BEF, color);
                }
            }
        }

        LCD_DrawString(20, 280, "UP/DOWN:NAV OK:OPEN", 0x7BEF, 0x0000);
        LCD_DrawString(40, 300, "BACK:RETURN", 0x7BEF, 0x0000);

        /* Handle Input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();

            switch(key) {
                case KEY_UP:
                    if(selectedIndex > 0) selectedIndex--;
                    break;
                case KEY_DOWN:
                    if(selectedIndex < fileCount-1) selectedIndex++;
                    break;
                case KEY_BACK:
                    exitFM = true;
                    break;
                default:
                    break;
            }
            USC_Pause_us(150000);
        }
    }

    Menu_InvalidateCache();
}

/* ============================================================================
 * STUB IMPLEMENTATIONS FOR APPS, SETTINGS, AND ADDITIONAL GAMES
 * These are placeholder implementations showing "NOT IMPLEMENTED YET"
 * ============================================================================ */

/* Apps Stubs */
static void Action_Calculator(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "CALCULATOR", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_Clock(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "CLOCK", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_Calendar(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "CALENDAR", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_Notes(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "NOTES", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

/* Settings Stubs */
static void Action_BrightnessSettings(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "BRIGHTNESS", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_TimeSettings(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "TIME SETTINGS", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_LanguageSettings(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "LANGUAGE", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_SoundSettings(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "SOUND SETTINGS", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

/* Game Stubs */
static void Action_Tetris(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "TETRIS", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_Pong(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "PONG", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_TicTacToe(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "TIC TAC TOE", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}

static void Action_MemoryGame(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "MEMORY GAME", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "NOT IMPLEMENTED YET", 0xFFE0, 0x0000);
    LCD_DrawString(20, 160, "PRESS ANY KEY", 0x07E0, 0x0000);
    while (!KP_IsKeyPressed()) {
        KeepBacklightOn();
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }
    KP_ReadKey();
}





static void Action_BeepTest(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 100, "AUDIO TEST", 0xFFFF, 0x0000);
    LCD_DrawString(20, 120, "INITIALIZING AFE...", 0xFFE0, 0x0000);

    /* Initialize audio system */
    AFE_initialize();

    LCD_DrawString(20, 140, "READY!", 0x07E0, 0x0000);
    LCD_DrawString(20, 160, "1: Play Scale", 0xFFFF, 0x0000);
    LCD_DrawString(20, 180, "2: Play Melody", 0xFFFF, 0x0000);
    LCD_DrawString(20, 200, "3: Beep (Old)", 0xFFFF, 0x0000);
    LCD_DrawString(20, 240, "BACK to exit", 0x7BEF, 0x0000);

    boolean exitBeep = false;
    while (!exitBeep) {
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            switch (key) {
                case KEY_1: // Scale
                    LCD_DrawString(20, 220, "Playing Scale...", 0x07E0, 0x0000);
                    /* Play a scale (approximate values) */
                    /* Lower value = Higher pitch */
                    PlayTone(30, 200); // Do
                    PlayTone(27, 200); // Re
                    PlayTone(24, 200); // Mi
                    PlayTone(22, 200); // Fa
                    PlayTone(20, 200); // Sol
                    PlayTone(18, 200); // La
                    PlayTone(16, 200); // Si
                    PlayTone(15, 200); // Do
                    LCD_DrawString(20, 220, "Done!           ", 0xFFFF, 0x0000);
                    break;

                case KEY_2: // Melody (Twinkle Twinkle)
                    LCD_DrawString(20, 220, "Playing Melody...", 0x07E0, 0x0000);
                    /* Twinkle Twinkle Little Star */
                    PlayTone(30, 300); PlayTone(30, 300); // Do Do
                    PlayTone(20, 300); PlayTone(20, 300); // Sol Sol
                    PlayTone(18, 300); PlayTone(18, 300); // La La
                    PlayTone(20, 600);                    // Sol

                    PlayTone(22, 300); PlayTone(22, 300); // Fa Fa
                    PlayTone(24, 300); PlayTone(24, 300); // Mi Mi
                    PlayTone(27, 300); PlayTone(27, 300); // Re Re
                    PlayTone(30, 600);                    // Do
                    LCD_DrawString(20, 220, "Done!            ", 0xFFFF, 0x0000);
                    break;

                case KEY_3: // Old Beep
                    LCD_DrawString(20, 220, "Beeping...      ", 0x07E0, 0x0000);
                    Beep();
                    LCD_DrawString(20, 220, "Done!           ", 0xFFFF, 0x0000);
                    break;

                case KEY_BACK:
                    exitBeep = true;
                    break;
                default:
                    break;
            }
        }
        USC_Pause_us(50000);
        RGU_RestartWDT();
    }

    Menu_InvalidateCache();
}

/* Flappy Bird Clone */
static void Action_FlappyBird(void)
{
    const int BIRD_X = 50;
    const int GRAVITY = 1;
    const int JUMP_STRENGTH = -6; // Negative Y is up
    const int PIPE_SPEED = 3;
    const int PIPE_WIDTH = 30;
    const int PIPE_GAP = 90;
    const int PIPE_INTERVAL = 140;

    int birdY = 160;
    int birdVelocity = 0;
    int score = 0;
    boolean gameOver = false;

    /* Pipe structure */
    typedef struct {
        int x;
        int gapY; // Top of the gap
        boolean active;
        boolean passed;
    } Pipe;

    Pipe pipes[3]; // Keep 3 pipes in rotation
    for(int i=0; i<3; i++) pipes[i].active = false;

    /* Rendering State */
    int oldBirdY = birdY;
    int oldPipeX[3];
    for(int i=0; i<3; i++) oldPipeX[i] = -100;
    int lastScore = -1;

    /* Initial Draw */
    LCD_FillRect(0, 0, 240, 320, 0x87CE); // Sky Blue
    LCD_DrawString(80, 100, "FLAPPY BIRD", 0xFFFF, 0x87CE);
    LCD_DrawString(60, 140, "PRESS 5 TO JUMP", 0xFFFF, 0x87CE);
    USC_Pause_us(1000000);
    LCD_FillRect(0, 0, 240, 320, 0x87CE); // Clear

    int frameCount = 0;

    while (!gameOver) {
        RGU_RestartWDT();
        KeepBacklightOn(); // Keep screen on during game

        /* Input Handling */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) return; // Exit game
            if (key == KEY_5 || key == KEY_OK) {
                birdVelocity = JUMP_STRENGTH;
            }
        }

        /* Erase Old Objects */
        // Erase Bird
        LCD_FillRect(BIRD_X, oldBirdY, 10, 10, 0x87CE);

        // Erase Pipes
        for(int i=0; i<3; i++) {
            if (pipes[i].active && oldPipeX[i] > -PIPE_WIDTH) {
                LCD_FillRect(oldPipeX[i], 0, PIPE_WIDTH, pipes[i].gapY, 0x87CE);
                LCD_FillRect(oldPipeX[i], pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, 320 - (pipes[i].gapY + PIPE_GAP), 0x87CE);
            }
        }

        /* Physics */
        birdVelocity += GRAVITY;
        birdY += birdVelocity;

        /* Floor/Ceiling Collision */
        if (birdY < 0) birdY = 0;
        if (birdY > 310) gameOver = true;

        /* Pipe Management */
        if (frameCount % 50 == 0) { // Spawn pipe
            for(int i=0; i<3; i++) {
                if (!pipes[i].active) {
                    pipes[i].active = true;
                    pipes[i].x = 240;
                    pipes[i].gapY = 50 + (frameCount % 150); // Randomish gap
                    pipes[i].passed = false;
                    oldPipeX[i] = 240; // Initialize old pos
                    break;
                }
            }
        }
        frameCount++;

        /* Update Pipe Positions & Draw */
        for(int i=0; i<3; i++) {
            if (pipes[i].active) {
                pipes[i].x -= PIPE_SPEED;

                /* Draw Top Pipe */
                LCD_FillRect(pipes[i].x, 0, PIPE_WIDTH, pipes[i].gapY, 0x07E0); // Green

                /* Draw Bottom Pipe */
                LCD_FillRect(pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, 320 - (pipes[i].gapY + PIPE_GAP), 0x07E0);

                /* Collision Check */
                if (BIRD_X + 10 > pipes[i].x && BIRD_X < pipes[i].x + PIPE_WIDTH) {
                    if (birdY < pipes[i].gapY || birdY + 10 > pipes[i].gapY + PIPE_GAP) {
                        gameOver = true;
                    }
                }

                /* Score Update */
                if (!pipes[i].passed && pipes[i].x < BIRD_X) {
                    score++;
                    pipes[i].passed = true;
                    Beep(); // Point sound
                }

                /* Deactivate off-screen pipes */
                if (pipes[i].x < -PIPE_WIDTH) pipes[i].active = false;

                /* Store old X for next frame erasure */
                oldPipeX[i] = pipes[i].x;
            }
        }

        /* Draw Bird */
        LCD_FillRect(BIRD_X, birdY, 10, 10, 0xFFE0); // Yellow Bird
        oldBirdY = birdY;

        /* Draw Score */
        // Always redraw score to ensure it's on top of pipes
        char scoreBuf[10];
        sprintf(scoreBuf, "%d", score);
        LCD_DrawString(110, 20, scoreBuf, 0xFFFF, 0x87CE);

        USC_Pause_us(30000); // ~30 FPS
    }

    /* Game Over Screen */
    LCD_DrawString(80, 140, "GAME OVER", 0xF800, 0x87CE);
    USC_Pause_us(2000000);
    Menu_InvalidateCache();
}

/* Breakout Game */
static void Action_Breakout(void)
{
    const int PADDLE_WIDTH = 50;
    const int PADDLE_HEIGHT = 6;
    const int BALL_SIZE = 6;
    const int BRICK_ROWS = 5;
    const int BRICK_COLS = 6;
    const int BRICK_HEIGHT = 15;
    const int BRICK_WIDTH = 240 / BRICK_COLS;

    int paddleX = (240 - PADDLE_WIDTH) / 2;
    int ballX = 120;
    int ballY = 200;
    int ballVX = 3;
    int ballVY = -3;
    int score = 0;
    boolean gameOver = false;
    boolean victory = false;

    /* Bricks State (1 = active, 0 = broken) */
    uint8_t bricks[BRICK_ROWS][BRICK_COLS];
    for(int r=0; r<BRICK_ROWS; r++)
        for(int c=0; c<BRICK_COLS; c++)
            bricks[r][c] = 1;

    int activeBricks = BRICK_ROWS * BRICK_COLS;

    /* Rendering State */
    int oldPaddleX = paddleX;
    int oldBallX = ballX;
    int oldBallY = ballY;

    /* Initial Draw */
    LCD_FillRect(0, 0, 240, 320, 0x0000); // Black Background

    /* Draw Bricks */
    for(int r=0; r<BRICK_ROWS; r++) {
        uint16_t color = 0;
        switch(r) {
            case 0: color = 0xF800; break; // Red
            case 1: color = 0xFD20; break; // Orange
            case 2: color = 0xFFE0; break; // Yellow
            case 3: color = 0x07E0; break; // Green
            case 4: color = 0x001F; break; // Blue
        }
        for(int c=0; c<BRICK_COLS; c++) {
            LCD_FillRect(c * BRICK_WIDTH + 1, r * BRICK_HEIGHT + 25, BRICK_WIDTH - 2, BRICK_HEIGHT - 2, color);
        }
    }

    LCD_DrawString(80, 150, "READY?", 0xFFFF, 0x0000);
    USC_Pause_us(1000000);
    LCD_FillRect(0, 140, 240, 40, 0x0000); // Clear text

    while (!gameOver && !victory) {
        RGU_RestartWDT();
        KeepBacklightOn(); // Keep screen on during game

        /* Input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) return;
            if (key == KEY_4 || key == KEY_LEFT) paddleX -= 15;
            if (key == KEY_6 || key == KEY_RIGHT) paddleX += 15;
        }

        /* Clamp Paddle */
        if (paddleX < 0) paddleX = 0;
        if (paddleX > 240 - PADDLE_WIDTH) paddleX = 240 - PADDLE_WIDTH;

        /* Erase Old Objects */
        LCD_FillRect(oldPaddleX, 300, PADDLE_WIDTH, PADDLE_HEIGHT, 0x0000);
        LCD_FillRect(oldBallX, oldBallY, BALL_SIZE, BALL_SIZE, 0x0000);

        /* Physics */
        ballX += ballVX;
        ballY += ballVY;

        /* Wall Collisions */
        if (ballX <= 0 || ballX >= 240 - BALL_SIZE) ballVX = -ballVX;
        if (ballY <= 0) ballVY = -ballVY;

        /* Paddle Collision */
        if (ballY + BALL_SIZE >= 300 && ballY < 300 + PADDLE_HEIGHT) {
            if (ballX + BALL_SIZE >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
                ballVY = -ballVY;
                /* Add some English based on hit position */
                int center = paddleX + PADDLE_WIDTH / 2;
                int hit = (ballX + BALL_SIZE/2) - center;
                ballVX = hit / 4;
                if (ballVX == 0) ballVX = (hit > 0) ? 1 : -1;

                Beep(); // Bounce sound
            }
        }

        /* Bottom Collision (Death) */
        if (ballY > 320) gameOver = true;

        /* Brick Collision */
        if (ballY < 25 + BRICK_ROWS * BRICK_HEIGHT) {
            int r = (ballY - 25) / BRICK_HEIGHT;
            int c = ballX / BRICK_WIDTH;

            if (r >= 0 && r < BRICK_ROWS && c >= 0 && c < BRICK_COLS) {
                if (bricks[r][c]) {
                    bricks[r][c] = 0;
                    ballVY = -ballVY;
                    score += 10;
                    activeBricks--;

                    /* Erase Brick */
                    LCD_FillRect(c * BRICK_WIDTH + 1, r * BRICK_HEIGHT + 25, BRICK_WIDTH - 2, BRICK_HEIGHT - 2, 0x0000);

                    if (activeBricks == 0) victory = true;
                }
            }
        }

        /* Draw New Objects */
        LCD_FillRect(paddleX, 300, PADDLE_WIDTH, PADDLE_HEIGHT, 0x07E0); // Green Paddle
        LCD_FillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, 0xFFFF); // White Ball

        /* Store Old Positions */
        oldPaddleX = paddleX;
        oldBallX = ballX;
        oldBallY = ballY;

        USC_Pause_us(30000);
    }

    if (victory) {
        LCD_DrawString(80, 150, "VICTORY!", 0x07E0, 0x0000);
    } else {
        LCD_DrawString(80, 150, "GAME OVER", 0xF800, 0x0000);
    }
    char buf[20];
    sprintf(buf, "SCORE: %d", score);
    LCD_DrawString(80, 170, buf, 0xFFFF, 0x0000);

    USC_Pause_us(3000000);
    Menu_InvalidateCache();
}

/* Space Invaders (RPG Edition) */
static void Action_SpaceInvaders(void)
{
    /* Game Constants */
    const int PLAYER_Y = 280;
    const int PLAYER_WIDTH = 15;
    const int PLAYER_HEIGHT = 10;
    const int ENEMY_ROWS = 3;
    const int ENEMY_COLS = 6;
    const int ENEMY_WIDTH = 12;
    const int ENEMY_HEIGHT = 8;
    const int MAX_BULLETS = 10;

    /* Player Stats (Upgradable) */
    int playerSpeed = 4;
    int bulletSpeed = 5;
    int maxHealth = 3;
    int damage = 1;
    int money = 0;

    int currentHealth = maxHealth;
    int level = 1;
    boolean gameOver = false;

    /* Game Objects */
    typedef struct {
        int x, y;
        boolean active;
    } Bullet;

    typedef struct {
        int x, y;
        boolean active;
        int type;
    } Enemy;

    Bullet bullets[MAX_BULLETS];
    Enemy enemies[ENEMY_ROWS * ENEMY_COLS];

    while (!gameOver) {
        /* Level Init */
        int playerX = 110;
        int enemySpeed = 1 + (level / 2);
        int enemyDir = 1;
        int activeEnemies = ENEMY_ROWS * ENEMY_COLS;

        for(int i=0; i<MAX_BULLETS; i++) bullets[i].active = false;

        for(int r=0; r<ENEMY_ROWS; r++) {
            for(int c=0; c<ENEMY_COLS; c++) {
                int idx = r * ENEMY_COLS + c;
                enemies[idx].active = true;
                enemies[idx].x = 20 + c * 30;
                enemies[idx].y = 40 + r * 20;
            }
        }

        /* Rendering State */
        int oldPlayerX = playerX;
        int oldBulletX[MAX_BULLETS], oldBulletY[MAX_BULLETS];
        int oldEnemyX[ENEMY_ROWS * ENEMY_COLS], oldEnemyY[ENEMY_ROWS * ENEMY_COLS];

        /* Clear Screen */
        LCD_FillRect(0, 0, 240, 320, 0x0000);

        char levelBuf[20];
        sprintf(levelBuf, "LEVEL %d", level);
        LCD_DrawString(90, 150, levelBuf, 0xFFFF, 0x0000);
        USC_Pause_us(1000000);
        LCD_FillRect(0, 140, 240, 40, 0x0000);

        /* Draw HUD */
        LCD_FillRect(0, 300, 240, 20, 0x18E3);

        boolean levelComplete = false;

        while (!gameOver && !levelComplete) {
            RGU_RestartWDT();
            KeepBacklightOn(); // Keep screen on during game

            /* Input */
            if (KP_IsKeyPressed()) {
                TKEY key = KP_ReadKey();
                if (key == KEY_BACK) { gameOver = true; break; }
                if (key == KEY_4 || key == KEY_LEFT) playerX -= playerSpeed;
                if (key == KEY_6 || key == KEY_RIGHT) playerX += playerSpeed;
                if (key == KEY_5 || key == KEY_OK) {
                    /* Fire Bullet */
                    for(int i=0; i<MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].x = playerX + PLAYER_WIDTH/2;
                            bullets[i].y = PLAYER_Y - 5;
                            oldBulletX[i] = bullets[i].x; // Init old pos
                            oldBulletY[i] = bullets[i].y;
                            Beep();
                            break;
                        }
                    }
                }
            }

            if (playerX < 0) playerX = 0;
            if (playerX > 240 - PLAYER_WIDTH) playerX = 240 - PLAYER_WIDTH;

            /* Erase Old Objects */
            LCD_FillRect(oldPlayerX, PLAYER_Y, PLAYER_WIDTH, PLAYER_HEIGHT, 0x0000);

            for(int i=0; i<MAX_BULLETS; i++) {
                if (bullets[i].active) {
                     LCD_FillRect(oldBulletX[i], oldBulletY[i], 2, 6, 0x0000);
                }
            }

            for(int i=0; i<ENEMY_ROWS*ENEMY_COLS; i++) {
                if (enemies[i].active) {
                    LCD_FillRect(oldEnemyX[i], oldEnemyY[i], ENEMY_WIDTH, ENEMY_HEIGHT, 0x0000);
                }
            }

            /* Update Bullets */
            for(int i=0; i<MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].y -= bulletSpeed;
                    if (bullets[i].y < 0) bullets[i].active = false;

                    /* Collision with Enemies */
                    for(int e=0; e<ENEMY_ROWS*ENEMY_COLS; e++) {
                        if (enemies[e].active && bullets[i].active) {
                            if (bullets[i].x >= enemies[e].x && bullets[i].x <= enemies[e].x + ENEMY_WIDTH &&
                                bullets[i].y >= enemies[e].y && bullets[i].y <= enemies[e].y + ENEMY_HEIGHT) {
                                    enemies[e].active = false;
                                    bullets[i].active = false;
                                    activeEnemies--;
                                    money += 10;
                                    if (activeEnemies == 0) levelComplete = true;
                            }
                        }
                    }
                }
            }

            /* Update Enemies */
            boolean hitWall = false;
            for(int i=0; i<ENEMY_ROWS*ENEMY_COLS; i++) {
                if (enemies[i].active) {
                    enemies[i].x += enemySpeed * enemyDir;
                    if (enemies[i].x <= 0 || enemies[i].x >= 240 - ENEMY_WIDTH) hitWall = true;
                }
            }

            if (hitWall) {
                enemyDir = -enemyDir;
                for(int i=0; i<ENEMY_ROWS*ENEMY_COLS; i++) {
                    if (enemies[i].active) enemies[i].y += 5;
                }
            }

            /* Draw New Objects */
            LCD_FillRect(playerX, PLAYER_Y, PLAYER_WIDTH, PLAYER_HEIGHT, 0x07E0); // Player

            for(int i=0; i<MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    LCD_FillRect(bullets[i].x, bullets[i].y, 2, 6, 0xFFE0); // Bullet
                    oldBulletX[i] = bullets[i].x;
                    oldBulletY[i] = bullets[i].y;
                }
            }

            for(int i=0; i<ENEMY_ROWS*ENEMY_COLS; i++) {
                if (enemies[i].active) {
                    LCD_FillRect(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, 0xF800); // Enemy
                    oldEnemyX[i] = enemies[i].x;
                    oldEnemyY[i] = enemies[i].y;

                    if (enemies[i].y > PLAYER_Y) gameOver = true; // Invasion successful
                }
            }

            oldPlayerX = playerX;

            /* Draw HUD Stats */
            char hudBuf[30];
            sprintf(hudBuf, "HP:%d  $: %d", currentHealth, money);
            LCD_DrawString(10, 302, hudBuf, 0xFFFF, 0x18E3);

            USC_Pause_us(30000);
        }

        if (levelComplete) {
            /* Upgrade Menu */
            boolean shopping = true;
            LCD_FillRect(0, 0, 240, 320, 0x0000);
            LCD_DrawString(60, 40, "LEVEL COMPLETE!", 0x07E0, 0x0000);

            while(shopping) {
                RGU_RestartWDT();

                LCD_FillRect(0, 80, 240, 200, 0x0000); // Clear menu area

                char moneyBuf[20];
                sprintf(moneyBuf, "MONEY: $%d", money);
                LCD_DrawString(80, 80, moneyBuf, 0xFFE0, 0x0000);

                LCD_DrawString(20, 120, "1. DMG UP ($100)", 0xFFFF, 0x0000);
                LCD_DrawString(20, 150, "2. SPD UP ($50)", 0xFFFF, 0x0000);
                LCD_DrawString(20, 180, "3. HEAL ($20)", 0xFFFF, 0x0000);
                LCD_DrawString(20, 240, "PRESS OK TO NEXT", 0x07E0, 0x0000);

                while(!KP_IsKeyPressed()) { USC_Pause_us(50000); RGU_RestartWDT(); }
                TKEY key = KP_ReadKey();

                if (key == KEY_1 && money >= 100) {
                    damage++; money -= 100;
                    LCD_DrawString(160, 120, "OK!", 0x07E0, 0x0000);
                }
                else if (key == KEY_2 && money >= 50) {
                    bulletSpeed += 2; money -= 50;
                    LCD_DrawString(160, 150, "OK!", 0x07E0, 0x0000);
                }
                else if (key == KEY_3 && money >= 20 && currentHealth < maxHealth) {
                    currentHealth++; money -= 20;
                    LCD_DrawString(160, 180, "OK!", 0x07E0, 0x0000);
                }
                else if (key == KEY_OK || key == KEY_5) {
                    shopping = false;
                }

                USC_Pause_us(200000);
            }
            level++;
        }
    }

    LCD_DrawString(80, 150, "GAME OVER", 0xF800, 0x0000);
    USC_Pause_us(3000000);
    Menu_InvalidateCache();
}

/* Text-Based Dungeon Crawler */
static void Action_Dungeon(void)
{
    /* Map size: 20x16 characters */
    #define MAP_W 20
    #define MAP_H 16
    #define CHAR_W 6
    #define CHAR_H 8

    /* Map: # = wall, . = floor, $ = coin, E = enemy, X = exit */
    char map[MAP_H][MAP_W + 1] = {
        "####################",
        "#@.....#....$.....E#",
        "#.###..#.####..###.#",
        "#...#..#....#....#.#",
        "#.#.#..####.####.#.#",
        "#.#......#.......#.#",
        "#.######.#.#######.#",
        "#......#.#.#.....$.#",
        "#.####.#.#.#.#####.#",
        "#.$..#...#...#...#.#",
        "#.##.#####.###.#.#.#",
        "#.#..........#E#...#",
        "#.#.########.#.###.#",
        "#...#......#.#.$...#",
        "#E..#...$..#.....X.#",
        "####################"
    };

    int playerX = 1, playerY = 1;
    int score = 0;
    int health = 3;
    boolean gameOver = false;
    boolean victory = false;

    /* Find player start position */
    for(int y=0; y<MAP_H; y++) {
        for(int x=0; x<MAP_W; x++) {
            if (map[y][x] == '@') {
                playerX = x;
                playerY = y;
                map[y][x] = '.';
            }
        }
    }

    /* Draw initial map */
    LCD_FillRect(0, 0, 240, 320, 0x0000);

    /* Draw HUD */
    LCD_DrawString(5, 2, "DUNGEON EXPLORER", 0xFFE0, 0x0000);

    /* Draw map */
    for(int y=0; y<MAP_H; y++) {
        for(int x=0; x<MAP_W; x++) {
            char c = map[y][x];
            uint16_t color = 0x7BEF; // Gray default

            switch(c) {
                case '#': color = 0x4208; break; // Dark gray wall
                case '.': color = 0x2104; break; // Darker floor
                case '$': color = 0xFFE0; break; // Yellow coin
                case 'E': color = 0xF800; break; // Red enemy
                case 'X': color = 0x07E0; break; // Green exit
            }

            char buf[2] = {c, 0};
            LCD_DrawString(x * CHAR_W + 60, y * CHAR_H + 60, buf, color, 0x0000);
        }
    }

    /* Draw player */
    LCD_DrawString(playerX * CHAR_W + 60, playerY * CHAR_H + 60, "@", 0x07FF, 0x0000);

    while (!gameOver && !victory) {
        RGU_RestartWDT();
        KeepBacklightOn();

        /* Update HUD */
        char hudBuf[30];
        sprintf(hudBuf, "HP:%d  GOLD:%d", health, score);
        LCD_DrawString(5, 290, hudBuf, 0xFFFF, 0x0000);

        /* Input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) return;

            int newX = playerX;
            int newY = playerY;

            if (key == KEY_2 || key == KEY_UP) newY--;
            if (key == KEY_8 || key == KEY_DOWN) newY++;
            if (key == KEY_4 || key == KEY_LEFT) newX--;
            if (key == KEY_6 || key == KEY_RIGHT) newX++;

            /* Check collision */
            if (newX >= 0 && newX < MAP_W && newY >= 0 && newY < MAP_H) {
                char target = map[newY][newX];

                if (target != '#') {
                    /* Erase old player */
                    LCD_DrawString(playerX * CHAR_W + 60, playerY * CHAR_H + 60, ".", 0x2104, 0x0000);

                    /* Handle tile */
                    if (target == '$') {
                        score += 10;
                        map[newY][newX] = '.';
                        Beep();
                    }
                    else if (target == 'E') {
                        health--;
                        map[newY][newX] = '.';
                        if (health <= 0) gameOver = true;
                    }
                    else if (target == 'X') {
                        victory = true;
                    }

                    /* Move player */
                    playerX = newX;
                    playerY = newY;

                    /* Draw new player */
                    LCD_DrawString(playerX * CHAR_W + 60, playerY * CHAR_H + 60, "@", 0x07FF, 0x0000);
                }
            }

            USC_Pause_us(100000); // Debounce
        }

        USC_Pause_us(50000);
    }

    if (victory) {
        LCD_DrawString(70, 150, "YOU ESCAPED!", 0x07E0, 0x0000);
    } else {
        LCD_DrawString(70, 150, "YOU DIED!", 0xF800, 0x0000);
    }

    char finalBuf[20];
    sprintf(finalBuf, "SCORE: %d", score);
    LCD_DrawString(80, 170, finalBuf, 0xFFFF, 0x0000);

    USC_Pause_us(3000000);
    Menu_InvalidateCache();
}

/* eBook Reader */
static void Action_EBookReader(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 40, "eBook Reader", 0xFFE0, 0x0000);

    /* Sample text display */
    LCD_DrawString(10, 80, "Sample eBook Content:", 0x07E0, 0x0000);
    LCD_DrawString(10, 100, "------------------------", 0x7BEF, 0x0000);
    LCD_DrawString(10, 120, "Once upon a time, in a", 0xFFFF, 0x0000);
    LCD_DrawString(10, 140, "land far far away...", 0xFFFF, 0x0000);
    LCD_DrawString(10, 160, "", 0xFFFF, 0x0000);
    LCD_DrawString(10, 180, "There lived a brave", 0xFFFF, 0x0000);
    LCD_DrawString(10, 200, "programmer who created", 0xFFFF, 0x0000);
    LCD_DrawString(10, 220, "amazing things on a", 0xFFFF, 0x0000);
    LCD_DrawString(10, 240, "tiny smartwatch.", 0xFFFF, 0x0000);

    LCD_DrawString(10, 280, "UP/DOWN: Scroll", 0x7BEF, 0x0000);
    LCD_DrawString(10, 295, "BACK: Exit", 0x7BEF, 0x0000);

    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
    Menu_InvalidateCache();
}

/* Internet Browser (Placeholder) */
static void Action_Browser(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(50, 100, "INTERNET BROWSER", 0xFFE0, 0x0000);
    LCD_DrawString(40, 150, "NO NETWORK AVAILABLE", 0xF800, 0x0000);
    LCD_DrawString(20, 180, "WiFi not supported", 0x7BEF, 0x0000);
    LCD_DrawString(20, 200, "GPRS requires SIM", 0x7BEF, 0x0000);
    LCD_DrawString(60, 280, "Press BACK", 0xFFFF, 0x0000);

    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            if (KP_ReadKey() == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
    Menu_InvalidateCache();
}

/* Contacts (Rehber) */
static void Action_Contacts(void)
{
    /* Simple in-memory contacts */
    typedef struct {
        char name[20];
        char phone[15];
        boolean used;
    } Contact;

    static Contact contacts[10] = {
        {"Ali", "555-1234", true},
        {"Mehmet", "555-5678", true},
        {"Ayse", "555-9012", true},
        {"", "", false}
    };

    int selected = 0;
    int contactCount = 3;
    boolean editing = false;

    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();

        LCD_FillRect(0, 0, 240, 320, 0x0000);
        LCD_DrawString(80, 10, "CONTACTS", 0xFFE0, 0x0000);

        /* Draw contacts */
        for(int i=0; i<10 && contacts[i].used; i++) {
            uint16_t color = (i == selected) ? 0x07E0 : 0xFFFF;
            uint16_t bg = (i == selected) ? 0x001F : 0x0000;

            char buf[40];
            sprintf(buf, "%s: %s", contacts[i].name, contacts[i].phone);
            LCD_DrawString(10, 40 + i*20, buf, color, bg);
        }

        LCD_DrawString(10, 280, "UP/DOWN: Select", 0x7BEF, 0x0000);
        LCD_DrawString(10, 295, "BACK: Exit", 0x7BEF, 0x0000);

        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
            if (key == KEY_UP && selected > 0) selected--;
            if (key == KEY_DOWN && selected < contactCount - 1) selected++;
        }

        USC_Pause_us(100000);
    }
    Menu_InvalidateCache();
}

/* Call Log (Arama Kaydi) */
static void Action_CallLog(void)
{
    /* Simulated call log */
    const char* callLog[] = {
        "-> Ali       10:30",
        "<- Mehmet    09:15",
        "X  Ayse      08:45",
        "-> Fatma     Yesterday",
        "<- Ahmet     Yesterday"
    };
    const char* types[] = {"Outgoing", "Incoming", "Missed", "Outgoing", "Incoming"};
    uint16_t colors[] = {0x07E0, 0xFFFF, 0xF800, 0x07E0, 0xFFFF};

    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(70, 10, "CALL LOG", 0xFFE0, 0x0000);

    for(int i=0; i<5; i++) {
        LCD_DrawString(10, 50 + i*30, callLog[i], colors[i], 0x0000);
    }

    LCD_DrawString(10, 250, "->: Outgoing", 0x07E0, 0x0000);
    LCD_DrawString(10, 265, "<-: Incoming", 0xFFFF, 0x0000);
    LCD_DrawString(10, 280, "X:  Missed", 0xF800, 0x0000);
    LCD_DrawString(130, 295, "BACK: Exit", 0x7BEF, 0x0000);

    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            if (KP_ReadKey() == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
    Menu_InvalidateCache();
}

/* Terminal / Code Editor with Phone Input */
static void Action_Terminal(void)
{
    /* T9-style character mapping */
    const char* keyChars[] = {
        " 0",           // KEY_0
        ".,;:!?(){}[]<>+-*/=&|^~#$@\\\"'`1", // KEY_1 - symbols
        "ABC2",         // KEY_2
        "DEF3",         // KEY_3
        "GHI4",         // KEY_4
        "JKL5",         // KEY_5
        "MNO6",         // KEY_6
        "PQRS7",        // KEY_7
        "TUV8",         // KEY_8
        "WXYZ9"         // KEY_9
    };

    #define MAX_CODE_LEN 500
    #define VISIBLE_LINES 12
    #define CHARS_PER_LINE 30

    char code[MAX_CODE_LEN];
    int codeLen = 0;
    int cursorPos = 0;
    int scrollLine = 0;

    int lastKey = -1;
    int charIndex = 0;
    uint32_t lastKeyTime = 0;

    code[0] = '\0';

    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 5, "TERMINAL", 0x07E0, 0x0000);
    LCD_DrawString(5, 290, "1:Sym 0:Spc #:Enter *:Del", 0x7BEF, 0x0000);
    LCD_DrawString(5, 305, "2-9:T9 OK:Run BACK:Exit", 0x7BEF, 0x0000);

    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();

        /* Draw code area */
        LCD_FillRect(0, 20, 240, 260, 0x0841); // Dark blue bg

        /* Draw code with cursor */
        int lineY = 25;
        int lineX = 5;
        int currentLine = 0;
        int charInLine = 0;

        for(int i=0; i<=codeLen && lineY < 270; i++) {
            char c = (i < codeLen) ? code[i] : '\0';

            /* Draw cursor */
            if (i == cursorPos) {
                LCD_FillRect(lineX, lineY, 6, 14, 0x07E0); // Green cursor
            }

            if (c == '\n' || charInLine >= CHARS_PER_LINE) {
                lineY += 16;
                lineX = 5;
                charInLine = 0;
                currentLine++;
                if (c == '\n') continue;
            }

            if (c != '\0' && c != '\n') {
                char buf[2] = {c, 0};
                uint16_t fg = 0xFFFF;

                /* Syntax highlighting */
                if (c >= '0' && c <= '9') fg = 0xFFE0; // Yellow for numbers
                else if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']') fg = 0xF81F; // Magenta for brackets
                else if (c == ';' || c == ':') fg = 0x07FF; // Cyan for punctuation

                LCD_DrawString(lineX, lineY, buf, fg, 0x0841);
                lineX += 6;
                charInLine++;
            }
        }

        /* Input handling */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            uint32_t now = USC_GetCurrentTicks() / 1000;

            if (key == KEY_BACK) break;

            if (key == KEY_OK) {
                /* "Run" - just show message */
                LCD_FillRect(50, 130, 140, 40, 0x001F);
                LCD_DrawString(60, 145, "Code Saved!", 0xFFFF, 0x001F);
                USC_Pause_us(1000000);
            }
            else if (key == KEY_STAR) {
                /* Delete character */
                if (cursorPos > 0 && codeLen > 0) {
                    for(int i=cursorPos-1; i<codeLen; i++) {
                        code[i] = code[i+1];
                    }
                    codeLen--;
                    cursorPos--;
                }
                lastKey = -1;
            }
            else if (key == KEY_HASH) {
                /* Enter/Newline */
                if (codeLen < MAX_CODE_LEN - 1) {
                    for(int i=codeLen; i>=cursorPos; i--) {
                        code[i+1] = code[i];
                    }
                    code[cursorPos] = '\n';
                    codeLen++;
                    cursorPos++;
                }
                lastKey = -1;
            }
            else if (key == KEY_LEFT && cursorPos > 0) {
                cursorPos--;
                lastKey = -1;
            }
            else if (key == KEY_RIGHT && cursorPos < codeLen) {
                cursorPos++;
                lastKey = -1;
            }
            else {
                /* T9-style input */
                int keyNum = -1;
                if (key == KEY_0) keyNum = 0;
                else if (key == KEY_1) keyNum = 1;
                else if (key == KEY_2) keyNum = 2;
                else if (key == KEY_3) keyNum = 3;
                else if (key == KEY_4) keyNum = 4;
                else if (key == KEY_5) keyNum = 5;
                else if (key == KEY_6) keyNum = 6;
                else if (key == KEY_7) keyNum = 7;
                else if (key == KEY_8) keyNum = 8;
                else if (key == KEY_9) keyNum = 9;

                if (keyNum >= 0 && codeLen < MAX_CODE_LEN - 1) {
                    const char* chars = keyChars[keyNum];
                    int numChars = 0;
                    while(chars[numChars]) numChars++;

                    if (keyNum == lastKey && (now - lastKeyTime) < 1000) {
                        /* Same key, cycle character */
                        charIndex = (charIndex + 1) % numChars;
                        code[cursorPos - 1] = chars[charIndex];
                    }
                    else {
                        /* New key, insert character */
                        for(int i=codeLen; i>=cursorPos; i--) {
                            code[i+1] = code[i];
                        }
                        charIndex = 0;
                        code[cursorPos] = chars[charIndex];
                        codeLen++;
                        cursorPos++;
                    }

                    lastKey = keyNum;
                    lastKeyTime = now;
                }
            }

            USC_Pause_us(150000); // Debounce
        }

        USC_Pause_us(30000);
    }

    Menu_InvalidateCache();
}


static void Action_Bluetooth(void) {
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_FillRect(0, 0, 240, 30, 0x001F);
    LCD_DrawString(70, 8, "BLUETOOTH", 0xFFFF, 0x001F);
    
    boolean isOn = BT_IsOn();
    LCD_DrawString(20, 50, "POWER:", 0xFFFF, 0x0000);
    if (isOn) {
        LCD_FillRect(100, 48, 60, 20, 0x07E0);
        LCD_DrawString(110, 50, "ON", 0x0000, 0x07E0);
    } else {
        LCD_FillRect(100, 48, 60, 20, 0xF800);
        LCD_DrawString(110, 50, "OFF", 0xFFFF, 0xF800);
    }
    
    LCD_DrawString(30, 95, "TOGGLE POWER", 0xFFFF, 0x2104);
    LCD_DrawString(30, 125, "SCAN DEVICES", 0xFFFF, 0x0000);
    LCD_DrawString(40, 280, "PRESS BACK/OK", 0xFFE0, 0x0000);

    int selected = 0;
    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
            
            if (key == KEY_UP || key == KEY_DOWN) {
                selected = !selected;
                // Redraw selection
                 LCD_FillRect(20, 90, 200, 25, (selected==0)?0x2104:0x0000);
                 LCD_DrawString(30, 95, "TOGGLE POWER", 0xFFFF, (selected==0)?0x2104:0x0000);
                 
                 LCD_FillRect(20, 120, 200, 25, (selected==1)?0x2104:0x0000);
                 LCD_DrawString(30, 125, "SCAN DEVICES", 0xFFFF, (selected==1)?0x2104:0x0000);
            }
            
            if (key == KEY_OK) {
                if (selected == 0) {
                     BT_Power(!isOn);
                     isOn = BT_IsOn();
                     // Redraw Status
                    if (isOn) {
                        LCD_FillRect(100, 48, 60, 20, 0x07E0);
                        LCD_DrawString(110, 50, "ON", 0x0000, 0x07E0);
                    } else {
                        LCD_FillRect(100, 48, 60, 20, 0xF800);
                        LCD_DrawString(110, 50, "OFF", 0xFFFF, 0xF800);
                    }
                } else {
                    LCD_DrawString(60, 160, "Scanning...", 0x07E0, 0x0000);
                    USC_Pause_us(1000000);
                    LCD_DrawString(60, 160, "No Devices  ", 0xF800, 0x0000);
                }
            }
        }
        USC_Pause_us(50000);
    }
}

static void Action_Flashlight(void)
{
    uint32_t originalBrightness = BL_GetValue();
    
    // Max brightness and turn on
    BL_SetupValue(100);
    BL_TurnOn(true);
    
    // Private GPIO Flashlight (Experimental)
    // Configure GPIO49 (LCD_SDA1) and GPIO50 (LCD_LPTE) as Output High
    GPIO_Setup(GPIO49, GPMODE(GPIO49_MODE_IO)); 
    GPIO_SETDIROUT(GPIO49);
    GPIO_DATAOUT(GPIO49, true); // High

    GPIO_Setup(GPIO50, GPMODE(0)); // GPIO50_MODE_IO is 0
    GPIO_SETDIROUT(GPIO50);
    GPIO_DATAOUT(GPIO50, true); // High

    // White Screen
    LCD_FillRect(0, 0, 240, 320, 0xFFFF);
    
    LCD_DrawString(40, 150, "FLASHLIGHT ON", 0x0000, 0xFFFF);
    LCD_DrawString(40, 170, "PRESS ANY KEY OFF", 0x0000, 0xFFFF);

    // Wait for exit
    while (!KP_IsKeyPressed()) {
         KeepBacklightOn();
         RGU_RestartWDT();
         USC_Pause_us(50000);
    }
    KP_ReadKey();

    // Restore GPIOs (Off)
    GPIO_DATAOUT(GPIO49, false);
    GPIO_DATAOUT(GPIO50, false);
    
    // Restore brightness
    BL_SetupValue(originalBrightness);
    BL_TurnOn(true); // Ensure it's on
    
    // Clear screen
    LCD_FillRect(0, 0, 240, 320, 0x0000); 
}

static void Action_TaskManager(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 5, "TASK MANAGER", 0x07E0, 0x0000); // Green title
    LCD_DrawString(0, 25, "PID   ADDR      INT   FLG", 0x7BEF, 0x0000); // Gray header
    LCD_FillRect(0, 40, 240, 1, 0x7BEF);

    int y = 50;
    int count = 0;

    if (TimersList != NULL)
    {
        pDLITEM item = DL_GetFirstItem(TimersList);
        while (item != NULL && y < 300)
        {
            pTIMER timer = (pTIMER)item->Data;
            if (timer != NULL)
            {
                char buf[40];
                // PID (Index), Handler Address (Pseudo-name), Interval (ms), Flags
                // Flags: E=Enabled, A=Auto, D=Direct
                char flags[4] = "---";
                if (timer->Flags & TF_ENABLED) flags[0] = 'E';
                if (timer->Flags & TF_AUTOREPEAT) flags[1] = 'A';
                if (timer->Flags & TF_DIRECT) flags[2] = 'D';

                sprintf(buf, "%02d  %08X  %4d  %s", count, (uint32_t)timer->Handler, timer->Interval/1000, flags);
                
                // Alternate row color
                uint16_t color = (count % 2 == 0) ? 0xFFFF : 0xFFE0;
                LCD_DrawString(5, y, buf, color, 0x0000);
                
                y += 16;
                count++;
            }
            item = DL_GetNextItem(item);
        }
    }
    else
    {
        LCD_DrawString(20, 100, "NO TASKS RUNNING", 0xF800, 0x0000);
    }

    char statBuf[30];
    sprintf(statBuf, "Total Tasks: %d", count);
    LCD_DrawString(5, 305, statBuf, 0x07E0, 0x0000);

    /* Wait for input */
    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            if (KP_ReadKey() == KEY_BACK) break;
        }
        USC_Pause_us(50000);
    }
}

static void Action_SDCardTest(void)
{
    SD_RunDiagnostic(MSDC_ITF0);

    LCD_DrawString(20, 280, "PRESS ANY KEY...", 0xFFFF, 0x0000);

    /* Wait for input */
    while(1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        if (KP_IsKeyPressed()) {
            KP_ReadKey(); // Consume key
            break;
        }
    }
}

/* 3D Cube Demo - Integer Fixed-Point Version */

/* Fixed-point: 8-bit fraction (multiply by 256) */
#define FP_SHIFT 8
#define FP_ONE (1 << FP_SHIFT)

/* Sine table (0-90 degrees, scaled by 256) */
static const int16_t sin_table[91] = {
    0,4,9,13,18,22,27,31,36,40,44,49,53,58,62,66,71,75,79,83,
    88,92,96,100,104,108,112,116,120,124,128,131,135,139,143,146,150,153,157,160,
    164,167,171,174,177,181,184,187,190,193,196,198,201,204,207,209,212,214,217,219,
    221,223,226,228,230,232,234,235,237,239,240,242,243,245,246,247,248,249,250,251,
    252,252,253,254,254,255,255,255,256,256,256
};

static int fp_sin(int angle) {
    angle = angle % 360;
    if (angle < 0) angle += 360;
    if (angle <= 90) return sin_table[angle];
    if (angle <= 180) return sin_table[180 - angle];
    if (angle <= 270) return -sin_table[angle - 180];
    return -sin_table[360 - angle];
}

static int fp_cos(int angle) {
    return fp_sin(angle + 90);
}

static void Action_3DCube(void)
{
    /* Cube vertices (scaled by FP_ONE) */
    int vx[8] = {-FP_ONE, FP_ONE, FP_ONE, -FP_ONE, -FP_ONE, FP_ONE, FP_ONE, -FP_ONE};
    int vy[8] = {-FP_ONE, -FP_ONE, FP_ONE, FP_ONE, -FP_ONE, -FP_ONE, FP_ONE, FP_ONE};
    int vz[8] = {-FP_ONE, -FP_ONE, -FP_ONE, -FP_ONE, FP_ONE, FP_ONE, FP_ONE, FP_ONE};
    
    const int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
    
    int angleX = 0, angleY = 0;
    
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 10, "3D CUBE DEMO", 0x07E0, 0x0000);
    LCD_DrawString(60, 300, "BACK: Exit", 0x7BEF, 0x0000);
    
    while (1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        
        if (KP_IsKeyPressed()) {
            if (KP_ReadKey() == KEY_BACK) break;
        }
        
        LCD_FillRect(40, 80, 160, 160, 0x0000);
        
        int projX[8], projY[8];
        
        for(int i=0; i<8; i++) {
            int x = vx[i], y = vy[i], z = vz[i];
            
            /* Rotate around X axis */
            int cosA = fp_cos(angleX), sinA = fp_sin(angleX);
            int ty = (y * cosA - z * sinA) >> FP_SHIFT;
            int tz = (y * sinA + z * cosA) >> FP_SHIFT;
            y = ty; z = tz;
            
            /* Rotate around Y axis */
            int cosB = fp_cos(angleY), sinB = fp_sin(angleY);
            int tx = (x * cosB - z * sinB) >> FP_SHIFT;
            tz = (x * sinB + z * cosB) >> FP_SHIFT;
            x = tx; z = tz;
            
            /* Perspective projection */
            int dist = 4 * FP_ONE;
            int zf = (FP_ONE * FP_ONE) / (dist - z);
            
            projX[i] = 120 + ((x * zf * 60) >> (FP_SHIFT * 2));
            projY[i] = 160 + ((y * zf * 60) >> (FP_SHIFT * 2));
        }
        
        /* Draw edges with different colors per face */
        /* Front face (0-3): Red */
        uint16_t colors[12] = {
            0xF800, 0xF800, 0xF800, 0xF800,  /* Front face - Red */
            0x001F, 0x001F, 0x001F, 0x001F,  /* Back face - Blue */
            0xFFE0, 0x07E0, 0xF81F, 0x07FF   /* Connecting edges - Yellow, Green, Magenta, Cyan */
        };
        
        for(int i=0; i<12; i++) {
            LCD_DrawLine(projX[edges[i][0]], projY[edges[i][0]],
                         projX[edges[i][1]], projY[edges[i][1]], colors[i]);
        }
        
        angleX = (angleX + 3) % 360;
        angleY = (angleY + 2) % 360;
        USC_Pause_us(30000);
    }
    Menu_InvalidateCache();
}

/* MSDC Register Viewer */
static void Action_MSDCRegs(void)
{
    volatile uint32_t *msdc_base = (volatile uint32_t *)0xA0130000; /* MSDC0 base - correct address */
    int scroll = 0, lastScroll = -1;
    const int maxRegs = 20;
    const char* regNames[] = {
        "MSDC_CFG", "MSDC_STA", "MSDC_INT", "MSDC_PS",
        "MSDC_DAT", "MSDC_IOCON", "MSDC_IOCON1", "RSVD0",
        "SDC_CFG", "SDC_CMD", "SDC_ARG", "SDC_STA",
        "SDC_RESP0", "SDC_RESP1", "SDC_RESP2", "SDC_RESP3",
        "SDC_CMDSTA", "SDC_DATSTA", "SDC_CSTA", "SDC_IRQM0"
    };
    
    while (1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        
        /* Only redraw if scroll changed */
        if (scroll != lastScroll) {
            lastScroll = scroll;
            
            LCD_FillRect(0, 0, 240, 320, 0x0000);
            LCD_DrawString(50, 5, "MSDC0 REGISTERS", 0x07E0, 0x0000);
            LCD_FillRect(0, 22, 240, 1, 0x07E0);
            
            for (int i = 0; i < 12 && (scroll + i) < maxRegs; i++) {
                int idx = scroll + i;
                char buf[30];
                sprintf(buf, "%s:", regNames[idx]);
                LCD_DrawString(5, 28 + i * 20, buf, 0xFFE0, 0x0000);
                sprintf(buf, "%08X", (unsigned int)msdc_base[idx]);
                LCD_DrawString(120, 28 + i * 20, buf, 0xFFFF, 0x0000);
            }
            
            LCD_DrawString(5, 300, "UP/DOWN:Scroll BACK:Exit", 0x7BEF, 0x0000);
        }
        
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
            if (key == KEY_UP && scroll > 0) scroll--;
            if (key == KEY_DOWN && scroll < maxRegs - 12) scroll++;
        }
        USC_Pause_us(50000);
    }
    Menu_InvalidateCache();
}

/* 3D Maze Game - Raycasting */
static void Action_3DMaze(void)
{
    /* Map: 8x8, 1=wall, 0=empty */
    const uint8_t map[8][8] = {
        {1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,0,1},
        {1,0,1,0,0,1,0,1},
        {1,0,0,0,1,1,0,1},
        {1,0,1,0,0,0,0,1},
        {1,0,0,0,1,0,0,1},
        {1,1,1,1,1,1,1,1}
    };
    
    /* Player position (fixed-point, 8-bit fraction) */
    int px = 384, py = 384; /* 1.5, 1.5 in map coords */
    int pdir = 0; /* 0-359 degrees */
    int lastPx = -1, lastPy = -1, lastDir = -1;
    
    while (1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        
        /* Handle input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
            if (key == KEY_LEFT) pdir = (pdir + 350) % 360;
            if (key == KEY_RIGHT) pdir = (pdir + 10) % 360;
            if (key == KEY_UP) {
                int dx = (fp_cos(pdir) * 20) >> FP_SHIFT;
                int dy = (fp_sin(pdir) * 20) >> FP_SHIFT;
                int nx = (px + dx) >> 8, ny = (py - dy) >> 8;
                if (map[ny][nx] == 0) { px += dx; py -= dy; }
            }
            if (key == KEY_DOWN) {
                int dx = (fp_cos(pdir) * 20) >> FP_SHIFT;
                int dy = (fp_sin(pdir) * 20) >> FP_SHIFT;
                int nx = (px - dx) >> 8, ny = (py + dy) >> 8;
                if (map[ny][nx] == 0) { px -= dx; py += dy; }
            }
        }
        
        /* Only redraw if position/direction changed */
        if (px != lastPx || py != lastPy || pdir != lastDir) {
            lastPx = px; lastPy = py; lastDir = pdir;
            
            /* Raycasting - cast 120 rays for 240 pixel width (2 pixels per column) */
            for (int col = 0; col < 120; col++) {
                int rayAngle = (pdir - 30 + col / 2) % 360;
                if (rayAngle < 0) rayAngle += 360;
                
                int rayX = px, rayY = py;
                int stepX = (fp_cos(rayAngle) * 8) >> FP_SHIFT;
                int stepY = (fp_sin(rayAngle) * 8) >> FP_SHIFT;
                
                int dist = 0;
                boolean hitWall = false;
                
                /* DDA - step until wall hit or max distance */
                while (!hitWall && dist < 2000) {
                    rayX += stepX;
                    rayY -= stepY;
                    int mapX = rayX >> 8, mapY = rayY >> 8;
                    
                    if (mapX >= 0 && mapX < 8 && mapY >= 0 && mapY < 8) {
                        if (map[mapY][mapX] == 1) hitWall = true;
                    } else {
                        hitWall = true;
                    }
                    dist += 8;
                }
                
                /* Fix fisheye */
                int angleDiff = col / 2 - 30;
                int cosAdj = fp_cos(angleDiff);
                dist = (dist * cosAdj) >> FP_SHIFT;
                
                /* Calculate wall height */
                int wallHeight = (dist > 0) ? (16000 / dist) : 320;
                if (wallHeight > 320) wallHeight = 320;
                
                int wallTop = 160 - wallHeight / 2;
                int wallBot = 160 + wallHeight / 2;
                if (wallTop < 0) wallTop = 0;
                if (wallBot > 320) wallBot = 320;
                
                /* Color based on distance */
                uint16_t wallColor = 0xF800; /* Red */
                if (dist > 500) wallColor = 0xC000;
                if (dist > 1000) wallColor = 0x8000;
                
                /* Draw complete column: ceiling + wall + floor */
                int x = col * 2;
                LCD_FillRect(x, 0, 2, wallTop, 0x4208);         /* Ceiling */
                LCD_FillRect(x, wallTop, 2, wallBot - wallTop, wallColor); /* Wall */
                LCD_FillRect(x, wallBot, 2, 320 - wallBot, 0x18C3);  /* Floor */
            }
            
            /* Mini-map (only redraw on change) */
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    uint16_t c = map[y][x] ? 0xFFFF : 0x0000;
                    LCD_FillRect(200 + x * 4, 4 + y * 4, 3, 3, c);
                }
            }
            /* Player dot */
            LCD_FillRect(200 + (px >> 8) * 4, 4 + (py >> 8) * 4, 2, 2, 0x07E0);
            
            LCD_DrawString(5, 305, "ARROWS:Move BACK:Exit", 0xFFFF, 0x0000);
        }
        
        USC_Pause_us(30000);
    }
    Menu_InvalidateCache();
}

/* 2D Platformer Game */
static void Action_Platformer(void)
{
    /* Platform data: {x, y, width} */
    const int platforms[][3] = {
        {0, 280, 240},    /* Ground */
        {20, 230, 60},    /* Platform 1 */
        {100, 190, 50},   /* Platform 2 */
        {160, 150, 60},   /* Platform 3 */
        {60, 110, 70},    /* Platform 4 */
        {140, 70, 50},    /* Platform 5 */
        {10, 40, 40},     /* Platform 6 - top */
    };
    const int numPlatforms = 7;
    
    /* Player state */
    int px = 100, py = 260;  /* Position */
    int vy = 0;              /* Vertical velocity */
    const int pw = 12, ph = 16; /* Player size */
    const int gravity = 1;
    const int jumpForce = -10;
    boolean onGround = false;
    int score = 0;
    int highestY = 260;
    
    /* Previous state for dirty detection */
    int lastPx = -1, lastPy = -1, lastScore = -1;
    
    /* Draw static background once */
    for (int y = 0; y < 32; y++) {
        uint16_t c = ((15 - y/2) << 11) | (20 << 5) | 31;
        LCD_FillRect(0, y * 10, 240, 10, c);
    }
    for (int i = 0; i < numPlatforms; i++) {
        uint16_t platColor = (i == 0) ? 0x18C3 : 0x3280;
        LCD_FillRect(platforms[i][0], platforms[i][1], platforms[i][2], 8, platColor);
        LCD_FillRect(platforms[i][0], platforms[i][1], platforms[i][2], 2, 0x07E0);
    }
    
    /* Game loop */
    while (1) {
        RGU_RestartWDT();
        KeepBacklightOn();
        
        /* Handle input */
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            if (key == KEY_BACK) break;
            if (key == KEY_LEFT && px > 0) px -= 5;
            if (key == KEY_RIGHT && px < 240 - pw) px += 5;
            if (key == KEY_UP && onGround) {
                vy = jumpForce;
                onGround = false;
            }
        }
        
        /* Apply gravity */
        vy += gravity;
        if (vy > 10) vy = 10;
        py += vy;
        
        /* Check platform collisions */
        onGround = false;
        for (int i = 0; i < numPlatforms; i++) {
            int platX = platforms[i][0];
            int platY = platforms[i][1];
            int platW = platforms[i][2];
            
            if (vy >= 0 && px + pw > platX && px < platX + platW) {
                if (py + ph >= platY && py + ph <= platY + 10) {
                    py = platY - ph;
                    vy = 0;
                    onGround = true;
                }
            }
        }
        
        /* Screen boundaries */
        if (py < 0) py = 0;
        if (py > 280) py = 280;
        if (px < 0) px = 0;
        if (px > 240 - pw) px = 240 - pw;
        
        /* Update score */
        if (py < highestY) {
            score += (highestY - py);
            highestY = py;
        }
        
        /* Only redraw if position or score changed */
        if (px != lastPx || py != lastPy || score != lastScore) {
            /* Erase old player position */
            if (lastPx >= 0) {
                uint16_t bgColor = 0x5DDF; /* Sky blue */
                LCD_FillRect(lastPx, lastPy, pw, ph, bgColor);
            }
            
            /* Draw player at new position */
            LCD_FillRect(px, py + 6, pw, ph - 6, 0xF800);
            LCD_FillRect(px + 2, py, pw - 4, 6, 0xFE00);
            
            /* Update score display */
            if (score != lastScore) {
                LCD_FillRect(5, 5, 100, 10, 0x0000);
                char scoreBuf[20];
                sprintf(scoreBuf, "SCORE:%d", score);
                LCD_DrawString(5, 5, scoreBuf, 0xFFFF, 0x0000);
            }
            
            lastPx = px; lastPy = py; lastScore = score;
        }
        
        USC_Pause_us(25000);
    }
    Menu_InvalidateCache();
}

/* 3D Freekick Game */
static void Action_Freekick(void)
{
    int score = 0, shots = 5;
    int aimX = 120, aimY = 100;
    int gkX = 90, gkDir = 1;
    
    /* Draw static scene once */
    LCD_FillRect(0, 0, 240, 160, 0x5DDF);  /* Sky */
    LCD_FillRect(30, 40, 6, 120, 0xFFFF);  /* Left post */
    LCD_FillRect(204, 40, 6, 120, 0xFFFF); /* Right post */
    LCD_FillRect(30, 40, 180, 6, 0xFFFF);  /* Crossbar */
    LCD_FillRect(36, 46, 168, 114, 0x0400); /* Goal area */
    LCD_FillRect(0, 160, 240, 160, 0x07E0); /* Grass */
    LCD_FillRect(115, 275, 10, 10, 0xFFFF); /* Ball */
    LCD_DrawString(140, 305, "OK:SHOOT", 0xFFFF, 0x07E0);
    
    int lastGkX = -1, lastAimX = -1, lastAimY = -1;
    
    while (shots > 0) {
        RGU_RestartWDT();
        KeepBacklightOn();
        
        boolean shooting = false;
        int targetX, targetY;
        
        /* Aiming phase */
        while (!shooting && shots > 0) {
            RGU_RestartWDT();
            KeepBacklightOn();
            
            /* Move goalkeeper */
            int oldGkX = gkX;
            gkX += gkDir * 2;
            if (gkX > 160) gkDir = -1;
            if (gkX < 60) gkDir = 1;
            
            /* Handle input */
            int oldAimX = aimX, oldAimY = aimY;
            if (KP_IsKeyPressed()) {
                TKEY key = KP_ReadKey();
                if (key == KEY_BACK) { shots = 0; break; }
                if (key == KEY_LEFT && aimX > 40) aimX -= 8;
                if (key == KEY_RIGHT && aimX < 200) aimX += 8;
                if (key == KEY_UP && aimY > 50) aimY -= 8;
                if (key == KEY_DOWN && aimY < 150) aimY += 8;
                if (key == KEY_OK) {
                    shooting = true;
                    targetX = aimX; targetY = aimY;
                }
            }
            
            /* Only update goalkeeper if moved */
            if (gkX != lastGkX) {
                if (lastGkX >= 0) {
                    LCD_FillRect(lastGkX, 90, 20, 50, 0x0400); /* Erase old */
                }
                LCD_FillRect(gkX, 100, 20, 40, 0xFFE0);     /* Body */
                LCD_FillRect(gkX + 6, 90, 8, 10, 0xFE00);  /* Head */
                lastGkX = gkX;
            }
            
            /* Only update crosshair if moved */
            if (aimX != lastAimX || aimY != lastAimY) {
                if (lastAimX >= 0) {
                    /* Erase old crosshair - draw over with background */
                    LCD_FillRect(lastAimX - 10, lastAimY, 20, 2, 0x0400);
                    LCD_FillRect(lastAimX, lastAimY - 10, 2, 20, 0x0400);
                }
                LCD_FillRect(aimX - 10, aimY, 20, 2, 0xF800);
                LCD_FillRect(aimX, aimY - 10, 2, 20, 0xF800);
                lastAimX = aimX; lastAimY = aimY;
            }
            
            /* Update shots/score display */
            char buf[30];
            sprintf(buf, "SHOTS:%d SCORE:%d", shots, score);
            LCD_FillRect(5, 5, 130, 10, 0x5DDF);
            LCD_DrawString(5, 5, buf, 0xFFFF, 0x0000);
            
            USC_Pause_us(40000);
        }
        
        /* Shooting animation */
        if (shooting) {
            shots--;
            int ballX = 120, ballY = 280;
            
            for (int t = 0; t < 20; t++) {
                RGU_RestartWDT();
                
                /* Erase old ball */
                if (t > 0) {
                    int oldBX = 120 + ((targetX - 120) * (t-1)) / 20;
                    int oldBY = 280 - ((280 - targetY) * (t-1)) / 20;
                    int oldSize = 10 - ((t-1) / 3);
                    if (oldSize < 4) oldSize = 4;
                    uint16_t bgColor = (oldBY < 160) ? 0x0400 : 0x07E0;
                    LCD_FillRect(oldBX - oldSize/2, oldBY - oldSize/2, oldSize, oldSize, bgColor);
                }
                
                /* Draw new ball */
                ballX = 120 + ((targetX - 120) * t) / 20;
                ballY = 280 - ((280 - targetY) * t) / 20;
                int ballSize = 10 - (t / 3);
                if (ballSize < 4) ballSize = 4;
                LCD_FillRect(ballX - ballSize/2, ballY - ballSize/2, ballSize, ballSize, 0xFFFF);
                
                USC_Pause_us(25000);
            }
            
            /* Check result */
            if (targetX > 36 && targetX < 200 && targetY > 46 && targetY < 160) {
                if (targetX > gkX - 15 && targetX < gkX + 35 && targetY > 90 && targetY < 150) {
                    LCD_DrawString(80, 130, "SAVED!", 0xFFE0, 0x0400);
                } else {
                    LCD_DrawString(80, 130, "GOAL!!!", 0x07E0, 0x0400);
                    score++;
                }
            } else {
                LCD_DrawString(80, 130, "MISSED!", 0xF800, 0x0400);
            }
            USC_Pause_us(800000);
            
            /* Redraw goal area to clear message */
            LCD_FillRect(36, 46, 168, 114, 0x0400);
            LCD_FillRect(115, 275, 10, 10, 0xFFFF);
            lastGkX = -1; lastAimX = -1; lastAimY = -1;
        }
    }
    
    /* Game over */
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(70, 140, "GAME OVER", 0xFFFF, 0x0000);
    char finalBuf[30];
    sprintf(finalBuf, "FINAL SCORE: %d/5", score);
    LCD_DrawString(60, 170, finalBuf, 0x07E0, 0x0000);
    LCD_DrawString(50, 200, "Press any key", 0x7BEF, 0x0000);
    
    while (!KP_IsKeyPressed()) {
        RGU_RestartWDT();
        USC_Pause_us(100000);
    }
    KP_ReadKey();
    
    Menu_InvalidateCache();
}
