#ifndef PROJECT_H
#define PROJECT_H

#include "device.h"
#include "display.h"
#include <M5Core2.h>

#define TRAME_SIZE 256

#define DEBUG_MODE 1
#define BLE_MODE 1

#if DEBUG_MODE
    #include "Serial.h" // Inclut uniquement en mode debug
    #define SERIAL_PRINT(x) Serial.print(x)
    #define SERIAL_PRINTLN(x) Serial.println(x)
#else
    #define SERIAL_PRINT(x)
    #define SERIAL_PRINTLN(x)
#endif

#if BLE_MODE
    #include "BLEManage.h" // Inclut uniquement en mode BLE
#endif

#endif // PROJECT_H