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


String strConfigFile("/config.json"); // ---------------------------- Nom du fichier de configuration
String strContactsFile("/contacts.json"); // ------------------------ Nom du fichier de contacts
String strPositiveListFile("/positivelist.json"); // ---------------- Nom du fichier de liste des positifs
String strTestFile("/spiffs_test.txt"); // -------------------------- Nom du fichier de test
String strTrackingFile("/spiffs_tracking.txt"); // ------------------ Nom du fichier de tracking
File configFile, trackingFile, contactsFile, positiveListFile; // --- Fichiers
const int MAX_CONTACTS = 50; // ------------------------------------- Maximum number of contacts


void logTracking(String strTrackingText){ // ---------------------- Ecriture dans le fichier de tracking
    trackingFile = SPIFFS.open(strTrackingFile, FILE_APPEND); // ----- Ouverture du fichier en écriture
    if (trackingFile) { 
        timeClient.update(); // ------------------------------------ Mise à jour de l'heure
        trackingFile.print(timeClient.getFormattedTime()); // ------- Ecriture de l'heure
        trackingFile.print("\t"); 
        trackingFile.println(strTrackingText); // ------------------- Ecriture du texte
        trackingFile.close(); // ------------------------------------ Fermeture du fichier
    } else{
        MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier"); 
    }
}

