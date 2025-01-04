/**
 * \addtogroup M5_Stamp
 * \{
 * \addtogroup M5_Stamp_Main
 * \{
 * 
 * @file    project.h
 * @brief   Header de gestion des apareils Bluetooth.
 * 
 * \}
 * \}
*/

#ifndef PROJECT_H
#define PROJECT_H

/****************************** Includes *********************************/

/** \brief Librairie d'appareils Bluetooth */
#include "device.h"

/***************************** Constants *********************************/

/** \brief Mode Dépannage */
#define DEBUG_MODE 1

/** \brief Mode Bluetooth */
#define BLE_MODE 1

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

/** \brief Bouton utilisé du M5 Stamp */
#define BOUTON  GPIO_NUM_9

/************************** Global Variables *****************************/

/** \brief Etat de l'action pour l'ajout d'un élément */
enum ActionOrder { EXISTING = 1 ,ADD = 2 , INFO = 3 };

/** \brief MAchine à état de l'information */
enum ActionInformation {NO_INFO , VALID, DENIED};

/* Information importée */
extern ActionInformation information;

/************************ Function Declaration ***************************/

/* Générer un identifiant aléatoire */
String generateRandomUID();

/* Générer un nom de pièce aléatoire */
String generateRandomRoomName();

/* Générer aléatoirement un type d'appareil */
int generateRandomDeviceType();

/* Rechercher un périphérique */
BLEDevice scanForDevice(const char* targetName);

#endif // PROJECT_H