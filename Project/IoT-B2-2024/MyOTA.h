/**
 * \file MyOTA.h
 * \page ota Over The Air & Remote Debug
 * \brief Mise à jour du firmware et accès aux logs, sans port série
 * 
 * Jusqu'ici notre objet connecté est resté relité à notre ordinateur via un câble USB pour
 * le téléversement du code et l'accès aux messages, le tout via le port série. Très bien, mais
 * un objet connecté a vocation à être débranché du câble USB qui le relie à un ordinateur et
 * vivre sa vie à distance et en tout autonomie.
 * \image html ESP32-OTA.png width=600px
 * 
 * \subsection updateOTA Mise à jour du firmware
 * Toutefois, une déconnecté de l'ordinateur, avec une alimentation électrique et le réseau WiFi,
 * comme réaliser des mises à jour du firmware sans port série ? Avec la bibliothèque <b>Over 
 * The Air</b> ! Cette dernière permet d'ajouter des ports réseau en plus des ports série
 * et de téléverser via l'IDE Arduino de la même façon.
 * 
 * Lorsque vous téléverser votre nouveau code à distance, la carte enregistre votre nouveau code
 * dans sa mémoire alors que le code actuel est toujours présent et actif. Une fois le téléchargement
 * terminé alors la carte redémarre, installe et éxecute votre nouveau code. Cela nécessite donc 
 * d'avoir dimensionné votre espace mémoire de façon à pouvoir stocker le nouveau code.
 * \image html ESP32-OTA-Sketch.png width=600px
 * 
 * \note Avant de téléverser ce programme pour la première fois via la liaison série, vérifier 
 * dans le menu "Outils" que la "Flash size" sélectionnée correspond bien à votre matériel. 
 * Classiquement pour un ESP32, il faut renseigner 4M (1M SPIFFS); on maximise
 * ainsi l'espace disponible pour OTA, cf. \ref spiffs . 
 * Au regard de la taille de votre programme et de vos données dans le SPIFFS, vous pouvez 
 * également sélectionner le mode Minimal SPIFFS (1.9 MB App with OTA/190 KB SPIFFS).
 * 
 * Dès que votre firmware intégrant la fonctionnalité Over The Air est installé sur votre ESP32,
 * vous disposez d'un nouveau "Port" qui n'est pas un port série mais un port réseau que vous
 * pouvez utiliser pour téléverser le firmware. A partir de ce moment là, vous pouvez donc
 * débrancher le cable USB (port série) et donc continuer à pouvoir téléverser votre firmware.
 * \note L'ordinateur (avec son environnement de développement) doit avoir accès à votre objet
 * connecté au travers du réseau. Par là, ils doivent être tous les 2 présents sur le même
 * sous-réseau. !
 * 
 * Pour éviter que toute personne ayant accès au réseau WiFi sur lequel est branché votre
 * objet puisse téléverser un nouveau firmware, il est préférable d'ajouter un peu de sécurité 
 * en demandant un mot de passe lors des mises à jour.
 * 
 * \subsection remote Debug à distance
 * S'affranchir de la liaison série est appréciable, toutefois on perd en même temps une
 * fonctionnalité très utile : l'affichage des messages de debug !
 * Une solution pour palier à ce problème est l'utilisation de la bibliothèque RemoteDebug qui 
 * permet de se connecter en telnet à notre carte et de récupérer les messages par ce biais.
 * 
 * \note \b Tous les messages de debug qui sont présents dans votre code doivent être modifiés
 * pour appeler les fonctions de génération de debug de la bibliothèque RemoteDebug.
 * 
 * Il y a possibilités de créer des messages avec différents niveaux de logs, les voici par niveau 
 * de détail :
 * - Verbose
 * - Debug
 * - Info
 * - Warnings
 * - Errors
 * Quand vous sélectionner un niveau de détail, tous les messages de niveau "inférieur" sont
 * également visibles.
 * 
 * Une fois ce firmware téléversé:
 * - Dans le menu "Outils/Port", vérifier que le nouveau port <OTA_HOSTNAME> at <IP ADDRESS> 
 *   apparaît dans la liste des "Ports réseau".
 * - Si tel est le cas, vous pouvez débrancher votre objet du port série de votre ordinateur
 *   et le brancher à une autre source d'alimentation.
 * - Il n'apparait donc plus dans la liste de vos "Ports série" mais est toujours présent dans
 *   les "Ports réseau".
 * - Vous pouvez désormais téléversez vos nouveaux firmwares \b Over \b The \b Air.
 * - Pour accéder aux logs, 2 possibilités :
 *   - Ouvrez un terminal et saisissez "telnet <adresse IP>" pour y voir
 *     apparaître vos messages de logs, ou utilisez un logiciel du type Putty : 
 *     https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
 *   - Remote Debug embarque une bibliothèque de web socket qui permet d'accéder aux logs.
 *     Télécharger la Web App (le zip décompressé de https://github.com/JoaoLopesF/RemoteDebugApp)
 *     sur votre ordinateur et lancer index.html pour pouvoir vous connecter à votre carte 
 *     à l'aide d'une web socket et récupérer les logs.
 * 
 * \subsection otalib Librairies nécessaires
 * Bibliothèques à installer pour utiliser ce module :
 * - Arduino OTA by Arduino Juraj Andrassy : https://github.com/jandrassy/ArduinoOTA
 * - RemoteDebug by Joao Lopez : https://github.com/JoaoLopesF/RemoteDebug
 * 
 * \note Vous devriez rencontrer une erreur de compilation du type
 * \verbatim .../libraries/RemoteDebug/src/utility/WebSockets.cpp:42:10: fatal error: hwcrypto/sha.h: No such file or directory\endverbatim
 * Si c'est le cas, pour corriger le problème, éditez le fichier en question et remplacer 
 * hwcrypto/sha.h par sha/sha_parallel_engine.h !
 * 
 * Fichier \ref MyOTA.h
 */

