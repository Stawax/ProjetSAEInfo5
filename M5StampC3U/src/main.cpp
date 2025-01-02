#include "project.h"
#include <M5StampC3LED.h>

#define BOUTON  GPIO_NUM_9
M5StampC3LED led = M5StampC3LED();
String generateRandomUID();
String generateRandomRoomName();
int generateRandomDeviceType();
BLEDevice scanForDevice(const char* targetName);

// Liste des pièces de la maison
const String roomNames[] = {"Salon", "Cuisine", "Chambre", "Salle de bain", "Bureau", "Entree"};
const int numRooms = sizeof(roomNames) / sizeof(roomNames[0]);  // Taille de la liste

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
  Serial.println("Recherche de périphérique M5Core2...");

  // Rechercher le périphérique "M5Core2" via BLE
  BLEDevice foundDevice = scanForDevice("M5Core2");

  // Si le périphérique est trouvé
  if (foundDevice.localName().length() > 0) {
    // Indiquer que le périphérique a été trouvé avec une LED blanche
    led.show(255, 255, 255);

    // Boucle principale pour gérer la connexion
    while (true) {
      // Si le bouton est pressé (en supposant que BOUTON est un pin connecté à un bouton)
      if (digitalRead(BOUTON) == 0) {
        // Tenter la connexion avec le périphérique trouvé
        if (foundDevice.connect()) {
          // Si la connexion est réussie, afficher une LED jaune et un message de succès
          led.show(255, 255, 0);
          Serial.println("Connecté à M5Core2 !");

          // Découverte du service spécifique
          if (foundDevice.discoverService(SERVICE_UUID)) {
            Serial.println("Service trouvé !");
            
            // Recherche de la caractéristique dans le service
            BLECharacteristic characteristic = foundDevice.characteristic(CHARACTERISTIC_UUID);
            if (characteristic) {
              Serial.println("Caractéristique trouvée !");
              
              // Exemple d'utilisation
              String uid = generateRandomUID();
              String roomName = generateRandomRoomName();
              int deviceType = generateRandomDeviceType();

              // Préparer les données à envoyer
              static char dataToSend[100]; // Taille suffisante pour contenir le message
              snprintf(dataToSend, sizeof(dataToSend), "{\"action\":2,\"idDevice\":1,\"idTypeDevice\":%d,\"uid\":\"%s\",\"nom\":\"%s\"}", 
                deviceType, uid.c_str(), roomName.c_str());

              // Envoyer les données à la caractéristique via BLE
              if (characteristic.writeValue((uint8_t *)dataToSend, strlen(dataToSend))) {
                Serial.println("Données envoyées avec succès !");
              } else {
                Serial.println("Échec de l'envoi des données.");
              }
            } else {
              Serial.println("Caractéristique non trouvée !");
            }
          } else {
            Serial.println("Service non trouvé !");
          }

          unsigned long startTime = millis();

          // Attendre que la connexion soit terminée avant de continuer
          while (foundDevice.connected() && millis() - startTime < 60000 && information == ActionInformation::NO_INFO) { // 60 secondes
                BLE_callback();
                delay(100); // Délai pour éviter une surcharge CPU
          }

          if(information == ActionInformation::VALID) {
            Serial.println("M5Stack validé par le M5Core !");
            led.show(0, 255, 0);
            delay(5000);
            information = ActionInformation::NO_INFO;
          } else if (information == ActionInformation::DENIED) {
            Serial.println("M5Stack refusé par le M5Core !");
            led.show(255, 0, 0);
            delay(5000);
            information = ActionInformation::NO_INFO;
          }

          // Déconnexion une fois la communication terminée
          foundDevice.disconnect();
          Serial.println("Déconnecté de M5Core2.");
        } else {
          // Si la connexion échoue
          Serial.println("Échec de la connexion à M5Core2.");
        }

        // Réinitialiser la LED à bleue après l'opération
        led.show(0, 0, 255);
        break; // Quitter la boucle dès qu'une opération est effectuée
      }
    }
  }
  // Attente avant de rechercher à nouveau
  delay(1000);
#endif
}

BLEDevice scanForDevice(const char* targetName) {
    BLEDevice foundDevice;

    while (true) { // Boucle infinie jusqu'à ce que le périphérique soit trouvé
        BLE.scan();

        while (BLE.available()) {
            foundDevice = BLE.available();

            Serial.println("======================");
            Serial.print("Nom du périphérique : ");
            Serial.println(foundDevice.localName());
            Serial.print("Adresse : ");
            Serial.println(foundDevice.address());
            Serial.print("RSSI : ");
            Serial.println(foundDevice.rssi());

            // Vérifier si le nom correspond à celui recherché
            if (strcmp(foundDevice.localName().c_str(), targetName) == 0) {
                Serial.println(targetName);
                Serial.println(" trouvé...");
                BLE.stopScan(); // Arrêter le scan après avoir trouvé le périphérique
                return foundDevice;
            }
        }

        delay(1000);      // Petite pause avant de relancer un scan
    }
}

String generateRandomUID() {
    char uid[18]; // Format "XX:XX:XX:XX:XX:XX" (17 + 1 caractère nul)

    for (int i = 0; i < 6; ++i) {
        char highNibble = "0123456789ABCDEF"[random(0, 16)];  // Génère un chiffre hexadécimal
        char lowNibble = "0123456789ABCDEF"[random(0, 16)];   // Génère un autre chiffre hexadécimal

        // Ajoute le format "XX"
        uid[i * 3] = highNibble;  // Premier caractère
        uid[i * 3 + 1] = lowNibble; // Deuxième caractère

        // Ajoute ":" entre les segments sauf après le dernier
        if (i < 5) {
            uid[i * 3 + 2] = ':'; 
        } else {
            uid[i * 3 + 2] = '\0';  // Pour terminer la chaîne après le dernier segment
        }
    }

    SERIAL_PRINTLN(uid);
    return String(uid);
}

// Fonction pour générer un nom de pièce aléatoire
String generateRandomRoomName() {
    int randomIndex = random(0, numRooms);  // Génère un index aléatoire entre 0 et numRooms - 1
    return roomNames[randomIndex];  // Retourne le nom de la pièce choisi
}

// Fonction pour générer un idTypeDevice aléatoire entre 1 et 5
int generateRandomDeviceType() {
    return random(1, 6);  // Génère un nombre entre 1 et 5
}