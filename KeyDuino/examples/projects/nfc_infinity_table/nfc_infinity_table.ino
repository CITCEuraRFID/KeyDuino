/*
This sketch is made to be used with any NDEF active device sending a color value (format #RRGGBB).
This then sets the connected LEDs to the specified color.
Please note that it won't work with a passive NFC tag.
You will need NDEF library and FastLED ( https://github.com/FastLED/FastLED )
Author: Mr Stein
Revised by Raymond Borenstein - CITC-EuraRFID
Compatible with KeyDuino 5.1
Join http://keyduino.forumsactifs.com/ to ask your questions, suggest your ideas, and show your projects!
*/

#include <FastLED.h>
#include <KeyDuino.h>
#include "snep.h"
#include "NdefMessage.h"

#define NUM_LEDS 152
#define DATA_PIN 13
CRGB leds[NUM_LEDS];
#define BRIGHTNESS  255

#define FRAMES_PER_SECOND 60
#define COOLING  55
#define SPARKING 120

KeyDuino keyDuino;
SNEP nfc(keyDuino);
uint8_t ndefBuf[128];

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  Color_ALL(255,0,0);
}

void Color_ALL(int RED, int GREEN, int BLUE)
{
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(RED, GREEN, BLUE);
  }
  FastLED.show();
}


unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;
  for (int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }

  return decValue;
}


String GET_NDEF(){
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
    return(payloadAsString);
  }
  else return("NULL");
}

void loop() {
  String Color = "NULL";

  while(Color=="NULL") Color = GET_NDEF();
  Serial.println(Color);


  //The received NDEF message must be formatted "#RRGGBB", where RR is red level, GG green level, BB blue level
  String r_string = Color.substring(1,3);
  String g_string = Color.substring(3,5);
  String b_string = Color.substring(5,7);
  int r = hexToDec(r_string);
  int g = hexToDec(g_string);
  int b = hexToDec(b_string);

  Color_ALL(r,g,b);
}

