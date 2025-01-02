#include "BLEManage.h"

QueueHandle_t queueBLEWrite;
QueueHandle_t queueBLERead;

BLEService M5StackService(SERVICE_UUID);
BLEStringCharacteristic M5StackCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite, TRAME_SIZE);

void BLESetup() {
    if (!BLE.begin()) {
        Serial.println("BLE initialization failed!");
        while (1);
    }

    BLE.setDeviceName("MStamp_BLE_Device");  // Nom de votre périphérique
    BLE.setLocalName("M5Stack");              // Nom local BLE
    BLE.setAdvertisedService(M5StackService);

    M5StackService.addCharacteristic(M5StackCharacteristic);

    BLE.addService(M5StackService);

    queueBLERead = xQueueCreate(10, TRAME_SIZE);
    xTaskCreatePinnedToCore(taskBLERead, "BLE_Read", 8192, nullptr, 2, nullptr, 1);
}

void BLE_callback() {
    if (M5StackCharacteristic.written()) {
        char receivedData[TRAME_SIZE];
        strncpy(receivedData, M5StackCharacteristic.value().c_str(), TRAME_SIZE - 1);
        receivedData[TRAME_SIZE - 1] = '\0';

        xQueueSendToBack(queueBLERead, (void *)&receivedData, portMAX_DELAY);
    }
}

void taskBLERead(void *pvParameters) {
    char receivedData[TRAME_SIZE];

    while (1) {
        if (xQueueReceive(queueBLERead, (void *)&receivedData, portMAX_DELAY)) {
            SERIAL_PRINT("Received: ");
            SERIAL_PRINTLN(receivedData);

            if (parseDATA(receivedData)) {
                SERIAL_PRINTLN("Les données sont analysés avec succès.");
            } else {
                SERIAL_PRINTLN("Erreur dans le traitement des données.");
                information = ActionInformation::DENIED;
            }
        }
        // Ajouter un délai pour éviter une surcharge
        vTaskDelay(50 / portTICK_PERIOD_MS); 
    }
}
