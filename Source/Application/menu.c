/*
 * Simple Menu System Implementation
 */
#include "systemconfig.h"
#include "menu.h"
#include "lcdif.h"
#include "fat32.h"

/* Forward declarations for menu actions */
static void Action_Settings(void);
static void Action_Games(void);
static void Action_BacklightSettings(void);
static void Action_SystemInfo(void);
static void Action_About(void);
static void Action_Snake(void);
static void Action_Racing(void);
static void Action_FileManager(void);

/* Tools Submenu */
static MenuItem toolsItems[] = {
    { "BACKLIGHT", MENU_ACTION_FUNCTION, .action.function = Action_BacklightSettings },
    { "SYSTEM INFO", MENU_ACTION_FUNCTION, .action.function = Action_SystemInfo },
    { "FILE MANAGER", MENU_ACTION_FUNCTION, .action.function = Action_FileManager },
};

static Menu toolsMenu = {
    .title = "Tools",
    .items = toolsItems,
    .itemCount = 3,
    .selectedIndex = 0,
    .parent = NULL
};

/* Games Submenu */
static MenuItem gamesItems[] = {
    { "SNAKE", MENU_ACTION_FUNCTION, .action.function = Action_Snake },
    { "RACING", MENU_ACTION_FUNCTION, .action.function = Action_Racing },
};

static Menu gamesMenu = {
    .title = "GAMES",
    .items = gamesItems,
    .itemCount = 2,
    .selectedIndex = 0,
    .parent = NULL
};

/* Main Menu */
static MenuItem mainItems[] = {
    { "SETTINGS", MENU_ACTION_FUNCTION, .action.function = Action_Settings },
    { "GAMES", MENU_ACTION_SUBMENU, .action.submenu = &gamesMenu },
    { "TOOLS", MENU_ACTION_SUBMENU, .action.submenu = &toolsMenu },
    { "ABOUT", MENU_ACTION_FUNCTION, .action.function = Action_About },
};

static Menu mainMenu = {
    .title = "MAIN MENU",
    .items = mainItems,
    .itemCount = 4,
    .selectedIndex = 0,
    .parent = NULL
};

/* External drawing functions from appinit.c */
extern void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg);

void Menu_Initialize(void)
{
    /* Set parent pointers */
    gamesMenu.parent = &mainMenu;
    toolsMenu.parent = &mainMenu;
}

void Menu_InvalidateCache(void);

/* Render cache variables */
static uint8_t g_lastSelectedIndex = 255;
static Menu *g_lastMenu = NULL;

void Menu_Render(Menu *menu)
{
    const uint16_t MENU_X = 20;
    const uint16_t MENU_Y = 40;
    const uint16_t LINE_HEIGHT = 20;
    
    /* Full redraw only if menu changed */
    if (g_lastMenu != menu) {
        LCD_FillRect(0, 0, 240, 320, 0x0000); // Black
        
        /* Draw title */
        LCD_DrawString(MENU_X, 10, menu->title, 0xFFFF, 0x0000);
        
        /* Draw all items */
        for (uint8_t i = 0; i < menu->itemCount; i++) {
            uint16_t y = MENU_Y + (i * LINE_HEIGHT);
            uint16_t bgColor = (i == menu->selectedIndex) ? 0x001F : 0x0000;
            uint16_t fgColor = (i == menu->selectedIndex) ? 0xFFFF : 0x7BEF;
            
            if (i == menu->selectedIndex) {
                LCD_FillRect(MENU_X - 5, y - 2, 220, LINE_HEIGHT - 4, bgColor);
            }
            
            LCD_DrawString(MENU_X, y, menu->items[i].title, fgColor, bgColor);
        }
        
        /* Draw navigation hint */
        LCD_DrawString(20, 280, "UP/DOWN:MOVE OK:SELECT", 0x7BEF, 0x0000);
        if (menu->parent) {
            LCD_DrawString(20, 300, "BACK:RETURN", 0x7BEF, 0x0000);
        }
        
        g_lastMenu = menu;
        g_lastSelectedIndex = menu->selectedIndex;
    }
    /* Partial update if only selection changed */
    else if (g_lastSelectedIndex != menu->selectedIndex) {
        /* Erase old selection */
        uint16_t oldY = MENU_Y + (g_lastSelectedIndex * LINE_HEIGHT);
        LCD_FillRect(MENU_X - 5, oldY - 2, 220, LINE_HEIGHT - 4, 0x0000);
        LCD_DrawString(MENU_X, oldY, menu->items[g_lastSelectedIndex].title, 0x7BEF, 0x0000);
        
        /* Draw new selection */
        uint16_t newY = MENU_Y + (menu->selectedIndex * LINE_HEIGHT);
        LCD_FillRect(MENU_X - 5, newY - 2, 220, LINE_HEIGHT - 4, 0x001F);
        LCD_DrawString(MENU_X, newY, menu->items[menu->selectedIndex].title, 0xFFFF, 0x001F);
        
        g_lastSelectedIndex = menu->selectedIndex;
    }
}

void Menu_InvalidateCache(void)
{
    g_lastMenu = NULL;
    g_lastSelectedIndex = 255;
}

void Menu_NavigateUp(Menu *menu)
{
    if (menu->selectedIndex == 0) {
        menu->selectedIndex = menu->itemCount - 1; // Wrap to bottom
    } else {
        menu->selectedIndex--;
    }
}

void Menu_NavigateDown(Menu *menu)
{
    if (menu->selectedIndex >= menu->itemCount - 1) {
        menu->selectedIndex = 0; // Wrap to top
    } else {
        menu->selectedIndex++;
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
    uint16_t gameSpeed = 100000UL;  // Initial speed (100ms)
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
