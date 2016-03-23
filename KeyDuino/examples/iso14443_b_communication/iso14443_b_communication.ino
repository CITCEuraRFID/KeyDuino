#include "KeyDuino.h"

KeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;
uint8_t response[64];
uint8_t responseLength = 64;

uint8_t SELECT_AID[] = { 0x00, 0xA4, 0x04, 0x00, 0x0B, 0xA0, 0x00, 0x00, 0x02, 0x91, 0xD2, 0x50, 0x30, 0x03, 0x93, 0x01, 0x00 };

void setup(void) {
  Serial.begin(115200);
  keyDuino.begin();

  Serial.println("KeyDuino is ready, please scan a tag.");
}

void loop(void) {
  bool success;
  String readID;
  //success = keyDuino.readTargetID(PN532_ISO14443B, uid, &uidLength);
  success = keyDuino.inListPassiveTarget(PN532_ISO14443B);
  if (success){
/*    readID = keyDuino.convertUintToString(uid, uidLength); 
    Serial.print("Tag found: ");
    Serial.println(readID);
*/
    success = keyDuino.inDataExchange(SELECT_AID, sizeof(SELECT_AID), response, &responseLength);

    Serial.print("\nSELECT_AID: ");
    keyDuino.PrintHex(response, responseLength);
  }
  delay(2000);
}
