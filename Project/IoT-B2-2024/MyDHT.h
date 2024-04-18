/**
 * \file MyDHT.h
 * \page dht Capteur de température et d'humidité
 * \brief Capteur numérique Digital Humidity & Temperature (DHT)
 * 
 * Nous allons connecter un capteur numérique DHT11 sur la carte afin de récupérer ses données
 * de température et d'humidité.
 * \image html ESP32-DHT.webp
 * 
 * Branchez 
 * - VCC sur 3V3
 * - GND sur GND ;)
 * - Data sur la pin 33
 * 
 * \image html ESP32-ESP21PINTOUT.png
 * 
 * Adafruit Unified Sensor Driver https://github.com/adafruit/Adafruit_Sensor
 * Librairies à installer :
 * - Adafruit Unified Sensor by Adafruit
 * 
 * Délai entre 2 mesures : il dépend du capteur, si on n'a pas l'information (via la librairie unifiée) on le configure à 5s
 * 
 * Fichier \ref MyDHT.h
 */

#include <DHT_U.h>                     // https://github.com/adafruit/Adafruit_Sensor

#define DHT_PIN    33                  // PIN numérique du DHT
#define DHT_TYPE   DHT11               // Type de capteur DHT : DHT11 ou DHT22

DHT_Unified myDht(DHT_PIN, DHT_TYPE); // Instantiation du DHT Unified
uint32_t delayMS = 5000;              // Délai entre 2 mesures

// Fonction de configuration du capteur DHT
void setupDhtSensor(){
  myDht.begin();

  MYDEBUG_PRINTLN("DHT Unified Sensor");
  // Information sur le capteur de température
  sensor_t sensor;
  myDht.temperature().getSensor(&sensor);
  MYDEBUG_PRINTLN("------------------------------------");
  MYDEBUG_PRINTLN("Temperature");
  MYDEBUG_PRINT  ("Sensor:       "); MYDEBUG_PRINTLN(sensor.name);
  MYDEBUG_PRINT  ("Driver Ver:   "); MYDEBUG_PRINTLN(sensor.version);
  MYDEBUG_PRINT  ("Unique ID:    "); MYDEBUG_PRINTLN(sensor.sensor_id);
  MYDEBUG_PRINT  ("Max Value:    "); MYDEBUG_PRINT(sensor.max_value); MYDEBUG_PRINTLN(" *C");
  MYDEBUG_PRINT  ("Min Value:    "); MYDEBUG_PRINT(sensor.min_value); MYDEBUG_PRINTLN(" *C");
  MYDEBUG_PRINT  ("Resolution:   "); MYDEBUG_PRINT(sensor.resolution); MYDEBUG_PRINTLN(" *C");  
  MYDEBUG_PRINTLN("------------------------------------");
  // Information sur le capteur de température
  myDht.humidity().getSensor(&sensor);
  MYDEBUG_PRINTLN("------------------------------------");
  MYDEBUG_PRINTLN("Humidity");
  MYDEBUG_PRINT  ("Sensor:       "); MYDEBUG_PRINTLN(sensor.name);
  MYDEBUG_PRINT  ("Driver Ver:   "); MYDEBUG_PRINTLN(sensor.version);
  MYDEBUG_PRINT  ("Unique ID:    "); MYDEBUG_PRINTLN(sensor.sensor_id);
  MYDEBUG_PRINT  ("Max Value:    "); MYDEBUG_PRINT(sensor.max_value); MYDEBUG_PRINTLN("%");
  MYDEBUG_PRINT  ("Min Value:    "); MYDEBUG_PRINT(sensor.min_value); MYDEBUG_PRINTLN("%");
  MYDEBUG_PRINT  ("Resolution:   "); MYDEBUG_PRINT(sensor.resolution); MYDEBUG_PRINTLN("%");  
  MYDEBUG_PRINTLN("------------------------------------");
  delayMS = sensor.min_delay/1000;
  MYDEBUG_PRINT  ("Delay:   "); MYDEBUG_PRINT(delayMS); MYDEBUG_PRINTLN(" ms");  
  MYDEBUG_PRINTLN("------------------------------------");
}

// Fonction de récupération des données du capteur DHT
void getDhtData(){

  // Délai minimum entre 2 mesures
  MYDEBUG_PRINT("-DHT : Délai entre 2 mesure [");
  MYDEBUG_PRINT(delayMS);
  MYDEBUG_PRINTLN("] ms");
  delay(delayMS);

  // Get temperature event and print its value.
  sensors_event_t event;  
  myDht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    MYDEBUG_PRINTLN("-DHT : Erreur de lecture de la température du capteur DHT !");
  }
  else {
    MYDEBUG_PRINT("-DHT : [");
    MYDEBUG_PRINT(event.temperature);
    MYDEBUG_PRINTLN("°C] température !");
  }
  // Get humidity event and print its value.
  myDht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    MYDEBUG_PRINTLN("-DHT : Erreur de lecture de l'humidité du capteur DHT !");
  }
  else {
    MYDEBUG_PRINT("-DHT : [");
    MYDEBUG_PRINT(event.relative_humidity);
    MYDEBUG_PRINTLN("%] Humidité !");
  }
}
