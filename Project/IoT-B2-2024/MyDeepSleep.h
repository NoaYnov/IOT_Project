/**
 * \file MyDeepSleep.h
 * \page deepsleep Deep Sleep
 * \brief Optimisation de la consommation électrique
 * 
 * Nous allons chercher à économiser l'énergie et ainsi prolonger la durée de vie de la batterie d'un objet connecté.
 * 
 * L'ESP32 dispose d'un mode "Deep Sleep" qui permet de l'endormir pour une durée donnée.
 * 
 * Le fait est que quand l'ESP32 se réveille, il a perdu toutes ses données.
 * Vous pouvez utiliser le prefixe RTC_DATA_ATTR devant la déclaration d'une variable pour qu'elle soit stockée
 * en mémoire et récupérée au redémarrage.
 * 
 * Fichier \ref MyDeepSleep.h
 */

#define uS_TO_S_FACTOR 1000000  /* Facteur de conversion de micro secondes en secondes */
#define TIME_TO_SLEEP  3        /* Durée d'endormissement en secondes */

RTC_DATA_ATTR int bootCount = 0;

/**
 * \brief Fonction de mise en veille profonde
 * 
 * Cette fonction permet de mettre en veille profonde l'ESP32 pour une durée de 3 secondes.
 * 
 * \code{.cpp}
 * void myDeepSleep(){
 *   MYDEBUG_PRINT("-DEEPSLEEP : ");
 *   MYDEBUG_PRINTLN(bootCount++);
 *   MYDEBUG_PRINTLN("-DEEPSLEEP : DeepSleep pendant 3 secondes");
 *   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
 *   esp_deep_sleep_start();
 * }
 * \endcode
 */
void myDeepSleep(){
  MYDEBUG_PRINT("-DEEPSLEEP : ");
  MYDEBUG_PRINTLN(bootCount++);
  MYDEBUG_PRINTLN("-DEEPSLEEP : DeepSleep pendant 3 secondes");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