void setupSPIFFS(bool bFormat = false){ // -------------------------- Initialisation du système de fichier

    MYDEBUG_PRINTLN("-SPIFFS : Montage du système de fichier"); 

    if (SPIFFS.begin(true)) { // ------------------------------------ Montage du système de fichier
        MYDEBUG_PRINTLN("-SPIFFS : MONTE");

        if (bFormat){
            SPIFFS.format(); // ------------- Au besoin, pour formatter le système de fichiers
            bFormat = false;
        }

        // Fichier de test
        if (SPIFFS.exists(strTestFile)) {  // ------------------------- Le fichier existe
            File testFile = SPIFFS.open(strTestFile, "r");
            if (testFile){ // ---------------------------------------- Ouverture du fichier en lecture
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
                    String parametre1 = jsonDocument["ssid"].as<String>(); // --- Récupération des paramètres
                    String parametre2 = jsonDocument["password"].as<String>();
                    String parametre3 = jsonDocument["APssid"].as<String>();
                    String parametre4 = jsonDocument["APpassword"].as<String>();
                    int parametre5 = jsonDocument["minutes_stand_by"].as<int>(); // --- Récupération des paramètres
                    int parametre6 = jsonDocument["days_of_historic"].as<int>();

                    sstation_ssid = parametre1; // ------------------------ Affectation des paramètres
                    sstation_password = parametre2; 
                    aap_ssid = parametre3;
                    aap_password = parametre4; 
                    minutes_stand_by = parametre5; 
                    days_of_historic = parametre6;

                    MYDEBUG_PRINT("-JSON [ssid] : "); // ------------------ Affichage des paramètres
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
            File configFile = SPIFFS.open(strConfigFile, "w"); // ----- Ouverture du fichier en écriture
            if (configFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                DynamicJsonDocument jsonDocument(512);
                // Initialisation des valeurs par défaut
                jsonDocument["ssid"] = String("DEFAULT_SSID");
                jsonDocument["password"] = String("DEFAULT_PASSWORD");
                jsonDocument["APssid"] = String("ESP32_AP");
                jsonDocument["APpassword"] = String("12345678");
                jsonDocument["minutes_stand_by"] = int(5);
                jsonDocument["days_of_historic"] = int(30);
                // Sérialisation du JSON dans le fichier de configuration
                if (serializeJson(jsonDocument, configFile) == 0) {
                    MYDEBUG_PRINTLN("-SPIFFS : 2222 Impossible d'écrire le JSON dans le fichier de configuration");
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
                /*JsonObject contact1 = contactsArray.createNestedObject();
                contact1["id-1"] = DEVICE_NAME;
                contact1["id-2"] = "ESP32-ADRIEN";
                contact1["timestamp"] = "2023-04-11T12:23:08";*/
                JsonObject contact2 = contactsArray.createNestedObject();
                contact2["id-1"] = "ESP32-NOA";
                contact2["id-2"] = DEVICE_NAME;
                contact2["timestamp"] = "2024-04-11T12:17:08";
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
                /*positiveListArray.add("ESP32-NOA");
                positiveListArray.add("ESP32-ADRIEN");*/
                positiveListArray.add("ESP32-DIMITRI");
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
    // Delete the existing config file
    SPIFFS.remove(strConfigFile);

    // Open the config file in write mode
    configFile = SPIFFS.open(strConfigFile, "w");
    if (configFile) {
        MYDEBUG_PRINTLN("-SPIFFS: Fichier ouvert en écriture");

        // Create a JSON document and set the configuration values
        DynamicJsonDocument jsonDocument(512);
        jsonDocument["ssid"] = config.ssid;
        jsonDocument["password"] = config.password;
        jsonDocument["APssid"] = config.APssid;
        jsonDocument["APpassword"] = config.APpassword;
        jsonDocument["minutes_stand_by"] = config.minutes_stand_by;
        jsonDocument["days_of_historic"] = config.days_of_historic;

        // Serialize the JSON document to the config file
        if (serializeJson(jsonDocument, configFile) == 0) {
            MYDEBUG_PRINTLN("-SPIFFS: Impossible d'écrire le JSON dans le fichier config.json");
        }

        // Close the config file
        configFile.close();
        MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening config.json");
    }
}

struct Config loadConfig(){
    configFile = SPIFFS.open(strConfigFile, "r");
    struct Config config;
    if (configFile) {
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
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening config.json");
    }
    return config;
}

struct Contact {
    String id1;
    String id2;
    String timestamp;
};

int calculateDifferenceInDays(String currentTime, String contactTime) {
    struct tm tm1 = {0};
    struct tm tm2 = {0};
    strptime(currentTime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm1);
    strptime(contactTime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm2);
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    return (t1 - t2) / 86400;
}

void deletePositive(String id) {
    // Open the file in read mode to read existing positive IDs
    File positiveListFile = SPIFFS.open(strPositiveListFile, "r");
    if (!positiveListFile) {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json for reading");
        return;
    }

    // Read the JSON data from the file
    DynamicJsonDocument jsonDocument(1024);
    DeserializationError error = deserializeJson(jsonDocument, positiveListFile);
    if (error) {
        MYDEBUG_PRINTLN("-SPIFFS: Error parsing positivelist.json");
        positiveListFile.close();
        return;
    }

    // Close the file after reading
    positiveListFile.close();

    // Extract positive IDs from JSON data
    JsonArray positiveListArray = jsonDocument["positive_list"].as<JsonArray>();
    JsonArray updatedPositiveList;
    bool found = false;

    // Iterate through positive IDs and remove the specified ID
    for (const auto& entry : positiveListArray) {
        String currentID = entry.as<String>();
        if (currentID != id) {
            updatedPositiveList.add(currentID);
        } else {
            found = true;
        }
    }

    // If ID not found, no need to update the file
    if (!found) {
        MYDEBUG_PRINTLN("-SPIFFS: ID not found in positive list, nothing to delete");
        return;
    }

    // Open the file in write mode to write updated positive IDs
    positiveListFile = SPIFFS.open(strPositiveListFile, "w");
    if (!positiveListFile) {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json for writing");
        return;
    }

    // Add updated positive IDs to the JSON array
    JsonObject jsonRoot = jsonDocument.to<JsonObject>();
    jsonRoot["positive_list"] = updatedPositiveList;

    // Serialize JSON data and write to the file
    if (serializeJson(jsonRoot, positiveListFile) == 0) {
        MYDEBUG_PRINTLN("-SPIFFS: Failed to write JSON to positivelist.json");
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: ID deleted from positive list");
    }

    // Close the file after writing
    positiveListFile.close();
}


void checkContacts(String currentTime){
    // Array to store existing contacts
    Contact keepContacts[MAX_CONTACTS];
    int numKeepContacts = 0;

    // Open the file in read mode to read existing contacts
    contactsFile = SPIFFS.open(strContactsFile, "r");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened for reading");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, contactsFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json in checkContacts()");
        } else {
            // Parse JSON data
            MYDEBUG_PRINTLN("-SPIFFS: Parsing contacts.json");
            JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
            MYDEBUG_PRINTLN("-SPIFFS: Contacts array parsed");
            //struct Config configTemp = loadConfig();
            int days = 30; //configTemp.days_of_historic

            // Check each contact
            for (int i = 0; i < contactsArray.size(); i++) {
                JsonObject contact = contactsArray[i];
                String contactTime = contact["timestamp"].as<String>();
                MYDEBUG_PRINTLN("Contact time: " + contactTime);
                int diff = calculateDifferenceInDays(currentTime, contactTime);
                MYDEBUG_PRINTLN("test print");

                if (diff < days) {
                    // Contact is still valid, move to keepContacts
                    keepContacts[numKeepContacts].id1 = contact["id-1"].as<String>();
                    keepContacts[numKeepContacts].id2 = contact["id-2"].as<String>();
                    keepContacts[numKeepContacts].timestamp = contact["timestamp"].as<String>();
                    numKeepContacts++;
                    MYDEBUG_PRINTLN("Contact valid: " + contact["id-1"].as<String>() + " - " + contact["id-2"].as<String>());
                } else {
                    // Contact is invalid, do not move to keepContacts
                    MYDEBUG_PRINTLN("Contact expired: " + contact["id-1"].as<String>() + " - " + contact["id-2"].as<String>());
                    // delete the contact in positive list
                    /*
                    if (contact["id-1"].as<String>() == DEVICE_NAME) {
                        MYDEBUG_PRINTLN("Delete positive: " + contact["id-2"].as<String>());
                        deletePositive(contact["id-2"].as<String>());
                    } else if (contact["id-2"].as<String>() == DEVICE_NAME) {
                        MYDEBUG_PRINTLN("Delete positive: " + contact["id-1"].as<String>());
                        deletePositive(contact["id-1"].as<String>());
                    }
                     */
                }
                MYDEBUG_PRINTLN("Difference in days: " + String(diff));
            }

            // Close the file after reading
            contactsFile.close();
            MYDEBUG_PRINTLN("-SPIFFS: File closed");

            // Open the file in write mode to write updated contacts
            contactsFile = SPIFFS.open(strContactsFile, "w");
            if (contactsFile) {
                MYDEBUG_PRINTLN("-SPIFFS: File opened for writing");
                DynamicJsonDocument jsonDocument(1024);
                JsonArray contactsArray = jsonDocument.createNestedArray("list_of_contacts");
                // Add valid contacts from the array to the JSON array
                for (int i = 0; i < numKeepContacts; i++) {
                    JsonObject obj = contactsArray.createNestedObject();
                    obj["id-1"] = keepContacts[i].id1;
                    obj["id-2"] = keepContacts[i].id2;
                    obj["timestamp"] = keepContacts[i].timestamp;
                }
                // Serialize JSON data and write to the file
                if (serializeJson(jsonDocument, contactsFile) == 0) {
                    MYDEBUG_PRINTLN("-SPIFFS: Failed to write JSON to contacts.json");
                }
                // Close the file after writing
                contactsFile.close();
                MYDEBUG_PRINTLN("-SPIFFS: File closed");
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json for writing");
            }
        }
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json for reading");
    }
}



void saveContact(String id1, String id2, String timestamp){
    // Array to store existing contacts
    Contact contactsList[MAX_CONTACTS];
    int numContacts = 0;
    bool Doublon = false;

    // Open the file in read mode to read existing contacts
    contactsFile = SPIFFS.open(strContactsFile, "r");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, contactsFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json in saveContact()");
        } else {
            // Parse JSON data
            JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
            // Ensure we don't exceed the maximum contacts
            int maxContacts = contactsArray.size() < MAX_CONTACTS ? contactsArray.size() : MAX_CONTACTS;
            for (int i = 0; i < maxContacts; i++) {
                JsonObject contact = contactsArray[i];
                String existingID1 = contact["id-1"].as<String>();
                String existingID2 = contact["id-2"].as<String>();
                Contact c;
                c.id1 = contact["id-1"].as<String>();
                c.id2 = contact["id-2"].as<String>();
                c.timestamp = contact["timestamp"].as<String>();
                contactsList[numContacts++] = c;
                String currentID1 = c.id1;
                String currentID2 = c.id2;

                // Check for duplicates
                if ((existingID1 == id1 && existingID2 == id2) || (existingID1 == id2 && existingID2 == id1)) {
                    Doublon = true;
                    MYDEBUG_PRINTLN("Duplicate found for IDs: " + id1 + " and " + id2);
                    break; // Exit the loop once a duplicate is found
                }
            }

            // Add the new contact to the list if no duplicate found and not reached max contacts
            if (!Doublon && numContacts < MAX_CONTACTS) {
                Contact newContact;
                newContact.id1 = id1;
                newContact.id2 = id2;
                newContact.timestamp = timestamp;
                contactsList[numContacts++] = newContact;
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Max contacts reached or duplicate found, new contact not added");
            }
        }
        contactsFile.close(); // Close the file after reading
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
        return; // Exit the function if unable to open the file
    }

    // Open the file in write mode to write updated contacts
    contactsFile = SPIFFS.open(strContactsFile, "w");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened for writing");
        DynamicJsonDocument jsonDocument(1024);
        JsonArray contactsArray = jsonDocument.createNestedArray("list_of_contacts");
        // Add contacts from the array to the JSON array
        for (int i = 0; i < numContacts; i++) {
            JsonObject obj = contactsArray.createNestedObject();
            obj["id-1"] = contactsList[i].id1;
            obj["id-2"] = contactsList[i].id2;
            obj["timestamp"] = contactsList[i].timestamp;
        }
        // Serialize JSON data and write to the file
        if (serializeJson(jsonDocument, contactsFile) == 0) {
            MYDEBUG_PRINTLN("-SPIFFS: Failed to write JSON to contacts.json");
        }
        contactsFile.close(); // Close the file after writing
        MYDEBUG_PRINTLN("-SPIFFS: File closed");
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json for writing");
    }
    //checkContacts(timestamp);
}


