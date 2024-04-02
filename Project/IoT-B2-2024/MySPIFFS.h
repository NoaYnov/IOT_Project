/**
 * \file MySPIFFS.h
 * \page spiffs SPIFFS
 * \brief (S)erial (P)eripheral (I)nterface (F)lash (F)ile (S)ystem 
 * La structure mémoire de l'ESP32 se découpe en plusieurs "zones":
 * \verbatim
|--------------|-------|---------------|--|--|--|--|--|
^              ^       ^               ^     ^
Sketch    OTA update   File system   EEPROM  WiFi config (SDK)
\endverbatim
 * Même si l'espace "File System" est situé sur le même chip que le programme,
 * le fait d'installer un nouveau programme n'affectera pas le contenu du File System. Cela peut nous permettre
 * de stocker des fichiers de données (type historique), de configuration ou du contenu pour un serveur web (feuille de
 * style, javascript ...) par exemple.
 * 
 * - Sketch : Cette zone est lue à chaque démarrage , les variables sont donc réinitialisées avec les valeurs par défaut.
 * - OTA Update : L’espace OTA est un dossier temporaire pour les mises à jour via OTA (Over The Air, cf. Device Management).
 * - EEPROM : Permet de stocker des infos, qui pourront être lues après redémarrage. Nous sommes sur une mémoire plus lente 
 *   (que la mémoire dédiée au sketch et qui dispose d’une durée de vie limitée en écriture . Ne pas en abuser).
 * - File system (SPIFFS) est un espace de stockage &quivalent à celui d'une carte SD. Selon les modules utilisés, la taille 
 *   du fichier systéme peut varier de 4MB/32Mb à 16MB/128Mb.
 *   
 * ESP8266 Sketch Data Upload 
 * Il s'agit d'un plugin Arduino qui permet d’uploader des fichiers dans la mémoire SPIFFS depuis 
 * l’interface Arduino. Rendez-vous sur la page GitHub du plugin pour la procédure d'installation : https://github.com/me-no-dev/arduino-esp32fs-plugin
 * - Télécharger https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/download/1.0/ESP32FS-1.0.zip
 * - Dans le dossier arduino (emplacement du carnet de croquis défini dans les préférences), créér un dossier "tools", s'il n'est pas déjà présent.
 * - Décompresser le fichier zip téléchargé dans ce dossier "tools".
 * - Re-démarrer l'IDE Arduino pour qu'il prenne en compte le nouveau plugin au redémarrage.
 * - Dans le dossier de votre Sketch, créér un dossier "data". Déposer un ou plusieurs petits fichiers format txt dans ce dossier; ils seront 
 *   transférés dans la mémoire SPIFFS par le plugin (le fichier spiffs_test.txt est a priori déjà présent).
 * - Ouvrez vos Sketch Arduino dans l'IDE et sélectionner dans le menu "Outils" : ESP32 Sketch data upload.
 * Le contenu du dossier data sera transféré dans la mémoire SPIFFS. Le transfert sera plus ou moins long en fonction de la 
 * vitesse d’upload sélectionnée.
 *  
 * Comment accéder au File System :
 * Pour pouvoir utiliser les fonctions du File System il faut inclure la bibliothèque SPIFFS.h
 * #include "SPIFFS.h"
 * Documentation complète : https://github.com/esp8266/Arduino/blob/master/doc/filesystem.rst
 *
 * Dans cet exemple nous utilisons le format JSON pour lire et écrire dans le fichier de configuration.
 * Nous utilisons la bibliothèque Arduino JSON de Benoît Blanchon. A télécharger dans le gestionnaire de bibliothèque.
 *
 * Fichier \ref MySPIFFS.h
 */

#include "SPIFFS.h"
#include <ArduinoJson.h>   //Arduino JSON by Benoit Blanchon : https://github.com/bblanchon/ArduinoJson

String strConfigFile("/config.json");
String strContactsFile("/contacts.json");
String strPositiveListFile("/positivelist.json");
String strTestFile("/spiffs_test.txt");
String strTrackingFile("/spiffs_tracking.txt");
File configFile, trackingFile, contactsFile, positiveListFile;

void logTracking(String strTrackingText){
    trackingFile = SPIFFS.open(strTrackingFile, FILE_APPEND);
    if (trackingFile) {
        timeClient.update();
        trackingFile.print(timeClient.getFormattedTime());
        trackingFile.print("\t");
        trackingFile.println(strTrackingText);
        trackingFile.close();
    } else{
        MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier");
    }
}

