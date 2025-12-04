/*
 * Original Firmware Loader - Full Implementation
 * Loads PRIMARY_MAUI + VIVA from SD card (bootloaders skipped)
 */
#include "systemconfig.h"
#include "fw_loader.h"
#include "fat32.h"
#include "lcdif.h"
#include "keypad.h"
#include "rgu.h"
#include "debug.h"
#include "sdcard.h"

/* Minimal firmware parts (no bootloaders needed) */
typedef struct {
    const char* filename;
    uint32_t target_address;
    uint32_t max_size;
} FirmwarePartMinimal;

static const FirmwarePartMinimal MinimalFirmwareParts[] = {
    { "PRIMARY_MAUI.BIN",   0x10100000, 0x500000 },  // ~5MB max
    { "VIVA.BIN",           0x100F3DF4, 0x50000  }   // ~320KB max
};

#define MINIMAL_FW_PARTS 2

/* Load a single firmware part from SD card to RAM */
static boolean LoadFirmwarePart(const FirmwarePartMinimal* part)
{
    uint8_t* dest = (uint8_t*)part->target_address;
    uint32_t bytesLoaded = 0;
    uint8_t sectorBuffer[512];
    
    DebugPrint("Loading %s to 0x%08X...\n", part->filename, part->target_address);
    
    // Find file in root directory
    FileEntry files[50];
    int fileCount = FAT32_ListRoot(files, 50);
    
    int fileIndex = -1;
    for (int i = 0; i < fileCount; i++) {
        // Simple case-insensitive compare
        boolean match = true;
        for (int j = 0; part->filename[j] != '\0' && files[i].name[j] != '\0'; j++) {
            char c1 = part->filename[j];
            char c2 = files[i].name[j];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
        }
        if (match) {
            fileIndex = i;
            break;
        }
    }
    
    if (fileIndex == -1) {
        DebugPrint("File not found: %s\n", part->filename);
        return false;
    }
    
    uint32_t cluster = files[fileIndex].firstCluster;
    uint32_t fileSize = files[fileIndex].size;
    
    // Check size limit
    if (fileSize > part->max_size) {
        DebugPrint("File too large: %d bytes (max %d)\n", fileSize, part->max_size);
        return false;
    }
    
    DebugPrint("File found, size: %d bytes\n", fileSize);
    
    // Read file cluster by cluster
    while (cluster < FAT32_EOC && bytesLoaded < fileSize) {
        RGU_RestartWDT();
        
        // Read cluster sectors
        extern FAT32_Context g_fat32;
        uint32_t firstSector = g_fat32.dataStart + ((cluster - 2) * g_fat32.sectorsPerCluster);
        
        for (uint8_t s = 0; s < g_fat32.sectorsPerCluster && bytesLoaded < fileSize; s++) {
            if (!SD_ReadBlock(MSDC_ITF1, firstSector + s, sectorBuffer)) {
                DebugPrint("Failed to read sector\n");
                return false;
            }
            
            uint32_t bytesToCopy = (fileSize - bytesLoaded > 512) ? 512 : (fileSize - bytesLoaded);
            
            // Copy to destination
            for (uint32_t i = 0; i < bytesToCopy; i++) {
                dest[bytesLoaded + i] = sectorBuffer[i];
            }
            
            bytesLoaded += bytesToCopy;
        }
        
        // Get next cluster
        cluster = GetFATEntry(cluster);
    }
    
    DebugPrint("Loaded %d bytes\n", bytesLoaded);
    return (bytesLoaded > 0);
}

/* Load minimal firmware parts from SD card */
boolean FW_LoadOriginalFirmware(void)
{
    LCD_FillRect(0, 0, 240, 320, 0x0000);
    LCD_DrawString(20, 10, "LOAD ORIGINAL FW", 0xFFFF, 0x0000);
    LCD_DrawString(20, 30, "(Minimal - No BL)", 0x7BEF, 0x0000);
    
    // Check if FAT32 is initialized
    if (!FAT32_IsInitialized()) {
        LCD_DrawString(20, 60, "SD Init...", 0x7BEF, 0x0000);
        if (!FAT32_Init()) {
            LCD_DrawString(20, 80, "SD INIT FAILED!", 0xF800, 0x0000);
            LCD_DrawString(20, 100, "Check SD card", 0xF800, 0x0000);
            LCD_DrawString(20, 280, "BACK: Return", 0xFFE0, 0x0000);
            return false;
        }
    }
    
    // Load each firmware part
    for (int i = 0; i < MINIMAL_FW_PARTS; i++) {
        RGU_RestartWDT();
        
        // Show progress
        LCD_FillRect(20, 60, 200, 60, 0x0000);
        char msg[32];
        int len = 0;
        msg[len++] = 'L';
        msg[len++] = 'o';
        msg[len++] = 'a';
        msg[len++] = 'd';
        msg[len++] = 'i';
        msg[len++] = 'n';
        msg[len++] = 'g';
        msg[len++] = ' ';
        msg[len++] = '0' + (i + 1);
        msg[len++] = '/';
        msg[len++] = '0' + MINIMAL_FW_PARTS;
        msg[len++] = '\0';
        
        LCD_DrawString(20, 60, msg, 0x07E0, 0x0000);
        LCD_DrawString(20, 80, MinimalFirmwareParts[i].filename, 0x7BEF, 0x0000);
        
        if (!LoadFirmwarePart(&MinimalFirmwareParts[i])) {
            LCD_FillRect(20, 100, 200, 40, 0x0000);
            LCD_DrawString(20, 100, "LOAD FAILED!", 0xF800, 0x0000);
            LCD_DrawString(20, 120, "Check files:", 0xF800, 0x0000);
            LCD_DrawString(20, 140, MinimalFirmwareParts[i].filename, 0xF800, 0x0000);
            LCD_DrawString(20, 280, "BACK: Return", 0xFFE0, 0x0000);
            return false;
        }
    }
    
    LCD_FillRect(20, 60, 200, 60, 0x0000);
    LCD_DrawString(20, 60, "LOAD COMPLETE!", 0x07E0, 0x0000);
    LCD_DrawString(20, 80, "Jumping...", 0xFFE0, 0x0000);
    
    USC_Pause_us(1000000); // 1 second delay
    
    return true;
}

/* Jump to original firmware entry point */
void FW_JumpToOriginalFirmware(void)
{
    typedef void (*firmware_entry_t)(void);
    
    // Entry point is at PRIMARY_MAUI start address
    firmware_entry_t entry = (firmware_entry_t)0x10100000;
    
    DebugPrint("Jumping to original firmware at 0x%08X\n", (uint32_t)entry);
    
    // Disable interrupts before jump
    __disable_interrupts();
    
    // Jump to original firmware
    entry();
    
    // Should never reach here
    while(1) {
        RGU_RestartWDT();
    }
}
