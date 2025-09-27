/**
 * Copyright (c) 2023 Michael Dawson
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 */

#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#define MAX_TOPIC_LEN 128
#define MAX_SERVERNAME_LEN 34
#define MAX_USRPW_LEN  32

class MqttClient {

  private:
    WiFiClient wclient;
    //n.u. String lastMessage;
    PubSubClient client;
    char topic[MAX_TOPIC_LEN];
    char server[MAX_SERVERNAME_LEN];
    char authUser[MAX_USRPW_LEN];
    char authPass[MAX_USRPW_LEN];
    char failMessage[MAX_TOPIC_LEN + 100];
    int port = 0;

  public:
    MqttClient(const String &passedServer, int port, const String &passedTopic, const String &authUser = "", const String &authPass = "");
    String getError();
    char* getLastMqttMessage();
    char* getNewMqttMessage();
    void updateMqttClient(const String &passedServer, int port, const String &passedTopic, const String &authUser = "", const String &authPass = "");

    void loop();

    bool connected() { return client.connected(); };
    bool publish(const char* topic, const char* payload) { return client.publish(topic, payload);};
    bool subscribe(const char* topic) { return client.subscribe(topic); };

};