void setupSPIFFS(bool bFormat = false){

    MYDEBUG_PRINTLN("-SPIFFS : Montage du système de fichier");

    if (SPIFFS.begin(true)) {
        MYDEBUG_PRINTLN("-SPIFFS : MONTE");

        if (bFormat){
            SPIFFS.format(); // ------------- Au besoin, pour formatter le système de fichiers
            bFormat = false;
        }

        // Fichier de test
        if (SPIFFS.exists(strTestFile)) {  // ------------------------- Le fichier existe
            File testFile = SPIFFS.open(strTestFile, "r");
            if (testFile){
                MYDEBUG_PRINTLN("-SPIFFS : Lecture du fichier spiffs_test.txt");
                while(testFile.available()){
                    Serial.write(testFile.read());
                }
                MYDEBUG_PRINTLN("");
            }
        }

        // Fichier de configuration
        if (SPIFFS.exists(strConfigFile)) {  // ------------------------- Le fichier existe
            //Ouverture du fichier en lecture
            MYDEBUG_PRINTLN("-SPIFFS : Lecture du fichier de configuration");
            configFile = SPIFFS.open(strConfigFile, "r");
            if (configFile) {
                MYDEBUG_PRINTLN("-SPIFFS : Fichier ouvert");
                //Lecture du fichier sous la forme d'un document JSON
                DynamicJsonDocument jsonDocument(512);
                // Désérialisation du document JSON lu
                DeserializationError error = deserializeJson(jsonDocument, configFile);
                if (error){
                    MYDEBUG_PRINTLN("-SPIFFS : Impossible de parser le JSON");
                } else {
                    MYDEBUG_PRINTLN("-JSON: Fichier parsé");
                    String parametre1 = jsonDocument["ssid"].as<String>();
                    String parametre2 = jsonDocument["password"].as<String>();
                    String parametre3 = jsonDocument["APssid"].as<String>();
                    String parametre4 = jsonDocument["APpassword"].as<String>();
                    int parametre5 = jsonDocument["minutes_stand_by"].as<int>();
                    int parametre6 = jsonDocument["days_of_historic"].as<int>();

                    sstation_ssid = parametre1;
                    sstation_password = parametre2;
                    aap_ssid = parametre3;
                    aap_password = parametre4;
                    minutes_stand_by = parametre5;
                    days_of_historic = parametre6;

                    MYDEBUG_PRINT("-JSON [ssid] : ");
                    MYDEBUG_PRINTLN(sstation_ssid);
                    MYDEBUG_PRINT("-JSON [password] : ");
                    MYDEBUG_PRINTLN(sstation_password);
                    MYDEBUG_PRINT("-JSON [APssid] : ");
                    MYDEBUG_PRINTLN(aap_ssid);
                    MYDEBUG_PRINT("-JSON [APpassword] : ");
                    MYDEBUG_PRINTLN(aap_password);
                    MYDEBUG_PRINT("-JSON [minutes_stand_by] : ");
                    MYDEBUG_PRINTLN(minutes_stand_by);
                    MYDEBUG_PRINT("-JSON [days_of_historic] : ");
                    MYDEBUG_PRINTLN(days_of_historic);

                }
            }
            configFile.close();
            MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
        } else {                              // ------------------- Le fichier n'existe pas
            // Initialisation du fichier de configuration avec des valeurs vides
            MYDEBUG_PRINTLN("-SPIFFS: Le fichier de configuration n'existe pas");
            File configFile = SPIFFS.open(strConfigFile, "w");
            if (configFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                DynamicJsonDocument jsonDocument(512);
                // Exemple de 3 paramètres
                jsonDocument["ssid"] = String("DefaultSSID");
                jsonDocument["password"] = String("DefaultPassword");
                jsonDocument["APssid"] = String("ESP32_AP");
                jsonDocument["APpassword"] = String("AP_Password");
                jsonDocument["minutes_stand_by"] = int(5);
                jsonDocument["days_of_historic"] = int(30);
                // Sérialisation du JSON dans le fichier de configuration
                if (serializeJson(jsonDocument, configFile) == 0) {
                    MYDEBUG_PRINTLN("-SPIFFS : Impossible d'écrire le JSON dans le fichier de configuration");
                }
                // Fermeture du fichier
                configFile.close();
                MYDEBUG_PRINTLN("-SPIFFS : Fichier fermé");
            } else{
                MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier en ecriture");
            }
        }

        // Fichier de Tracking
        if (SPIFFS.exists(strTrackingFile)) {  // ------------------------- Le fichier existe
            //Ouverture du fichier en lecture
            MYDEBUG_PRINTLN("-SPIFFS : Lecture du fichier de tracking");
            trackingFile = SPIFFS.open(strTrackingFile, "r");
            if (trackingFile) {
                while(trackingFile.available()){
                    Serial.write(trackingFile.read());
                }
                MYDEBUG_PRINTLN("");
            }
            //trackingFile.close();
            //MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
            } else {                              // ------------------- Le fichier n'existe pas
            // Initialisation du fichier de configuration avec des valeurs vides
            MYDEBUG_PRINTLN("-SPIFFS: Le fichier de tracking n'existe pas");
            trackingFile = SPIFFS.open(strTrackingFile, "w");
            if (trackingFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                // Fermeture du fichier
                trackingFile.close();
                MYDEBUG_PRINTLN("-SPIFFS : Fichier fermé");
            } else{
                MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier en ecriture");
            }
        }

        if (SPIFFS.exists(strContactsFile)) {
            MYDEBUG_PRINTLN("-SPIFFS: Lecture du fichier contacts.json");
            contactsFile = SPIFFS.open(strContactsFile, "r");
            if (contactsFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier ouvert");
                DynamicJsonDocument jsonDocument(1024);
                DeserializationError error = deserializeJson(jsonDocument, contactsFile);
                if (error) {
                    MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json");
                } else {
                    //Gestion contenu de fichier JSON
                    JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
                    for (JsonVariant contact : contactsArray) {
                        MYDEBUG_PRINTLN("Contact : ");
                        String id1 = contact["id-1"].as<String>();
                        String id2 = contact["id-2"].as<String>();
                        String timestamp = contact["timestamp"].as<String>();
                        MYDEBUG_PRINTLN("Contact ID-1: " + id1);
                        MYDEBUG_PRINTLN("Contact ID-2: " + id2);
                        MYDEBUG_PRINTLN("Contact Timestamp: " + timestamp);
                    }
                }
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
            }
        } else {
            MYDEBUG_PRINTLN("-SPIFFS: contacts.json does not exist");
            File contactsFile = SPIFFS.open(strContactsFile, "w");
            if (contactsFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                DynamicJsonDocument jsonDocument(1024);
                JsonArray contactsArray = jsonDocument.createNestedArray("list_of_contacts");
                JsonObject contact1 = contactsArray.createNestedObject();
                contact1["id-1"] = "103";
                contact1["id-2"] = "227";
                contact1["timestamp"] = "2024-04-01 12:23:08";
                JsonObject contact2 = contactsArray.createNestedObject();
                contact2["id-1"] = "103";
                contact2["id-2"] = "228";
                contact2["timestamp"] = "2024-04-01 12:23:08";
                if (serializeJson(jsonDocument, contactsFile) == 0) {
                    MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier contacts.json");
                }
                contactsFile.close();
                MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
            }

        }

        if (SPIFFS.exists(strPositiveListFile)) {
            MYDEBUG_PRINTLN("-SPIFFS: Lecture du fichier positivelist.json");
            positiveListFile = SPIFFS.open(strPositiveListFile, "r");
            if (positiveListFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier ouvert");
                DynamicJsonDocument jsonDocument(512);
                DeserializationError error = deserializeJson(jsonDocument, positiveListFile);
                if (error) {
                    MYDEBUG_PRINTLN("-SPIFFS: Error parsing positivelist.json");
                } else {
                    //Gestion contenu de fichier JSON
                    JsonArray positiveListArray = jsonDocument["positive_list"].as<JsonArray>();
                    for (JsonVariant contact : positiveListArray) {
                        String id = contact.as<String>();
                        MYDEBUG_PRINTLN("Positive ID: " + id);
                    }
                }
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json");
            }
        } else {
            MYDEBUG_PRINTLN("-SPIFFS: positivelist.json does not exist");
            File positiveListFile = SPIFFS.open(strPositiveListFile, "w");
            if (positiveListFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                DynamicJsonDocument jsonDocument(512);
                JsonArray positiveListArray = jsonDocument.createNestedArray("positive_list");
                positiveListArray.add("228");
                positiveListArray.add("104");
                positiveListArray.add("105");
                positiveListArray.add("106");
                positiveListArray.add("107");
                positiveListArray.add("108");
                if (serializeJson(jsonDocument, positiveListFile) == 0) {
                    MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier positivelist.json");
                }
                positiveListFile.close();
                MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json");
            }
        }

        //SPIFFS.end();
    } else {
        MYDEBUG_PRINT("-SPIFFS : Impossible de monter le système de fichier");
    }
}