void savePositiveContact(String id) {
    MYDEBUG_PRINTLN("Saving positive contact");
    // Array to store existing positive IDs
    String positiveIDs[MAX_CONTACTS];
    int numPositiveIDs = 0;

    // Open the file in read mode to read existing positive IDs
    positiveListFile = SPIFFS.open(strPositiveListFile, "r");
    if (positiveListFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, positiveListFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing positivelist.json");
        } else {
            // Parse JSON data
            JsonArray positiveListArray = jsonDocument["positive_list"].as<JsonArray>();
            // Ensure we don't exceed the maximum positive IDs
            int maxPositiveIDs = positiveListArray.size() < MAX_CONTACTS ? positiveListArray.size() : MAX_CONTACTS;
            for (int i = 0; i < maxPositiveIDs; i++) {
                positiveIDs[numPositiveIDs++] = positiveListArray[i].as<String>();
            }
            // Add the new positive ID to the list
            if (numPositiveIDs < MAX_CONTACTS) {
                positiveIDs[numPositiveIDs++] = id;
            } else {
                MYDEBUG_PRINTLN("-SPIFFS: Max positive IDs reached, new ID not added");
            }
        }
        positiveListFile.close(); // Close the file after reading
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json");
        return; // Exit the function if unable to open the file
    }

    // Open the file in write mode to write updated positive IDs
    positiveListFile = SPIFFS.open(strPositiveListFile, "w");
    if (positiveListFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened for writing");
        DynamicJsonDocument jsonDocument(1024);
        JsonArray positiveListArray = jsonDocument.createNestedArray("positive_list");
        // Add positive IDs from the array to the JSON array
        for (int i = 0; i < numPositiveIDs; i++) {
            positiveListArray.add(positiveIDs[i]);
        }
        // Serialize JSON data and write to the file
        if (serializeJson(jsonDocument, positiveListFile) == 0) {
            MYDEBUG_PRINTLN("-SPIFFS: Failed to write JSON to positivelist.json");
        }
        positiveListFile.close(); // Close the file after writing
        MYDEBUG_PRINTLN("-SPIFFS: File closed");
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json for writing");
    }
}

