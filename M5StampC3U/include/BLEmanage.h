#ifndef BLEMANAGE_H
#define BLEMANAGE_H

#include <ArduinoBLE.h>
#include "project.h"

#define TRAME_SIZE 256

#define SERVICE_UUID           "1234"
#define CHARACTERISTIC_UUID    "5678"

extern QueueHandle_t queueBLEWrite;
extern QueueHandle_t queueBLERead;

void BLESetup();
void BLE_callback();
void taskBLERead(void *pvParameters);

#endif // BLEMANAGE_H