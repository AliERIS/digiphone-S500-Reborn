/*
 * Minimal FAT32 Read-Only Filesystem
 */
#ifndef _FAT32_H_
#define _FAT32_H_

#include "systemconfig.h"

/* FAT Types */
#define FAT_TYPE_FAT12  12
#define FAT_TYPE_FAT16  16
#define FAT_TYPE_FAT32  32

/* File Attributes */
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_LONG_NAME  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

/* Special Cluster Values */
#define FAT32_EOC       0x0FFFFFF8  // End of cluster chain
#define FAT32_BAD       0x0FFFFFF7  // Bad cluster

/* FAT32 Boot Sector */
typedef struct __attribute__((packed)) {
    uint8_t  jmpBoot[3];
    uint8_t  OEMName[8];
    uint16_t bytesPerSector;
    uint8_t  sectorsPerCluster;
    uint16_t reservedSectorCount;
    uint8_t  numFATs;
    uint16_t rootEntryCount;
    uint16_t totalSectors16;
    uint8_t  media;
    uint16_t FATSize16;
    uint16_t sectorsPerTrack;
    uint16_t numberOfHeads;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;
    
    /* FAT32 Extended */
    uint32_t FATSize32;
    uint16_t extFlags;
    uint16_t FSVersion;
    uint32_t rootCluster;
    uint16_t FSInfo;
    uint16_t backupBootSector;
    uint8_t  reserved[12];
    uint8_t  driveNumber;
    uint8_t  reserved1;
    uint8_t  bootSignature;
    uint32_t volumeID;
    uint8_t  volumeLabel[11];
    uint8_t  fileSystemType[8];
} FAT32_BootSector;

/* Directory Entry (8.3 format) */
typedef struct __attribute__((packed)) {
    uint8_t  name[11];          // 8.3 filename
    uint8_t  attr;
    uint8_t  ntRes;
    uint8_t  crtTimeTenth;
    uint16_t crtTime;
    uint16_t crtDate;
    uint16_t lstAccDate;
    uint16_t fstClusHI;
    uint16_t wrtTime;
    uint16_t wrtDate;
    uint16_t fstClusLO;
    uint32_t fileSize;
} FAT32_DirEntry;

/* File Entry (simplified for UI) */
typedef struct {
    char name[13];              // Null-terminated 8.3 name
    uint32_t size;
    boolean isDir;
    uint32_t firstCluster;
} FileEntry;

/* FAT32 Filesystem Context */
typedef struct {
    boolean initialized;
    uint32_t fatStart;
    uint32_t dataStart;
    uint32_t rootDirCluster;
    uint16_t bytesPerSector;
    uint8_t  sectorsPerCluster;
    uint32_t bytesPerCluster;
} FAT32_Context;

/* API Functions */
extern boolean FAT32_Init(void);
extern boolean FAT32_IsInitialized(void);
extern int FAT32_ListDirectory(uint32_t cluster, FileEntry *files, int maxFiles);
extern int FAT32_ListRoot(FileEntry *files, int maxFiles);
extern uint32_t FAT32_GetSubdirCluster(const char *name, uint32_t parentCluster);

/* Internal functions (for firmware loader) */
extern uint32_t GetFATEntry(uint32_t cluster);
extern FAT32_Context g_fat32;

#endif /* _FAT32_H_ */
