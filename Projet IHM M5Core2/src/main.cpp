/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Main
 * \{
 * 
 *  @file   main.cpp
 * 
 *  @brief  Ce code permet de réaliser une IHM sur l'écran tactille du M5Core, permettant de faire l'ajout
 *          de différent appareil communiquant en BLE. Le code est principalement sérparé en trois parties
 *          séparé dans des tâches FreeRTOS et le loop principale : 
 *
 *              - La première tâche est "Update_Display", comme sont nom l'indique elle permet de mettre à jour
 *              l'affichage sur l'écran. Ces la seul tâche qui peut modifier l'écran, permettant une meilleure
 *              gestion de ce dernier.
 *
 *              - La seconde tâche est "Touch_Pressed". Cette tâche permet de réaliser l'intéraction tactille de
 *              l'écran et les différentes action que celà effectue. 
 *
 *              - Le loop principale quand à lui s'occupe à maintenir la communication BLE du M5Core.
 * 
 *              La communication BLE est initialisé par la fonction BLESetup(). Cette initialisation vas créer
 *              deux tâches FreeRTOS qui peuvent être considéré comme tâche mineur :
 *
 *              - Les tâches "BLE_Read" et "BLE_Write", permettent de faire la réception et l'envoie de donnés par la
 *              communication BLE. Les donnés envoyés ou réceptionnés sont d'abord placé dans des queue (queueBLERead, 
 *              queueBLEWrite). L'utilité de ces queue est d'avoir une sauvegarde des différentes donnés et un traitement
 *              dans l'ordre de ces dernières.
 * 
 *          Si on rentre dans les détails le fonctionnement du M5Core se réalise d'abords par l'initialisation 
 *          des différentes parties ainsi que de l'affichage du HOME_SCREEN sur l'écran. L'utilisateur à alors deux
 *          choix le ADD_BUTTON (permet d'aller sur l'écran ADD_SCREEN) ou le MENU_BUTTON (permet d'aller sur 
 *          l'écran MENU_SCREEN). Si l'utilisateur appuye sur ADD_BUTTON, il va voir afficher tout les différents
 *          appareils qui ont fait une demande d'ajout dans les 60 secondes (durée maximale pour valider l'ajout
 *          d'un appareil). L'utilisateur peut alors sélectionner l'appareils souhaité pour ensuite le valider 
 *          ou le refuser avec les différents boutons. Il peut aussi revenir sur l'écran HOME_SCREEN en appuyant
 *          sur la petite maison. Une fois un appareil validé, il peut le retrouver sur le MENU_SCREEN qui affiche 
 *          tout les appareils enregistrés (par la suite on pourra rajouter une intéraction permettant d'aller
 *          sur les IHM des différents groupe avec leus capteurs).     
 * 
 *          Au niveaux de la communication BLE, les trames sont très ressemblante au JSON. Par exemple une trame
 *          peut ressembler à ceci : 
 *          - {"action":2,"idDevice":2,"idTypeDevice":2,"uid":"FF:FF:FF:FF:FF","nom":"Chambre"}.
 * 
 *          On à différent paramètre comme "action" qui dit se que fait le message (1 -> ajout d'appareil déjà connu,
 *          2 -> ajout d'appareil, 3 -> Info). Il y aussi "idTypeDevice" qui dit le type de l'appareil, "uid" qui
 *          dit sont adresse UID, et puis il y a "nom" qui est le nom de l'appareil. Ces une trame typique de celle
 *          reçu par le M5 Core.
 * 
 *          On a aussi un autre type de trame très similaire qui est envoyé par le M5Core, pour dire si l'appareil
 *          est reçu ou refusé par ce dernié. Les trames sont {"action":3,"info":"valid"} ou {"action":3,"info":"denied"}
 *    
 *          * Maintenance:
 *              Au niveaux de la maintenance du code, il y a plusieurs option qui permettent de traiter tout problème obtenue:
 *             
 *              - Notament le DEBUG_MODE qui permet d'activer ou de désactiver la communication Série qui permet le renvoie de toutes
 *              les actions éffectué par le M5Core. Mais aussi permet de lui envoyer des trames que le M5Core vas traiter.
 *              Celà permet de tester le M5 si on n'a pas d'appareil qui communique en BLE disponible.
 * 
 *              - Il y aussi le BLE_MODE qui permet d'activer ou désactiver la communication BLE et donc de choisir de tester
 *              si un problème survient à cause de la communication BLE. 
 * 
 *          * Test: Trame qui permet de tester le M5Core 
 *              {"action":1,"idDevice":1,"idTypeDevice":1,"uid":"FF:FF:FF:FF:FF","nom":"Salle de bain"}
 *              {"action":2,"idDevice":2,"idTypeDevice":2,"uid":"11:22:33:44:55","nom":"Chambre"}
 *              {"action":2,"idDevice":3,"idTypeDevice":4,"uid":"AA:BB:CC:DD:EE:FF","nom":"Cuisine"}
 * 
 *          * Environnement:
 *              M5 Stack Core2
 *                
 *          * Librairie:
 *              ArduinoBLE
 *              M5Core2
 * 
 *  @author Bartholin Matthieu, Marjorie Koehl, Lucas Lapoirière 
 *  @date   02/01/2025
 * 
 * \}
 * \}
 */

/****************************** Includes *********************************/

/** \brief Header général du projet */
#include "project.h"

/******************************** SETUP **********************************/

void setup() {

    /* Initialisation du M5 STack Core 2 */
    M5.begin();
    M5.update();

    /* Création des tâches de mise à jour de l'affichage, et de la gestion du tacile */
    xTaskCreatePinnedToCore(taskUpdateDisplay, "Update_Display", 8192, nullptr, 2, nullptr, 1);
    xTaskCreatePinnedToCore(taskTouchPressed, "Touch_Pressed", 8192, nullptr, 2, nullptr, 1);

#if DEBUG_MODE
    SerialSetup(); // Initialisation du mode série en debug
#endif
#if BLE_MODE
    BLESetup(); // Initialisation du mode BLE
#endif
}

/********************************* LOOP **********************************/

void loop() {
#if BLE_MODE
    BLE.poll();
    central = BLE.central(); //Variable global qui permet de sauvegarder le client qui c'est connecté.
    if (central) {
        SERIAL_PRINT("Client connecté : ");
        SERIAL_PRINTLN(central.address());

        while (central.connected()) {
            BLE_callback();
        }

        SERIAL_PRINTLN("Client déconnecté");
    }
#endif
}