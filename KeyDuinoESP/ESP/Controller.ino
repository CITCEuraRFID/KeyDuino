/********** Controller functions **********/

String getID_Keyduino() {
  while (!mySerial.available()) {
    Serial << ".";
    delay(100);
  }
  NFCID = mySerial.readString();
  if (NFCID != "") {
    Serial << endl;
    Serial << "Reading tag ID: " << NFCID << endl;
    return NFCID;
  } else {
    return getID_Keyduino();
  }
}

void sendID_MQTT() {
  if (NFCID != "") {
    // send id to the serial console
    Serial << "Sending ID to MQTT broker: " << NFCID << endl;
    // send id to the MQTT topic
    mqtt_payload = strToChar(NFCID);
    client.publish(mqtt_topic, mqtt_payload, retained);
  }
  client.loop();
}

boolean verifyAccess() {
  client.loop();

  if (access == ACCESS_GRANTED) {
    Serial << "You are authorized !" << endl;
    Serial << "Sending 1 to Keyduino" << endl;
    mySerial.print(1);
    Serial << "Closing MQTT connection..." << endl;
    return true;
  }
  if (access == ACCESS_DENIED) {
    Serial << "You are not authorized !" << endl;
    Serial << "Sending 0 to Keyduino" << endl;
    mySerial.print(0);
    Serial << "Closing MQTT connection..." << endl;
    return false;
  }
}

void destroyAccess() {
  access = ACCESS_DENIED;
}
