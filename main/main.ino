#include "KeyboardAzertyFr.h"
#include <SPI.h>
#include <SD.h>

File myFile;
const int chipSelect = 10;  // Utilisez le bon numéro de broche pour le CS de votre module SD
const int buttonPin = 9;  // Broche où le bouton est connecté
bool buttonState = 0;         // Variable pour lire l'état du bouton

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(LED_BUILTIN_RX, OUTPUT);  // RX LED comme sortie
  pinMode(LED_BUILTIN_TX, OUTPUT);  // TX LED comme sortie

  delay(50);
  KeyboardAzertyFr.begin();
  delay(500);

  
}

void typeKey(int key) {
  KeyboardAzertyFr.press(key);
  delay(50);
  KeyboardAzertyFr.release(key);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {  // Si le bouton est enfoncé (état bas car il est connecté au GND)
    if (!SD.begin(chipSelect)) {
    // Si la carte SD n'est pas détectée, clignotez la LED
    for(int i = 0; i < 3; i++) { // Clignote 5 fois
      digitalWrite(LED_BUILTIN_RX, HIGH);  // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH);  // Allumer la TX LED
      delay(200);
      digitalWrite(LED_BUILTIN_RX, LOW);   // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW);   // Éteindre la TX LED
      delay(200);
    }
    return;
  }



  myFile = SD.open("data.txt");

  if (myFile) {
    delay(600);
    while (myFile.available()) {
      char c = myFile.read();
      delay(20);
      KeyboardAzertyFr.print(c);

      digitalWrite(LED_BUILTIN_RX, HIGH);  // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH);  // Allumer la TX LED
      delay(20);
      digitalWrite(LED_BUILTIN_RX, LOW);   // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW);   // Éteindre la TX LED
 
    }
    myFile.close();
  } else {
    // Si le fichier n'est pas trouvé, vous pouvez également ajouter un code d'erreur LED ici si vous le souhaitez
    for(int i = 0; i < 6; i++) { // Clignote 5 fois
      digitalWrite(LED_BUILTIN_RX, HIGH);  // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH);  // Allumer la TX LED
      delay(200);
      digitalWrite(LED_BUILTIN_RX, LOW);   // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW);   // Éteindre la TX LED
      delay(200);
    }
  }

  typeKey(KEY_RETURN);
    delay(1000); // Petite pause pour éviter des répétitions accidentelles
  }
}
