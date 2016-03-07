/*
This sketch is made to be used with any NDEF active device, to lock/unlock something.
The unlock is made with the reception of a plain text password through NDEF.
Please note that it won't work with a passive NFC tag.
More informations in the video of the project: https://www.youtube.com/watch?v=0NFij0T6PGA

Author: Raymond Borenstein - CITC-EuraRFID

Compatible with KeyDuino 5.1

Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/


#include <KeyDuino.h>
#include <snep.h>
#include "NdefMessage.h"

KeyDuino keyDuino;
SNEP nfc(keyDuino);
uint8_t ndefBuf[128];

const String defined_password = "password"; //The unlocking password

void setup() {
  Serial.begin(115200);
}

//Format data to be sent as NDEF plain text
int SEND_NDEF(String val) {
  NdefMessage message = NdefMessage();
  message.addTextRecord(val);

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

  keyDuino.buzz(20);
  Serial.println("Content received: " + content);

  //If the received message is the password, then unlock
  if(content == defined_password){
    if(!SEND_NDEF("unlocked")){
      Serial.println("Retrying response");
      delay(100);
      SEND_NDEF("unlocked");
    }
  }
  //Else if the message is "0", then the user asks to lock
  else if (content == "0"){
    if(!SEND_NDEF("locked")){
      Serial.println("Retrying response");
      delay(100);
      SEND_NDEF("locked");
    }
  }
  delay(2000);
}

