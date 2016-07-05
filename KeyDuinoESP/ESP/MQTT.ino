/********** MQTT functions **********/

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("\t/!\\ connected /!\\");
      client.subscribe(mqtt_topic_access);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

char* createMqtt_topic(String mqtt_topic_syntax) {
  String clientMac = mqtt_topic_syntax;
  unsigned char mac[6];
  WiFi.macAddress(mac);
  clientMac += macToStr(mac);
  return strToChar(clientMac);
}

char* createMqtt_topic_access(String mqtt_topic_syntax) {
  String clientMac = mqtt_topic_syntax;
  unsigned char mac[6];
  WiFi.macAddress(mac);
  clientMac += macToStr(mac)+"/access";
  return strToChar(clientMac);
}

// handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  //Serial.println("Message arrived:  topic: " + String(topic));
  //Serial.println("Length: " + String(length,DEC));

  // create character buffer with ending null terminator (string)
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);

  Serial << "Payload recieved: " << msgString << endl;

  if (msgString.equals("{\"access\":{\"status\": \"1\"}}")) {
    access = ACCESS_GRANTED;
  } else if (msgString.equals("{\"access\":{\"status\": \"0\"}}")) {
    access = ACCESS_DENIED;
  }
}
