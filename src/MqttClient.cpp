//FROM: https://github.com/Qrome/marquee-scroller/pull/265
/**
 * Copyright (c) 2023 Michael Dawson
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *
 * Source: PubSub example, https://github.com/Qrome/marquee-scroller/pull/265
 */

#include "MqttClient.h"

WiFiClient wclient;

char lastMqttMessage[128];
bool lastMqttMessageNew;

void callback(char* topic, uint8_t* message, unsigned int length) {
  (void)topic; // unused argument
  if (length >= (unsigned int)sizeof(lastMqttMessage))
    length = (unsigned int)sizeof(lastMqttMessage)-1;
  memcpy(lastMqttMessage, message, length);
  lastMqttMessage[length] = 0;
  lastMqttMessageNew = true;
}

MqttClient::MqttClient(const String &passedServer, int port, const String &passedTopic, const String &passedAuthUser, const String &passedAuthPass):
  client("", 0, callback, wclient) {
  updateMqttClient(passedServer, port, passedTopic, passedAuthUser, passedAuthPass);
}

void MqttClient::updateMqttClient(const String &passedServer, int port, const String &passedTopic, const String &passedAuthUser, const String &passedAuthPass) {
  this->port = port;
  passedServer.toCharArray(server, MAX_SERVERNAME_LEN);
  passedTopic.toCharArray(topic, MAX_TOPIC_LEN);
  passedAuthUser.toCharArray(authUser, sizeof(authUser));
  passedAuthPass.toCharArray(authPass, sizeof(authPass));
  client.setServer(server, port);
  client.disconnect();
  failMessage[0] = 0;
  lastMqttMessage[0] = 0;
  lastMqttMessageNew = false;
}

char* MqttClient::getLastMqttMessage() {
  if (strlen(lastMqttMessage) != 0) {
    return lastMqttMessage;
  }
  return (char*)"";
}

char* MqttClient::getNewMqttMessage() {
  if (lastMqttMessageNew == true) {
    lastMqttMessageNew = false;
    return getLastMqttMessage();
  }
  return (char*)"";
}


String MqttClient::getError() {
  return failMessage;
}

void MqttClient::loop() {
  if (!client.connected()) {
    bool connectstatus;
    char client_id[20];
    // NOTE: the connect client-ID MUST be unique on the MQTT server
    // Here we use the ESP Chip ID to make it unique
    sprintf_P(client_id, PSTR("marquee-%08X"), ESP.getChipId());
    Serial.println(F("MQTT (Re-)connect"));
    if (strlen(authUser) > 0) {
      connectstatus = client.connect(client_id, authUser, authPass);
    } else {
      connectstatus = client.connect(client_id);
    }
    if (connectstatus) {
      failMessage[0] = 0;
      if (!client.subscribe(topic)) {
        sprintf_P(failMessage, PSTR("MQTT Failed to connect to topic: %s"), topic);
        Serial.println(failMessage);
      } else {
        Serial.printf_P(PSTR("MQTT connected and subscribed to: %s\n"), topic);
      }
    } else {
      sprintf_P(failMessage, PSTR("MQTT Failed to connect to: %s:%d, reason: %d"), server, port, client.state());
      Serial.println(failMessage);
    }
  }
  if (client.connected()) {
    //Serial.println(F("MQTT connected loop"));
    client.loop();
  }
}
