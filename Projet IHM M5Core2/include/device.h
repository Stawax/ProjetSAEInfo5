/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Appareils
 * \{
 * 
 * @file    device.h
 * @brief   Header de gestion des apareils Bluetooth.
 * 
 * \}
 * \}
*/

#ifndef DEVICE_H
#define DEVICE_H

/****************************** Includes *********************************/

/** \brief Librairie du M5 Stack Core 2 */
#include <M5Core2.h>
/** \brief Librairie du type String */
#include <string>
/** \brief Librairie de tableaux dynamique en taille */
#include <vector>

/*************************** Type Definition *****************************/

/** \brief Structure représentant un appareil */
struct Device {
    int idDevice;
    int idTypeDevice;
    std::string uid;
    std::string name;
    unsigned long timestamp;

    void reset() {
        idDevice = 0;
        idTypeDevice = 0;
        uid.clear();
        name.clear();
        timestamp = 0;
    }

    bool isReset() const {
        return idDevice == 0 &&
               idTypeDevice == 0 &&
               uid.empty() &&
               name.empty() &&
               timestamp == 0;
    }
};

/************************** Global Variables *****************************/

/* Indice de l'appareil actuel dans la liste des appareils existants */
extern int currentDeviceIndex;

/* Indice de l'appareil actuel dans la liste des nouveaux appareils */
extern int currentNewDeviceIndex;

/* Liste globale pour stocker les appareils existants */
extern std::vector<Device> devices;

/* Liste globale pour stocker les nouveaux appareils détectés */
extern std::vector<Device> new_devices;

/* Tampon global pour les données reçues */
extern String buffer;

/* File de queue pour la gestion des appareils */
extern QueueHandle_t queueDevice;

/************************** Task Declaration *****************************/

/* Tâche validant appareils dans la liste des nouveaux appareils */
void taskValidate(void* parameter);

/************************ Function Declaration ***************************/

/* Analyse des données reçues */
bool parseDATA(const char *json);

/* Analyse d'un objet pour extraire les informations d'un appareil */
bool parseDevice(const char *deviceData, Device &device);

/* Extraire une valeur d'un objet en fonction de la clé et du type */
bool extractJsonValue(const char *deviceData, const char *key, const char *type, void *data);

/* Gestion des commandes d'ajout pour les nouveaux appareils */
void handleAddCommand(const char * extractData);

#endif // DEVICE_H
