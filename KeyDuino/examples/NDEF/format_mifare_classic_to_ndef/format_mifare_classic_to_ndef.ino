/****************************************************************************************************
/Formats a Mifare Classic tags as an NDEF tag
/ This will fail if the tag is already formatted NDEF
/ nfc.clean will turn a NDEF formatted Mifare Classic tag back to the Mifare Classic format
Copyright : Don https://github.com/don/NDEF
/****************************************************************************************************/

#include <KeyDuino.h>
#include <NfcAdapter.h>

KeyDuino keyDuino;
NfcAdapter nfc = NfcAdapter(keyDuino);

void setup(void) {
    Serial.begin(9600);
    Serial.println("NDEF Formatter");
    nfc.begin();
}

void loop(void) {
    
    Serial.println("\nPlace an unformatted Mifare Classic tag on the reader.");
    if (nfc.tagPresent()) {

        bool success = nfc.format();
        if (success) {
          Serial.println("\nSuccess, tag formatted as NDEF.");
        } else {
          Serial.println("\nFormat failed.");
        }

    }
    delay(5000);
}
