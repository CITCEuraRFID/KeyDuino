/*
This sketch allows a simple communication between KeyDuino and the KeyDuino Android App.
Main features include control of input/output pins of the board.
The pin control is made through a JSON message received through NDEF.

Author: Raymond Borenstein - CITC-EuraRFID

Compatible with KeyDuino 5.1

Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/


#include <KeyDuino.h>
#include <ArduinoJson.h>
#include "snep.h"
#include "NdefMessage.h"

#define BUZZER_PIN    15

KeyDuino keyDuino;
SNEP nfc(keyDuino);
uint8_t ndefBuf[128];

void setup() {
  Serial.begin(115200);

  //Output definition
  for (int i = 2 ; i <= 16 ; i++) {
    pinMode(i, OUTPUT);
    delay(10);
  }

  //Input definition
  for (int i = 18 ; i <= 23 ; i++) {
    pinMode(i, INPUT);
    delay(10);
  }
}

//Format data to be sent as NFDEF plain text
int SEND_NDEF(int val) {
  NdefMessage message = NdefMessage();
  message.addTextRecord(String(val));

  int messageSize = message.getEncodedSize();
  message.encode(ndefBuf);
  if (0 >= nfc.write(ndefBuf, messageSize)) return (0);
  else return (1);
}

//Read NDEF received data and return as a string
String GET_NDEF() {
  int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf));
  if (msgSize > 0) {
    NdefMessage msg  = NdefMessage(ndefBuf, msgSize);
    NdefRecord record = msg.getRecord(0);
    int payloadLength = record.getPayloadLength();
    byte payload[payloadLength];
    record.getPayload(payload);
    int startChar = 0;
    
    if (record.getTnf() == TNF_WELL_KNOWN && record.getType() == "T") {
      startChar = payload[0] + 1;
    }
    else if (record.getTnf() == TNF_WELL_KNOWN && record.getType() == "U") {
      startChar = 1;
    }

    String payloadAsString = "";
    for (int c = startChar; c < payloadLength; c++) {
      payloadAsString += (char)payload[c];
    }
    return (payloadAsString);
  }
  else return ("NULL");
}

void loop() {
  String content = "NULL";

  //Loop reading NDEF messages
  while (content == "NULL") content = GET_NDEF();

  Serial.println("Json array: " + content);

  StaticJsonBuffer<600> jsonBuffer;
  JsonArray& root = jsonBuffer.parseArray(content);

  if (!root.success())
  {
    Serial.println("parseArray() failed");
    return;
  }

  for (JsonArray::iterator it = root.begin(); it != root.end(); ++it)
  {
    JsonArray& array = *it;
    int pin = array[0];
    int value;
    if (pin < 17 && pin != BUZZER_PIN)
      value =  array[1];
    
    //If output pin, then write
    if (pin >= 2 && pin < 17 && pin != BUZZER_PIN) {
      digitalWrite(pin, value);
      Serial.print("Writing on pin ");
      Serial.print(pin);
      Serial.print(" with value ");
      Serial.println(value);
    } 
    //Else if buzzer pin, then buzz one time
    else if (pin == BUZZER_PIN) {
        keyDuino.buzz(100);
    }
    //Else if input pin, then read and send back the value through NDEF
    else if (pin >= 18 && pin < 24) {
      int readValue = analogRead(pin);
      Serial.print("Reading pin ");
      Serial.print(pin);
      Serial.print(" with value ");
      Serial.println(readValue);
      SEND_NDEF(readValue);
    }
    delay(10);
  }
  delay(2000);
}

