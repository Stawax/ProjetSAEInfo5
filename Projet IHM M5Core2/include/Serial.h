/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Série
 * \{
 * 
 * @file    Serial.h
 * @brief   Header des icons à afficher sur l'écran LCD.
 * 
 * \}
 * \}
*/

#ifndef SERIAL_H
#define SERIAL_H

/****************************** Includes *********************************/

/** \brief Librairie d'appareils Bluetooth */
#include "device.h"
/** \brief Header général du projet */
#include "project.h"

/***************************** Constants *********************************/

/** \brief Taille d'une trame de données */
#define TRAME_SIZE 256

/************************** Task Declaration *****************************/

/* Tâche de lecture du port série */
void taskSerialRead(void *pvParameters);

/************************ Function Declaration ***************************/

/* Initialisation de la communication série */
void SerialSetup();

#endif // SERIAL_H