#include <ArduinoOTA.h>
#include <RemoteDebug.h>

#define OTA_HOSTNAME  "ESP32_Valentin"
#define OTA_PASSWORD  "1234567890"

RemoteDebug Debug;
Ticker debugTicker;

/**
 * Fonction appelée par un ticker pour générer des messages de debug à différents niveaux
 */
void generateDebugLog(){
  rdebugVln("-Remote DEBUG : Message VERBOSE");
  rdebugDln("-Remote DEBUG : Message DEBUG");
  rdebugIln("-Remote DEBUG : Message INFO");
  rdebugWln("-Remote DEBUG : Message WARNING");
  rdebugEln("-Remote DEBUG : Message ERROR");
}

/**
 * Configuration et démarrage des services OTA & Remote Debug
 */
void setupOTA(){
  // On a besoin d'une connexion WiFi !
  if (WiFi.status() != WL_CONNECTED){setupWiFi();}  // Connexion WiFi

  // Démarrage d'OTA
  MYDEBUG_PRINTLN("-OTA : Démarrage");
  ArduinoOTA.setHostname(OTA_HOSTNAME); // Nommage pour l'identification
  ArduinoOTA.setPassword(OTA_PASSWORD); // Mot de passe pour les téléversements
  ArduinoOTA.begin();                   // Initialisation de l'OTA

  // Initialisation de la librairie RemoteDebug
  Debug.begin(OTA_HOSTNAME); 
  Debug.setPassword(OTA_PASSWORD);      // Mot de passe pour les connexions telnet et Web app
  Debug.setResetCmdEnabled(true);       // Pour permettre le Reset par telnet
  Debug.showColors(true);               // Un peu de couleurs pour faire joli
  //Debug.setSerialEnabled(true);       // Pour activer un écho des logs sur le port série (si branché)

  // Ticker pour générer des logs
  debugTicker.attach(2, generateDebugLog);
}

/**
 * A chaque itération, on verifie si une mise a jour nous est envoyée.
 * Si tel est cas, la bibliothèque ArduinoOTA se charge de tout !
 */
void loopOTA(){
  ArduinoOTA.handle();          // Gestion des demandes de téléversement
  debugTicker.attach(2, generateDebugLog);
  Debug.handle();               // Gestion des messages de remote debug
}
