/**
 * \addtogroup M5_Stamp
 * \{
 * \addtogroup M5_Stamp_Bluetooth
 * \{
 * 
 * @file    BLEmanage.h
 * @brief   Header de la communication Bluetooth.
 * 
 * \}
 * \}
*/

#ifndef BLEMANAGE_H
#define BLEMANAGE_H

/****************************** Includes *********************************/

/** \brief Librairie de Bluetooth Arduino */
#include <ArduinoBLE.h>
/** \brief Header général du projet */
#include "project.h"

/***************************** Constants *********************************/

/** \brief Taille d'une trame de données */
#define TRAME_SIZE 256

/** \brief Identifiant du service */
#define SERVICE_UUID           "1234"

/** \brief Identifiant des caractéristiques */
#define CHARACTERISTIC_UUID    "5678"

/************************** Task Declaration *****************************/

/* Tâche d'écriture en Bluetooth */
void taskBLEWrite(void *pvParameters);

/* Tâche de lecture en Bluetooth */
void taskBLERead(void *pvParameters);

/************************ Function Declaration ***************************/

/* Initialisation de la communication Bluetooth */
void BLESetup();

/* Réception des données Bluetooth */
void BLE_callback();

/* Envoi des données par caractéristique Bluetooth */
bool sendDataBLE(BLEDevice& central, const char* serviceUuid, const char* characteristicUuid, const char* dataToSend);

/************************** Global Variables *****************************/

/* File pour l'écriture en Bluetooth */
extern QueueHandle_t queueBLEWrite;

/* File pour la lecture en Bluetooth */
extern QueueHandle_t queueBLERead;

/* Appareil principal Bluetooth */
extern BLEDevice central;

#endif // BLEMANAGE_H