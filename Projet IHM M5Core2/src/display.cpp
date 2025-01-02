#include "display.h"

StateScreen screen = HOME_SCREEN;

// Liste des types de capteurs
const char* typeSensors[] = {
    "Infrarouge",
    "Temperature",
    "Thermostat",
    "Humidite",
    "Mouvement"
};

/**
 * Tâche pour mettre à jour l'affichage en fonction de l'état actuel du M5Core.
 * @param pvParameters - Paramètres passés à la tâche (non utilisés ici).
 */
void taskUpdateDisplay(void *pvParameters) {
    // Initialisation de l'état actuel de l'écran et de l'action reçue
    StateScreen actualStateScreen = HOME_SCREEN;
    ActionPress receivedAction = ActionPress::NO_PRESS;

    // Initialisation de l'objet Device pour stocker l'état actuel des nouveaux appareils
    Device actualNewDevices;
    actualNewDevices.reset();

    // Délai entre chaque mise à jour (100 ms)
    const TickType_t xDelay = pdMS_TO_TICKS(100);
    TickType_t lastTimeUpdate = xTaskGetTickCount();

    // Affiche l'écran d'accueil au démarrage
    displayScreenHome();

    // Boucle principale de la tâche
    while(1) {
        // Vérifie si l'état de l'écran a changé, si oui affiche le nouvelle écran.
        if (actualStateScreen != screen) {
            actualStateScreen = screen;
            switch (actualStateScreen) {
                case HOME_SCREEN:
                    displayScreenHome();
                    break;
                case ADD_SCREEN:
                    displayScreenAdd();
                    if (!new_devices.empty()) {
                        displayDevice(new_devices[currentDeviceIndex], TEXT_COLOR, BACK_COLOR);
                        displayIcon(cross_button_bits, 30, HEIGHT - 50, ICONS_WIDTH, ICONS_HEIGHT, TEXT_COLOR, BACK_COLOR);
                        displayIcon(valid_button_bits, WIDTH - 70, HEIGHT - 50, ICONS_WIDTH, ICONS_HEIGHT, TEXT_COLOR, BACK_COLOR);
                    }
                    break;
                case MENU_SCREEN:
                    displayScreenMenu();
                    if (!devices.empty()) {
                        displayDevice(devices[currentDeviceIndex], TEXT_COLOR, BACK_COLOR);
                    }
                    break;
                default:
                    break;
            }
        }

        // Gestion des actions utilisateur dans l'écran "ADD_SCREEN"
        if (actualStateScreen == ADD_SCREEN) {
            if (xQueueReceive(actionQueue, &receivedAction, 0)) {
                switch (receivedAction) {
                    case ActionPress::LEFT_PRESS:
                        if(!new_devices.empty()) {
                            currentNewDeviceIndex = (currentNewDeviceIndex - 1 + new_devices.size()) % new_devices.size();
                        } else {
                            currentNewDeviceIndex = 0;
                        }
                        break;
                    case ActionPress::RIGHT_PRESS:
                        if(!new_devices.empty()) {
                            currentNewDeviceIndex = (currentNewDeviceIndex + 1) % new_devices.size();
                        } else {
                            currentNewDeviceIndex = 0;
                        }
                        break;
                    case ActionPress::VALIDATE_PRESS:
                        validateDevice(currentNewDeviceIndex);
                        if(!new_devices.empty()) {
                            currentNewDeviceIndex = currentNewDeviceIndex % new_devices.size();
                        } else {
                            currentNewDeviceIndex = 0;
                        }
                        break;
                    case ActionPress::DENIED_PRESS:
                        deniedDevice(currentNewDeviceIndex);
                        if(!new_devices.empty()) {
                            currentNewDeviceIndex = currentNewDeviceIndex % new_devices.size();
                        } else {
                            currentNewDeviceIndex = 0;
                        }
                        break;
                    default:
                        break;
                }
            }

            // Mise à jour de l'écran si le new device a changé
            if (!new_devices.empty() && actualNewDevices.uid != new_devices[currentNewDeviceIndex].uid) {
                actualNewDevices = new_devices[currentNewDeviceIndex];
                displayDevice(actualNewDevices, TEXT_COLOR, BACK_COLOR);
                displayTime(actualNewDevices, TEXT_COLOR, BACK_COLOR);
                displayIcon(cross_button_bits, 30, HEIGHT - 50, ICONS_WIDTH, ICONS_HEIGHT, TEXT_COLOR, BACK_COLOR);
                displayIcon(valid_button_bits, WIDTH - 70, HEIGHT - 50, ICONS_WIDTH, ICONS_HEIGHT, TEXT_COLOR, BACK_COLOR);
            } else if (new_devices.empty() && !actualNewDevices.isReset()) {
                actualNewDevices.reset();
                refreshPartScreen(CENTER, TEXT_COLOR, TFT_BLACK);
            }
        }

        // Gestion des actions utilisateur dans l'écran "MENU_SCREEN"
        if (actualStateScreen == MENU_SCREEN) {
            if (xQueueReceive(actionQueue, &receivedAction, 0)) {
                switch (receivedAction) {
                    case ActionPress::LEFT_PRESS:
                        currentDeviceIndex = (currentDeviceIndex - 1 + devices.size()) % devices.size();
                        if (!devices.empty()) {
                            displayDevice(devices[currentDeviceIndex], TEXT_COLOR, BACK_COLOR);
                        }
                        break;
                    case ActionPress::RIGHT_PRESS:
                        currentDeviceIndex = (currentDeviceIndex + 1) % devices.size();
                        if (!devices.empty()) {
                            displayDevice(devices[currentDeviceIndex], TEXT_COLOR, BACK_COLOR);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        // Mise à jour de automatique de l'écran toutes les secondes
        if (xTaskGetTickCount() - lastTimeUpdate >= pdMS_TO_TICKS(1000)) {
            lastTimeUpdate = xTaskGetTickCount(); // Met à jour le dernier temps
            switch (actualStateScreen) {
                case ADD_SCREEN:
                    if (!new_devices.empty()) {
                        displayTime(new_devices[currentNewDeviceIndex], TEXT_COLOR, BACK_COLOR);
                    }
                    break;
                default:
                    break;
            }
        }

        // Délai avant la prochaine mise à jour
        vTaskDelay(xDelay);
    }
}

/**
 * Affiche l'écran principal (HOME_SCREEN).
 * Initialise les boutons et leurs couleurs.
 */
void displayScreenHome() {
    M5.Lcd.fillScreen(TFT_BLACK);
    displayString("GEII", WIDTH / 2, HEIGHT - 160 ,TEXT_COLOR);
    refreshPartScreenHome(ADD_BUTTON, TEXT_COLOR, BACK_COLOR);
    refreshPartScreenHome(MENU_BUTTON, TEXT_COLOR, BACK_COLOR);
}

/**
 * Met à jour une partie de l'écran principal (HOME_SCREEN).
 * @param state - La partie de l'écran à mettre à jour (ADD_BUTTON, MENU_BUTTON, etc.).
 * @param textColor - Couleur du texte.
 * @param backColor - Couleur d'arrière-plan.
 */
void refreshPartScreenHome(PartScreenHome state, uint16_t textColor, uint16_t backColor) {
    switch(state) {
        case ADD_BUTTON:
            displayIcon(add_button_bits, POS_ADD_BUTTON_X, POS_ADD_BUTTON_Y, ICON_SIZE, ICON_SIZE, textColor, backColor);
            break;
        case MENU_BUTTON:
            displayIcon(menu_button_bits, POS_MENU_BUTTON_X, POS_MENU_BUTTON_Y, ICON_SIZE, ICON_SIZE, textColor, backColor);
            break;
    }
}

/**
 * Affiche l'écran du menu (MENU_SCREEN).
 * Initialise les boutons de navigation et leurs couleurs.
 */
void displayScreenMenu() {
    M5.Lcd.fillScreen(BACK_COLOR);
    refreshPartScreen(BASE, HOVER_TEXT_COLOR, HOVER_BACK_COLOR);
    refreshPartScreen(HOME, TEXT_COLOR, BACK_COLOR);
    refreshPartScreen(RIGHT_ARROW, TEXT_COLOR, BACK_COLOR);
    refreshPartScreen(LEFT_ARROW, TEXT_COLOR, BACK_COLOR);
    refreshPartScreen(CENTER, TEXT_COLOR, BACK_COLOR);
}

/**
 * Affiche l'écran d'ajout (ADD_SCREEN).
 * Initialise les boutons de navigation et leurs couleurs.
 */
void displayScreenAdd() {
    M5.Lcd.fillScreen(BACK_COLOR);
    refreshPartScreen(BASE, HOVER_TEXT_COLOR, HOVER_BACK_COLOR);
    refreshPartScreen(HOME, TEXT_COLOR, BACK_COLOR);
    refreshPartScreen(RIGHT_ARROW, TEXT_COLOR, BACK_COLOR);
    refreshPartScreen(LEFT_ARROW, TEXT_COLOR, BACK_COLOR);
    refreshPartScreen(CENTER, TEXT_COLOR, BACK_COLOR);
}

/**
 * Met à jour une partie de l'écran (générique).
 * @param state - La partie de l'écran à mettre à jour (BASE, RIGHT_ARROW, etc.).
 * @param textColor - Couleur du texte.
 * @param backColor - Couleur d'arrière-plan.
 */
void refreshPartScreen(PartScreen state, uint16_t textColor, uint16_t backColor) {
    switch(state) {
        case BASE :
            M5.Lcd.fillRect(0,HEIGHT-180,WIDTH,2,backColor);
			break;
		case RIGHT_ARROW :
            M5.Lcd.fillRect(0,0,WIDTH - 238,HEIGHT - 180,backColor);
            displayIcon(right_arrow_bits, WIDTH - 300, 10, ICONS_WIDTH , ICONS_HEIGHT , TEXT_COLOR, BACK_COLOR);
			break;
        case LEFT_ARROW :
            M5.Lcd.fillRect(WIDTH - 78,0,WIDTH,HEIGHT-180,backColor);
            displayIcon(left_arrow_bits, WIDTH - 60, 10, ICONS_WIDTH , ICONS_HEIGHT , TEXT_COLOR, BACK_COLOR);
			break;
        case HOME :
            M5.Lcd.fillRect(WIDTH - 238,0,WIDTH - 82,HEIGHT-180,backColor);
            displayIcon(HOME_bits, WIDTH - 180, 10, ICONS_WIDTH , ICONS_HEIGHT , TEXT_COLOR, BACK_COLOR);
			break;
        case CENTER :
            M5.Lcd.fillRect(0, HEIGHT - 178, WIDTH, HEIGHT, backColor);
            break;
	}
}

/**
 * Affiche les informations d'un appareil.
 * @param device - L'objet Device contenant les informations.
 * @param textColor - Couleur du texte.
 * @param backColor - Couleur d'arrière-plan.
 */
void displayDevice(const Device& device, uint16_t textColor, uint16_t backColor) {
    refreshPartScreen(CENTER, TEXT_COLOR, TFT_BLACK);

    int textX = 20;
    int textY = HEIGHT - 160;
    int lineSpacing = 30;

    M5.Lcd.setTextColor(textColor);
    M5.Lcd.setTextSize(2);

    M5.Lcd.setCursor(textX, textY);
    M5.Lcd.printf("UID: %s", device.uid.c_str());
    M5.Lcd.setCursor(textX, textY + lineSpacing);
    M5.Lcd.printf("Nom: %s", device.name.c_str());

    // Conversion du type de capteur en texte
    std::string typeString = convertSensorType(device.idTypeDevice - 1);
    M5.Lcd.setCursor(textX, textY + 2*lineSpacing);
    M5.Lcd.printf("Type: %s", typeString.c_str());
}

/**
 * Affiche le temps écoulé depuis un certain moment pour un appareil donné.
 * @param device - L'objet Device contenant le timestamp.
 * @param textColor - Couleur du texte affiché.
 * @param backColor - Couleur de l'arrière-plan.
 */
void displayTime(const Device& device, uint16_t textColor, uint16_t backColor) {
    M5.Lcd.fillRect(140, HEIGHT - 30, 50, 30, backColor);

    int textX = 140;
    int textY = HEIGHT - 30;

    uint32_t elapsedMillis = millis() - device.timestamp;
    uint32_t elapsedSeconds = (elapsedMillis / 1000) % 60; // Limiter à 60 secondes

    M5.Lcd.setTextColor(textColor); 
    M5.Lcd.setTextSize(2);          
    M5.Lcd.setCursor(textX, textY);
    M5.Lcd.printf("%u s", elapsedSeconds); 
}

/**
 * Affiche une icône à une position donnée.
 * @param bitmap - L'image bitmap de l'icône.
 * @param x - Position en X.
 * @param y - Position en Y.
 * @param w - Largeur de l'icône.
 * @param h - Hauteur de l'icône.
 * @param frontColor - Couleur de premier plan.
 * @param backColor - Couleur d'arrière-plan.
 */
void displayIcon(const uint8_t *bitmap, int x, int y, int w, int h, uint16_t frontColor, uint16_t backColor) {
    uint16_t color;
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            color = bitmap[j * w + i] == 1 ? frontColor : backColor;
            M5.Lcd.drawPixel(x + i, y + j, color);
        }
    }
}

/**
 * Convertit un ID de type de capteur en une chaîne.
 * @param idType - L'ID numérique du type de capteur.
 * @return Le nom du type de capteur ou "Inconnu" si l'ID est invalide.
 */
std::string convertSensorType(int idType) {
    size_t nbTypes = sizeof(typeSensors) / sizeof(typeSensors[0]);
    if (idType >= 0 && idType < nbTypes) {
        return std::string(typeSensors[idType]);
    } else {
        return "Inconnu";
    }
}

/**
 * @brief Affiche une chaîne de caractères en gras à une position donnée sur l'écran.
 * 
 * @param string - La chaîne de caractères à afficher.
 * @param poX - La position en pixels sur l'axe X où centrer le texte.
 * @param poY - La position en pixels sur l'axe Y où centrer le texte.
 * @param textColor - La couleur du texte (valeur RGB).
 */
void displayString(const char *string, int32_t poX, int32_t poY ,uint16_t textColor) {
    M5.Lcd.setTextColor(textColor);
    M5.Lcd.setTextSize(7); 
    M5.Lcd.setTextDatum(CC_DATUM);

    // Affiche le sting
    M5.Lcd.drawString(string, poX, poY);
}