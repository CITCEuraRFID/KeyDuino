#include "KeyDuino.h"

KeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;

void setup(void) {
  Serial.begin(115200);
  keyDuino.begin();

  Serial.println("KeyDuino is ready, please scan a tag.");
}

void loop(void) {
  bool success;
  String readID;
  success = keyDuino.readTargetID(PN532_ISO14443B, uid, &uidLength);
  if (success){
    readID = keyDuino.convertUintToString(uid, uidLength); 
    Serial.print("Tag found: ");
    Serial.println(readID);
  }
  delay(1);
}
