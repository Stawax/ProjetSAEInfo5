/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Affichage
 * \{
 * 
 * @file    display.h
 * @brief   Header de l'affichage sur l'écran LCD.
*/

#ifndef DISPLAY_H
#define DISPLAY_H

/****************************** Includes *********************************/

#include "icons.h"
/* Librairie d'appareils Bluetooth */
#include "device.h"
/* Header général du projet */
#include "project.h"
/* Librairie du M5 STack Core 2 */
#include <M5Core2.h>

/***************************** Constants *********************************/

/** \brief Couleur du texte */
#define TEXT_COLOR TFT_YELLOW
/** \brief Couleur de fond du texte */
#define BACK_COLOR TFT_BLACK

/** \brief Couleur d'une icone */
#define HOVER_TEXT_COLOR TFT_BLACK
/** \brief Couleur de fond d'une icone */
#define HOVER_BACK_COLOR TFT_YELLOW

/** \brief Laargeur de l'écran LCD */
#define WIDTH 320
/** \brief Hauteur de l'écran LCD */
#define HEIGHT 240

/** \brief Coordonnée horizontale du bouton d'ajout */
#define POS_ADD_BUTTON_X 30
/** \brief Coordonnée verticale du bouton d'ajout */
#define POS_ADD_BUTTON_Y (HEIGHT - 90)
/** \brief Coordonnée horizontale du bouton menu */
#define POS_MENU_BUTTON_X (WIDTH - 90)
/** \brief Coordonnée verticale du bouton menu */
#define POS_MENU_BUTTON_Y (HEIGHT - 90)
/** \brief Dimension d'une icone */
#define ICON_SIZE 60

/*************************** Type Definition *****************************/

/** \brief Etat de l'action pour l'ajout d'un élément */
enum ActionOrder { EXISTING = 1 ,ADD = 2  };

/** \brief Machine à état des parties de l'écran Menu */
enum PartScreenHome { LOGO, ADD_BUTTON, MENU_BUTTON };

/** \brief Machine à état des parties de l'écran */
enum PartScreen { BASE, RIGHT_ARROW, LEFT_ARROW, HOME, CENTER };

/** \brief Machine à état de l'écran */
enum StateScreen { HOME_SCREEN, MENU_SCREEN, ADD_SCREEN };

/** \brief Machine à état des pressions tactiles sur l'écran */
enum ActionPress {
    NO_PRESS, 
    VALIDATE_PRESS, 
    DENIED_PRESS, 
    RIGHT_PRESS, 
    LEFT_PRESS
};

/************************** Global Variables *****************************/

/** Variable globale pour l'état de l'écran */
extern StateScreen screen;

/** Variable qui indique l'action préssé par l'utilisateur */
extern ActionPress actionPress;

/** File pour communiquer les actions */
extern QueueHandle_t actionQueue;

/** Sémaphore pour protéger l'accès à actionPress */
extern SemaphoreHandle_t actionMutex;

/************************** Task Declaration *****************************/

/* Mettre à jour l'affichage du M5 Core 2 */
void taskUpdateDisplay(void *pvParameters);
/* Gérer les pressions sur l'écran */
void taskTouchPressed(void *pvParameters);

/************************ Function Declaration ***************************/

/* Initialisation de l'affichage principal */
void displayScreenHome();
/* Mettre à jour une partie de l'écran principal */
void refreshPartScreenHome(PartScreenHome state, uint16_t textColor, uint16_t backColor);

/* Initialisation de l'affichage de l'écran de Menu */
void displayScreenMenu();
/* Initialisation de l'affichage de l'écran d'ajout */
void displayScreenAdd();
/* Mettre à jour une partie de l'écran */
void refreshPartScreen(PartScreen state, uint16_t textColor, uint16_t backColor);

/* Afficher le temps écoulé d'un appareil */
void displayTime(const Device& device, uint16_t textColor, uint16_t backColor);
/* Affiche une icone à une position spécifique */
void displayIcon(const uint8_t *bitmap, int x, int y, int w, int h, uint16_t frontColor, uint16_t backColor);
/* Afficher les informations d'un appareil */
void displayDevice(const Device& device, uint16_t textColor, uint16_t backColor);
/* Afficher un caractère en gras à un endroit spécifique de l'affichage */
void displayString(const char *string, int32_t poX, int32_t poY ,uint16_t textColor);

/* Gère les pressions sur l'écran tactile dans l'écran principal */
void handleHomeScreenTouch(const TouchPoint& point);
/* Gère les pressions sur l'écran tactile dans l'écran d'ajout */
void handleAddScreenTouch(const TouchPoint& point);
/* Gère les pressions sur l'écran tactile dans l'écran de Menu */
void handleMenuScreenTouch(const TouchPoint& point);

/* Valider un appareil sélectionné */
void validateDevice(int index);
/* Refuser un appareil sélectionné */
void deniedDevice(int index);

/* Convertir un identifiant de capteur en une chaîne de caractère */
std::string convertSensorType(int idType);

#endif // DISPLAY_H

/**
 * \}
 * \}
 */