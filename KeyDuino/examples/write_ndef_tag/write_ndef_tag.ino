#include <KeyDuino.h>
#include <NfcAdapter.h>

/****************************************************************************************************
/ This sketch is made to be used with a formatted NFC tag.
/ NFC Forum tags like NTAG213 and Mifare Ultralight will work by default.
/ If you want to use it with a Mifare Classic, first use the example to format Mifare Classic to NDEF.
/****************************************************************************************************/

KeyDuino keyDuino;
NfcAdapter nfc = NfcAdapter(keyDuino);

String URI = "http://github.com/CITCEuraRFID/KeyDuino"; //Website URL
//String URI = "tel:0600000000"; //Phone number
//String URI = "mailto:keyduino@outlook.com"; //Email address

void setup() {
      Serial.begin(115200);
      Serial.println("NDEF Writer");
      nfc.begin();
}

void loop() {
    Serial.println("\nPlace a formatted NFC tag on the reader.");
    if (nfc.tagPresent()) {
      
        NdefMessage message = NdefMessage();
        message.addUriRecord(URI);

        bool success = nfc.write(message);
        if (success) {
          Serial.println("Success. Try reading this tag with your phone.");        
        } else {
          Serial.println("Write failed.");
        }
    }
    delay(5000);
}
