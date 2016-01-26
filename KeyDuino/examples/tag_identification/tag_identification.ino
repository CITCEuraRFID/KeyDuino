#include "KeyDuino.h"
#define BUZZER_PIN 15

KeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;

void setup(void) {
  Serial.begin(115200);
  keyDuino.begin();

  while(!Serial){delay(1);}
  
  Serial.println("KeyDuino is ready, please scan a tag.");

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop(void) {
  uint8_t success;
  success = keyDuino.readTargetID(uid, &uidLength);
  if (success){  
    Serial.print("Tag found: ");
    keyDuino.PrintHex(uid, uidLength);
    buzz(20);
    delay(250);
  }
  delay(1);
}

void buzz(int duration){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}



