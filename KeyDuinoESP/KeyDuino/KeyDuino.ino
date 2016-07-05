/*
 * The KeyDuino reads an NFC tag and sends the tag ID to the ESP in serial communication
 * The tag ID is then read by the ESP and sent to the MQTT broker to check for permissions (server side) 
 * The ESP sends back to the KeyDuino 0 or 1 depending on the authorization
 */
 
#include "KeyDuino.h"
#include <SoftwareSerial.h>
#include <FastLED.h>
#define NUM_LEDS 4
#define DATA_PIN 13
#define BRIGHTNESS 255
CRGB leds[NUM_LEDS];

KeyDuino keyDuino;
#define RX 9
#define TX 8
SoftwareSerial mySerial(RX, TX);  //RX, TX to be defined
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;
int i = 0 ;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  keyDuino.begin();
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
}

void validate() {
  while (!mySerial.available())  sinelon(0, 255, 255);
  int access = mySerial.read();
  Serial.println(access);

  if (access == 49) {
    keyDuino.buzz(20);
    Color_ALL(0, 255, 0);
    Serial.print("ACCESS GRANTED");
  }
  else if (access == 48) {
    keyDuino.buzz(20);
    delay(250);
    keyDuino.buzz(20);
    Color_ALL(255, 0, 0);
    Serial.print("ACCESS DENIED");
  }
}

String get_ID() {
  uint8_t success = 0;
  String readID;
  while (!success) {
    success = keyDuino.readTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    Color_wait(255, 255, 255);
  }
  readID = keyDuino.convertUintToString(uid, uidLength);
  return (readID);
}

void loop() {
  String readID = get_ID();
  Serial.println("Tag found: ");
  Serial.println(readID);
  mySerial.print(readID);
  validate();
  delay(2000);
  Color_ALL(0, 0, 0);
}
