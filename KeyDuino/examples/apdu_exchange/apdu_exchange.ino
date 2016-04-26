/*
This sketch is an example on how to exchange APDU commands with a compatible card.
For the moment, these are ISO14443-A (like Mifare) and ISO14443-B (like Calypso) cards.
APDU commands are defined in ISO7816-4, and you can learn more at http://www.cardwerk.com/smartcards/smartcard_standard_ISO7816-4.aspx for example.

Author: Raymond Borenstein - CITC-EuraRFID

Compatible with KeyDuino 5.1

Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/

#include "KeyDuino.h"

KeyDuino keyDuino;

//APDU commands examples, you have to adapt depending on your card type and structure
uint8_t SELECT_EF[] = { 0x00, 0xA4, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t READ_RECORD[] = { 0x00, 0xB2, 0x01, 0x04, 0x00 };

void setup(void) {
  Serial.begin(115200);
  keyDuino.begin();

  Serial.println("KeyDuino is ready, please scan a card.");
}

void loop(void) {
  bool success;
  String readID;
  
  success = keyDuino.inListPassiveTarget(PN532_ISO14443B); //Change to PN532_ISO14443A if needed
  if (success){ //If tag was enlisted, send the SELECT_EF command
    
    uint8_t response[64];
    uint8_t responseLength = 64;
    success = keyDuino.inDataExchange(SELECT_EF, sizeof(SELECT_EF), response, &responseLength);

    Serial.print("\nSELECT_EF: ");
    keyDuino.PrintHex(response, responseLength);

    if (success){ //If EF is selected, read specified record
      memset(response, 0, sizeof(response));
      responseLength = 64;
      success = keyDuino.inDataExchange(READ_RECORD, sizeof(READ_RECORD), response, &responseLength);

      Serial.print("\nREAD_RECORD: ");
      keyDuino.PrintHex(response, responseLength);
    }
  }
  delay(2000);
}