//create a config object

struct Config {
    String ssid;
    String password;
    String APssid;
    String APpassword;
    int minutes_stand_by;
    int days_of_historic;
};

void saveConfig(struct Config config){
    configFile = SPIFFS.open(strConfigFile, "r");
    if (configFile) {
        MYDEBUG_PRINTLN("-SPIFFS : Fichier ouvert");
        DynamicJsonDocument jsonDocument(512);
        DeserializationError error = deserializeJson(jsonDocument, configFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing config.json");
        } else {
            jsonDocument["ssid"] = config.ssid;
            jsonDocument["password"] = config.password;
            jsonDocument["APssid"] = config.APssid;
            jsonDocument["APpassword"] = config.APpassword;
            jsonDocument["minutes_stand_by"] = config.minutes_stand_by;
            jsonDocument["days_of_historic"] = config.days_of_historic;
            if (serializeJson(jsonDocument, configFile) == 0) {
                MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier config.json");
            }
            configFile.close();
            MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
        }
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening config.json");
    }
}

struct Config loadConfig(){
    configFile = SPIFFS.open(strConfigFile, "r");
    struct Config config;
    if (configFile) {
        MYDEBUG_PRINTLN("-SPIFFS : Fichier ouvert");
        DynamicJsonDocument jsonDocument(512);
        DeserializationError error = deserializeJson(jsonDocument, configFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing config.json");
        } else {
            config.ssid = jsonDocument["ssid"].as<String>();
            config.password = jsonDocument["password"].as<String>();
            config.APssid = jsonDocument["APssid"].as<String>();
            config.APpassword = jsonDocument["APpassword"].as<String>();
            config.minutes_stand_by = jsonDocument["minutes_stand_by"].as<int>();
            config.days_of_historic = jsonDocument["days_of_historic"].as<int>();
        }
        configFile.close();
        MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening config.json");
    }
    return config;
}

