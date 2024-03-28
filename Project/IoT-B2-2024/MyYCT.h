#include <BLEDevice.h>  //
#include <BLEUtils.h>   // BLE
#include <BLEServer.h>  //
#include <ArduinoOTA.h> // OTA
#include <RemoteDebug.h>  // Remote Debug
#include <WiFi.h>         // WiFi
#include "esp_wpa2.h"     //
#include "Adafruit_MQTT.h"         // Adafruit
#include "Adafruit_MQTT_Client.h"  //
#include "SPIFFS.h"      // SPIFFS
#include <ArduinoJson.h> //

// Variables to use :
// set up your variables (for exemple the parameters from Adafruit or the contact list)


// NOTE : if we use setup and loops we must write again all the programs of bluetooth, wifi ...
// And deactivate the base setups and loops
// There is still the possiblité to call these programs in their respective loops (BLE, WIFI, Adafruit)

// ------------------------------------------------------------------------------------------------
// Setup of our application
// ------------------------------------------------------------------------------------------------
void setupYCT(){
  // TODO : simple setup
}

// ------------------------------------------------------------------------------------------------
// Loop of our application
// ------------------------------------------------------------------------------------------------
void loopYCT(){
  // TODO : simple loop 
  // (for example, you can get nearby contacts, update your contacts and update your state)
  // All of the programs could also be called by one program (getNearbyContact) it is as you see fit ...
}

// ------------------------------------------------------------------------------------------------
// Main Functions for our application
// ------------------------------------------------------------------------------------------------


void getNearbyContacts(){
  // TODO : Implementing the function that gets the informations of nearby cards (with the right format)
  // This function should use the bluetooth and the Adafruit parameters in order to save the cards
  // WARNING : this function should be in two parts, saving the cards and adding contact to follow
  // if we were next to a cards during a fixed duration (parameters in Adafruit)
}

void updateState(){
  // TODO : Implementing the function that Updates your state from Adafruit and your contacts
  // if one of your contact to follow is sick, you should be changed to contact case
}

void updateContacts(){
  // TODO : Implementing the function that Updates your contacts
  // the goal of this function is to update the informations of yours contacts to follow
}

// There should be a admin part, where you could setup OTA and Remote Debug,
// at least to ease your tests and the debug part ...
void remoteDebug(){}
