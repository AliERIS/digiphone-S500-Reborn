#ifndef _FW_LOADER_H_
#define _FW_LOADER_H_

#include "systemconfig.h"

/* Firmware part definition */
typedef struct {
    const char* filename;
    uint32_t target_address;
    uint32_t expected_size;
} FirmwarePart;

/* Original firmware parts based on memory map */
#define ORIGINAL_FW_PARTS 4

/* Function prototypes */
boolean FW_LoadOriginalFirmware(void);
void FW_JumpToOriginalFirmware(void);

#endif /* _FW_LOADER_H_ */
