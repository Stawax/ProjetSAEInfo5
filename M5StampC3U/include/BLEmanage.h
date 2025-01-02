#ifndef BLEMANAGE_H
#define BLEMANAGE_H

#include <ArduinoBLE.h>
#include "project.h"

#define TRAME_SIZE 256

#define SERVICE_UUID           "1234"
#define CHARACTERISTIC_UUID    "5678"

extern QueueHandle_t queueBLEWrite;
extern QueueHandle_t queueBLERead;

extern BLEDevice central;

void BLESetup();
void BLE_callback();

void taskBLEWrite(void *pvParameters);
void taskBLERead(void *pvParameters);

bool sendDataBLE(BLEDevice& central, const char* serviceUuid, const char* characteristicUuid, const char* dataToSend);

#endif // BLEMANAGE_H