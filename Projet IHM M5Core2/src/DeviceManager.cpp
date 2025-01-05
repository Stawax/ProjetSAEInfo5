/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Appareils
 * \{
 * 
 * @file    DeviceManager.cpp
 * @brief   Programme de gestion des apareils Bluetooth.
*/

/****************************** Includes *********************************/

/* Header général du projet */
#include "project.h"

/************************** Global Variables *****************************/

/** \brief Liste globale pour stocker les appareils existants */
std::vector<Device> devices = {};

/** \brief Liste globale pour stocker les nouveaux appareils détectés */
std::vector<Device> new_devices = {};

/** \brief Indice de l'appareil actuel dans la liste des appareils existants */
int currentDeviceIndex = 0;

/** \brief Indice de l'appareil actuel dans la liste des nouveaux appareils */
int currentNewDeviceIndex = 0;

/** \brief Tampon global pour les données reçues */
String buffer = "";

/** \brief File de queue pour la gestion des appareils */
QueueHandle_t queueDevice;

/*************************** Task Definition *****************************/

/**
 * \brief Tâche pour valider les appareils dans la liste des nouveaux appareils.
 *
 * @param parameter - Paramètre de la tâche (non utilisé).
 * \return None
 */
void taskValidate(void* parameter) {
    while (true) {
        unsigned long currentTime = millis();

        // Vérifie tous les appareils
        for (int i = 0; i < new_devices.size(); i++) {
            Device& device = new_devices[i];

            // Si l'appareil a dépassé 60 secondes, il est supprimé
            if (currentTime - device.timestamp >= 60000) {
                new_devices.erase(new_devices.begin() + i);
                i--; 
                SERIAL_PRINTLN("Appareil supprimé après 60 secondes.");
            }
        }

        if (new_devices.empty()) {
            break;  // Quitte la tâche si tous les appareils ont été traités
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);  // Attent avant de vérifier à nouveau
    }

    vTaskDelete(NULL);  // Supprime la tâche lorsque tout est terminé
}

/************************* Function Definition ***************************/

/**
 * \brief Analyse les données reçues pour extraire les informations des appareils.
 *
 * @param data - Chaîne contenant les données.
 * @return true si l'analyse a réussi, false sinon.
 */
bool parseDATA(const char *data) {
    const char *dataKeyStart = "{";
    const char *dataKeyEnd = "}";

    // Vérifie si les données reçues sont valides
    if (data == nullptr || strlen(data) == 0) {
        Serial.println("Trame vide reçue.");
        return false;
    }

    // Parcourt chaque élément dans la trame
    const char *dataPos = strstr(data, dataKeyStart); 

    while (dataPos) {
        Device device;
        
        // Identifie la fin de l'objet
        const char *endPos = strstr(dataPos, dataKeyEnd);
        if (!endPos) {
            break;
        }

        // Extrait l'objet
        char extractData[endPos - dataPos + 1];
        strncpy(extractData, dataPos, endPos - dataPos);
        extractData[endPos - dataPos] = '\0';  // Ajouter la fin de chaîne

        ActionOrder action;
        // Analyse l'action spécifiée dans l'objet
        if (extractJsonValue(extractData, "\"action\":", "int", &action)) {
            if(action == EXISTING) {
                // Ajoute l'appareil à la liste des appareils existants
                if (!parseDevice(extractData, device)) {
                    SERIAL_PRINTLN("Erreur dans l'analyse de l'appareil.");
                    return false;
                }
                devices.push_back(device);
            }
            else if(action == ADD) {
                // Traite les commandes d'ajout
                handleAddCommand(extractData);
            }
            else {
                SERIAL_PRINTLN("Action inconnue.");
                return false;
            }
        }

        // Passe à l'objet suivant
        dataPos = strstr(endPos, dataKeyStart);
    }

    return true;
}

/**
 * \brief Analyse un objet pour extraire les informations d'un appareil.
 *
 * @param deviceData - Chaîne contenant les données de l'appareil.
 * @param device - Référence à l'objet Device à remplir.
 * @return true si l'analyse a réussi, false sinon.
 */
bool parseDevice(const char *deviceData, Device &device) {
    // Extraire idDevice (int)
    if (!extractJsonValue(deviceData, "\"idDevice\":", "int", &device.idDevice)) {
        SERIAL_PRINTLN("Erreur lors de l'extraction de idDevice.");
        return false;
    }

    // Extraire idTypeDevice (int)
    if (!extractJsonValue(deviceData, "\"idTypeDevice\":", "int", &device.idTypeDevice)) {
        SERIAL_PRINTLN("Erreur lors de l'extraction de idTypeDevice.");
        return false;
    }

    // Extraire UID (string)
    if (!extractJsonValue(deviceData, "\"uid\":", "string", &device.uid)) {
        SERIAL_PRINTLN("Erreur lors de l'extraction de UID.");
        return false;
    }

    // Extraire nom (string)
    if (!extractJsonValue(deviceData, "\"nom\":", "string", &device.name)) {
        SERIAL_PRINTLN("Erreur lors de l'extraction de nom.");
        return false;
    }

    SERIAL_PRINT("ID: ");
    SERIAL_PRINT(device.idDevice);
    SERIAL_PRINT(", Type: ");
    SERIAL_PRINT(device.idTypeDevice);
    SERIAL_PRINT(", UID: ");
    SERIAL_PRINT(device.uid.c_str());
    SERIAL_PRINT(", Nom: ");
    SERIAL_PRINTLN(device.name.c_str());

    return true;  // Toutes les extractions ont réussi
}

/**
 * \brief Extrait une valeur d'un objet en fonction de la clé et du type.
 *
 * @param deviceData - Chaîne contenant les données.
 * @param key - Clé à rechercher dans les données.
 * @param type - Type de la valeur à extraire ("int" ou "string").
 * @param data - Pointeur pour stocker la valeur extraite.
 * @return true si l'extraction a réussi, false sinon.
 */
bool extractJsonValue(const char *deviceData, const char *key, const char *type, void *data) {
    const char *keyPos = strstr(deviceData, key);
    if (!keyPos) {
        return false; 
    }

    if (strcmp(type, "int") == 0) {
        sscanf(keyPos + strlen(key), "%d", (int*)data);
    } 
    else if (strcmp(type, "string") == 0) {
        char buffer[32];
        sscanf(keyPos + strlen(key), " \"%[^\"]\"", buffer); 
        *(std::string*)data = std::string(buffer);
    }

    return true;  // Extraction réussie
}

/**
 * \brief Gère les commandes d'ajout pour les nouveaux appareils.
 *
 * @param extractData - Données de l'appareil.
 * \return None
 */
void handleAddCommand(const char * extractData) {
    Device device;
    if (!parseDevice(extractData, device)) {
        SERIAL_PRINTLN("Erreur dans l'analyse de l'appareil.");
        return;
    }

    device.timestamp = millis();
    new_devices.push_back(device);

    // Crée une seule tâche FreeRTOS pour gérer la validation de tous les nouveaux appareils.
    if (new_devices.size() == 1) {  // Lance la tâche de validation seulement si c'est le premier appareil
        SERIAL_PRINTLN("Démarage Tache Valide");
        xTaskCreate(taskValidate, "ValidateAllDevices", 2048, NULL, 1, NULL);
    }
}

/**
 * \}
 * \}
 */