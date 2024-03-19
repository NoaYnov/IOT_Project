/**
 * \file MyAdafruitIO.h
 * \page adafruitio Adafruit IO
 * \brief Adafruit IO Platform
 * 
 * <H2>Adafruit IO</H2>
 * Nous allons utiliser la plateforme Adafruit IO en guise de première approche de MQTT.
 * Pour ce faire, installez la bibliothèque "Adafruit IO Arduino" ainsi que toutes ses dépendances.
 * - Rendez vous sur la plateforme : https://io.adafruit.com/
 * - Enregistrez vous : https://accounts.adafruit.com/users/sign_up
 * - Une fois connecté, récupérez votre Aadafruit Key en cliquant sur "My Key"
 * - Copier vos identifiants pour Arduino (IO_USERNAME et IO_KEY) et coller dans la section
 *   configuration ci-dessous
 * - Dans la page des Feeds (topics MQTT), créer les Feeds suivants, sans les rattacher à un groupe : 
 *   - slider 
 *   - onoff
 *   - temperature
 *   - humidity
 *   
 *   Adafruit a défini la notion de Feeds qui est utilisé indifféremment pour des attributs et
 *   des données de télémétrie MQTT.
 * - Dans la page des Dashboards, créer un dashboard et éditer le. Dans celui-ci, ajouter les "blocks" suivants:
 *   - Un Slider associé au feed slider
 *   - Un Toggle associé au feed onoff, en laissant les valeurs par défaut ("ON" et "OFF")
 *   - Un Indicator associé au feed onoff, en indiquant la condition "=ON"
 *   - un Line Chart associé aux feeds temperature et humidity
 * - Tester le code en le téléversant
 * - Installer l'application IFTTT sur votre téléphone mobile
 *   - Tester différentes actions possibles avec Adafruit IO
 *     - If this : Buton widget / Button press
 *     - Then that : Adafruit / Send data to Adafruit IO. Sélectionner votre feed et renseigner
 *       la valeur que vous voulez transmettre (ON par exemple)
 *       A noter que vous pouvez ajouter des "ingrédients" qui sont d'autres données qui sont à
 *       votre disposition (du téléphone ou depuis le service appelé en "If this".
 * 
 * Documentation sur l'API MQTT : https://learn.adafruit.com/adafruit-io/mqtt-api
 * Voici un exemple de clients qui utilisent l'API MQTT : https://learn.adafruit.com/desktop-mqtt-client-for-adafruit-io/overview
 * Vous pouvez aussi utiliser la REST API pour intéragir avec la plateforme Adafruit IO.
 * 
 * <H2>Bibliothèque à installer</H2>
 * Pour utiliser l'API MQTT d'Adafruit, installer :
 * - Adafruit MQTT library by Adafruit : https://github.com/adafruit/Adafruit_MQTT_Library
 * 
 * Fichier \ref MyAdafruitIO.h
 */

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* Configuration *************************************/
// Connexion Adafruit
#define IO_SERVER         "io.adafruit.com"
#define IO_SERVERPORT     1883
#define IO_USERNAME       "user123841294"
#define IO_KEY            "aio_Myzr43AsYOGv7TjiYhUh9gZxj43E"
// Feeds
#define FEED_SLIDER       "/feeds/slider"
#define FEED_ONOFF        "/feeds/onoff"
#define FEED_TEMPERATURE  "/feeds/temperature"
#define FEED_HUMIDITY     "/feeds/humidity"
#define FEED_ES_MOI       "/feeds/maxime.etat-de-sante"
#define FEED_ES_CONTACT   "/feeds/francois.etat-de-sante"
// Frequence d'envoi des données
#define FEED_FREQ         10

/************************** Variables ****************************************/
// Instanciation du client WiFi qui servira à se connecter au broker Adafruit
WiFiClient client;
// Instanciation du client Adafruit avec les informations de connexion
Adafruit_MQTT_Client MyAdafruitMqtt(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_USERNAME, IO_KEY);
// Variable de stockage de la valeur du slider
uint32_t uiSliderValue=0;
Ticker MyAdafruitTicker;

/****************************** Feeds ****************************************/
// Création des Feed auxquels nous allons souscrire :
// Un FEED 'time' pour récupérer l'heure
Adafruit_MQTT_Subscribe timefeed = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, "time/seconds");
// Un FEED 'slider' pour récupérer la valeur d'un slider présent sur le dashboard
Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, IO_USERNAME FEED_SLIDER, MQTT_QOS_1);
// Un FEED 'onoff' pour récupérer l'état d'un interrupteur présent sur le dashboard
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, IO_USERNAME FEED_ONOFF, MQTT_QOS_1);
// Un FEED 'temperature' et 'humidity' pour publier des données de télémétrie
Adafruit_MQTT_Publish temperatureFeed = Adafruit_MQTT_Publish(&MyAdafruitMqtt, IO_USERNAME FEED_TEMPERATURE);
Adafruit_MQTT_Publish humidityFeed = Adafruit_MQTT_Publish(&MyAdafruitMqtt, IO_USERNAME FEED_HUMIDITY);
Adafruit_MQTT_Subscribe subEsMaxime = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, IO_USERNAME FEED_ES_MOI, MQTT_QOS_1);
Adafruit_MQTT_Subscribe subEsFrancois = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, IO_USERNAME FEED_ES_CONTACT, MQTT_QOS_1);
Adafruit_MQTT_Publish pubEsMaxime = Adafruit_MQTT_Publish(&MyAdafruitMqtt, IO_USERNAME FEED_ES_MOI);
/*************************** Sketch Code ************************************/

