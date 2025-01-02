#ifndef PROJECT_H
#define PROJECT_H

#define DEBUG_MODE 1

#define BLE_MODE 1

#include "device.h"

#if DEBUG_MODE
    //#include "Serial.h" // Inclure uniquement en mode debug
    #define SERIAL_PRINT(x) Serial.print(x)
    #define SERIAL_PRINTLN(x) Serial.println(x)
#else
    #define SERIAL_PRINT(x)
    #define SERIAL_PRINTLN(x)
#endif

#if BLE_MODE
    #include "BLEManage.h" // Inclure uniquement en mode debug
#endif

enum ActionOrder { EXISTING = 1 ,ADD = 2 , INFO = 3 };

enum ActionInformation {NO_INFO , VALID, DENIED};
extern ActionInformation information;

#endif // PROJECT_H