/*
This sketch is made to be used with an HCE (Host-based Card Emulation) application, for example to unlock a door.
KeyDuino sends an APDU command to select an Application ID, and receives an answer from the app.
You can then compare the answer with what you expect, and proceed to unlocking if it matches.

Author: Raymond Borenstein - CITC-EuraRFID

Compatible with KeyDuino 5.1

Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/

#include <KeyDuino.h>

KeyDuino keyDuino;

uint8_t openingCode[] = { 
  0xC0, 0xC0, 0x13, 0x37, 0x90, 0x00 }; //90 00 is the success code, you need to add it - the rest of the code is the same as the one defined in the HCE application

void setup()
{    
  Serial.begin(115200);
  Serial.println("-------Peer to Peer HCE--------");

  keyDuino.begin();
}

void loop()
{
  bool success;

  uint8_t responseLength = 32;

  Serial.println("Waiting for an HCE Application...");

  // set shield to inListPassiveTarget
  success = keyDuino.inListPassiveTarget(PN532_MIFARE_ISO14443A); //If it doesn't work with your device, you can try with PN532_ISO14443B

  if(success) {

    Serial.println("Found something!");

    uint8_t selectApdu[] = { 
      0x00, /* CLA */
      0xA4, /* INS */
      0x04, /* P1  */
      0x00, /* P2  */
      0x07, /* Length of AID  */
      0xF0, 0x4D, 0x45, 0x4C, 0x48, 0x43, 0x45, /* AID defined on HCE App */
      0x00  /* Le  */     };

    uint8_t response[32];  

    success = keyDuino.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);

    if(success) {

      Serial.print("responseLength: "); 
      Serial.println(responseLength);

      Serial.print("response: ");
      keyDuino.PrintHex(&response[0], responseLength);
      
      if (sizeof(openingCode)/sizeof(uint8_t) == responseLength && memcmp(response, openingCode, responseLength) == 0){
        //Response is correct, do whatever you need here
        Serial.println("Opening!");
      } else {
        Serial.println("Incorrect response, not opening...");
      }
    }
    else {
      Serial.println("Failed sending SELECT AID"); 
    }
  }
  delay(1000);
}
