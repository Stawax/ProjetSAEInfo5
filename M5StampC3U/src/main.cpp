/*
  @author Bartholin Matthieu, Marjorie Koehl, Lucas Lapoirière 
  @date   02/01/2025
  @brief  Ce code permet de faire la simulation d'ajout d'un M5 Stamp sur un M5 Core 2. Pour faire cela le Stamp
          vas d'abords chercher à trouvé le M5 Core avec le nom voulu, dans notre cas "M5Core2". Quand il est
          trouvé la LED du Stamp passe de Bleue à Blanc. 
          
          Ensuite l'utilisateur peut appuyer sur le bouton du Stamp. La LED passe alors en violet et le Stamp
          va chercher à se connecter au M5 Core pour lui envoyer une trame. La trame est généré aléatoirement 
          permettant de faire des tests dans diverses situations. Par exemple cette trames :

          - "{\"action\":2,\"idDevice\":1,\"idTypeDevice\":1,\"uid\":\"AA:BB:CC:DD:EE:FF\",\"nom\":\"Chambre\"}" 

          La trame à comme action la 2, donc l'ajout d'un appareil inconu. L'id du type l'appareil, mais aussi sont
          UID qui est généré aléatoirement et pour finir le nom qui est pris dans une liste roomNames. 

          Le Stamp vas ensuite attendre pendant 60 secondes que le M5 Core, lui renvoie une trame qui valide sont
          ajout ou le refuse. La trame qui doit être réceptionné est de la forme {\"action\":3,\"info\":\"x\"}
          avec x qui est soit "valid" ou "denied". Selon la réception la led vas passer en vert ou en rouge pendant
          5 secondes. Suite à celà le M5 Stamp se déconnecte pour rélancer une nouvelle boucle (la LED passe en bleu).  

 * Maintenance:
      Au niveaux de la maintenance du code, il y a plusieurs option qui permettent de traiter tout problème obtenue:
      
      - Notament le DEBUG_MODE qui permet d'activer ou de désactiver la communication Série qui permet le renvoie de toutes
      les actions éffectué par le M5Stamp.

      - Il y aussi le BLE_MODE qui permet d'activer ou désactiver la communication BLE et donc de choisir de tester
      si un problème survient à cause de la communication BLE. 

 * Environnement:
      M5 Stamp C3U
      
 * Librairie:
      ArduinoBLE
      M5StampC3LED
 */

#include "project.h"
#include <M5StampC3LED.h>

M5StampC3LED led = M5StampC3LED();

// Liste des pièces de la maison
const String roomNames[] = {"Salon", "Cuisine", "Chambre", "Salle de bain", "Bureau", "Entree"};
const int numRooms = sizeof(roomNames) / sizeof(roomNames[0]);  // Taille de la liste

BLEDevice central;

void setup() {
    QueueHandle_t queueDevice = xQueueCreate(5, 30);

    pinMode(BOUTON, INPUT);
    led.show(0,0,255);

#if DEBUG_MODE
    Serial.begin(115200); // Initialisation du mode série en debug
#endif
#if BLE_MODE
    BLESetup(); // Initialisation du mode série en debug
#endif
}

