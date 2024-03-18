/**
 * \file MyTicker.h
 * \page tickers Tickers
 * \brief Repassez dans 5 minutes s'il vous plaît
 * 
 * Un Ticker est un objet qui est appelé à une fréquence définie.
 * Chaque Ticker est associé à une fonction qu'il va appeler quand il est l'heure.
 * Important : un ticker n'interrompt pas l'exécution du code, il se "glisse" au milieu
 * et est exécuté dès que le processeur a le temps de le faire !
 * 
 * Vous pouvez définir autant de Ticker que vous le souhaitez, dans la limite de la mémoire.
 * 
 * Vous pouvez attacher et détacher une fonction à un Ticker, ou changer de fonction ou changer
 * la fréquence à souhait, par exemple quand un paramètre est modifié.
 * 
 * Il y a 2 façons d'attacher une fonction avec attach(secondes) et attach_ms(milliseconds)
 * 
 * A noter que comme on ne connaît pas l'heure (et que ce n'est pas forcément nécessaire dans
 * ce cas), o
 * 
 * Fichier \ref MyTicker.h
 */
 
#include <Ticker.h>

Ticker myTicker;

int count = 0;
unsigned long myTickerTime;

void tickerFunction() {
  MYDEBUG_PRINT("-TICKER [");
  MYDEBUG_PRINT(count);
  MYDEBUG_PRINT("] Depuis la dernière fois :");
  MYDEBUG_PRINT(micros()-myTickerTime);
  MYDEBUG_PRINTLN(" us (micro secondes)");
  myTickerTime = micros();
  count++;
}

void setupTicker(){
  MYDEBUG_PRINTLN("-TICKER : Initialisation d'un ticker toutes les 30 secondes");
  myTicker.attach(30, tickerFunction);         // Association du Ticker avec une fonction appelée toutes les 30 secondes
  myTickerTime = micros();                     // J'enregistre l'heure en nombre de us
}
