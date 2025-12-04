/*
 * Minimal FAT32 Read-Only Filesystem Implementation
 */
#include "systemconfig.h"
#include "fat32.h"
#include "sdcard.h"
#include "msdc.h"
#include "memory.h"
#include "debug.h"
#include "rgu.h"

FAT32_Context g_fat32;
static uint8_t g_sectorBuffer[512];

/* Helper: Read sector from SD card */
static boolean ReadSector(uint32_t sector, void *buffer)
{
    RGU_RestartWDT(); // Restart watchdog before each sector read
    boolean result = SD_ReadBlock(MSDC_ITF1, sector, buffer);
    RGU_RestartWDT(); // Restart watchdog after sector read
    return result;
}

/* Helper: Get FAT entry for cluster */
uint32_t GetFATEntry(uint32_t cluster)
{
    if (cluster < 2 || cluster >= FAT32_EOC) {
        return FAT32_EOC; // Invalid cluster
    }
    
    RGU_RestartWDT(); // Restart watchdog before FAT read
    
    uint32_t fatOffset = cluster * 4;
    uint32_t fatSector = g_fat32.fatStart + (fatOffset / g_fat32.bytesPerSector);
    uint32_t entOffset = fatOffset % g_fat32.bytesPerSector;
    
    if (!ReadSector(fatSector, g_sectorBuffer)) {
        DebugPrint("FAT32: Failed to read FAT sector %d\n", fatSector);
        return FAT32_EOC; // Return EOC on error
    }
    
    RGU_RestartWDT(); // Restart watchdog after FAT read
    
    uint32_t entry = *(uint32_t*)&g_sectorBuffer[entOffset];
    entry = entry & 0x0FFFFFFF;
    
    /* Validate entry */
    if (entry == 0 || entry == FAT32_BAD) {
        return FAT32_EOC;
    }
    
    return entry;
}

/* Helper: Convert 8.3 name to readable string */
static void ConvertName(const uint8_t *rawName, char *output)
{
    int i, j = 0;
    
    /* Copy name part (8 chars) */
    for (i = 0; i < 8 && rawName[i] != ' '; i++) {
        output[j++] = rawName[i];
    }
    
    /* Add extension if exists */
    if (rawName[8] != ' ') {
        output[j++] = '.';
        for (i = 8; i < 11 && rawName[i] != ' '; i++) {
            output[j++] = rawName[i];
        }
    }
    
    output[j] = '\0';
}

#include "lcdif.h"

