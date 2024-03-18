/**
 * \file MyBLE.h
 * \page ble Bluetooth Low Energy
 * 
 * Dans le cadre de ce cours de découverte de l'IoT nous utiliserons les fonctionnalités de base du BLE, dans un mode
 * potentiellement dégradé. L'idée est que vous preniez simplement la main sur le module BLE et que vous l'intégriez
 * à l'ensemble du projet.
 * 
 * En mode serveur chaque équipement BLE
 * - Permet d'être vu par les autres équipements BLE,
 * - Propose différents services,
 * - Chaque service propose différentes caractéristiques en mode lecture ou écriture.
 * Il existe une nomenclature des services et caractéristiques, que nous n'utiliserons pas ici.
 * Les serveurs "publient" ou font la promotion de leurs services pour que les clients puissent savoir ce qu'ils
 * proposent avant de se connecter.
 *   \image  html ESP32-ble-bulletin-board-model.png
 * 
 * En mode client, il est possible de scanner les équipements bluetooth et de potentiellement se connecter à eux
 * pour échanger des données.
 * 
 * Dans notre cas, nous publierons simplement un service et un nom d'équipement. Cela sera suffisant pour savoir
 * quels sont les équipements à proximité qui proposent un service donné "Contact Tracker" et leurs noms.
 * - Le service nous permet d'identifier les objets à proximité qui utilisent ce service,
 * - Leur identifiant permet de connaître leur nom et de pouvoir les reconnaître individuellement.
 * 
 * Au regard de la taille des composants logiciels, vous aurez besoin de changer le partionnement de la carte en
 * mode "Minimal SPIFFS (1.9 MB App with OTA/190 KB SPIFFS)" sinon vous ne pourrez pas charger le binaire sur la
 * carte, par manque de place.
 */


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "436f6e74-6163-7420-5472-61636b657273" // "Contact Trackers" d'ascii en hexa, ça ne sert à rien mais bon                             
//#define DEVICE_NAME         "MaximeBui-10"                          // Nom de votre serveur BLE qui sera détecté par les autres
#define DEVICE_NAME         "MaximeBui-20"                          // Nom de votre serveur BLE qui sera détecté par les autres

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

/**
 * Configuration du serveur BLE
 * - Initialisation du serveur avec un nom unique
 *   Ce nom doit être le même que celui qui vous servira pour vous abonner en MQTT !
 * - Démarrage du serveur en mode advertising pour publier ses informations : nom et service
 */
void setupBLEServer() {
  MYDEBUG_PRINT("-BLE : Démarrage du serveur sous le nom : ");
  MYDEBUG_PRINTLN(DEVICE_NAME);

  BLEDevice::init(DEVICE_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();
  MYDEBUG_PRINTLN("-BLE : Serveur démarré");
}

/**
 * Callback quand un device BLE est détecté.
 * Dans notre cas, on vérifie si son service UUID est celui que nous recherchons.
 * Si c'est le cas, on récupère son nom et le niveau de puissance (RSSI)
 * qui nous permet de calculer une distance approximative
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {    
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (SERVICE_UUID == advertisedDevice.getServiceUUID().toString()) {
        MYDEBUG_PRINT("-BLE client / CONTACT TRACKER trouvé : ");
        MYDEBUG_PRINTLN(advertisedDevice.getServiceUUID().toString().c_str());
        MYDEBUG_PRINT("    -- Device Name : ");
        MYDEBUG_PRINTLN(advertisedDevice.getName().c_str());
        MYDEBUG_PRINT("    -- Device RSSI : ");
        MYDEBUG_PRINTLN(advertisedDevice.getRSSI());
        float ratio = (-69- advertisedDevice.getRSSI())/(10 * 2);
        float Distance = pow(10,ratio);
        MYDEBUG_PRINT("    -- Device DISTANCE : ");
        MYDEBUG_PRINTLN(Distance);
      }
    }
};

/**
 * Fonction régulière pour notre client BLE qui va scanner les devices à proximité
 * - on limite le temps de scan à scanTime secondes pour ne pas bloquer la carte
 * - une fois terminé, on efface les résultats pour pouvoir recommencer tranquillement
 */
void loopBLEClient() {
  MYDEBUG_PRINT("-BLE Client : Scan @ ");
  timeClient.update();
  MYDEBUG_PRINTLN(timeClient.getFormattedTime());
  
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);

  MYDEBUG_PRINT("-BLE Client : Devices trouvés ");
  MYDEBUG_PRINTLN(foundDevices.getCount());

  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  MYDEBUG_PRINTLN("-BLE Client : Scan terminé");
}

/**
 * Configuration du client BLE
 * - Initialisation de l'objet qui va scanner
 * - Association d'un callback
 * - Activation du scan
 */
void setupBLEClient() {
  MYDEBUG_PRINTLN("-BLE Client : Démarrage");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  MYDEBUG_PRINTLN("-BLE Client : Démarré");
}
