/**
 * \file MyLED.h
 * \page led LED & PWM
 * 
 * Des composants ou capteurs peuvent être branchés sur les connecteurs de la carte.
 * Chaque composant a ses caractéristiques, nous prendrons le plus simple : une LED.
 * 
 * Brancher une LED :
 * - La patte la plus longue sur la broche 23 de l'ESP 32,
 * - La patte la plus courte sur une patte GND.
 * Nous piloterons l'ESP32 afin qu'il envoie du courant, ou pas, sur la patte 23.
 * 
 * Pour gérer l'éclairage de la LED, nous avons 2 solutions :
 * - "Numérique" en mode "On/Off" en mettant la patte à HIGH ou LOW.
 * - "Analogique" en utilisant le PWM (Pulse Width Modulation) qui permet de "simuler"
 *   un courant analogique. \image html ESP32-PWM.gif
 * 
 * Dans cet exemple :
 * - La patte de la LED est configurée en mode OUTPUT : on va écrire l'état sur cette patte.
 * - Lors du setup nous faisons clignoter la LED en faisant un alternance de 
 *   digitalWrite HIGH et LOW sur la patte de la LED
 * - Dans la boucle nous réalisons des boucles où nous incrémentons puis décrémentons le
 *   niveau de "puissance" que nous voulons sur la patte. Ainsi on a l'impression que
 *   l'intensité lumineuse augmente puis diminue.
 * 
 * Fichier \ref MyLED.h
 */

 #define LED_PIN    23                  // PIN + de la LED
int PWMRANGE = 256;

 void setupLED() {
  pinMode(LED_PIN, OUTPUT);     // Configuration de la LED_PIN en sortie
  digitalWrite(LED_PIN, LOW);   // Initialisation de l'état de la LED

  MYDEBUG_PRINTLN("- PWM : clignote");
  for (int i=0; i<10; i++){     // On fait clignoter la LED
    MYDEBUG_PRINT("- LED : [");
    MYDEBUG_PRINT(i);
    MYDEBUG_PRINTLN("]");
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);    
    delay(500);
  }
}

void playWithLED(){
  // On augmente la fréquence PWN de 0 au Maximum
  // Pour la valeur maximale on utilisera PWMRANGE
  MYDEBUG_PRINTLN("- PWM : crescendo");
  for(int i=0; i<PWMRANGE; i++){
    analogWrite(LED_PIN, i);
    delay(10);
  }
  // On diminue la fréquence PWN du maximum à 0
  MYDEBUG_PRINTLN("- PWM : decrescendo");
  for(int i=PWMRANGE; i>0; i--){
    analogWrite(LED_PIN, i);
    delay(10);
  }
}
