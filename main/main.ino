#include "KeyboardAzertyFr.h"
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
#define I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int chipSelect = 10;  // Utilisez le bon numéro de broche pour le CS de votre module SD

const int buttonPin = 9;   // Broche où le bouton est connecté
bool buttonState = 0;      // Variable pour lire l'état du bouton
const int buttonPin2 = 8;  // Broche où le bouton est connecté
bool buttonState2 = 0;     // Variable pour lire l'état du bouton
int codeInput = 0000;
int unite = 1;



void typeKey(int key) {
  KeyboardAzertyFr.press(key);
  delay(50);
  KeyboardAzertyFr.release(key);
}

void writePass() {
  File myFile;
  if (!SD.begin(chipSelect)) {
    // Si la carte SD n'est pas détectée, clignotez la LED
    for (int i = 0; i < 3; i++) {          // Clignote 5 fois
      digitalWrite(LED_BUILTIN_RX, HIGH);  // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH);  // Allumer la TX LED
      delay(200);
      digitalWrite(LED_BUILTIN_RX, LOW);  // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW);  // Éteindre la TX LED
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
      digitalWrite(LED_BUILTIN_RX, LOW);  // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW);  // Éteindre la TX LED
    }
    myFile.close();
  } else {
    // Si le fichier n'est pas trouvé, vous pouvez également ajouter un code d'erreur LED ici si vous le souhaitez
    for (int i = 0; i < 6; i++) {          // Clignote 5 fois
      digitalWrite(LED_BUILTIN_RX, HIGH);  // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH);  // Allumer la TX LED
      delay(200);
      digitalWrite(LED_BUILTIN_RX, LOW);  // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW);  // Éteindre la TX LED
      delay(200);
    }
  }

  typeKey(KEY_RETURN);
  delay(1000);  // Petite pause pour éviter des répétitions accidentelles
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);   // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(buttonPin2, INPUT_PULLUP);  // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(LED_BUILTIN_RX, OUTPUT);    // RX LED comme sortie
  pinMode(LED_BUILTIN_TX, OUTPUT);    // TX LED comme sortie

  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(4);      // Taille du texte
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(20,20);   // Position du curseur
  display.print("0000");      // Votre nombre à 4 chiffres
  display.display();

  delay(50);
  KeyboardAzertyFr.begin();
  delay(500);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);

  if (buttonState == LOW) {  // Si le bouton est enfoncé (état bas car il est connecté au GND)
    writePass();
    
    delay(500);
  }

  if (buttonState2 == LOW) {
    codeInput = codeInput + unite;
    if(codeInput > 999){
      unite = unite - 999;
    }
    display.clearDisplay();
    display.setTextSize(4);      // Taille du texte
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(20,20);   // Position du curseur
    display.print(codeInput);      // Votre nombre à 4 chiffres
    display.display();
    delay(500);
  }
}

