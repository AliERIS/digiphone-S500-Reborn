/*
 * Bluetooth Driver Stub for DZ09
 */
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "systypes.h"

typedef struct {
    char name[32];
    uint8_t mac[6];
    int rssi;
} BT_Device;

void BT_Init(void);
boolean BT_Power(boolean on);
boolean BT_IsOn(void);
int BT_Scan(BT_Device *results, int max_devices);
boolean BT_Connect(uint8_t *mac);

#endif
