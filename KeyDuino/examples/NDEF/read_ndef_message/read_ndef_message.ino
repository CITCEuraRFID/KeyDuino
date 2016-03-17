/*
  This sketch is made to be used with any NDEF active device, and allow you to print raw NDEF message.
  Please note that it won't work with a passive NFC tag.

  Compatible with KeyDuino 5.1

  Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/

#include <KeyDuino.h>
#include <snep.h>
#include "NdefMessage.h"

KeyDuino keyDuino;
SNEP nfc(keyDuino);
uint8_t ndefBuf[128];

void setup() {
  Serial.begin(115200);
}

void loop() {
  int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf));
  NdefMessage msg  = NdefMessage(ndefBuf, msgSize);

  if (msgSize != 0) {

    msg.print(); //raw print
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
    
    keyDuino.buzz(20);
    Serial.println("\nContent received: " + payloadAsString);
    delay(2000);
  }
}


