/*
 * Bluetooth Driver Stub for DZ09
 */
#include "bluetooth.h"
#include "bluetooth.h"
#include "bluetooth.h"
#include "lcdif.h"
#include <string.h>
#include "ustimer.h"

static boolean g_bt_power = false;

void BT_Init(void)
{
    // Stub: No real hardware init possible without register map
    g_bt_power = false;
}

boolean BT_Power(boolean on)
{
    g_bt_power = on;
    if (on) {
        // Simulate startup delay
        USC_Pause_us(50000);
    }
    return true;
}

boolean BT_IsOn(void)
{
    return g_bt_power;
}

int BT_Scan(BT_Device *results, int max_devices)
{
    if (!g_bt_power) return 0;

    // Return dummy devices for UI testing
    int count = 0;
    
    // Simulate finding devices
    USC_Pause_us(200000); // Simulate scan time

    if (max_devices > 0) {
        strcpy(results[0].name, "iPhone 13");
        results[0].mac[0] = 0xAB; results[0].mac[1] = 0xCD; results[0].mac[2] = 0xEF;
        results[0].rssi = -65;
        count++;
    }
    
    if (max_devices > 1) {
        strcpy(results[1].name, "JBL Speaker");
        results[1].mac[0] = 0x12; results[1].mac[1] = 0x34; results[1].mac[2] = 0x56;
        results[1].rssi = -42;
        count++;
    }

    if (max_devices > 2) {
        strcpy(results[2].name, "Galaxy S21");
        results[2].mac[0] = 0x99; results[2].mac[1] = 0x88; results[2].mac[2] = 0x77;
        results[2].rssi = -80;
        count++;
    }

    return count;
}

boolean BT_Connect(uint8_t *mac)
{
    if (!g_bt_power) return false;
    
    USC_Pause_us(100000); // Simulate connection attempt
    return true; // Always succeed in stub
}