/**
 * Callback associée au Slider présent sur le dashboard
 */
void slidercallback(double uiSliderValue) {
  MYDEBUG_PRINT("-AdafruitIO : Callback du feed slider avec la valeur ");
  MYDEBUG_PRINTLN(uiSliderValue);
}

/**
 * Callback associée à l'interrupteur sur le dashboard
 */
void onoffcallback(char *data, uint16_t len) {
  MYDEBUG_PRINT("-AdafruitIO : Callback du feed onoff avec la valeur ");
  MYDEBUG_PRINTLN(data);
  if (!strcmp(data, "ON")){
    MYDEBUG_PRINTLN("-AdafruitIO : J'allume");
  }
  else { 
    MYDEBUG_PRINTLN("-AdafruitIO : J'éteins");
  }
}

/**
 * Récupération et envoi des données de télémétrie
 */
void getAndSendDataToAdafruit(){
  // Récupération des données
  float myRandomTemp = 20+(float)random(-50,50)/10;
  float myRandomHum = 50+(float)random(-100,100)/10;

  MYDEBUG_PRINTLN("-AdafruitIO TICKER : Envoi des données");

  // Envoi des données au Broker
  temperatureFeed.publish(myRandomTemp);
  humidityFeed.publish(myRandomHum);
}

void EsMaximeCallback(char *data, uint16_t len) {
  MYDEBUG_PRINT("-AdafruitIO : Callback sur l'état de santé de Maxime : ");
  MYDEBUG_PRINTLN(data);
  if (!strcmp(data, "MALADE")){
    MYDEBUG_PRINTLN("-AdafruitIO : Je suis malade");
    logTracking("MQTT \t Je suis malade");
  } else if (!strcmp(data, "CAS CONTACT")){
    MYDEBUG_PRINTLN("-AdafruitIO : Je suis cas contact");    
    logTracking("MQTT \t Je suis cas contact");
  } else { 
    MYDEBUG_PRINTLN("-AdafruitIO : Je suis en pleine forme");
    logTracking("MQTT \t Je suis en pleine forme");
  }
}

void EsFrancoisCallback(char *data, uint16_t len) {
  MYDEBUG_PRINT("-AdafruitIO : Callback sur l'état de santé de François :");
  MYDEBUG_PRINTLN(data);
  if (!strcmp(data, "MALADE")){
    MYDEBUG_PRINTLN("-AdafruitIO : François est malade");
    MYDEBUG_PRINTLN("-AdafruitIO : Maxime est donc un cas contact");
    logTracking("MQTT \t Mon contact est malade");
    pubEsMaxime.publish("CAS CONTACT");
  } else if (!strcmp(data, "CAS CONTACT")){
    MYDEBUG_PRINTLN("-AdafruitIO : François est cas contact"); 
    MYDEBUG_PRINTLN("-AdafruitIO : CA NE CHANGE RIEN POUR MOI");
    logTracking("MQTT \t Mon contact est cas contact");
  } else { 
    MYDEBUG_PRINTLN("-AdafruitIO : François est en pleine forme");
    logTracking("MQTT \t Mon contact est en pleine forme");
  }
}


/**
 * Configuration de la connexion au borker Adafruit IO
 * - Connexion WiFi
 * - Configuration de l'actuateur
 * - Configuration des callbacks
 */
void setupAdafruitIO() {

  if (WiFi.status() != WL_CONNECTED){setupWiFi();}     // Vérification de la connexion WiFi

  // Configuration des callbacks pour les FEEDs auxquels on veut souscrire
  //timefeed.setCallback(timecallback);
  slider.setCallback(slidercallback);
  onoffbutton.setCallback(onoffcallback);
  
  // Souscription aux FEEDs
  MyAdafruitMqtt.subscribe(&timefeed);
  MyAdafruitMqtt.subscribe(&slider);
  MyAdafruitMqtt.subscribe(&onoffbutton);

  subEsMaxime.setCallback(EsMaximeCallback);
  subEsFrancois.setCallback(EsFrancoisCallback);
  MyAdafruitMqtt.subscribe(&subEsMaxime);
  MyAdafruitMqtt.subscribe(&subEsFrancois);

  MyAdafruitTicker.attach(FEED_FREQ, getAndSendDataToAdafruit);
}

/**
 * Connexion au broker Adafruit IO
 */
void connectAdafruitIO() {
  if (MyAdafruitMqtt.connected()) { return; }                      // Si déjà connecté, alors c'est tout bon

  MYDEBUG_PRINT("-AdafruitIO : Connexion au broker ... ");
  int8_t ret;
  while ((ret = MyAdafruitMqtt.connect()) != 0) {                  // Retourne 0 si déjà connecté
     MYDEBUG_PRINT("[ERREUR : ");
     MYDEBUG_PRINT(MyAdafruitMqtt.connectErrorString(ret));
     MYDEBUG_PRINT("] nouvelle tentative dans 10 secondes ...");
     MyAdafruitMqtt.disconnect();                                  // Deconnexion pour être propre
     delay(10000);                                                 // On attend 10 secondes avant de retenter le coup
  }
  MYDEBUG_PRINTLN("[OK]");
}

/**
 * Boucle Adafruit IO
 * - Vérification de l'état de la connexion
 * - Traitement des messages reçus
 * - Maintien de la connexion en vie avec un Ping si on aucun publish télémétrie n'est fait
 */
void loopAdafruitIO() {
  connectAdafruitIO();
  MyAdafruitMqtt.processPackets(10000);
  if(! MyAdafruitMqtt.ping()) {
    MyAdafruitMqtt.disconnect();
  }
}
