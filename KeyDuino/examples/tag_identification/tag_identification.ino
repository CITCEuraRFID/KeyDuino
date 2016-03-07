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
  uint8_t success;
  String readID;
  success = keyDuino.readTargetID(uid, &uidLength);
  if (success){
    readID = keyDuino.convertUintToString(uid, uidLength); 
    Serial.print("Tag found: ");
    Serial.println(readID);
    keyDuino.buzz(20);
    delay(250);
  }
  delay(1);
}
