/*
This sketch is made to be used with a Mifare Classic card, and serves as an example to write its sectors.
Be sure to know what you do before writing on your card.
It uses the MifareClassicKeyDuino class, which is an extension of KeyDuino class, easing the learning with Mifare Classic.

Author: Raymond Borenstein - CITC-EuraRFID

Compatible with KeyDuino 5.1

Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/

#include <KeyDuino.h>

uint8_t definedKey[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t data[16] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t trailerBlockData[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

#define SECTOR  5 //The sector to write on (0 - 15 on Mifare 1K, 0 - 47 on Mifare 4K)
#define BLOCK   0 //The block of the sector (0-3)

MifareClassicKeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; 
uint8_t uidLength;

void setup(void) {
  Serial.begin(115200);
  
  keyDuino.begin();

  keyDuino.SAMConfig();
  
  Serial.println("/!\\ Warning! /!\\");
  Serial.println("Be sure to know what you are doing when writing on a card!");
  Serial.println("Remember a wrong usage can lead you to make the card unusable ...");
  Serial.println("/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\");
}

void loop(void) {
  uint8_t success;
  success = keyDuino.readTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    if (uidLength == 4) {
      Serial.println("Mifare classic identified");
      if (keyDuino.authenticateDefinedKey(definedKey, MIFARE_KEY_A, SECTOR) || 
            keyDuino.authenticateDefinedKey(definedKey, MIFARE_KEY_B, SECTOR) ||
            keyDuino.mifareclassic_AuthenticateSectorDefaultKeys(SECTOR))
        {
          if (keyDuino.mifareclassic_IsTrailerBlock(BLOCK))
            keyDuino.writeBlock(SECTOR * 4 + BLOCK, trailerBlockData);
          else
            keyDuino.writeBlock(SECTOR * 4 + BLOCK, data);
        }
      Serial.println("Operation complete.");
      Serial.println("Press a key for next operation.");
      while (!Serial.available());
      while (Serial.available()) Serial.read();
    } else {
      Serial.println("Detected tag is not Mifare Classic.");
      
    }
  }
  delay(1);
}

