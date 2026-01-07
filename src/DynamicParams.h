/****************************************************************************************************************************
  DynamicParams.h
  For ESP8266 / ESP32 boards

  ESP_WiFiManager_Lite (https://github.com/khoih-prog/ESP_WiFiManager_Lite) is a library
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager_Lite
  Licensed under MIT license
 *****************************************************************************************************************************/

#ifndef DYNAMICPARAMS_H
#define DYNAMICPARAMS_H

//#include "defines.h"

// USE_DYNAMIC_PARAMETERS defined in defined.h

/////////////// Start dynamic Credentials ///////////////

//Defined in <ESP_WiFiManager_Lite.h>
/**************************************
  #define MAX_ID_LEN                5
  #define MAX_DISPLAY_NAME_LEN      16

  typedef struct
  {
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
  } MenuItem;
**************************************/

#if USE_DYNAMIC_PARAMETERS

#define MAX_SERVER_LEN      34
#define MAX_TOKEN_LEN       34

char Server1 [MAX_SERVER_LEN + 1]  = "account.duckdns.org";
char Token1  [MAX_TOKEN_LEN + 1]   = "token1";

char Server2 [MAX_SERVER_LEN + 1]  = "account.ddns.net";
char Token2  [MAX_TOKEN_LEN + 1]   = "token2";

#define MAX_PORT_LEN        6
char Port   [MAX_PORT_LEN + 1]  = "8080";

#define MAX_MQTT_SERVER_LEN      34
char MQTT_Server  [MAX_MQTT_SERVER_LEN + 1]   = "mqtt.duckdns.org";
char MQTT_Pub  [MAX_MQTT_SERVER_LEN + 1]   = "display/status";
char MQTT_Sub  [MAX_MQTT_SERVER_LEN + 1]   = "display/set";

MenuItem myMenuItems [] =
{
  { "sv1", "Server1", Server1,  MAX_SERVER_LEN },
  { "tk1", "Token1",  Token1,   MAX_TOKEN_LEN },
  { "sv2", "Server2", Server2,  MAX_SERVER_LEN },
  { "tk2", "Token2",  Token2,   MAX_TOKEN_LEN },
  { "prt", "Port",    Port,     MAX_PORT_LEN },
  { "mqt",  "MQTT Server",        MQTT_Server,  MAX_MQTT_SERVER_LEN },
  { "mqtp", " publish topic",     MQTT_Pub,     MAX_MQTT_SERVER_LEN },
  { "mqts", " subscribe topic",   MQTT_Sub,     MAX_MQTT_SERVER_LEN },
};

uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);  //MenuItemSize;

#else

MenuItem myMenuItems [] = {};

uint16_t NUM_MENU_ITEMS = 0;

#endif    //USE_DYNAMIC_PARAMETERS


#endif //DYNAMICPARAMS_H