void CheckAddPositive(String id) {
    // Array to store existing contacts
    Contact contactsList[MAX_CONTACTS];
    int numContacts = 0;
    bool idFound = false;

    if (id == DEVICE_NAME) {
        MYDEBUG_PRINTLN("ID trouvé dans la liste des positifs");
        return;
    }

    // Open the file in read mode to read existing contacts
    contactsFile = SPIFFS.open(strContactsFile, "r");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, contactsFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json");
        } else {
            // Parse JSON data
            JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
            // Ensure we don't exceed the maximum contacts
            int maxContacts = contactsArray.size() < MAX_CONTACTS ? contactsArray.size() : MAX_CONTACTS;
            for (int i = 0; i < maxContacts; i++) {
                JsonObject contact = contactsArray[i];
                Contact c;
                c.id1 = contact["id-1"].as<String>();
                c.id2 = contact["id-2"].as<String>();
                c.timestamp = contact["timestamp"].as<String>();
                contactsList[numContacts++] = c;

                // Check if the id is in the contacts list
                if (c.id1 == id || c.id2 == id) {
                    MYDEBUG_PRINTLN("ID trouvé dans la liste des contacts");
                    savePositiveContact(id);
                    idFound = true;
                    break;
                }
            }
        }
        contactsFile.close(); // Close the file after reading
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
        return; // Exit the function if unable to open the file
    }

    if (!idFound) {
        MYDEBUG_PRINTLN("ID non trouvé dans la liste des contacts");
    }
}

