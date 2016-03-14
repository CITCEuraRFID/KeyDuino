/*
The goal of this sketch is to unlock your computer (for example) with KeyDuino and a NFC tag.
Get the ID of your tag, put it in NFC_KEY below, and set the PASSWORD of your computer.
When you scan the tag with KeyDuino, it will enter your password automatically, as if you had typed it with your keyboard.
More information in the video of the project: https://www.youtube.com/watch?v=TPVR7IkHJbM 
Author: Mr Stein
Revised by Raymond Borenstein - CITC-EuraRFID
Compatible with KeyDuino 5.1
Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/

#include <KeyDuino.h>
#include <Keyboard.h>

KeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;

String NFC_KEY_1 = "xxxxxxxxxxxxxx";
String NFC_KEY_2 = "xxxxxxxxxxxxxx";

String PASSWORD = "password\n"; //Printed with a qwerty layout, you can change characters in Keyboard.cpp if you need. \n is added to enter the password automatically.

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
    Serial.print("Tag found: ");
    readID = keyDuino.convertUintToString(uid, uidLength);

    if (readID == NFC_KEY_1 || readID == NFC_KEY_2){
      keyDuino.buzz(20);
      Keyboard.println(PASSWORD);
      delay(1000);
    }

  }
  else Serial.print(".");
}