/* Initialize FAT32 filesystem */
boolean FAT32_Init(void)
{
    FAT32_BootSector *bs;
    
    RGU_RestartWDT(); // Restart watchdog at start
    
    LCD_DrawString(20, 120, "SD: START", 0xFFFF, 0x0000);
    
    /* Initialize SD card */
    RGU_RestartWDT(); // Restart watchdog before SD init
    if (!SD_Initialize(MSDC_ITF1)) {
        DebugPrint("SD Init failed\n");
        LCD_DrawString(20, 140, "SD: INIT FAIL", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog after SD init
    LCD_DrawString(20, 120, "SD: READ BOOT", 0xFFFF, 0x0000);
    
    /* Read boot sector */
    RGU_RestartWDT(); // Restart watchdog before reading boot sector
    if (!ReadSector(0, g_sectorBuffer)) {
        DebugPrint("Read boot sector failed\n");
        LCD_DrawString(20, 140, "SD: READ FAIL", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog after reading boot sector
    bs = (FAT32_BootSector*)g_sectorBuffer;
    
    RGU_RestartWDT(); // Restart watchdog before validation
    /* Verify boot signature */
    if (g_sectorBuffer[510] != 0x55 || g_sectorBuffer[511] != 0xAA) {
        DebugPrint("Invalid boot signature\n");
        LCD_DrawString(20, 140, "SD: BAD SIG", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog before parameter validation
    /* Validate FAT32 parameters */
    if (bs->bytesPerSector == 0 || bs->bytesPerSector % 512 != 0) {
        DebugPrint("Invalid bytes per sector: %d\n", bs->bytesPerSector);
        LCD_DrawString(20, 140, "SD: BAD PARAM", 0xF800, 0x0000);
        return false;
    }
    
    if (bs->sectorsPerCluster == 0 || bs->sectorsPerCluster > 128) {
        DebugPrint("Invalid sectors per cluster: %d\n", bs->sectorsPerCluster);
        LCD_DrawString(20, 140, "SD: BAD CLUST", 0xF800, 0x0000);
        return false;
    }
    
    RGU_RestartWDT(); // Restart watchdog before parsing
    /* Parse FAT32 parameters */
    g_fat32.bytesPerSector = bs->bytesPerSector;
    g_fat32.sectorsPerCluster = bs->sectorsPerCluster;
    g_fat32.bytesPerCluster = g_fat32.bytesPerSector * g_fat32.sectorsPerCluster;
    
    /* Calculate FAT and data start */
    g_fat32.fatStart = bs->reservedSectorCount;
    g_fat32.dataStart = g_fat32.fatStart + (bs->numFATs * bs->FATSize32);
    g_fat32.rootDirCluster = bs->rootCluster;
    
    RGU_RestartWDT(); // Restart watchdog before root cluster validation
    /* Validate root cluster */
    if (g_fat32.rootDirCluster < 2) {
        DebugPrint("Invalid root cluster: %d\n", g_fat32.rootDirCluster);
        LCD_DrawString(20, 140, "SD: BAD ROOT", 0xF800, 0x0000);
        return false;
    }
    
    g_fat32.initialized = true;
    
    RGU_RestartWDT(); // Restart watchdog before returning
    
    DebugPrint("FAT32 Init OK\n");
    DebugPrint("Root cluster: %d\n", g_fat32.rootDirCluster);
    LCD_DrawString(20, 120, "SD: FAT32 OK ", 0x07E0, 0x0000);
    
    return true;
}

boolean FAT32_IsInitialized(void)
{
    return g_fat32.initialized;
}

/* List directory contents */
int FAT32_ListDirectory(uint32_t cluster, FileEntry *files, int maxFiles)
{
    uint32_t currentCluster = cluster;
    int fileCount = 0;
    uint32_t clusterCount = 0;
    const uint32_t MAX_CLUSTERS = 100; // Reduced to prevent long operations
    
    if (!g_fat32.initialized) return 0;
    
    if (cluster < 2 || cluster >= FAT32_EOC) return 0; // Invalid cluster
    
    RGU_RestartWDT(); // Restart watchdog at start
    
    while (currentCluster >= 2 && currentCluster < FAT32_EOC && fileCount < maxFiles) {
        /* Prevent infinite loop */
        if (clusterCount++ >= MAX_CLUSTERS) {
            DebugPrint("FAT32: Max clusters reached\n");
            break;
        }
        
        RGU_RestartWDT(); // Restart watchdog before each cluster
        
        /* Calculate sector for this cluster */
        uint32_t firstSector = g_fat32.dataStart + 
                               ((currentCluster - 2) * g_fat32.sectorsPerCluster);
        
        /* Read all sectors in cluster */
        for (uint8_t s = 0; s < g_fat32.sectorsPerCluster && fileCount < maxFiles; s++) {
            RGU_RestartWDT(); // Restart watchdog before each sector read
            
            if (!ReadSector(firstSector + s, g_sectorBuffer)) {
                DebugPrint("FAT32: Read sector failed at cluster %d, sector %d\n", currentCluster, s);
                return fileCount;
            }
            
            RGU_RestartWDT(); // Restart watchdog after sector read
            
            /* Parse directory entries (16 entries per 512-byte sector) */
            FAT32_DirEntry *entries = (FAT32_DirEntry*)g_sectorBuffer;
            
            for (int i = 0; i < 16 && fileCount < maxFiles; i++) {
                FAT32_DirEntry *entry = &entries[i];
                
                /* Skip if empty, deleted, or long name */
                if (entry->name[0] == 0x00) {
                    RGU_RestartWDT(); // Restart before returning
                    return fileCount; // End of directory
                }
                if (entry->name[0] == 0xE5) continue; // Deleted
                if (entry->attr == ATTR_LONG_NAME) continue; // Long filename
                if (entry->attr & ATTR_VOLUME_ID) continue; // Volume label
                
                /* Skip "." and ".." */
                if (entry->name[0] == '.') continue;
                
                /* Convert and store file info */
                FileEntry *file = &files[fileCount];
                ConvertName(entry->name, file->name);
                file->size = entry->fileSize;
                file->isDir = (entry->attr & ATTR_DIRECTORY) != 0;
                file->firstCluster = ((uint32_t)entry->fstClusHI << 16) | entry->fstClusLO;
                
                fileCount++;
            }
        }
        
        RGU_RestartWDT(); // Restart watchdog before reading FAT
        
        /* Get next cluster in chain */
        uint32_t nextCluster = GetFATEntry(currentCluster);
        
        /* Check for invalid cluster chain */
        if (nextCluster == 0 || nextCluster == FAT32_BAD) {
            DebugPrint("FAT32: Invalid cluster chain at %d\n", currentCluster);
            break;
        }
        
        /* Prevent going back to same cluster (circular reference) */
        if (nextCluster == currentCluster) {
            DebugPrint("FAT32: Circular cluster reference at %d\n", currentCluster);
            break;
        }
        
        currentCluster = nextCluster;
    }
    
    RGU_RestartWDT(); // Restart watchdog before returning
    return fileCount;
}

/* List root directory */
int FAT32_ListRoot(FileEntry *files, int maxFiles)
{
    return FAT32_ListDirectory(g_fat32.rootDirCluster, files, maxFiles);
}

/* Get first cluster of subdirectory by name */
uint32_t FAT32_GetSubdirCluster(const char *name, uint32_t parentCluster)
{
    FileEntry files[50];
    int count = FAT32_ListDirectory(parentCluster, files, 50);
    
    for (int i = 0; i < count; i++) {
        if (files[i].isDir) {
            /* Simple case-insensitive compare */
            int j = 0;
            while (name[j] && files[i].name[j]) {
                char c1 = name[j];
                char c2 = files[i].name[j];
                if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
                if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
                if (c1 != c2) break;
                j++;
            }
            if (name[j] == '\0' && files[i].name[j] == '\0') {
                return files[i].firstCluster;
            }
        }
    }
    
    return 0; // Not found
}
