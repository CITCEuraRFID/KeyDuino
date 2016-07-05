/*
 * ESP-8266 that connects to a WiFi network and chats with an MQTT server (publish & subscribe)
 * It checks if the NFC tag ID is authorized and sends the response to the KeyDuino in order to enable actions... or not...
 * Wakes up periodically (configure SLEEP_DELAY_IN_SECONDS accordingly).
 * If you wish to enable deep_sleep for the ESP: 
 * Hookup guide:
  - connect D0 pin to RST pin in order to enable the ESP-8266 to wake up periodically
*/

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Streaming.h>

//#define SLEEP_DELAY_IN_SECONDS   30

// defines for serial communication to Keyduino
#define RX  4
#define TX  5

// defines and variable for access control
#define ACCESS_DENIED    0
#define ACCESS_GRANTED   1
int access = 0;
String NFCID;
char message_buff[100];

const char* ssid = "<YOUR WIFI NETWORK SSID>";
const char* password = "YOUR WIFI PASSWORD";

const char* mqtt_server = "<YOUR MQTT SERVER ADDRESS>";
const char* mqtt_username = "<A USERNAME>"; // used for connecting to the broker 
const char* mqtt_password = ""; // leave empty if not needed
const String mqtt_topic_syntax = "keyduino/nfc/"; // the first part of the MQTT topic, the mac address of the ESP is added automaticaly
char* mqtt_topic = "";
char* mqtt_topic_access = "";
char* mqtt_payload = "";
boolean retained = false;

WiFiClient espClient;
PubSubClient client(espClient);

SoftwareSerial mySerial(RX, TX);  //RX, TX to be defined

void setup() {
  // setup serial port
  Serial.begin(115200);

  // setup WiFi
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // setup UART
  Serial.println("Initializing UART connection...");
  mySerial.begin(9600);
  mySerial.setTimeout(10);

  // create mqtt_topic with MAC address
  mqtt_topic = createMqtt_topic(mqtt_topic_syntax);
  mqtt_topic_access = createMqtt_topic_access(mqtt_topic_syntax);
  Serial << "MQTT Topic generated (dialog): " << mqtt_topic << endl;
  Serial << "MQTT Topic generated (access control): " << mqtt_topic_access << endl;

  // sending "connected" to broker
  client.publish(mqtt_topic, "connected", retained);

}

void loop() {
  Serial << endl << "---------- Starting LOOP ----------" << endl;
  NFCID = "";

  if (!client.connected()) {
    reconnect();
  }

  getID_Keyduino();
  sendID_MQTT();
  Serial << "--- Waiting 3 seconds for server response ---" << endl;
  verifyAccess();

  //Serial << "Closing MQTT connection..." << endl;
  //client.disconnect();

  //Serial << "Closing WiFi connection..." << endl;
  //WiFi.disconnect();

  delay(100);

  //Serial << "Entering deep sleep mode for " << SLEEP_DELAY_IN_SECONDS << " seconds..." << endl;
  //ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  //ESP.deepSleep(10 * 1000, WAKE_NO_RFCAL);
  delay(500);   // wait for deep sleep to happen

  Serial << "---------- Ending LOOP ----------" << endl;
}










