#include <KeyDuino.h>
#include <NfcAdapter.h>

KeyDuino keyDuino;
NfcAdapter nfc = NfcAdapter(keyDuino);

void setup(void) {
    Serial.begin(115200);
    Serial.println("NDEF Reader");
    nfc.begin();
}

void loop(void) {
    Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        tag.print();
    }
    delay(3000);
}