unsigned long calculateEpochTime(String timestamp) {
    // Split the timestamp into date and time
    int splitT = timestamp.indexOf("T");
    String dateStr = timestamp.substring(0, splitT);
    String timeStr = timestamp.substring(splitT + 1);

    // Convert date and time strings to integers
    int year = dateStr.substring(0, 4).toInt();
    int month = dateStr.substring(5, 7).toInt();
    int day = dateStr.substring(8, 10).toInt();
    int hour = timeStr.substring(0, 2).toInt();
    int minute = timeStr.substring(3, 5).toInt();
    int second = timeStr.substring(6, 8).toInt();

    // Calculate and return epoch time
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    time_t epochTime = mktime(&tm);
    return static_cast<unsigned long>(epochTime);
}

int calculateDifferenceInDays(unsigned long currentTime, unsigned long contactTime) {
    // Calculate difference in seconds
    unsigned long diffSeconds = currentTime - contactTime;
    // Convert seconds to days
    int diffDays = diffSeconds / 86400;
    return diffDays;
}

void checkContacts(){
    contactsFile = SPIFFS.open(strContactsFile, "r+");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS : Fichier ouvert");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, contactsFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json");
        } else {
            JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
            struct Config config = loadConfig();
            int days = config.days_of_historic;
            unsigned long currentTime = timeClient.getEpochTime();
            for (int i = 0; i < contactsArray.size(); i++) {
                unsigned long contactTime = calculateEpochTime(contactsArray[i]["timestamp"].as<String>());
                MYDEBUG_PRINTLN("Contact Time: " + String(contactTime));
                int diff = calculateDifferenceInDays(currentTime, contactTime);
                MYDEBUG_PRINTLN("Difference in days: " + diff);
                if (diff > days) {
                    contactsArray.remove(i);
                    i--; // Adjust index since we removed an element
                } else {
                    MYDEBUG_PRINTLN("Contact still valid");
                }
            }
            contactsFile.seek(0); // Move cursor to the beginning of the file
            if (serializeJson(jsonDocument, contactsFile) == 0) {
                MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier contacts.json");
            }
            contactsFile.close();
            MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
        }
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
    }
}

void saveContact(String id1, String id2, String timestamp){
    contactsFile = SPIFFS.open(strContactsFile, "r");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS : Fichier ouvert");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, contactsFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json");
        } else {
            JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
            JsonObject contact = contactsArray.createNestedObject();
            contact["id-1"] = id1;
            contact["id-2"] = id2;
            contact["timestamp"] = timestamp;
            if (serializeJson(jsonDocument, contactsFile) == 0) {
                MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier contacts.json");
            }
            contactsFile.close();
            MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
            checkContacts();
        }
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
    }
}

void savePositive(String id){
    positiveListFile = SPIFFS.open(strPositiveListFile, "r");
    if (positiveListFile) {
        MYDEBUG_PRINTLN("-SPIFFS : Fichier ouvert");
        DynamicJsonDocument jsonDocument(512);
        DeserializationError error = deserializeJson(jsonDocument, positiveListFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing positivelist.json");
        } else {
            JsonArray positiveListArray = jsonDocument["positive_list"].as<JsonArray>();
            positiveListArray.add(id);
            if (serializeJson(jsonDocument, positiveListFile) == 0) {
                MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier positivelist.json");
            }
            positiveListFile.close();
            MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
        }
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json");
    }
}
