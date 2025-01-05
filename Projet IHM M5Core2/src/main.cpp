/**
 * \addtogroup M5_Core2
 * \{
 * \addtogroup M5_Core2_Main
 * \{
 * 
 *  @file   main.cpp
 * 
 *  @brief  Ce code permet de réaliser une IHM sur l'écran tactile du M5Core, permettant de faire l'ajout
 *          de différents appareils communiquant en BLE. Le code est principalement sérparé en trois parties
 *          séparées dans des tâches FreeRTOS et la loop principale : 
 *
 *              - La première tâche est "Update_Display", comme son nom l'indique elle permet de mettre à jour
 *              l'affichage sur l'écran. C'est la seule tâche qui peut modifier l'écran, permettant une meilleure
 *              gestion de ce dernier.
 *
 *              - La seconde tâche est "Touch_Pressed". Cette tâche permet de réaliser l'intéraction tactile de
 *              l'écran et les différentes actions que cela effectue. 
 *
 *              - La loop principale quand à elle s'occupe de maintenir la communication BLE du M5Core.
 * 
 *              La communication BLE est initialisée par la fonction BLESetup(). Cette initialisation va créer
 *              deux tâches FreeRTOS qui peuvent être considérées comme tâches mineures :
 *
 *              - Les tâches "BLE_Read" et "BLE_Write", permettent de faire la réception et l'envoi de données par la
 *              communication BLE. Les données envoyées ou réceptionnées sont d'abord placées dans des queues (queueBLERead, 
 *              queueBLEWrite). L'utilitée de ces queues est d'avoir une sauvegarde des différentes données et un traitement
 *              dans l'ordre de ces dernières.
 * 
 *          Le fonctionnement du M5Core se réalise d'abord par l'initialisation
 *          des différentes parties ainsi que de l'affichage du HOME_SCREEN sur l'écran. L'utilisateur a alors deux
 *          choix : le ADD_BUTTON (permet d'aller sur l'écran ADD_SCREEN) ou le MENU_BUTTON (permet d'aller sur 
 *          l'écran MENU_SCREEN). Si l'utilisateur appuye sur ADD_BUTTON, il va voir afficher tous les différents
 *          appareils qui ont fait une demande d'ajout dans les 60 secondes (durée maximale pour valider l'ajout
 *          d'un appareil). L'utilisateur peut alors sélectionner l'appareil souhaité pour ensuite le valider 
 *          ou le refuser avec les différents boutons. Il peut aussi revenir sur l'écran HOME_SCREEN en appuyant
 *          sur la petite maison. Une fois un appareil validé, il peut le retrouver sur le MENU_SCREEN qui affiche 
 *          tous les appareils enregistrés (par la suite on pourra rajouter une intéraction permettant d'aller
 *          sur les IHM des différents groupes avec leurs capteurs).     
 * 
 *          Au niveau de la communication BLE, les trames sont très ressemblantes au JSON. Par exemple, une trame
 *          ressemble à ceci : 
 *          - {"action":2,"idDevice":2,"idTypeDevice":2,"uid":"FF:FF:FF:FF:FF","nom":"Chambre"}.
 * 
 *          On a différents paramètres comme "action" qui dit ce que fait le message (1 -> ajout d'appareil déjà connu,
 *          2 -> ajout d'appareil, 3 -> Info). Il y a aussi "idTypeDevice" qui dit le type de l'appareil, "uid" qui
 *          dit son adresse UID, et puis il y a "nom" qui est le nom de l'appareil. C'est une trame typique de celle
 *          reçue par le M5 Core.
 * 
 *          On a aussi un autre type de trame très similaire qui est envoyée par le M5Core, pour dire si l'appareil
 *          est validé ou refusé par ce dernier. Les trames sont {"action":3,"info":"valid"} ou {"action":3,"info":"denied"}
 *    
 *          * Maintenance:
 *              Au niveau de la maintenance du code, il y a plusieurs options qui permettent de traiter tout problème obtenu:
 *             
 *              - Notamment le DEBUG_MODE qui permet d'activer ou de désactiver la communication Série qui permet le renvoi de toutes
 *              les actions éffectuées par le M5Core. Mais aussi, qui permet de lui envoyer des trames que le M5Core va traiter.
 *              Cela permet de tester le M5 si on n'a pas d'appareil qui communique en BLE disponible.
 * 
 *              - Il y a aussi le BLE_MODE qui permet d'activer ou désactiver la communication BLE et donc de choisir de tester
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
 */

/****************************** Includes *********************************/

/* Header général du projet */
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

/**
 * \}
 * \}
 */