String getEtatSante(String id) {
    //cette fonction vérifie la liste des positifs et des contacts et si un contact est positif, elle retourne "cas contact"
    //sinon elle retourne "négatif"
    // la fonction dans un premier temps etablie la liste des rencontres (on parcours contact et on recupere tt les id n'étant pas le notre)
    // puis on parcours la liste des positifs et si on trouve un id dans la liste des rencontres on retourne "cas contact"
    String contactsList[MAX_CONTACTS];
    String positiveIDs[MAX_CONTACTS];
    int numContacts = 0;
    int numPositiveIDs = 0;


    // Open the file in read mode to read existing contacts
    contactsFile = SPIFFS.open(strContactsFile, "r");
    if (contactsFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, contactsFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing contacts.json");
        } else {
            // Parse JSON data
            JsonArray contactsArray = jsonDocument["list_of_contacts"].as<JsonArray>();
            // Ensure we don't exceed the maximum contacts
            int maxContacts = contactsArray.size() < MAX_CONTACTS ? contactsArray.size() : MAX_CONTACTS;
            for (int i = 0; i < maxContacts; i++) {
                JsonObject contact = contactsArray[i];
                Contact c;
                c.id1 = contact["id-1"].as<String>();
                c.id2 = contact["id-2"].as<String>();
                c.timestamp = contact["timestamp"].as<String>();

                if (c.id1 != id) {
                    contactsList[numContacts++] = c.id1;
                } else if (c.id2 != id) {
                    contactsList[numContacts++] = c.id2;
                }
            }
        }
        contactsFile.close(); // Close the file after reading
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening contacts.json");
        return "Internal Error"; // Exit the function if unable to open the file
    }

    // Open the file in read mode to read existing positive IDs
    positiveListFile = SPIFFS.open(strPositiveListFile, "r");
    if (positiveListFile) {
        MYDEBUG_PRINTLN("-SPIFFS: File opened");
        DynamicJsonDocument jsonDocument(1024);
        DeserializationError error = deserializeJson(jsonDocument, positiveListFile);
        if (error) {
            MYDEBUG_PRINTLN("-SPIFFS: Error parsing positivelist.json");
        } else {
            // Parse JSON data
            JsonArray positiveListArray = jsonDocument["positive_list"].as<JsonArray>();
            // Ensure we don't exceed the maximum positive IDs
            int maxPositiveIDs = positiveListArray.size() < MAX_CONTACTS ? positiveListArray.size() : MAX_CONTACTS;
            for (int i = 0; i < maxPositiveIDs; i++) {
                positiveIDs[numPositiveIDs++] = positiveListArray[i].as<String>();
            }
            // Check if the id is in the contacts list
            for (int i = 0; i < numContacts; i++) {
                for (int j = 0; j < numPositiveIDs; j++) {
                    if (contactsList[i] == positiveIDs[j]) {
                        return "cas contact";
                    }
                }
            }
        }
        positiveListFile.close(); // Close the file after reading
    } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json");
        return "Internal Error"; // Exit the function if unable to open the file
    }
    return "négatif";
}
