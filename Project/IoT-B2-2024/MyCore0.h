/**
 * \file MyCore0.h
 * \page core0 Utilisation du Core 0
 * 
 * L'ESP32 dispose d'un microprocesseur dual core : Core 0 et Core 1.
 * \image html ESP32-DUALCORE.webp
 * 
 * Le code Arduino tourne par défaut sur le core 1. Nous allons voir comment affecter une
 * tâche pour qu'elle tourne sur le core 0 en parrallèle du core 1.
 * 
 * Dans cet exemple nous créer une tâche qui va gérer les requêtes reçues par le serveur
 * web et l'affecter au core 0, délestant ainsi que Core 1 de cette tâche, apportant
 * potentiellement plus de réactivité pour l'utilisateur.
 * 
 * Fichier \ref MyCore0.h
 */

TaskHandle_t Task1;

void loop2( void * parameter )
{
  for (;;) {
    loopWebServer();
    delay(20);
  }
}

/**
 * \brief Fonction de configuration du Core 0
 * 
 * Cette fonction permet de configurer le Core 0 pour y affecter une tâche.
 * 
 * \code{.cpp}
 * void setupMyCore0(){
 *   // Initialisation d'une tâche que nous allons mettre sur le Core 0
 *   xTaskCreatePinnedToCore(
 *     loop2,          // Nom de la fonction associée à la tâche
 *     "secondLoop",   // Nom de la tâche
 *     10000,           // Taille mémoire assignée à la tâche
 *     NULL,           // Mettre NULL dans tous les cas
 *     0,              // Priorité de la tâche : 0 étant la haute priorité. *IMPORTANT* Si priorité 1 et pas d'activité sur la tâche alors l'ESP est redémarré
 *     &Task1,         // Reference d'une variable taskHandle
 *     0);             // Choisir le core 0 ou 1
 * }
 * \endcode
 */
void setupMyCore0(){
  // Initialisation d'une tâche que nous allons mettre sur le Core 0
  xTaskCreatePinnedToCore(
    loop2,          // Nom de la fonction associée à la tâche
    "secondLoop",   // Nom de la tâche
    10000,           // Taille mémoire assignée à la tâche
    NULL,           // Mettre NULL dans tous les cas
    0,              // Priorité de la tâche : 0 étant la haute priorité. *IMPORTANT* Si priorité 1 et pas d'activité sur la tâche alors l'ESP est redémarré
    &Task1,         // Reference d'une variable taskHandle
    0);             // Choisir le core 0 ou 1
}
