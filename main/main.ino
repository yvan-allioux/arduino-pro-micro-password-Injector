#include "KeyboardAzertyFr.h"
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

File myFile;
const int chipSelect = 10; // Utilisez le bon numéro de broche pour le CS de votre module SD

const int buttonPin = 9;  // Broche où le bouton est connecté
bool buttonState = 0;     // Variable pour lire l'état du bouton
const int buttonPin2 = 8; // Broche où le bouton est connecté
bool buttonState2 = 0;    // Variable pour lire l'état du bouton

int currentDigit = 0;           // Chiffre actuellement en cours de modification (0 à 3)
int codeInput[] = {0, 0, 0, 0}; // Code à 4 chiffres
int codeInputInt = 0;           // Code à 4 chiffres

String validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);  // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(buttonPin2, INPUT_PULLUP); // Configure la broche du bouton comme entrée avec résistance de rappel
  pinMode(LED_BUILTIN_RX, OUTPUT);   // RX LED comme sortie
  pinMode(LED_BUILTIN_TX, OUTPUT);   // TX LED comme sortie

  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(4); // Taille du texte
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20); // Position du curseur
  display.print("0000");     // Votre nombre à 4 chiffres
  display.display();

  delay(50);
  KeyboardAzertyFr.begin();
  delay(500);
}

void typeKey(int key)
{
  KeyboardAzertyFr.press(key);
  delay(50);
  KeyboardAzertyFr.release(key);
}

void writePass(int pin)
{
  if (!SD.begin(chipSelect))
  {
    // Si la carte SD n'est pas détectée, clignotez la LED
    for (int i = 0; i < 3; i++)
    {                                     // Clignote 5 fois
      digitalWrite(LED_BUILTIN_RX, HIGH); // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH); // Allumer la TX LED
      delay(200);
      digitalWrite(LED_BUILTIN_RX, LOW); // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW); // Éteindre la TX LED
      delay(200);
    }
    return;
  }

  myFile = SD.open("data.txt");

  if (myFile)
  {
    //xorWithPin
    String unencrypted = "";
    
    delay(600);
    while (myFile.available())
    {
      char c = myFile.read();
      delay(20);
      unencrypted += c;
      //KeyboardAzertyFr.print(c);

      digitalWrite(LED_BUILTIN_RX, HIGH); // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH); // Allumer la TX LED
      delay(20);
      digitalWrite(LED_BUILTIN_RX, LOW); // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW); // Éteindre la TX LED
    }
    myFile.close();
    KeyboardAzertyFr.print(xorWithPin(pin, unencrypted));
  }
  else
  {
    // Si le fichier n'est pas trouvé, vous pouvez également ajouter un code d'erreur LED ici si vous le souhaitez
    for (int i = 0; i < 6; i++)
    {                                     // Clignote 5 fois
      digitalWrite(LED_BUILTIN_RX, HIGH); // Allumer la RX LED
      digitalWrite(LED_BUILTIN_TX, HIGH); // Allumer la TX LED
      delay(200);
      digitalWrite(LED_BUILTIN_RX, LOW); // Éteindre la RX LED
      digitalWrite(LED_BUILTIN_TX, LOW); // Éteindre la TX LED
      delay(200);
    }
  }

  typeKey(KEY_RETURN);
  delay(1000); // Petite pause pour éviter des répétitions accidentelles
}

void displayUpdate()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 5);

  for (int i = 0; i < 4; i++)
  {
    display.print(codeInput[i]);
  }

  // Dessine le marqueur sous le chiffre actuel
  int markerPosition = 5 + (currentDigit * 12);               // 12 est un espace approximatif pour un caractère de taille 1
  display.fillRect(markerPosition, 15, 10, 2, SSD1306_WHITE); // Dessine un rectangle comme marqueur

  display.display();
}

String xorWithPin(int pin[4], String password)
{
  String encrypted = "";

  for (int i = 0; i < password.length(); i++)
  {
    char c = password[i];
    int index = validChars.indexOf(c);

    if (index == -1)
    {
      return "Erreur: Caractère non valide dans le mot de passe!";
    }

    index = index ^ pin[i % 4];          // Utilisez XOR ici
    index = index % validChars.length(); // Gardez l'index dans la plage valide
    encrypted += validChars[index];
  }

  return encrypted;
}

void loop()
{
  if (buttonState == LOW)
  {
    codeInput[currentDigit] = (codeInput[currentDigit] + 1) % 10;
    displayUpdate();
    delay(500);
  }

  if (buttonState2 == LOW)
  {
    currentDigit = (currentDigit + 1) % 4;
    // si le curseur est sur le dernier chiffre, on valide le code et on retourne à 0
    if (currentDigit == 0)
    { 
      //convertir le tableau en int
      for (int i = 0; i < 4; i++)
      {
        codeInputInt = codeInputInt * 10 + codeInput[i];
      }
      writePass(codeInputInt);
      currentDigit = 0;
    }
    displayUpdate();
    delay(500);
  }
}
