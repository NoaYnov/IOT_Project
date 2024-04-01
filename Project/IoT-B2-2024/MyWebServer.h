/**
 * \file MyWebServer.h
 * \page webserver Serveur Web
 * \brief Un serveur web interne à l'ESP32 pour configurer et voir les données
 * 
 * Comme son nom l'indique, un objet WebServer permet d'écouter des requêtes web, de les analyser et
 * d'y répondre. Le documentation est disponible ici : https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h.
 * Ainsi que de nombreux exemples : https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer/examples
 * 
 * Le WebServer est instancié avec le constructeur avec un numéro de port.
 * Les princales fonctions sont :
 * - begin() pour démarrer le serveur
 * - on(uri, fonction) pour appeler une fonction dédiée lorsqu'une route est sollicitée
 * - onNotFound(fonction) pour aller une fonction par défaut quand la route n'est pas connue
 * - handleClient() à appeler dans la loop pour gérer la récupération et le traitemnet de nouveaux messages
 * - send(code, type de contenu, contenu) pour envoyer une réponse avec un code, et un contenu d'un type donné
 * 
 * Dans cet exemple, le serveur web reçoit des requêtes HTTP et y répond sur les routes :
 * - / avec la fonction handleRoot()
 *   Elle donne accès aux fonctionnalités proposées
 * - /scan avec la fonction handleScan()
 *   Affiche la liste des réseaux WiFi disponibles
 * - /config avec la fonction handleConfig()
 *   Affiche un formulaire pour configurer la carte
 * - ...
 * - et avec handleNotFound() si la route n'est pas connue
 * 
 * Fichier \ref MyWebServer.h
 */

// librairies nécessaires 
#include <WebServer.h>


// Variables
WebServer monWebServeur(80);           // Serveur web sur le port 80

/**
 * Fonction de gestion de la route /
 */
void handleRoot() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete root");

  // Préparation du code HTML à retourner
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Bienvenue</h1><br>";
  out += "Depuis cette page, vous pouvez<br><ul>";
  out +="<li><a href=\"scan\"> Scanner le WiFi</a></li>";
  out +="<li><a href=\"adafruit\"> Adafruit</a></li>";
  out +="<li><a href=\"format\"> Formater le SPIFFS</a></li>";
  out +="<li><a href=\"config\"> Configurer le WiFi</a></li></ul>";
  out +="<li><a href=\"contact_tracer\">Vos Contacts</a></li></ul>";
  out += "</body></html>";

  // Envoi de la réponse en HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /scan
 */
void handleScan() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete scan");

  // La carte scanne les réseaux WiFi à proximité
  int n = WiFi.scanNetworks();

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='5'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Page de scan</h1><br>";

  // Intégration des réseaux WiFi trouvés dans la page HTML
  if (n == 0) {
    MYDEBUG_PRINTLN("- AUCUN réseau WiFi trouvé");
  } else {
    out += "<ul>";
    for (int i = 0; i < n; ++i) {
      out += "<li>"+ WiFi.SSID(i) +"</li>";
    }
    out += "</ul>";
  }

  // Fin de la réponse HTML
  out += "</body></html>";

  // Envoi de la page HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /config
 */
/**
 * Fonction de gestion de la route /config
 */
void handleConfig() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete config");

  // Check if the form has been submitted
  if (monWebServeur.args() > 0) {
    // Form data was submitted, print it in the console
    Serial.println("Form submitted:");
    for (uint8_t i = 0; i < monWebServeur.args(); i++) {
      Serial.print(monWebServeur.argName(i));
      Serial.print(": ");
      Serial.println(monWebServeur.arg(i));
    }
    Serial.println();
  }

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>Formulaire SSID et Mot de passe</title>";
  out += "<style>body {font-family: Arial, sans-serif;background-color: #f0f0f0;margin: 0;padding: 20px;}#range {display: flex;justify-content: center;align-items: center;}#outputSeconds, #outputDays {display: flex;justify-content: flex-end;align-items: center;padding-left: 15%;         font-weight: bold;}h1 {text-align: center;color: #333;}form {background-color: #fff;border-radius: 5px;padding: 20px;max-width: 400px;margin: 0 auto;}label {font-weight: bold;color: #666;}input[type='text'],input[type='password'],input[type='submit'] {width: 100%;padding: 10px;margin-bottom: 15px;border: 1px solid #ccc; border-radius: 4px;box-sizing: border-box;}input[type='submit'] {background-color: #4CAF50;color: white;border: none;cursor: pointer;}input[type='submit']:hover {background-color: #45a049;</style>";
  out += "</head><body>";
  out += "<h1>Page de config</h1><br>";
  out += "<form action='#' method='post'>";
  out += "<label for='ssid'>SSID :</label><br>";
  out += "<input type='text' id='ssid' name='ssid'><br><br>";
  out += "<label for='password'>Mot de passe :</label><br>";
  out += "<input type='password' id='password' name='password'><br><br>";
  out += "<label for='ap_ssid'>Access point SSID :</label><br>";
  out += "<input type='text' id='ap_ssid' name='ap_ssid'><br><br>";
  out += "<label for='ap_mdp'>Mot de passe :</label><br>";
  out += "<input type='password' id='ap_mdp' name='ap_mdp'><br><br>";
  out += "<label for='minutes'>minutes</label>";
  out += "<input type='range' id='seconds' name='seconds' min='0' max='60' value='0' step='1'>";
  out += "<output id='outputSeconds'>0</output>Temps proche d'une autre carte avant ajout aux contacts<br><br>";
  out += "<label for='days'>jours</label>";
  out += "<input type='range' id='days' name='days' min='0' max='30' value='0' step='1'>";
  out += "<output id='outputDays'>0</output>Nombres de jours avant suppression de la liste de contact<br><br>";
  out += "<input type='submit' value='Envoyer'>";
  out += "</form>";
  out += "<script>const secondsInput = document.getElementById('minutes');const daysInput = document.getElementById('days');const outputSeconds = document.getElementById('outputSeconds');const outputDays = document.getElementById('outputDays');secondsInput.addEventListener('input', function() {outputSeconds.textContent = this.value;});daysInput.addEventListener('input', function() {outputDays.textContent = this.value;});</script>";
  out += "</body></html>";

  // Envoi de la réponse HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /format
 */
