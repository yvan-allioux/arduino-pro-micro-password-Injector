#include "KeyboardAzertyFr.h"
#include <SPI.h>
#include <SD.h>

#include <ssd1306.h>

const uint8_t chipSelect = 10;  // Utilisez le bon numéro de broche pour le CS de votre module SD

const uint8_t buttonPin = 9;   // Broche où le bouton est connecté
bool buttonState = 0;      // Variable pour lire l'état du bouton
const uint8_t buttonPin2 = 8;  // Broche où le bouton est connecté
bool buttonState2 = 0;     // Variable pour lire l'état du bouton

uint8_t currentDigit = 0;              // Chiffre actuellement en cours de modification (0 à 3)
uint8_t codeInput[] = { 0, 0, 0, 0 };  // Code à 4 chiffres


void typeKey(uint8_t key) {
  KeyboardAzertyFr.press(key);
  delay(50);
  KeyboardAzertyFr.release(key);
}

void clignote(uint8_t nbDeFlash, uint8_t delayMs,uint8_t secondDelayMs) {
  for (uint8_t i = 0; i < nbDeFlash; i++) {  // Clignote 5 fois
    digitalWrite(LED_BUILTIN_RX, LOW);  // Éteindre la RX LED
    digitalWrite(LED_BUILTIN_TX, LOW);  // Éteindre la TX LED
    delay(delayMs);
    digitalWrite(LED_BUILTIN_RX, HIGH);  // Allumer la RX LED
    digitalWrite(LED_BUILTIN_TX, HIGH);  // Allumer la TX LED
    delay(secondDelayMs);
  }
}

void displayUpdate() {
  ssd1306_fillScreen(0x00);
  
  char buf[5]; 
  sprintf(buf, "%d%d%d%d", codeInput[0], codeInput[1], codeInput[2], codeInput[3]);
  ssd1306_setFixedFont(comic_sans_font24x32_123);
  ssd1306_printFixed(15, 24, buf, STYLE_NORMAL);
  //TODO Dessine un rectangle comme marqueur
}


void writePass() {
  File myFile;
  if (!SD.begin(chipSelect)) {
    clignote(5, 2000, 2000);
    return;
  }

  myFile = SD.open("data.txt");
  if (myFile) {
    String unencrypted = "";
    delay(600);
    while (myFile.available()) {
      char c = myFile.read();
      unencrypted += c;
      //KeyboardAzertyFr.print(c);
      delay(5);
      
    }
    xorWithPin(unencrypted);
    //KeyboardAzertyFr.print(unencrypted);
    myFile.close();

  } else {
    // Si le fichier n'est pas trouvé, vous pouvez également ajouter un code d'erreur LED ici si vous le souhaitez
    clignote(10, 2000, 2000);
  }

  //typeKey(KEY_RETURN);
  delay(1000);  // Petite pause pour éviter des répétitions accidentelles
}


void xorWithPin(String password) {
    uint16_t pin = (codeInput[0] * 1000) + (codeInput[1] * 100) + (codeInput[2] * 10) + codeInput[3];
    String validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    for (uint8_t i = 0; i < password.length(); i++) {
        char c = password[i];
        int index = validChars.indexOf(c);

        if (index == -1) {
            clignote(15, 2000, 2000);
            return;  // Note: La fonction était censée retourner void, donc pas de "Erreur 10" ici.
        }

        index = index ^ (pin % 256);  // XOR avec la partie basse du code PIN
        pin = (pin >> 8) | (pin << 8);  // Rotation des bits pour mélanger le code PIN

        index = index % validChars.length();  // Gardez l'index dans la plage valide
        KeyboardAzertyFr.print(validChars[index]);
        clignote(1, 20, 20);
    }
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);   // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(buttonPin2, INPUT_PULLUP);  // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(LED_BUILTIN_RX, OUTPUT);    // RX LED comme sortie
  pinMode(LED_BUILTIN_TX, OUTPUT);    // TX LED comme sortie

  ssd1306_128x64_i2c_init();
  displayUpdate();

  delay(50);
  KeyboardAzertyFr.begin();
  delay(500);
}


void loop() {
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);

  if (buttonState == LOW) {
    clignote(1, 20, 0);
    codeInput[currentDigit] = (codeInput[currentDigit] + 1) % 10;
    displayUpdate();
    delay(200);
  }

  if (buttonState2 == LOW) {
    clignote(1, 20, 0);
    currentDigit = (currentDigit + 1) % 4;
    // si le curseur est sur le dernier chiffre, on valide le code et on retourne à 0
    if (currentDigit == 0) {
      writePass();
      currentDigit = 0;
    }
    displayUpdate();
    delay(200);
  }
  delay(10);
}