/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Main
 * \{
 * 
 * @file    project.h
 * @brief   Header général du projet IHM.
 * 
 * \}
 * \}
*/

#ifndef PROJECT_H
#define PROJECT_H

/****************************** Includes *********************************/

/** \brief Librairie d'appareils Bluetooth */
#include "device.h"
/** \brief Header de l'affichage LCD */
#include "display.h"
/** \brief Librairie du M5 STack Core 2 */
#include <M5Core2.h>

/***************************** Constants *********************************/

/** \brief Taille d'une trame de données */
#define TRAME_SIZE 256

/** \brief Mode Dépannage */
#define DEBUG_MODE 1

/** \brief Mode Bluetooth */
#define BLE_MODE 1

#if DEBUG_MODE
    /* Inclu uniquement en mode DEBUG */
    #include "Serial.h"
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