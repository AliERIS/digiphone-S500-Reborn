/*
 * RAM File System - Volatile temporary storage
 * Stores small notes/bookmarks in RAM (lost on reset)
 */
#include "systemconfig.h"
#include "lcdif.h"
#include "keypad.h"
#include "utils.h"
#include "rgu.h"
#include <string.h>

/* External LCD functions */
extern void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void LCD_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg);

#define MAX_RAM_FILES 10
#define FILE_SIZE 200

typedef struct {
    boolean used;
    char data[FILE_SIZE];
} RAMFile;

static RAMFile ram_files[MAX_RAM_FILES];
static boolean initialized = false;

/* Initialize RAM file system */
static void InitRAMFiles(void)
{
    if (!initialized) {
        for (int i = 0; i < MAX_RAM_FILES; i++) {
            ram_files[i].used = false;
            ram_files[i].data[0] = '\0';
        }
        initialized = true;
    }
}

/* Create new file */
static int CreateFile(const char* content)
{
    for (int i = 0; i < MAX_RAM_FILES; i++) {
        if (!ram_files[i].used) {
            ram_files[i].used = true;
            strncpy(ram_files[i].data, content, FILE_SIZE - 1);
            ram_files[i].data[FILE_SIZE - 1] = '\0';
            return i;
        }
    }
    return -1; // No space
}

/* Delete file */
static void DeleteFile(int index)
{
    if (index >= 0 && index < MAX_RAM_FILES) {
        ram_files[index].used = false;
        ram_files[index].data[0] = '\0';
    }
}

/* View file list */
static void ViewFileList(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(60, 10, "RAM FILES", 0xFFFF, 0x0000);
    LCD_DrawString(20, 30, "VOLATILE STORAGE", 0xF800, 0x0000);
    
    int count = 0;
    uint16_t yPos = 60;
    
    for (int i = 0; i < MAX_RAM_FILES; i++) {
        if (ram_files[i].used) {
            char line[30];
            line[0] = 'F';
            line[1] = 'I';
            line[2] = 'L';
            line[3] = 'E';
            line[4] = ' ';
            line[5] = '0' + (i / 10);
            line[6] = '0' + (i % 10);
            line[7] = ':';
            line[8] = ' ';
            
            // Copy first 20 chars of content
            int j;
            for (j = 0; j < 20 && ram_files[i].data[j] != '\0'; j++) {
                line[9 + j] = ram_files[i].data[j];
            }
            line[9 + j] = '\0';
            
            LCD_DrawString(20, yPos, line, 0x07E0, 0x0000);
            yPos += 20;
            count++;
        }
    }
    
    if (count == 0) {
        LCD_DrawString(60, 100, "NO FILES", 0x7BEF, 0x0000);
        LCD_DrawString(40, 120, "PRESS OK TO", 0x7BEF, 0x0000);
        LCD_DrawString(40, 140, "CREATE ONE", 0x7BEF, 0x0000);
    }
    
    // Stats
    char stats[30];
    stats[0] = 'F';
    stats[1] = 'I';
    stats[2] = 'L';
    stats[3] = 'E';
    stats[4] = 'S';
    stats[5] = ':';
    stats[6] = ' ';
    stats[7] = '0' + count;
    stats[8] = '/';
    stats[9] = '1';
    stats[10] = '0';
    stats[11] = '\0';
    LCD_DrawString(20, 260, stats, 0xFFE0, 0x0000);
    
    LCD_DrawString(20, 280, "OK:NEW #:CLEAR", 0xFFE0, 0x0000);
    LCD_DrawString(20, 300, "BACK:EXIT", 0xFFE0, 0x0000);
}

/* RAM Files main function */
void Action_RAMFiles(void)
{
    InitRAMFiles();
    KP_ClearKeyBuffer();
    boolean running = true;
    
    // Draw once
    ViewFileList();
    
    while (running) {
        RGU_RestartWDT();
        
        // Wait for input
        while (!KP_IsKeyPressed() && running) {
            RGU_RestartWDT();
            USC_Pause_us(50000);
        }
        
        if (KP_IsKeyPressed()) {
            TKEY key = KP_ReadKey();
            
            switch (key) {
                case KEY_OK:
                    // Create sample note
                    {
                        char note[FILE_SIZE];
                        note[0] = 'N';
                        note[1] = 'O';
                        note[2] = 'T';
                        note[3] = 'E';
                        note[4] = ' ';
                        note[5] = 'C';
                        note[6] = 'R';
                        note[7] = 'E';
                        note[8] = 'A';
                        note[9] = 'T';
                        note[10] = 'E';
                        note[11] = 'D';
                        note[12] = '!';
                        note[13] = '\0';
                        
                        int idx = CreateFile(note);
                        if (idx == -1) {
                            LCD_FillRect(40, 150, 160, 40, 0xF800);
                            LCD_DrawString(50, 160, "NO SPACE!", 0xFFFF, 0xF800);
                            USC_Pause_us(1000000);
                        }
                        // Redraw list
                        ViewFileList();
                    }
                    break;
                    
                case KEY_HASH:
                    // Clear all files
                    for (int i = 0; i < MAX_RAM_FILES; i++) {
                        DeleteFile(i);
                    }
                    // Redraw list
                    ViewFileList();
                    break;
                    
                case KEY_BACK:
                    running = false;
                    break;
                    
                default:
                    break;
            }
            
            USC_Pause_us(200000);
        }
    }
}
