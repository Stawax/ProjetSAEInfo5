/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Affichage
 * \{
 * 
 * @file    touchScreen.cpp
 * @brief   Programme de gestion du tactile de l'écran LCD.
 * 
 * \}
 * \}
*/

/****************************** Includes *********************************/

/** \brief Header pour l'affichage LCD */
#include "display.h"

/************************** Global Variables *****************************/

/** \brief Action pressée, initialisée à "aucune action" */
ActionPress actionPress = NO_PRESS;

/** \brief File de queue pour les actions à traiter, capacité de 5 actions */
QueueHandle_t actionQueue = xQueueCreate(5, sizeof(ActionPress));

/** \brief Mutex pour synchroniser les accès aux actions */
SemaphoreHandle_t actionMutex = xSemaphoreCreateMutex();

/*************************** Task Definition *****************************/

/**
 * \brief Tâche pour gérer les pressions sur l'écran tactile.
 * \brief Cette tâche vérifie si un point est pressé sur l'écran tactile, identifie la pression
 *        et effectue les actions correspondantes en fonction de l'écran affiché.
 *
 * @param pvParameters - Paramètres de la tâche (non utilisés ici).
 * \return None
 */
void taskTouchPressed(void *pvParameters) {
    TouchPoint pointPress;
    while (1) {
        // Vérification de l'écran tactile
        if (M5.Touch.ispressed()) {
            // Récupération des coordonnées du point pressé
            pointPress = M5.Touch.getPressPoint();
            
            // Gestion en fonction de l'écran actuel
            switch (screen) {
                case HOME_SCREEN:
                    handleHomeScreenTouch(pointPress);
                    break;
                case ADD_SCREEN:
                    handleAddScreenTouch(pointPress);
                    break;
                case MENU_SCREEN:
                    handleMenuScreenTouch(pointPress);
                    break;
                default:
                    break;
            }

            // Délai pour éviter les rebonds
            vTaskDelay(300 / portTICK_PERIOD_MS);
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/************************* Function Definition ***************************/

/**
 * \brief Gère les pressions sur l'écran tactile dans l'écran d'accueil (HOME_SCREEN).
 *
 * @param point - Coordonnées du point pressé.
 * \return None
 */
void handleHomeScreenTouch(const TouchPoint& point) {
    if (point.x >= 30 && point.x <= 90 && point.y >= (HEIGHT - 90) && point.y <= (HEIGHT - 30)) {
        // Changement vers l'écran d'ajout
        screen = ADD_SCREEN;
    } else if (point.x >= (WIDTH - 90) && point.x <= (WIDTH - 30) && point.y >= (HEIGHT - 90) && point.y <= (HEIGHT - 30)) {
        // Changement vers le menu
        screen = MENU_SCREEN;
    }
}

/**
 * \brief Gère les pressions sur l'écran tactile dans l'écran d'ajout (ADD_SCREEN).
 *
 * @param point - Coordonnées du point pressé.
 * \return None
 */
void handleAddScreenTouch(const TouchPoint& point) {
    ActionPress tempAction = ActionPress::NO_PRESS;

    if (point.x >= (WIDTH - 238) && point.x <= (WIDTH - 82) && point.y >= 0 && point.y <= (HEIGHT - 180)) {
        // Retour à l'écran d'accueil
        screen = HOME_SCREEN;
    } else if (!new_devices.empty() && point.x >= 0 && point.x <= (WIDTH - 238) && point.y >= 0 && point.y <= (HEIGHT - 180)) {
        tempAction = ActionPress::LEFT_PRESS;
    } else if (!new_devices.empty() && point.x >= (WIDTH - 78) && point.x <= WIDTH && point.y >= 0 && point.y <= (HEIGHT - 180)) {
        tempAction = ActionPress::RIGHT_PRESS;
    } else if (!new_devices.empty() && point.x >= (WIDTH - 70) && point.x <= (WIDTH - 30) && point.y >= (HEIGHT - 50) && point.y <= (HEIGHT - 10)) {
        tempAction = ActionPress::VALIDATE_PRESS;
    } else if (!new_devices.empty() && point.x >= 30 && point.x <= 70 && point.y >= (HEIGHT - 50) && point.y <= (HEIGHT - 10)) {
        tempAction = ActionPress::DENIED_PRESS;
    }

    // Envoi de l'action dans la file d'attente
    if (tempAction != ActionPress::NO_PRESS) {
        xQueueSend(actionQueue, &tempAction, 0);
    }
}

/**
 * \brief Gère les pressions sur l'écran tactile dans l'écran du menu (MENU_SCREEN).
 *
 * @param point - Coordonnées du point pressé.
 * \return None
 */
void handleMenuScreenTouch(const TouchPoint& point) {
    ActionPress tempAction = ActionPress::NO_PRESS;

    if (point.x >= (WIDTH - 238) && point.x <= (WIDTH - 82) && point.y >= 0 && point.y <= (HEIGHT - 180)) {
        // Retour à l'écran d'accueil
        screen = HOME_SCREEN;
    } else if (!devices.empty() && point.x >= 0 && point.x <= (WIDTH - 238) && point.y >= 0 && point.y <= (HEIGHT - 180)) {
        tempAction = ActionPress::LEFT_PRESS;
    } else if (!devices.empty() && point.x >= (WIDTH - 78) && point.x <= WIDTH && point.y >= 0 && point.y <= (HEIGHT - 180)) {
        tempAction = ActionPress::RIGHT_PRESS;
    }

    // Envoi de l'action dans la file d'attente
    if (tempAction != ActionPress::NO_PRESS) {
        xQueueSend(actionQueue, &tempAction, 0);
    }
}

/**
 * \brief Valide un appareil sélectionné en le déplaçant de la liste des nouveaux appareils
 *        à la liste des appareils existants.
 *
 * @param index - Index de l'appareil.
 * \return None
 */
void validateDevice(int index) {
    if (new_devices.empty()) { return; } 

    devices.push_back(new_devices[index]);

#if BLE_MODE
    // Construire un message de validation pour BLE
    char message[TRAME_SIZE];
    snprintf(message, TRAME_SIZE, "{\"action\":3,\"info\":\"valid\"}");

    // Ajouter le message dans la file BLEWrite
    xQueueSendToBack(queueBLEWrite, (void *)message, portMAX_DELAY);
#endif

    new_devices.erase(new_devices.begin() + index);
    SERIAL_PRINTLN("Appareil validé !");
}

/**
 * \brief Refuse un appareil sélectionné.
 *
 * @param index - Index de l'appareil.
 * \return None
 */
void deniedDevice(int index) {
    if (new_devices.empty()) { return; } 

#if BLE_MODE
    // Construire un message de validation pour BLE
    char message[TRAME_SIZE];
    snprintf(message, TRAME_SIZE, "{\"action\":3,\"info\":\"denied\"}");

    // Ajouter le message dans la file BLEWrite
    xQueueSendToBack(queueBLEWrite, (void *)message, portMAX_DELAY);
#endif

    new_devices.erase(new_devices.begin() + index);
    SERIAL_PRINTLN("Appareil refusé !");
}