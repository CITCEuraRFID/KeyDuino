/*
 The goal of this sketch is to unlock a "giftbox" with KeyDuino and a NFC tag, via a servomotor.
 Get the ID of your tag, put it in NFC_KEY below.
 When you scan the right tag with KeyDuino, the servo will unlock the box.
 More information in the video of the project: https://www.youtube.com/watch?v=cIedvE9mYfM
 Author: Mr Stein
 Revised by Raymond Borenstein - CITC-EuraRFID
 Compatible with KeyDuino 5.1
 Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
 */

#include <Servo.h> //Servo library is included by default, you should not need to install anything
#include <KeyDuino.h>

#define LED_PIN     13
#define SERVO_PIN   12

KeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;

Servo myServo;  // create servo object to control the servo

String NFC_KEY_1 = "xxxxxxxxxxxx";
String NFC_KEY_2 = "xxxxxxxxxxxx";

//Function to open the servo
void openServo() {
  myServo.write(180);
  delay(8000);
  myServo.write(0);
}

void setup(void) {
  Serial.begin(115200);
  keyDuino.begin();
  myServo.attach(SERVO_PIN);
  myServo.write(0);
  Serial.println("KeyDuino BOX is ready, please scan a NFC tag");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop(void) {
  uint8_t success;
  String readID;
  success = keyDuino.readTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success){
    readID = keyDuino.convertUintToString(uid, uidLength);

    if (readID == NFC_KEY_1 || readID == NFC_KEY_2){
      openServo();
      digitalWrite(LED_PIN, LOW);
      delay(500);
      digitalWrite(LED_PIN, HIGH);
    }
    else {
      keyDuino.buzz(20);
      keyDuino.buzz(20);
    }
  }
  delay(1);
}

