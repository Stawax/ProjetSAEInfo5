/**
 * \addtogroup M5_Stamp
 * \{
 * \addtogroup M5_Stamp_Appareils
 * \{
 * 
 * @file    device.h
 * @brief   Header de gestion des apareils Bluetooth.
*/

#ifndef DEVICE_H
#define DEVICE_H

/************************ Function Declaration ***************************/

/* Analyse des données reçues */
bool parseDATA(const char *json);

/* Extraire une valeur d'un objet en fonction de la clé et du type */
bool extractJsonValue(const char *deviceData, const char *key, const char *type, void *data);

#endif // DEVICE_H

/**
 * \}
 * \}
 */