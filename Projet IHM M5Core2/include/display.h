#ifndef DISPLAY_H
#define DISPLAY_H

#include "icons.h"
#include "device.h"
#include "project.h"
#include <M5Core2.h>

// Couleurs de design
#define TEXT_COLOR TFT_YELLOW
#define BACK_COLOR TFT_BLACK
#define HOVER_TEXT_COLOR TFT_BLACK
#define HOVER_BACK_COLOR TFT_YELLOW

// Dimensions de l'écran
#define WIDTH 320
#define HEIGHT 240

// Positions des éléments
#define POS_ADD_BUTTON_X 30
#define POS_ADD_BUTTON_Y (HEIGHT - 90)
#define POS_MENU_BUTTON_X (WIDTH - 90)
#define POS_MENU_BUTTON_Y (HEIGHT - 90)
#define ICON_SIZE 60

// Énumérations pour les parties de l'écran et les états d'écran
enum ActionOrder { EXISTING = 1 ,ADD = 2  };
enum PartScreenHome { LOGO, ADD_BUTTON, MENU_BUTTON };
enum PartScreen { BASE, RIGHT_ARROW, LEFT_ARROW, HOME, CENTER };
enum StateScreen { HOME_SCREEN, MENU_SCREEN, ADD_SCREEN };

enum ActionPress {
    NO_PRESS, 
    VALIDATE_PRESS, 
    DENIED_PRESS, 
    RIGHT_PRESS, 
    LEFT_PRESS
};

// Variable globale
extern StateScreen screen; //Variable qui indique l'état de l'écran.

extern ActionPress actionPress; //Variable qui indique l'action préssé par l'utilisateur.

extern QueueHandle_t actionQueue; // File pour communiquer les actions
extern SemaphoreHandle_t actionMutex; // Sémaphore pour protéger l'accès à actionPress

// Prototypes de tâches
void taskUpdateDisplay(void *pvParameters);
void taskTouchPressed(void *pvParameters);

// Prototypes des fonctions
void displayScreenHome();
void refreshPartScreenHome(PartScreenHome state, uint16_t textColor, uint16_t backColor);

void displayScreenMenu();
void displayScreenAdd();
void refreshPartScreen(PartScreen state, uint16_t textColor, uint16_t backColor);

void displayTime(const Device& device, uint16_t textColor, uint16_t backColor);
void displayIcon(const uint8_t *bitmap, int x, int y, int w, int h, uint16_t frontColor, uint16_t backColor);
void displayDevice(const Device& device, uint16_t textColor, uint16_t backColor);
void displayString(const char *string, int32_t poX, int32_t poY ,uint16_t textColor);

void handleHomeScreenTouch(const TouchPoint& point);
void handleAddScreenTouch(const TouchPoint& point);
void handleMenuScreenTouch(const TouchPoint& point);

void validateDevice(int index);
void deniedDevice(int index);

std::string convertSensorType(int idType);

#endif // DISPLAY_H