void handleFormat() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete format");
  setupSPIFFS(true);
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Formatage fini</h1><br>";
  out += "<a href=\"/\"> Retour</a>";
  out += "</body></html>";
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /adafruit
 */
void handleAdafruit() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete adafruit");

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='5'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Adafruit</h1><br>";
  out += "<form action=\"\" method=\"get\" class=\"form-example\">";
  out += "<label for=\"slider\">Valeur du slider :</label><input type=\"range\" id=\"slider\" name=\"slider\" min=\"0\" max=\"100\" value=\"";
//  out += dSliderValue;
  out += "On verra ça plus tard";
  out += "\" step=\"10\">";
  out += "</form>";
  out += "</body></html>";

  // Envoi de la réponse HTML 
  monWebServeur.send(200, "text/html", out);
}

void handleContactTracer() {
  // Read positive list from positivelist.json
  DynamicJsonDocument positiveListDocument(512);
  if (SPIFFS.exists(strPositiveListFile)) {
    positiveListFile = SPIFFS.open(strPositiveListFile, "r");
    if (positiveListFile) {
      DeserializationError error = deserializeJson(positiveListDocument, positiveListFile);
      if (!error) {
        JsonArray positiveListArray = positiveListDocument["positive_list"].as<JsonArray>();
        String contactList[positiveListArray.size()];

        MYDEBUG_PRINTLN("-WEBSERVER : requete contact tracer");
        String out = "";
        int n = positiveListArray.size();
        MYDEBUG_PRINT("- Number of contacts: ");
        MYDEBUG_PRINTLN(n);

        out += "<html><head><meta http-equiv='refresh' content='5'/>";
        out += "<title>YNOV - Projet IoT B2</title>";
        out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
        out += "</head><body>";
        out += "<h1>Page des contacts</h1>";
        out += "<h2>Cette page liste les id de toutes les personnes que vous avez rencontrees et qui se sont declares positives.</h2><br>";

        // Integration des reseaux WiFi trouves dans la page HTML
        if (n == 0) {
          MYDEBUG_PRINTLN("- AUCUN Contact Trouvé");
        } else {
          out += "<ol>";
          int i = 0;
          for (JsonVariant id : positiveListArray) {
            contactList[i] = id.as<String>();
            out += "<li>"+ contactList[i] +"</li>";
            i++;
          }
          out += "</ol>";
        }

        // Fin de la réponse HTML
        out += "</body></html>";

        // Envoi de la page HTML
        MYDEBUG_PRINTLN("- Sending HTML response");
        monWebServeur.send(200, "text/html", out);

      } else {
        MYDEBUG_PRINTLN("-SPIFFS: Error parsing positivelist.json");
      }
      positiveListFile.close();
    } else {
      MYDEBUG_PRINTLN("-SPIFFS: Error opening positivelist.json");
    }
  } else {
    MYDEBUG_PRINTLN("-SPIFFS: positivelist.json does not exist");
  }
}


/**
 * En cas d'erreur de route, renvoi d'un message d'erreur 404
 */
void handleNotFound() {
  MYDEBUG_PRINTLN("-WEBSERVER : erreur de route");

  // Construction de la réponse HTML
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += monWebServeur.uri();
  message += "\nMethod: ";
  message += (monWebServeur.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += monWebServeur.args();
  message += "\n";
  for (uint8_t i = 0; i < monWebServeur.args(); i++) {
    message += " " + monWebServeur.argName(i) + ": " + monWebServeur.arg(i) + "\n";
  }

  // Envoi de la réponse HTML
  monWebServeur.send(404, "text/plain", message);
}

/**
 * Initialisation du serveur web
 */
void setupWebServer(){
  // On a besoin d'une connexion WiFi !
  // Test de la connexion WiFi. Si elle n'est pas active alors on démarre le WiFi
  if (WiFi.status() != WL_CONNECTED){setupWiFi();}  // Connexion WiFi
  MYDEBUG_PRINTLN("-WEBSERVER : Démarrage");

  // Configuration de mon serveur web en définissant plusieurs routes
  // A chaque route est associée une fonction
  monWebServeur.on("/", handleRoot);
  monWebServeur.on("/scan", handleScan);
  monWebServeur.on("/config", handleConfig);
  monWebServeur.on("/adafruit", handleAdafruit);
  monWebServeur.on("/contact_tracer", handleContactTracer);
  monWebServeur.onNotFound(handleNotFound);
  monWebServeur.on("/format", handleFormat);            // A ajouter quand le SPIFFFS est activé

  monWebServeur.begin();                                  // Démarrage du serveur
  MYDEBUG_PRINTLN("-WEBSERVER : Serveur Web démarré");
}

/**
 * Loop pour le serveur web afin qu'il regarde s'il a reçu des requêtes afin de les traiter
 */
void loopWebServer(void) {
  monWebServeur.handleClient();
}
