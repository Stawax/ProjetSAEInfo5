/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Série
 * \{
 * 
 * @file    serial.cpp
 * @brief   Header de la communication série.
 * 
 * \}
 * \}
*/

/****************************** Includes *********************************/

/** \brief Header de la communication série */
#include "Serial.h"
/** \brief Librairie du M5 STack Core 2 */
#include <M5Core2.h>

/*************************** Task Definition *****************************/

/**
 * \brief Tâche pour la lecture de données via le port série.
 * \brief Cette tâche lit les données reçues, les affiches, et tente de les analyser.
 * \brief Elle permet aussi  de faire des tests et sur le M5 Core.
 *
 * @param pvParameters - Paramètres de la tâche, non utilisé.
 * \return None
 */
void taskSerialRead(void *pvParameters) {
    char receivedData[1024];

    while (1) {
        // Vérification des données disponibles sur le port série
        if (Serial.available()) {
            // Lecture jusqu'à un caractère de fin de ligne ('\n') ou jusqu'à la taille du buffer - 1
            size_t len = Serial.readBytesUntil('\n', receivedData, sizeof(receivedData) - 1);

            // Ajout d'un caractère de fin de chaîne pour sécuriser les données
            receivedData[len] = '\0';  // Ajouter la fin de chaîne

            // Affichage des données reçues
            Serial.print("Données reçues via le port série : ");
            Serial.println(receivedData);

            // Analyse des données reçues
            if (parseDATA(receivedData)) {
                Serial.println("Appareils analysés avec succès.");
            } else {
                Serial.println("Erreur dans le traitement des appareils.");
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Pause pour éviter une surcharge
    }
}

/************************* Function Definition ***************************/

/**
 * \brief Initialisation du port série et création d'une tâche dédiée à la lecture série.
 * 
 * \param None
 * \return None
 */
void SerialSetup() {
    Serial.begin(115200);

    // Création de la tâche de lecture série
    xTaskCreatePinnedToCore(taskSerialRead, "Serial_Read", 8192, nullptr, 2, nullptr, 1);
}