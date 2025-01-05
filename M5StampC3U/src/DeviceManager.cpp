/** 
 * \addtogroup M5_Stamp
 * \{
 * \addtogroup M5_Stamp_Appareils
 * \{
 * 
 * @file    DeviceManager.cpp
 * @brief   Programme de gestion des apareils Bluetooth.
*/

/****************************** Includes *********************************/

/* Header général du projet */
#include "project.h"

/************************** Global Variables *****************************/

/** \brief Information actuelle initialisée à NO_INFO */
ActionInformation information = NO_INFO;

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
            if(action == INFO) {
                String info;
                if (!extractJsonValue(extractData, "\"info\":", "string", &info)) {
                    SERIAL_PRINTLN("Erreur lors de l'extraction de nom.");
                    return false;
                }

                if(info == "valid") {
                    information = ActionInformation::VALID;
                    return true;
                } else if (info == "denied") {
                    information = ActionInformation::DENIED;
                    return true;
                } else {
                    SERIAL_PRINTLN("Info inconnue.");
                }
            }
            else {
                SERIAL_PRINTLN("Action inconnue.");
                return false;
            }
        }

        // Passe à l'objet suivant
        dataPos = strstr(endPos, dataKeyStart);
    }

    return false;
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
        *((String*)data) = String(buffer); 
    }

    return true;  // Extraction réussie
}

/**
 * \}
 * \}
 */