void loop() {
#if BLE_MODE
  SERIAL_PRINTLN("Recherche de périphérique M5Core2...");

  central = scanForDevice("M5Core2");

  // Si le périphérique est trouvé
  if (central.localName().length() > 0) {
    led.show(255, 255, 255);

    while (true) {
      if (digitalRead(BOUTON) == 0) {

        // Tenter la connexion avec le périphérique trouvé
        if (central.connect()) {
          
          led.show(255, 0, 255);
          SERIAL_PRINTLN("Connecté à M5Core2 !");

          // Préparer les données à envoyer
          String uid = generateRandomUID();
          String roomName = generateRandomRoomName();
          int deviceType = generateRandomDeviceType();

          static char dataToSend[100];
          snprintf(dataToSend, 
            sizeof(dataToSend), 
            "{\"action\":2,\"idDevice\":1,\"idTypeDevice\":%d,\"uid\":\"%s\",\"nom\":\"%s\"}", 
            deviceType, uid.c_str(), roomName.c_str());

          // Ajouter le message dans la file BLEWrite
          xQueueSendToBack(queueBLEWrite, (void *)dataToSend, portMAX_DELAY);

          // Attendre que la connexion soit terminée ou que 60 seconde passe ou la réception d'une info
          unsigned long startTime = millis();
          while (central.connected() && millis() - startTime < 60000 && information == ActionInformation::NO_INFO) { 
                BLE_callback();
                delay(100);
          }

          if(information == ActionInformation::VALID) {
            SERIAL_PRINTLN("M5Stack validé par le M5Core !");

            information = ActionInformation::NO_INFO;
            led.show(0, 255, 0);
            delay(5000);
          } else {
            SERIAL_PRINTLN("M5Stack refusé par le M5Core !");

            information = ActionInformation::NO_INFO;
            led.show(255, 0, 0);
            delay(5000);
          }

          // Déconnexion une fois la communication terminée
          central.disconnect();

          SERIAL_PRINTLN("Déconnecté de M5Core2.");
        } else {
          SERIAL_PRINTLN("Échec de la connexion à M5Core2.");
        }

        // Réinitialiser la LED à bleue après l'opération
        led.show(0, 0, 255);
        break; // Quitter la boucle dès qu'une opération est effectuée
      }
    }
  }
  delay(1000);
#endif
}

/**
 * Recherche un périphérique BLE avec un nom cible spécifique.
 * @param targetName - Le nom du périphérique à rechercher.
 * @return BLEDevice - Le périphérique BLE correspondant au nom cible, s'il est trouvé.
 */
BLEDevice scanForDevice(const char* targetName) {
    BLEDevice foundDevice;

    while (true) { // Boucle infinie jusqu'à ce que le périphérique soit trouvé
        BLE.scan();

        while (BLE.available()) {
            foundDevice = BLE.available();

            SERIAL_PRINTLN("======================");
            SERIAL_PRINT("Nom du périphérique : ");
            SERIAL_PRINTLN(foundDevice.localName());
            SERIAL_PRINT("Adresse : ");
            SERIAL_PRINTLN(foundDevice.address());
            SERIAL_PRINT("RSSI : ");
            SERIAL_PRINTLN(foundDevice.rssi());

            // Vérifier si le nom correspond à celui recherché
            if (strcmp(foundDevice.localName().c_str(), targetName) == 0) {
                SERIAL_PRINTLN(targetName);
                SERIAL_PRINTLN(" trouvé...");
                BLE.stopScan(); // Arrêter le scan après avoir trouvé le périphérique
                return foundDevice;
            }
        }

        delay(1000); // Pause de 1 seconde avant de relancer un nouveau scan
    }
}

/**
 * Génère un identifiant UID aléatoire au format "XX:XX:XX:XX:XX:XX".
 * @return String - L'UID généré.
 */
String generateRandomUID() {
    char uid[18];

    for (int i = 0; i < 6; ++i) {
        char highNibble = "0123456789ABCDEF"[random(0, 16)]; 
        char lowNibble = "0123456789ABCDEF"[random(0, 16)];  

        uid[i * 3] = highNibble;  // Premier caractère
        uid[i * 3 + 1] = lowNibble; // Deuxième caractère

        if (i < 5) {
            uid[i * 3 + 2] = ':'; 
        } else {
            uid[i * 3 + 2] = '\0';  // Pour terminer la chaîne après le dernier segment
        }
    }

    SERIAL_PRINTLN(uid);
    return String(uid);
}

/**
 * Génère un nom de pièce aléatoire en sélectionnant un élément dans une liste préexistante.
 * @return String - Le nom de pièce généré.
 */
String generateRandomRoomName() {
    int randomIndex = random(0, numRooms);  // Génère un index aléatoire entre 0 et numRooms - 1
    return roomNames[randomIndex];  
}

/**
 * Génère un type d'appareil aléatoire (idTypeDevice) compris entre 1 et 5.
 * @return int - Le type d'appareil généré.
 */
int generateRandomDeviceType() {
    return random(1, 6);  // Génère un nombre entre 1 et 5
}