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
  success = keyDuino.readTargetID(uid, &uidLength);
  if (success){  
    Serial.print("Tag found: ");
    keyDuino.PrintHex(uid, uidLength);
    keyDuino.buzz(20);
    delay(250);
  }
  delay(1);
}




