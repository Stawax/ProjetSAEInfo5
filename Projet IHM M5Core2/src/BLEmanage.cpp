/** 
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Bluetooth
 * \{
 * 
 * @file    BLEmanage.cpp
 * @brief   Programme de gestion de la communication Bluetooth.
*/

/****************************** Includes *********************************/

/* Header de gestion du Bluetooth */
#include "BLEManage.h"
/* Librairie du M5 STack Core 2 */
#include <M5Core2.h>

/************************** Global Variables *****************************/

/** \brief File d'écriture en Bluetooth */
QueueHandle_t queueBLEWrite;
/** \brief File de lecture en Bluetooth */
QueueHandle_t queueBLERead;

/** \brief Appareil Bluetooth principal */
BLEDevice central;

/** \brief Initialisation du service de l'appareil Bluetooth */
BLEService M5CoreService(SERVICE_UUID);
/** \brief Initialisation des caractéristiques de l'appareil Bluetooth */
BLEStringCharacteristic M5CoreCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite, TRAME_SIZE);

/*************************** Task Definition *****************************/

/**
 * @brief Tâche FreeRTOS pour lire les données reçues via BLE.
 * 
 * \brief Récupère les données de la queue "queueBLERead", les traite en appelant `parseDATA`,
 *        et affiche le résultat. Ajoute un délai pour éviter une surcharge.
 * 
 * @param pvParameters Paramètres de la tâche (non utilisés ici).
 * @return None
 */
void taskBLERead(void *pvParameters) {
    char receivedData[TRAME_SIZE];

    while (1) {
        // Attend de recevoir des données dans la queue "queueBLERead"
        if (xQueueReceive(queueBLERead, (void *)&receivedData, portMAX_DELAY)) {
            SERIAL_PRINT("Received: ");
            SERIAL_PRINTLN(receivedData);

            if (parseDATA(receivedData)) {
                SERIAL_PRINTLN("Appareils analysés avec succès.");
            } else {
                SERIAL_PRINTLN("Erreur dans le traitement des appareils.");
            }
        }
        // Ajouter un délai pour éviter une surcharge
        vTaskDelay(50 / portTICK_PERIOD_MS); 
    }
}

/**
 * @brief Tâche FreeRTOS pour envoyer des données via BLE.
 * 
 * \brief Récupère les données de la queue "queueBLEWrite", vérifie si une connexion BLE
 *        est active, et tente d'envoyer les données via la fonction `sendDataBLE`.
 * \brief Ajoute un délai pour éviter une surcharge.
 * 
 * @param pvParameters Paramètres de la tâche (non utilisés ici).
 * \return None
 */
void taskBLEWrite(void *pvParameters) {
    char dataToSend[TRAME_SIZE];

    while (1) {
        // Attend de recevoir des données dans la queue "queueBLEWrite"
        if (xQueueReceive(queueBLEWrite, (void *)&dataToSend, portMAX_DELAY)) {
            SERIAL_PRINT("Envoi BLE : ");
            SERIAL_PRINTLN(dataToSend);

            // Vérifier si une connexion BLE est active
            if (BLE.connected()) {
                sendDataBLE(central, SERVICE_UUID, CHARACTERISTIC_UUID, dataToSend);
            }
            else {
                SERIAL_PRINTLN("Aucune connexion BLE active. Envoi annulé.");
            }
        }

        // Ajouter un délai pour éviter une surcharge
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/************************* Function Definition ***************************/

/**
 * @brief Initialise les services et caractéristiques BLE ainsi que les tâches de gestion BLE.
 * 
 * \brief Configure le périphérique BLE, initialise les services et caractéristiques nécessaires,
 *        et démarre les tâches pour la lecture et l'écriture BLE via FreeRTOS.
 * 
 * \param None
 * \return None
 */
void BLESetup() {

    if (!BLE.begin()) {
        Serial.println("BLE initialization failed!");
        while (1);
    }

    BLE.setDeviceName("M5Core2_BLE_Device");  
    BLE.setLocalName("M5Core2");              
    BLE.setAdvertisedService(M5CoreService);

    M5CoreService.addCharacteristic(M5CoreCharacteristic);

    BLE.addService(M5CoreService);
    BLE.advertise();

    SERIAL_PRINTLN("En attente de connexion BLE...");

    queueBLERead = xQueueCreate(10, TRAME_SIZE);
    xTaskCreatePinnedToCore(taskBLERead, "BLE_Read", 8192, nullptr, 2, nullptr, 1);

    queueBLEWrite = xQueueCreate(10, TRAME_SIZE);
    xTaskCreatePinnedToCore(taskBLEWrite, "BLE_Write", 8192, nullptr, 2, nullptr, 1);
}

/**
 * @brief Callback BLE pour traiter les données reçues via la caractéristique BLE.
 * 
 * \brief Vérifie si la caractéristique BLE a été écrite, extrait les données reçues
 *        et les insère dans la queue pour un traitement ultérieur.
 * 
 * \param None
 * \return None
 */
void BLE_callback() {
    if (M5CoreCharacteristic.written()) {
        char receivedData[TRAME_SIZE];
        strncpy(receivedData, M5CoreCharacteristic.value().c_str(), TRAME_SIZE - 1);
        receivedData[TRAME_SIZE - 1] = '\0';

        xQueueSendToBack(queueBLERead, (void *)&receivedData, portMAX_DELAY);
    }
}

/**
 * \brief Fonction pour envoyer des données via une caractéristique BLE.
 * \brief Cette fonction découvre le service BLE spécifié, recherche la caractéristique,
 *        puis tente d'envoyer les données fournies.
 *
 * @param central - L'objet représentant l'appareil central BLE.
 * @param serviceUuid - UUID du service BLE à découvrir.
 * @param characteristicUuid - UUID de la caractéristique BLE à utiliser pour l'envoi des données.
 * @param dataToSend - Les données à envoyer sous forme de chaîne de caractères.
 * @return bool - Retourne true si les données ont été envoyées avec succès, sinon false.
 */
bool sendDataBLE(BLEDevice& central, const char* serviceUuid, const char* characteristicUuid, const char* dataToSend) {
    // Découvrir le service BLE spécifié
    if (central.discoverService(serviceUuid)) {
        SERIAL_PRINTLN("Service trouvé !");
        
        // Trouver la caractéristique associée
        BLECharacteristic characteristic = central.characteristic(characteristicUuid);
        if (characteristic) {
            // Tenter d'envoyer les données
            if (characteristic.writeValue((uint8_t*)dataToSend, strlen(dataToSend))) {
                SERIAL_PRINTLN("Données envoyées avec succès !");
                return true; // Succès
            } else {
                SERIAL_PRINTLN("Échec de l'envoi des données.");
            }
        } else {
            SERIAL_PRINTLN("Caractéristique non trouvée !");
        }
    } else {
        SERIAL_PRINTLN("Service non trouvé !");
    }
    return false; // Échec
}

/**
 * \}
 * \}
 */