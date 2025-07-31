/****************************************************************************************************************************
  ESP_WiFiManager_Lite.h
  For ESP8266 / ESP32 boards

  ESP_WiFiManager_Lite (https://github.com/khoih-prog/ESP_WiFiManager_Lite) is a library
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager_Lite
  Licensed under MIT license

  Version: 1.10.5

  Version Modified By   Date        Comments
  ------- -----------  ----------   -----------
  1.0.0   K Hoang      04/02/2021  Initial coding for ESP32/ESP8266
  ...
  1.8.1   K Hoang      11/02/2022  Add LittleFS support to ESP32-C3. Use core LittleFS instead of Lorol's LITTLEFS for v2.0.0+
  1.8.2   K Hoang      21/02/2022  Optional Board_Name in Menu. Optimize code by using passing by reference
  1.9.0   K Hoang      09/09/2022  Fix ESP32 chipID and add ESP_getChipOUI()
  1.10.0  K Hoang      10/01/2023  Add Captive-Portal feature
  1.10.1  K Hoang      12/01/2023  Added public methods to load and save dynamic data
  1.10.2  K Hoang      15/01/2023  Add Config Portal scaling support to mobile devices
  1.10.3  K Hoang      19/01/2023  Fix compiler error if EEPROM is used
  1.10.4  K Hoang      27/01/2023  Using PROGMEM for HTML strings
  1.10.5  K Hoang      28/01/2023  Using PROGMEM for strings in examples
 *****************************************************************************************************************************/

#pragma once

#ifndef ESP_WiFiManager_Lite_h
#define ESP_WiFiManager_Lite_h

#if !( defined(ESP8266) ||  defined(ESP32) )
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#elif ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_ESP32S2_THING_PLUS || ARDUINO_MICROS2 || \
        ARDUINO_METRO_ESP32S2 || ARDUINO_MAGTAG29_ESP32S2 || ARDUINO_FUNHOUSE_ESP32S2 || \
        ARDUINO_ADAFRUIT_FEATHER_ESP32S2_NOPSRAM )
  #warning Using ESP32_S2. To follow library instructions to install esp32-s2 core and WebServer Patch
  #define USING_ESP32_S2        true
#elif ( ARDUINO_ESP32C3_DEV )
  #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
    #warning Using ESP32_C3 using core v2.0.0+. Either LittleFS, SPIFFS or EEPROM OK.
  #else
    #warning Using ESP32_C3 using core v1.0.6-. To follow library instructions to install esp32-c3 core. Only SPIFFS and EEPROM OK.
  #endif

  #warning You have to select Flash size 2MB and Minimal APP (1.3MB + 700KB) for some boards
  #define USING_ESP32_C3        true
#elif ( defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_ESP32_S3_BOX) || defined(ARDUINO_TINYS3) || \
        defined(ARDUINO_PROS3) || defined(ARDUINO_FEATHERS3) )
  #warning Using ESP32_S3. To install esp32-s3-support branch if using core v2.0.2-.
  #define USING_ESP32_S3        true
#endif

///////////////////////////////////////////

#ifndef ESP_WIFI_MANAGER_LITE_VERSION
  #define ESP_WIFI_MANAGER_LITE_VERSION             "ESP_WiFiManager_Lite v1.10.5"

  #define ESP_WIFI_MANAGER_LITE_VERSION_MAJOR       1
  #define ESP_WIFI_MANAGER_LITE_VERSION_MINOR       10
  #define ESP_WIFI_MANAGER_LITE_VERSION_PATCH       5

  #define ESP_WIFI_MANAGER_LITE_VERSION_INT         1010005
#endif

///////////////////////////////////////////

#ifdef ESP8266

  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
  #include <ESP8266WebServer.h>

  //default to use EEPROM, otherwise, use LittleFS or SPIFFS
  #if ( USE_LITTLEFS || USE_SPIFFS )

    #if USE_LITTLEFS
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
      #warning Using LittleFS in ESP_WiFiManager_Lite.h
    #else
      #define FileFS        SPIFFS
      #define FS_Name       "SPIFFS"
      #warning Using SPIFFS in ESP_WiFiManager_Lite.h
    #endif

    #include <FS.h>
    #include <LittleFS.h>
  #else
    #include <EEPROM.h>
    #define FS_Name         "EEPROM"
    #define EEPROM_SIZE     2048
    #warning Using EEPROM in ESP_WiFiManager_Lite.h
  #endif

#else   //ESP32

  #include <WiFi.h>
  #include <WiFiMulti.h>
  #include <WebServer.h>

  // To be sure no LittleFS for ESP32-C3 for core v1.0.6-
  #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
    // For core v2.0.0+, ESP32-C3 can use LittleFS, SPIFFS or EEPROM
    // LittleFS has higher priority than SPIFFS.
    // For core v2.0.0+, if not specified any, use better LittleFS
    #if ! (defined(USE_LITTLEFS) || defined(USE_SPIFFS) )
      #define USE_LITTLEFS      true
      #define USE_SPIFFS        false
    #endif
  #elif defined(ARDUINO_ESP32C3_DEV)
    // For core v1.0.6-, ESP32-C3 only supporting SPIFFS and EEPROM. To use v2.0.0+ for LittleFS
    #if USE_LITTLEFS
      #undef USE_LITTLEFS
      #define USE_LITTLEFS            false
      #undef USE_SPIFFS
      #define USE_SPIFFS              true
    #endif
  #else
    // For core v1.0.6-, if not specified any, use SPIFFS to not forcing user to install LITTLEFS library
    #if ! (defined(USE_LITTLEFS) || defined(USE_SPIFFS) )
      #define USE_SPIFFS      true
    #endif
  #endif

  #if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
    //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      #warning Using ESP32 Core 1.0.6 or 2.0.0+
      // The library has been merged into esp32 core from release 1.0.6
      #include <LittleFS.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS

      FS* filesystem =      &LittleFS;
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
    #else
      #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
      // The library has been merged into esp32 core from release 1.0.6
      #include <LITTLEFS.h>       // https://github.com/lorol/LITTLEFS

      FS* filesystem =      &LITTLEFS;
      #define FileFS        LITTLEFS
      #define FS_Name       "LittleFS"
    #endif

  #elif USE_SPIFFS
    #include "FS.h"
    #include <SPIFFS.h>
    FS* filesystem =        &SPIFFS;
    #define FileFS          SPIFFS
    #define FS_Name         "SPIFFS"
    #warning Using SPIFFS in ESP_WiFiManager_Lite.h
  #else
    #include <EEPROM.h>
    #define FS_Name         "EEPROM"
    #define EEPROM_SIZE     2048
    #warning Using EEPROM in ESP_WiFiManager_Lite.h
  #endif

#endif

#define DNS_PORT      53

///////////////////////////////////////////

#include <DNSServer.h>
#include <memory>
#undef min
#undef max
#include <algorithm>

//KH, for ESP32
#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}

#define ESP_getChipId()   (ESP.getChipId())

#else   //ESP32

#include <esp_wifi.h>

uint32_t getChipID();
uint32_t getChipOUI();

#if defined(ESP_getChipId)
  #undef ESP_getChipId
#endif

#if defined(ESP_getChipOUI)
  #undef ESP_getChipOUI
#endif

#define ESP_getChipId()   getChipID()
#define ESP_getChipOUI()  getChipOUI()
#endif

#include <ESP_WiFiManager_Lite_Debug.h>

//////////////////////////////////////////////

// New from v1.3.0
// KH, Some minor simplification
#if !defined(SCAN_WIFI_NETWORKS)
  #define SCAN_WIFI_NETWORKS     true     //false
#endif

#if SCAN_WIFI_NETWORKS
  #if !defined(MANUAL_SSID_INPUT_ALLOWED)
    #define MANUAL_SSID_INPUT_ALLOWED     true
  #endif

  #if !defined(MAX_SSID_IN_LIST)
    #define MAX_SSID_IN_LIST     10
  #elif (MAX_SSID_IN_LIST < 2)
    #warning Parameter MAX_SSID_IN_LIST defined must be >= 2 - Reset to 10
    #undef MAX_SSID_IN_LIST
    #define MAX_SSID_IN_LIST      10
  #elif (MAX_SSID_IN_LIST > 15)
    #warning Parameter MAX_SSID_IN_LIST defined must be <= 15 - Reset to 10
    #undef MAX_SSID_IN_LIST
    #define MAX_SSID_IN_LIST      10
  #endif
#else
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #warning SCAN_WIFI_NETWORKS disabled
  #endif
#endif

///////// NEW for DRD /////////////

#if !defined(USING_MRD)
  #define USING_MRD       false
#endif

#if USING_MRD

  ///////// NEW for MRD /////////////
  // These defines must be put before #include <ESP_DoubleResetDetector.h>
  // to select where to store DoubleResetDetector's variable.
  // For ESP32, You must select one to be true (EEPROM or SPIFFS/LittleFS)
  // For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS/LittleFS)
  // Otherwise, library will use default EEPROM storage
  #define ESP8266_MRD_USE_RTC     false   //true

  #if USE_LITTLEFS
    #define ESP_MRD_USE_LITTLEFS    true
    #define ESP_MRD_USE_SPIFFS      false
    #define ESP_MRD_USE_EEPROM      false
  #elif USE_SPIFFS
    #define ESP_MRD_USE_LITTLEFS    false
    #define ESP_MRD_USE_SPIFFS      true
    #define ESP_MRD_USE_EEPROM      false
  #else
    #define ESP_MRD_USE_LITTLEFS    false
    #define ESP_MRD_USE_SPIFFS      false
    #define ESP_MRD_USE_EEPROM      true
  #endif

  #ifndef MULTIRESETDETECTOR_DEBUG
    #define MULTIRESETDETECTOR_DEBUG     false
  #endif

  // These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
  // Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
  // Number of subsequent resets during MRD_TIMEOUT to activate
  #ifndef MRD_TIMES
    #define MRD_TIMES               3
  #endif

  // Number of seconds after reset during which a
  // subsequent reset will be considered a double reset.
  #ifndef MRD_TIMEOUT
    #define MRD_TIMEOUT 10
  #endif

  // EEPROM Memory Address for the MultiResetDetector to use
  #ifndef MRD_TIMEOUT
    #define MRD_ADDRESS 0
  #endif

  #include <ESP_MultiResetDetector.h>      //https://github.com/khoih-prog/ESP_MultiResetDetector

  //MultiResetDetector mrd(MRD_TIMEOUT, MRD_ADDRESS);
  MultiResetDetector* mrd;

  ///////// NEW for MRD /////////////

#else

  ///////// NEW for DRD /////////////
  // These defines must be put before #include <ESP_DoubleResetDetector.h>
  // to select where to store DoubleResetDetector's variable.
  // For ESP32, You must select one to be true (EEPROM or SPIFFS/LittleFS)
  // For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS/LittleFS)
  // Otherwise, library will use default EEPROM storage
  #define ESP8266_DRD_USE_RTC     false   //true

  #if USE_LITTLEFS
    #define ESP_DRD_USE_LITTLEFS    true
    #define ESP_DRD_USE_SPIFFS      false
    #define ESP_DRD_USE_EEPROM      false
  #elif USE_SPIFFS
    #define ESP_DRD_USE_LITTLEFS    false
    #define ESP_DRD_USE_SPIFFS      true
    #define ESP_DRD_USE_EEPROM      false
  #else
    #define ESP_DRD_USE_LITTLEFS    false
    #define ESP_DRD_USE_SPIFFS      false
    #define ESP_DRD_USE_EEPROM      true
  #endif

  #ifndef DOUBLERESETDETECTOR_DEBUG
    #define DOUBLERESETDETECTOR_DEBUG     false
  #endif

  // Number of seconds after reset during which a
  // subsequent reset will be considered a double reset.
  #define DRD_TIMEOUT 10

  // RTC Memory Address for the DoubleResetDetector to use
  #define DRD_ADDRESS 0

  #include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

  //DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
  DoubleResetDetector* drd;

  ///////// NEW for DRD /////////////

#endif

///////////////////////////////////////////

//NEW
#define MAX_ID_LEN                5
#define MAX_DISPLAY_NAME_LEN      16

///////////////////////////////////////////

typedef struct
{
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
} MenuItem;

///////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #warning Using Dynamic Parameters
  #endif

  ///NEW
  extern uint16_t NUM_MENU_ITEMS;
  extern MenuItem myMenuItems [];
  bool *menuItemUpdated = NULL;
#else
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #warning Not using Dynamic Parameters
  #endif
#endif

///////////////////////////////////////////

#define SSID_MAX_LEN      32
// WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN      64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

#define NUM_WIFI_CREDENTIALS      2

#if USING_BOARD_NAME
  // Configurable items besides fixed Header, just add board_name
  #define NUM_CONFIGURABLE_ITEMS    ( ( 2 * NUM_WIFI_CREDENTIALS ) + 1 )
#else
  // Configurable items besides fixed Header, just add board_name
  #define NUM_CONFIGURABLE_ITEMS    ( ( 2 * NUM_WIFI_CREDENTIALS ))
#endif

///////////////////////////////////////////

#define HEADER_MAX_LEN            16
#define BOARD_NAME_MAX_LEN        24

typedef struct Configuration
{
  char header         [HEADER_MAX_LEN];
  WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
  char board_name     [BOARD_NAME_MAX_LEN];
  int  checkSum;
} ESP_WM_LITE_Configuration;

// Currently CONFIG_DATA_SIZE  =   236  = (16 + 96 * 2 + 4 + 24)
uint16_t CONFIG_DATA_SIZE = sizeof(ESP_WM_LITE_Configuration);

///////////////////////////////////////////

extern bool LOAD_DEFAULT_CONFIG_DATA;
extern ESP_WM_LITE_Configuration defaultConfig;

///////////////////////////////////////////

// -- HTML page fragments

const char ESP_WM_LITE_HTML_HEAD_START[] PROGMEM = "<!DOCTYPE html><html><head><title>ESP_WM_LITE</title><meta name='viewport' content='width=device-width, initial-scale=1'>";

const char ESP_WM_LITE_HTML_HEAD_STYLE[] PROGMEM =
  "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";

#if USING_BOARD_NAME
  const char ESP_WM_LITE_HTML_HEAD_END[]   PROGMEM =
  "</head><div style='text-align:left;display:inline-block;min-width:260px;'>\
  <fieldset><div><label>*WiFi SSID</label><div>[[input_id]]</div></div>\
  <div><label>*PWD (8+ chars)</label><input value='[[pw]]' id='pw'><div></div></div>\
  <div><label>*WiFi SSID1</label><div>[[input_id1]]</div></div>\
  <div><label>*PWD1 (8+ chars)</label><input value='[[pw1]]' id='pw1'><div></div></div></fieldset>\
  <fieldset><div><label>Board Name</label><input value='[[nm]]' id='nm'><div></div></div></fieldset>";  // DO NOT CHANGE THIS STRING EVER!!!!
#else
  const char ESP_WM_LITE_HTML_HEAD_END[]   PROGMEM =
  "</head><div style='text-align:left;display:inline-block;min-width:260px;'>\
  <fieldset><div><label>*WiFi SSID</label><div>[[input_id]]</div></div>\
  <div><label>*PWD (8+ chars)</label><input value='[[pw]]' id='pw'><div></div></div>\
  <div><label>*WiFi SSID1</label><div>[[input_id1]]</div></div>\
  <div><label>*PWD1 (8+ chars)</label><input value='[[pw1]]' id='pw1'><div></div></div></fieldset>";  // DO NOT CHANGE THIS STRING EVER!!!!
#endif

const char ESP_WM_LITE_HTML_INPUT_ID[]   PROGMEM = "<input value='[[id]]' id='id'>";
const char ESP_WM_LITE_HTML_INPUT_ID1[]  PROGMEM = "<input value='[[id1]]' id='id1'>";

const char ESP_WM_LITE_FLDSET_START[]  PROGMEM = "<fieldset>";
const char ESP_WM_LITE_FLDSET_END[]    PROGMEM = "</fieldset>";
const char ESP_WM_LITE_HTML_PARAM[]    PROGMEM =
  "<div><label>{b}</label><input value='[[{v}]]'id='{i}'><div></div></div>";
const char ESP_WM_LITE_HTML_BUTTON[]   PROGMEM = "<button onclick=\"sv()\">Save</button></div>";

#if USING_BOARD_NAME
  const char ESP_WM_LITE_HTML_SCRIPT[]   PROGMEM = "<script id=\"jsbin-javascript\">\
  function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);\
  request.open('GET',url,false);request.send(null);}\
  function sv(){udVal('id',document.getElementById('id').value);udVal('pw',document.getElementById('pw').value);\
  udVal('id1',document.getElementById('id1').value);udVal('pw1',document.getElementById('pw1').value);\
  udVal('nm',document.getElementById('nm').value);";
#else
  const char ESP_WM_LITE_HTML_SCRIPT[]   PROGMEM = "<script id=\"jsbin-javascript\">\
  function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);\
  request.open('GET',url,false);request.send(null);}\
  function sv(){udVal('id',document.getElementById('id').value);udVal('pw',document.getElementById('pw').value);\
  udVal('id1',document.getElementById('id1').value);udVal('pw1',document.getElementById('pw1').value);";
#endif

const char ESP_WM_LITE_HTML_SCRIPT_ITEM[]  PROGMEM = "udVal('{d}',document.getElementById('{d}').value);";
const char ESP_WM_LITE_HTML_SCRIPT_END[]   PROGMEM = "alert('Updated');}</script>";
const char ESP_WM_LITE_HTML_END[]          PROGMEM = "</html>";

#if SCAN_WIFI_NETWORKS
  const char ESP_WM_LITE_SELECT_START[]      PROGMEM = "<select id=";
  const char ESP_WM_LITE_SELECT_END[]        PROGMEM = "</select>";
  const char ESP_WM_LITE_DATALIST_START[]    PROGMEM = "<datalist id=";
  const char ESP_WM_LITE_DATALIST_END[]      PROGMEM = "</datalist>";
  const char ESP_WM_LITE_OPTION_START[]      PROGMEM = "<option>";
  const char ESP_WM_LITE_OPTION_END[]        PROGMEM = "";      // "</option>"; is not required
  const char ESP_WM_LITE_NO_NETWORKS_FOUND[] PROGMEM = "No suitable WiFi networks available!";
#endif

//////////////////////////////////////////

//KH Add repeatedly used const

const char WM_HTTP_HEAD_CL[]         PROGMEM = "Content-Length";
const char WM_HTTP_HEAD_TEXT_HTML[]  PROGMEM = "text/html";
const char WM_HTTP_HEAD_TEXT_PLAIN[] PROGMEM = "text/plain";

const char WM_HTTP_CACHE_CONTROL[]   PROGMEM = "Cache-Control";
const char WM_HTTP_NO_STORE[]        PROGMEM = "no-cache, no-store, must-revalidate";
const char WM_HTTP_PRAGMA[]          PROGMEM = "Pragma";
const char WM_HTTP_NO_CACHE[]        PROGMEM = "no-cache";
const char WM_HTTP_EXPIRES[]         PROGMEM = "Expires";
const char WM_HTTP_CORS[]            PROGMEM = "Access-Control-Allow-Origin";
const char WM_HTTP_CORS_ALLOW_ALL[]  PROGMEM = "*";

//////////////////////////////////////////

#if (ESP32)

uint32_t getChipID()
{
  uint64_t chipId64 = 0;

  for (int i = 0; i < 6; i++)
  {
    chipId64 |= ( ( (uint64_t) ESP.getEfuseMac() >> (40 - (i * 8)) ) & 0xff ) << (i * 8);
  }

  return (uint32_t) (chipId64 & 0xFFFFFF);
}

//////////////////////////////////////////

uint32_t getChipOUI()
{
  uint64_t chipId64 = 0;

  for (int i = 0; i < 6; i++)
  {
    chipId64 |= ( ( (uint64_t) ESP.getEfuseMac() >> (40 - (i * 8)) ) & 0xff ) << (i * 8);
  }

  return (uint32_t) (chipId64 >> 24);
}

#endif

//////////////////////////////////////////

String IPAddressToString(const IPAddress& _address)
{
  String str = String(_address[0]);
  str += ".";
  str += String(_address[1]);
  str += ".";
  str += String(_address[2]);
  str += ".";
  str += String(_address[3]);
  return str;
}

//////////////////////////////////////////

class ESP_WiFiManager_Lite
{
  public:

    ESP_WiFiManager_Lite()
    {

    }

//////////////////////////////////////////

    ~ESP_WiFiManager_Lite()
    {
      if (dnsServer)
      {
        delete dnsServer;
      }

      if (server)
      {
        delete server;

#if SCAN_WIFI_NETWORKS

        if (indices)
        {
          free(indices); //indices array no longer required so free memory
        }

#endif
      }
    }

//////////////////////////////////////////

    void connectWiFi(const char* ssid, const char* pass)
    {
      ESP_WML_LOGINFO1(F("Con2:"), ssid);
      WiFi.mode(WIFI_STA);

      if (static_IP != IPAddress(0, 0, 0, 0))
      {
        ESP_WML_LOGINFO(F("UseStatIP"));
        WiFi.config(static_IP, static_GW, static_SN, static_DNS1, static_DNS2);
      }

      setHostname();

      if (WiFi.status() != WL_CONNECTED)
      {
        if (pass && strlen(pass))
        {
          WiFi.begin(ssid, pass);
        }
        else
        {
          WiFi.begin(ssid);
        }
      }

      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
      }

      ESP_WML_LOGINFO(F("Conn2WiFi"));
      displayWiFiData();
    }

//////////////////////////////////////////

    void begin(const char* ssid,
               const char* pass )
    {
      ESP_WML_LOGERROR(F("conW"));
      connectWiFi(ssid, pass);
    }

//////////////////////////////////////////

#if !defined(USE_LED_BUILTIN)
  #define USE_LED_BUILTIN     true      // use builtin LED to show configuration mode
#endif

#if ESP8266

  // For ESP8266
  #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #endif

  #define LED_ON      LOW
  #define LED_OFF     HIGH

#else

  // For ESP32
  #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #endif

  #define LED_OFF     LOW
  #define LED_ON      HIGH

#endif

///////////////////////////////////////////

#if !defined(REQUIRE_ONE_SET_SSID_PW)
  #define REQUIRE_ONE_SET_SSID_PW     false
#endif

#define PASSWORD_MIN_LEN        8

    //////////////////////////////////////////

    void begin(const char *iHostname = "")
    {
#define TIMEOUT_CONNECT_WIFI      30000

#if USE_LED_BUILTIN
      // Turn OFF
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LED_OFF);
#endif

#if USING_MRD
      //// New MRD ////
      mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
      bool noConfigPortal = true;

      if (mrd->detectMultiReset())
#else
      //// New DRD ////
      drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

      bool noConfigPortal = true;

      if (drd->detectDoubleReset())
#endif
      {
        ESP_WML_LOGINFO(F("Multi or Double Reset Detected"));
        noConfigPortal = false;
      }

      //// New DRD/MRD ////

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        ESP_WML_LOGDEBUG(F("======= Start Default Config Data ======="));
        displayConfigData(defaultConfig);
      }

      WiFi.mode(WIFI_STA);

      if (iHostname[0] == 0)
      {
        String _hostname = "ESP_" + String(ESP_getChipId(), HEX);
        _hostname.toUpperCase();

        getRFC952_hostname(_hostname.c_str());
      }
      else
      {
        // Prepare and store the hostname only not NULL
        getRFC952_hostname(iHostname);
      }

      ESP_WML_LOGINFO1(F("Hostname="), RFC952_hostname);

      hadConfigData = getConfigData();

      isForcedConfigPortal = isForcedCP();

      //// New DRD/MRD ////
      //  noConfigPortal when getConfigData() OK and no MRD/DRD'ed
      if (hadConfigData && noConfigPortal && (!isForcedConfigPortal) )
      {
        hadConfigData = true;

        ESP_WML_LOGDEBUG(noConfigPortal ? F("bg: noConfigPortal = true") : F("bg: noConfigPortal = false"));

        for (uint16_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
        {
          if ( strlen(ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw) >= PASSWORD_MIN_LEN )
          {
            ESP_WML_LOGDEBUG5(F("bg: addAP : index="), i, F(", SSID="), ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid, F(", PWD="),
                              ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw);
            wifiMulti.addAP(ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid, ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw);
          }
          else
          {
            ESP_WML_LOGWARN3(F("bg: Ignore invalid WiFi PWD : index="), i, F(", PWD="), ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw);
          }
        }

        if (connectMultiWiFi() == WL_CONNECTED)
        {
          ESP_WML_LOGINFO(F("bg: WiFi OK."));
        }
        else
        {
          ESP_WML_LOGINFO(F("bg: Fail2connect WiFi"));
          // failed to connect to WiFi, will start configuration mode
          startConfigurationMode();
        }
      }
      else
      {
        ESP_WML_LOGDEBUG(isForcedConfigPortal ? F("bg: isForcedConfigPortal = true") : F("bg: isForcedConfigPortal = false"));

        // If not persistent => clear the flag so that after reset. no more CP, even CP not entered and saved
        if (persForcedConfigPortal)
        {
          ESP_WML_LOGINFO1(F("bg:Stay forever in CP:"),
                           isForcedConfigPortal ? F("Forced-Persistent") : (noConfigPortal ? F("No ConfigDat") : F("DRD/MRD")));
        }
        else
        {
          ESP_WML_LOGINFO1(F("bg:Stay forever in CP:"),
                           isForcedConfigPortal ? F("Forced-non-Persistent") : (noConfigPortal ? F("No ConfigDat") : F("DRD/MRD")));
          clearForcedCP();

        }

        hadConfigData = isForcedConfigPortal ? true : (noConfigPortal ? false : true);

        // failed to connect to WiFi, will start configuration mode
        startConfigurationMode();
      }
    }

    //////////////////////////////////////////

#ifndef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   10000L
#else
    // Force range of user-defined TIMEOUT_RECONNECT_WIFI between 10-60s
#if (TIMEOUT_RECONNECT_WIFI < 10000L)
#warning TIMEOUT_RECONNECT_WIFI too low. Reseting to 10000
#undef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   10000L
#elif (TIMEOUT_RECONNECT_WIFI > 60000L)
#warning TIMEOUT_RECONNECT_WIFI too high. Reseting to 60000
#undef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   60000L
#endif
#endif

#ifndef RETRY_TIMES_RECONNECT_WIFI
#define RETRY_TIMES_RECONNECT_WIFI   2
#else
    // Force range of user-defined RETRY_TIMES_RECONNECT_WIFI between 2-5 times
#if (RETRY_TIMES_RECONNECT_WIFI < 2)
#warning RETRY_TIMES_RECONNECT_WIFI too low. Reseting to 2
#undef RETRY_TIMES_RECONNECT_WIFI
#define RETRY_TIMES_RECONNECT_WIFI   2
#elif (RETRY_TIMES_RECONNECT_WIFI > 5)
#warning RETRY_TIMES_RECONNECT_WIFI too high. Reseting to 5
#undef RETRY_TIMES_RECONNECT_WIFI
#define RETRY_TIMES_RECONNECT_WIFI   5
#endif
#endif

#ifndef RESET_IF_CONFIG_TIMEOUT
#define RESET_IF_CONFIG_TIMEOUT   true
#endif

#ifndef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET          10
#else
    // Force range of user-defined TIMES_BEFORE_RESET between 2-100
#if (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET < 2)
#warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too low. Reseting to 2
#undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   2
#elif (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET > 100)
#warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too high. Reseting to 100
#undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   100
#endif
#endif

    //////////////////////////////////////////

#if !defined(WIFI_RECON_INTERVAL)
#define WIFI_RECON_INTERVAL       0         // default 0s between reconnecting WiFi
#else
#if (WIFI_RECON_INTERVAL < 0)
#define WIFI_RECON_INTERVAL     0
#elif  (WIFI_RECON_INTERVAL > 600000)
#define WIFI_RECON_INTERVAL     600000    // Max 10min
#endif
#endif

    //////////////////////////////////////////

    void run()
    {
      static int retryTimes = 0;

      static bool wifiDisconnectedOnce = false;

      // Lost connection in running. Give chance to reconfig.
      // Check WiFi status every 5s and update status
      // Check twice to be sure wifi disconnected is real
      static unsigned long checkstatus_timeout = 0;
#define WIFI_STATUS_CHECK_INTERVAL    5000L

      static uint32_t curMillis;

      curMillis = millis();

#if USING_MRD
      //// New MRD ////
      // Call the multi reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call mrd.stop() when you wish to no longer
      // consider the next reset as a multi reset.
      mrd->loop();
      //// New MRD ////
#else
      //// New DRD ////
      // Call the double reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call drd.stop() when you wish to no longer
      // consider the next reset as a double reset.
      drd->loop();
      //// New DRD ////
#endif

      if (configuration_mode && dnsServer)
      {
        dnsServer->processNextRequest();
      }

      if ( !configuration_mode && (curMillis > checkstatus_timeout) )
      {
        if (WiFi.status() == WL_CONNECTED)
        {
          wifi_connected = true;
        }
        else
        {
          if (wifiDisconnectedOnce)
          {
            wifiDisconnectedOnce = false;
            wifi_connected = false;
            ESP_WML_LOGERROR(F("r:Check&WLost"));
          }
          else
          {
            wifiDisconnectedOnce = true;
          }
        }

        checkstatus_timeout = curMillis + WIFI_STATUS_CHECK_INTERVAL;
      }

      // Lost connection in running. Give chance to reconfig.
      if ( WiFi.status() != WL_CONNECTED )
      {
        // If configTimeout but user hasn't connected to configWeb => try to reconnect WiFi
        // But if user has connected to configWeb, stay there until done, then reset hardware
        if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
        {
          retryTimes = 0;

          if (server)
          {
            server->handleClient();

            // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
            if ( String(ARDUINO_BOARD) == "ESP32S2_DEV" )
            {
              delay(1);
            }
          }

          return;
        }
        else
        {
#if RESET_IF_CONFIG_TIMEOUT

          // If we're here but still in configuration_mode, permit running TIMES_BEFORE_RESET times before reset hardware
          // to permit user another chance to config.
          if ( configuration_mode && (configTimeout != 0) )
          {
            ESP_WML_LOGDEBUG(F("r:Check RESET_IF_CONFIG_TIMEOUT"));

            if (++retryTimes <= CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET)
            {
              ESP_WML_LOGINFO1(F("run: WiFi lost, configTimeout. Connect WiFi. Retry#:"), retryTimes);
            }
            else
            {
              resetFunc();
            }
          }

#endif

          // Not in config mode, try reconnecting before forcing to config mode
          if ( WiFi.status() != WL_CONNECTED )
          {
#if (WIFI_RECON_INTERVAL > 0)

            static uint32_t lastMillis = 0;

            if ( (lastMillis == 0) || (curMillis - lastMillis) > WIFI_RECON_INTERVAL )
            {
              lastMillis = curMillis;

              ESP_WML_LOGERROR(F("r:WLost.ReconW"));

              if (connectMultiWiFi() == WL_CONNECTED)
              {
#if USE_LED_BUILTIN
                // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
                digitalWrite(LED_BUILTIN, LED_OFF);
#endif
                ESP_WML_LOGINFO(F("run: WiFi reconnected"));
              }
            }

#else
            ESP_WML_LOGINFO(F("run: WiFi lost. Reconnect WiFi"));

            if (connectMultiWiFi() == WL_CONNECTED)
            {
#if USE_LED_BUILTIN
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);
#endif
              ESP_WML_LOGINFO(F("run: WiFi reconnected"));
            }

#endif
          }

          //ESP_WML_LOGINFO(F("run: Lost connection => configMode"));
          //startConfigurationMode();
        }
      }
      else if (configuration_mode)
      {
        // WiFi is connected and we are in configuration_mode
        configuration_mode = false;
        ESP_WML_LOGINFO(F("run: got WiFi back"));

#if USE_LED_BUILTIN
        // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
        digitalWrite(LED_BUILTIN, LED_OFF);
#endif

        if (dnsServer)
        {
          dnsServer->stop();
          delete dnsServer;
          dnsServer = nullptr;
        }

        if (server)
        {
          server->stop();
          delete server;
          server = nullptr;
        }
      }
    }

    //////////////////////////////////////////////

    void setHostname()
    {
      if (RFC952_hostname[0] != 0)
      {
#if ESP8266
        WiFi.hostname(RFC952_hostname);
#else


        // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
#if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
        WiFi.setHostname(RFC952_hostname);
#else

        // Still have bug in ESP32_S2 for old core. If using WiFi.setHostname() => WiFi.localIP() always = 255.255.255.255
        if ( String(ARDUINO_BOARD) != "ESP32S2_DEV" )
        {
          // See https://github.com/espressif/arduino-esp32/issues/2537
          WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
          WiFi.setHostname(RFC952_hostname);
        }

#endif
#endif
      }
    }

    //////////////////////////////////////////////

    void setConfigPortalIP(const IPAddress& portalIP = IPAddress(192, 168, 4, 1))
    {
      portal_apIP = portalIP;
    }

    //////////////////////////////////////////////

    void setConfigPortal(const String& ssid = "", const String& pass = "")
    {
      portal_ssid = ssid;
      portal_pass = pass;
    }

    //////////////////////////////////////////////

#define MIN_WIFI_CHANNEL      1
#define MAX_WIFI_CHANNEL      11    // Channel 13 is flaky, because of bad number 13 ;-)

    int setConfigPortalChannel(const int& channel = 1)
    {
      // If channel < MIN_WIFI_CHANNEL - 1 or channel > MAX_WIFI_CHANNEL => channel = 1
      // If channel == 0 => will use random channel from MIN_WIFI_CHANNEL to MAX_WIFI_CHANNEL
      // If (MIN_WIFI_CHANNEL <= channel <= MAX_WIFI_CHANNEL) => use it
      if ( (channel < MIN_WIFI_CHANNEL - 1) || (channel > MAX_WIFI_CHANNEL) )
        WiFiAPChannel = 1;
      else if ( (channel >= MIN_WIFI_CHANNEL - 1) && (channel <= MAX_WIFI_CHANNEL) )
        WiFiAPChannel = channel;

      return WiFiAPChannel;
    }

    //////////////////////////////////////////////

    void setSTAStaticIPConfig(const IPAddress& ip, const IPAddress& gw,
                              const IPAddress& sn = IPAddress(255, 255, 255, 0),
                              const IPAddress& dns_address_1 = IPAddress(0, 0, 0, 0),
                              const IPAddress& dns_address_2 = IPAddress(0, 0, 0, 0))
    {
      static_IP     = ip;
      static_GW     = gw;
      static_SN     = sn;

      // Default to local GW
      if (dns_address_1 == IPAddress(0, 0, 0, 0))
        static_DNS1   = gw;
      else
        static_DNS1   = dns_address_1;

      // Default to Google DNS (8, 8, 8, 8)
      if (dns_address_2 == IPAddress(0, 0, 0, 0))
        static_DNS2   = IPAddress(8, 8, 8, 8);
      else
        static_DNS2   = dns_address_2;
    }

    //////////////////////////////////////////////

    String getWiFiSSID(const uint8_t& index)
    {
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(ESP_WM_LITE_config.WiFi_Creds[index].wifi_ssid));
    }

    //////////////////////////////////////////////

    String getWiFiPW(const uint8_t& index)
    {
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(ESP_WM_LITE_config.WiFi_Creds[index].wifi_pw));
    }

    //////////////////////////////////////////////

    String getBoardName()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(ESP_WM_LITE_config.board_name));
    }

    //////////////////////////////////////////////

    bool getWiFiStatus()
    {
      return wifi_connected;
    }

    //////////////////////////////////////////////

    ESP_WM_LITE_Configuration* getFullConfigData(ESP_WM_LITE_Configuration *configData)
    {
      if (!hadConfigData)
        getConfigData();

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_Configuration));

      return (configData);
    }

    //////////////////////////////////////////////

    String localIP()
    {
      ipAddress = IPAddressToString(WiFi.localIP());

      return ipAddress;
    }

    //////////////////////////////////////////////

    void clearConfigData()
    {
      memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

#if USE_DYNAMIC_PARAMETERS

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
      }

#endif

      saveConfigData();
    }

    //////////////////////////////////////////////

    bool isConfigDataValid()
    {
      return hadConfigData;
    }

    //////////////////////////////////////////////

    bool isConfigMode()
    {
      return configuration_mode;
    }

    //////////////////////////////////////////////

    // Forced CP => Flag = 0xBEEFBEEF. Else => No forced CP
    // Flag to be stored at (EEPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
    // to avoid corruption to current data
    const uint32_t FORCED_CONFIG_PORTAL_FLAG_DATA       = 0xDEADBEEF;
    const uint32_t FORCED_PERS_CONFIG_PORTAL_FLAG_DATA  = 0xBEEFDEAD;

#define FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE     4

    void resetAndEnterConfigPortal()
    {
      persForcedConfigPortal = false;

      setForcedCP(false);

      // Delay then reset the ESP8266 after save data
      resetFunc();
    }

    //////////////////////////////////////////////

    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    void resetAndEnterConfigPortalPersistent()
    {
      persForcedConfigPortal = true;

      setForcedCP(true);

      // Delay then reset the ESP8266 after save data
      resetFunc();
    }

    //////////////////////////////////////////////

    void resetFunc()
    {
      delay(1000);

#if ESP8266
      ESP.reset();
#else
      ESP.restart();
#endif
    }

    //////////////////////////////////////

    // Add customs headers from v1.2.0

    // New from v1.2.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"

#if USING_CUSTOMS_STYLE
    //sets a custom style, such as color
    // "<style>div,input{padding:5px;font-size:1em;}
    // input{width:95%;}body{text-align: center;}
    // button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}
    // fieldset{border-radius:0.3rem;margin:0px;}</style>";
    void setCustomsStyle(PGM_P CustomsStyle = ESP_WM_LITE_HTML_HEAD_STYLE)
    {
      _CustomsHeadStyle = CustomsStyle;
      ESP_WML_LOGDEBUG1(F("Set CustomsStyle to : "), FPSTR(_CustomsHeadStyle));
    }

    //////////////////////////////////////

    PGM_P getCustomsStyle()
    {
      ESP_WML_LOGDEBUG1(F("Get CustomsStyle = "), FPSTR(_CustomsHeadStyle));
      return _CustomsHeadStyle;
    }
#endif

    //////////////////////////////////////

#if USING_CUSTOMS_HEAD_ELEMENT
    //sets a custom element to add to head, like a new style tag
    void setCustomsHeadElement(PGM_P CustomsHeadElement = NULL)
    {
      _CustomsHeadElement = CustomsHeadElement;
      ESP_WML_LOGDEBUG1(F("Set CustomsHeadElement to : "), _CustomsHeadElement);
    }

    //////////////////////////////////////

    PGM_P getCustomsHeadElement()
    {
      ESP_WML_LOGDEBUG1(F("Get CustomsHeadElement = "), _CustomsHeadElement);
      return _CustomsHeadElement;
    }
#endif

    //////////////////////////////////////

#if USING_CORS_FEATURE
    void setCORSHeader(PGM_P CORSHeaders = NULL)
    {
      _CORS_Header = CORSHeaders;
      ESP_WML_LOGDEBUG1(F("Set CORS Header to : "), _CORS_Header);
    }

    //////////////////////////////////////

    PGM_P getCORSHeader()
    {
      ESP_WML_LOGDEBUG1(F("Get CORS Header = "), _CORS_Header);
      return _CORS_Header;
    }
#endif

    //////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS

    bool extLoadDynamicData()
    {
#if ( USE_LITTLEFS || USE_SPIFFS )

  #if ESP8266
      // SPIFFS and LittleFS do auto-format if not yet
      if (!FileFS.begin())
  #else
      // Format SPIFFS if not yet
      if (!FileFS.begin(true))
  #endif
      {
        ESP_WML_LOGERROR(F("SPIFFS/LittleFS failed!"));
        return false;
      }

      return loadDynamicData();

#else   // #if ( USE_LITTLEFS || USE_SPIFFS )

      return EEPROM_getDynamicData();

#endif   // #if ( USE_LITTLEFS || USE_SPIFFS )
    }

    //////////////////////////////////////////////

    void extSaveDynamicData()
    {
#if ( USE_LITTLEFS || USE_SPIFFS )

  #if ESP8266
      // SPIFFS and LittleFS do auto-format if not yet
      if (!FileFS.begin())
  #else
      // Format SPIFFS if not yet
      if (!FileFS.begin(true))
  #endif
      {
        ESP_WML_LOGERROR(F("SPIFFS/LittleFS failed!"));
        return;
      }

      saveDynamicData();

#else   // #if ( USE_LITTLEFS || USE_SPIFFS )

      EEPROM_putDynamicData();

#endif   // #if ( USE_LITTLEFS || USE_SPIFFS )
    }

#endif

    //////////////////////////////////////


  private:
    String ipAddress = "0.0.0.0";

#ifdef ESP8266
    ESP8266WebServer *server = nullptr;
    ESP8266WiFiMulti wifiMulti;
#else   //ESP32
    WebServer *server = nullptr;
    WiFiMulti wifiMulti;
#endif

    DNSServer *dnsServer = nullptr;

    bool configuration_mode = false;

    unsigned long configTimeout;
    bool hadConfigData = false;
    bool hadDynamicData = false;

    bool isForcedConfigPortal   = false;
    bool persForcedConfigPortal = false;

    ESP_WM_LITE_Configuration ESP_WM_LITE_config;

    uint16_t totalDataSize = 0;

    String macAddress = "";
    bool wifi_connected = false;

    IPAddress portal_apIP = IPAddress(192, 168, 4, 1);
    int WiFiAPChannel = 1;

    String portal_ssid = "";
    String portal_pass = "";

    IPAddress static_IP   = IPAddress(0, 0, 0, 0);
    IPAddress static_GW   = IPAddress(0, 0, 0, 0);
    IPAddress static_SN   = IPAddress(255, 255, 255, 0);
    IPAddress static_DNS1 = IPAddress(0, 0, 0, 0);
    IPAddress static_DNS2 = IPAddress(0, 0, 0, 0);

    /////////////////////////////////////

    // Add customs headers from v1.2.0

#if USING_CUSTOMS_STYLE
    PGM_P _CustomsHeadStyle = nullptr;
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
    PGM_P _CustomsHeadElement = nullptr;
#endif

#if USING_CORS_FEATURE
    PGM_P _CORS_Header = WM_HTTP_CORS_ALLOW_ALL;   // "*";
#endif

    //////////////////////////////////////
    // Add WiFi Scan from v1.5.0

#if SCAN_WIFI_NETWORKS
    int WiFiNetworksFound = 0;    // Number of SSIDs found by WiFi scan, including low quality and duplicates
    int *indices;                 // WiFi network data, filled by scan (SSID, BSSID)
    String ListOfSSIDs = "";      // List of SSIDs found by scan, in HTML <option> format
#endif

    //////////////////////////////////////

#define RFC952_HOSTNAME_MAXLEN      24

    char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

    char* getRFC952_hostname(const char* iHostname)
    {
      memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

      size_t len = ( RFC952_HOSTNAME_MAXLEN < strlen(iHostname) ) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

      size_t j = 0;

      for (size_t i = 0; i < len - 1; i++)
      {
        if ( isalnum(iHostname[i]) || iHostname[i] == '-' )
        {
          RFC952_hostname[j] = iHostname[i];
          j++;
        }
      }

      // no '-' as last char
      if ( isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-') )
        RFC952_hostname[j] = iHostname[len - 1];

      return RFC952_hostname;
    }

    //////////////////////////////////////

    void displayConfigData(const ESP_WM_LITE_Configuration& configData)
    {
      ESP_WML_LOGERROR5(F("Hdr="),   configData.header, F(",SSID="), configData.WiFi_Creds[0].wifi_ssid,
                        F(",PW="),   configData.WiFi_Creds[0].wifi_pw);
      ESP_WML_LOGERROR3(F("SSID1="), configData.WiFi_Creds[1].wifi_ssid, F(",PW1="),  configData.WiFi_Creds[1].wifi_pw);
      ESP_WML_LOGERROR1(F("BName="), configData.board_name);

#if USE_DYNAMIC_PARAMETERS

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        ESP_WML_LOGINFO5("i=", i, ",id=", myMenuItems[i].id, ",data=", myMenuItems[i].pdata);
      }

#endif
    }

    //////////////////////////////////////

    void displayWiFiData()
    {
      ESP_WML_LOGERROR3(F("SSID="), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
      ESP_WML_LOGERROR1(F("IP="), WiFi.localIP() );
    }

    //////////////////////////////////////

#define ESP_WM_LITE_BOARD_TYPE   "ESP_WM_LITE"
#define WM_NO_CONFIG             "blank"

    int calcChecksum()
    {
      int checkSum = 0;

      for (uint16_t index = 0; index < (sizeof(ESP_WM_LITE_config) - sizeof(ESP_WM_LITE_config.checkSum)); index++)
      {
        checkSum += * ( ( (byte*) &ESP_WM_LITE_config ) + index);
      }

      return checkSum;
    }

    //////////////////////////////////////////////

    bool isWiFiConfigValid()
    {
#if REQUIRE_ONE_SET_SSID_PW

      // If SSID ="blank" or NULL, or PWD length < 8 (as required by standard) => return false
      // Only need 1 set of valid SSID/PWD
      if (!( ( ( strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid, WM_NO_CONFIG, strlen(WM_NO_CONFIG))
                 && strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) >  0 )  &&
               (   strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw) >= PASSWORD_MIN_LEN ) ) ||
             ( ( strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid, WM_NO_CONFIG, strlen(WM_NO_CONFIG))
                 && strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) >  0 )  &&
               ( strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw) >= PASSWORD_MIN_LEN ) ) ))
#else

      // If SSID ="blank" or NULL, or PWD length < 8 (as required by standard) => invalid set
      // Need both sets of valid SSID/PWD
      if ( !strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
           !strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
           !strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
           !strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
           ( strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) == 0 ) ||
           ( strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) == 0 ) ||
           ( strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw)   < PASSWORD_MIN_LEN ) ||
           ( strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw)   < PASSWORD_MIN_LEN ) )
#endif
      {
        // If SSID, PW ="blank" or NULL, set the flag
        ESP_WML_LOGERROR(F("Invalid Stored WiFi Config Data"));

        // Nullify the invalid data to avoid displaying garbage
        memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

        hadConfigData = false;

        return false;
      }

      return true;
    }

    //////////////////////////////////////////////

#if ( USE_LITTLEFS || USE_SPIFFS )

    // Use LittleFS/InternalFS for nRF52
#define  CONFIG_FILENAME                  ("/wm_config.dat")
#define  CONFIG_FILENAME_BACKUP           ("/wm_config.bak")

#define  CREDENTIALS_FILENAME             ("/wm_cred.dat")
#define  CREDENTIALS_FILENAME_BACKUP      ("/wm_cred.bak")

#define  CONFIG_PORTAL_FILENAME           ("/wm_cp.dat")
#define  CONFIG_PORTAL_FILENAME_BACKUP    ("/wm_cp.bak")

    //////////////////////////////////////////////

    void saveForcedCP(const uint32_t& value)
    {
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "w");

      ESP_WML_LOGINFO(F("SaveCPFile "));

      if (file)
      {
        file.write((uint8_t*) &value, sizeof(value));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }

      // Trying open redundant CP file
      file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "w");

      ESP_WML_LOGINFO(F("SaveBkUpCPFile "));

      if (file)
      {
        file.write((uint8_t *) &value, sizeof(value));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }
    }

    //////////////////////////////////////////////

    void setForcedCP(const bool& isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent ? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA :
                                            FORCED_CONFIG_PORTAL_FLAG_DATA;

      ESP_WML_LOGDEBUG(isPersistent ? F("setForcedCP Persistent") : F("setForcedCP non-Persistent"));

      saveForcedCP(readForcedConfigPortalFlag);
    }

    //////////////////////////////////////////////

    void clearForcedCP()
    {
      uint32_t readForcedConfigPortalFlag = 0;

      ESP_WML_LOGDEBUG(F("clearForcedCP"));

      saveForcedCP(readForcedConfigPortalFlag);
    }

    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

      ESP_WML_LOGDEBUG(F("Check if isForcedCP"));

      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCPFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCPFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
      }

      file.readBytes((char *) &readForcedConfigPortalFlag, sizeof(readForcedConfigPortalFlag));

      ESP_WML_LOGINFO(F("OK"));
      file.close();

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {
        persForcedConfigPortal = true;
        return true;
      }
      else
      {
        return false;
      }
    }

    //////////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS

    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      char* readBuffer;

      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCredFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCredFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
      }

      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data

      uint16_t maxBufferLength = 0;

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        if (myMenuItems[i].maxlen > maxBufferLength)
          maxBufferLength = myMenuItems[i].maxlen;
      }

      if (maxBufferLength > 0)
      {
        readBuffer = new char[ maxBufferLength + 1 ];

        // check to see NULL => stop and return false
        if (readBuffer == NULL)
        {
          ESP_WML_LOGERROR(F("ChkCrR: Error can't allocate buffer."));
          return false;
        }
        else
        {
          ESP_WML_LOGDEBUG1(F("ChkCrR: Buffer allocated, sz="), maxBufferLength + 1);
        }

        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          char* _pointer = readBuffer;
  
          // Actual size of pdata is [maxlen + 1]
          memset(readBuffer, 0, myMenuItems[i].maxlen + 1);
  
          file.readBytes(_pointer, myMenuItems[i].maxlen);
  
          ESP_WML_LOGDEBUG3(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
  
          for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++)
          {
            checkSum += *_pointer;
          }
        }
  
        file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
  
        ESP_WML_LOGINFO(F("OK"));
        file.close();
  
        ESP_WML_LOGINFO3(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
  
        // Free buffer
        delete [] readBuffer;
        ESP_WML_LOGDEBUG(F("Buffer freed"));
  
        if ( checkSum == readCheckSum)
        {
          return true;
        }
      }

      return false;
    }

    //////////////////////////////////////////////

    bool loadDynamicData()
    {
      if (hadDynamicData)
      {
        return true;
      }

      int checkSum = 0;
      int readCheckSum;
      totalDataSize = sizeof(ESP_WM_LITE_config) + sizeof(readCheckSum);

      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCredFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCredFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
      }

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;

        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

        file.readBytes(_pointer, myMenuItems[i].maxlen);

        ESP_WML_LOGDEBUG3(F("CrR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);

        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++)
        {
          checkSum += *_pointer;
        }
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));

      ESP_WML_LOGINFO(F("OK"));
      file.close();

      ESP_WML_LOGINFO3(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));

      if ( checkSum != readCheckSum)
      {
        return false;
      }

      hadDynamicData = true;
      return true;
    }

    //////////////////////////////////////////////

    void saveDynamicData()
    {
      int checkSum = 0;

      File file = FileFS.open(CREDENTIALS_FILENAME, "w");
      ESP_WML_LOGINFO(F("SaveCredFile "));

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        char* _pointer = myMenuItems[i].pdata;

        ESP_WML_LOGDEBUG3(F("CW1:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);

        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);
        }
        else
        {
          ESP_WML_LOGINFO(F("failed"));
        }

        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++)
        {
          checkSum += *_pointer;
        }
      }

      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }

      ESP_WML_LOGINFO1(F("CrWCSum=0x"), String(checkSum, HEX));

      // Trying open redundant Auth file
      file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "w");
      ESP_WML_LOGINFO(F("SaveBkUpCredFile "));

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        char* _pointer = myMenuItems[i].pdata;

        ESP_WML_LOGDEBUG3(F("CW2:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);

        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);
        }
        else
        {
          ESP_WML_LOGINFO(F("failed"));
        }

        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++)
        {
          checkSum += *_pointer;
        }
      }

      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }
    }
#endif

    //////////////////////////////////////////////

    void NULLTerminateConfig()
    {
      //#define HEADER_MAX_LEN      16
      //#define SERVER_MAX_LEN      32
      //#define TOKEN_MAX_LEN       36

      // NULL Terminating to be sure
      ESP_WM_LITE_config.header[HEADER_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid[SSID_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw  [PASS_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid[SSID_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw  [PASS_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.board_name[BOARD_NAME_MAX_LEN - 1]  = 0;
    }

    //////////////////////////////////////////////

    bool loadConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCfgFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCfgFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
      }

      file.readBytes((char *) &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_config));

      ESP_WML_LOGINFO(F("OK"));
      file.close();

      return isWiFiConfigValid();
    }

    //////////////////////////////////////////////

    void saveConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "w");
      ESP_WML_LOGINFO(F("SaveCfgFile "));

      int calChecksum = calcChecksum();
      ESP_WM_LITE_config.checkSum = calChecksum;
      ESP_WML_LOGINFO1(F("WCSum=0x"), String(calChecksum, HEX));

      if (file)
      {
        file.write((uint8_t*) &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_config));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }

      // Trying open redundant Auth file
      file = FileFS.open(CONFIG_FILENAME_BACKUP, "w");
      ESP_WML_LOGINFO(F("SaveBkUpCfgFile "));

      if (file)
      {
        file.write((uint8_t *) &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_config));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }
    }

    //////////////////////////////////////////////

    void saveAllConfigData()
    {
      saveConfigData();

#if USE_DYNAMIC_PARAMETERS
      saveDynamicData();
#endif
    }

    //////////////////////////////////////////////

    void loadAndSaveDefaultConfigData()
    {
      // Load Default Config Data from Sketch
      memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
      strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

      // Including config and dynamic data, and assume valid
      saveConfigData();

      ESP_WML_LOGERROR(F("======= Start Loaded Config Data ======="));
      displayConfigData(ESP_WM_LITE_config);
    }

    //////////////////////////////////////////////

    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid = true;
      int calChecksum;

      hadConfigData = false;

#if ESP8266

      // Format SPIFFS if not yet
      if (!FileFS.begin())
      {
        FileFS.format();
#else

      // Format SPIFFS if not yet
      if (!FileFS.begin(true))
      {
        ESP_WML_LOGERROR(F("SPIFFS/LittleFS failed! Formatting."));
#endif

        if (!FileFS.begin())
        {
#if USE_LITTLEFS
          ESP_WML_LOGERROR(F("LittleFS failed!. Please use SPIFFS or EEPROM."));
#else
          ESP_WML_LOGERROR(F("SPIFFS failed!. Please use LittleFS or EEPROM."));
#endif
          return false;
        }
      }

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

        // Including config and dynamic data, and assume valid
        saveAllConfigData();

        ESP_WML_LOGINFO(F("======= Start Loaded Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        // Don't need Config Portal anymore
        return true;
      }

#if USE_DYNAMIC_PARAMETERS
      else if ( ( FileFS.exists(CONFIG_FILENAME)      || FileFS.exists(CONFIG_FILENAME_BACKUP) ) &&
                ( FileFS.exists(CREDENTIALS_FILENAME) || FileFS.exists(CREDENTIALS_FILENAME_BACKUP) ) )
#else
      else if ( FileFS.exists(CONFIG_FILENAME) || FileFS.exists(CONFIG_FILENAME_BACKUP) )
#endif
      {
        // Load stored config data from LittleFS
        // Get config data. If "blank" or NULL, set false flag and exit
        if (!loadConfigData())
        {
          return false;
        }

        ESP_WML_LOGINFO(F("======= Start Stored Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        calChecksum = calcChecksum();

        ESP_WML_LOGINFO3(F("CCSum=0x"), String(calChecksum, HEX),
                         F(",RCSum=0x"), String(ESP_WM_LITE_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS
        // Load dynamic data
        dynamicDataValid = loadDynamicData();

        if (dynamicDataValid)
        {
          ESP_WML_LOGINFO(F("Valid Stored Dynamic Data"));
        }
        else
        {
          ESP_WML_LOGINFO(F("Invalid Stored Dynamic Data. Ignored"));
        }

#endif
      }
      else
      {
        // Not loading Default config data, but having no config file => Config Portal
        return false;
      }

      if ( (strncmp(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE, strlen(ESP_WM_LITE_BOARD_TYPE)) != 0) ||
           (calChecksum != ESP_WM_LITE_config.checkSum) || !dynamicDataValid )

      {
        // Including Credentials CSum
        ESP_WML_LOGINFO1(F("InitCfgFile,sz="), sizeof(ESP_WM_LITE_config));

        // doesn't have any configuration
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        }
        else
        {
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.board_name, WM_NO_CONFIG);

#if USE_DYNAMIC_PARAMETERS

          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }

#endif
        }

        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

#if USE_DYNAMIC_PARAMETERS

        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          ESP_WML_LOGDEBUG3(F("g:myMenuItems["), i, F("]="), myMenuItems[i].pdata );
        }

#endif

        // Don't need
        ESP_WM_LITE_config.checkSum = 0;

        saveAllConfigData();

        return false;
      }
      else if ( !isWiFiConfigValid() )
      {
        // If SSID, PW ="blank" or NULL, stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(ESP_WM_LITE_config);
      }

      return true;
    }

    //////////////////////////////////////////////

#else   // #if ( USE_LITTLEFS || USE_SPIFFS )

#ifndef EEPROM_SIZE
#define EEPROM_SIZE     2048
#else
#if (EEPROM_SIZE > 2048)
#warning EEPROM_SIZE must be <= 2048. Reset to 2048
#undef EEPROM_SIZE
#define EEPROM_SIZE     2048
#elif (EEPROM_SIZE < 2048)
#warning Preset EEPROM_SIZE <= 2048. Reset to 2048
#undef EEPROM_SIZE
#define EEPROM_SIZE     2048
#endif
    // FLAG_DATA_SIZE is 4, to store DRD/MRD flag
#if (EEPROM_SIZE < FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
#warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 512
#undef EEPROM_SIZE
#define EEPROM_SIZE     2048
#endif
#endif

#ifndef EEPROM_START
#define EEPROM_START     0      //define 256 in DRD/MRD
#else
#if (EEPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE)
#error EPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE. Please adjust.
#endif
#endif

    // Stating positon to store ESP_WM_LITE_config
#define CONFIG_EEPROM_START    (EEPROM_START + FLAG_DATA_SIZE)

    //////////////////////////////////////////////

    void setForcedCP(const bool& isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent ? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA :
                                            FORCED_CONFIG_PORTAL_FLAG_DATA;

      ESP_WML_LOGINFO(F("setForcedCP"));

      EEPROM.put(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      EEPROM.commit();
    }

    //////////////////////////////////////////////

    void clearForcedCP()
    {
      ESP_WML_LOGINFO(F("clearForcedCP"));

      EEPROM.put(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, 0);
      EEPROM.commit();
    }

    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

      ESP_WML_LOGINFO(F("Check if isForcedCP"));

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      EEPROM.get(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {
        persForcedConfigPortal = true;
        return true;
      }
      else
      {
        return false;
      }
    }

    //////////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS

    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;

#define BUFFER_LEN      128
      char readBuffer[BUFFER_LEN + 1];

      uint16_t offset = CONFIG_EEPROM_START + sizeof(ESP_WM_LITE_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;

      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        if (myMenuItems[i].maxlen > BUFFER_LEN)
        {
          // Size too large, abort and flag false
          ESP_WML_LOGERROR(F("ChkCrR: Error Small Buffer."));
          return false;
        }
      }

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        char* _pointer = readBuffer;

        // Prepare buffer, more than enough
        memset(readBuffer, 0, sizeof(readBuffer));

        // Read more than necessary, but OK and easier to code
        EEPROM.get(offset, readBuffer);
        // NULL terminated
        readBuffer[myMenuItems[i].maxlen] = 0;

        ESP_WML_LOGDEBUG3(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);

        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++)
        {
          checkSum += *_pointer;
        }

        offset += myMenuItems[i].maxlen;
      }

      EEPROM.get(offset, readCheckSum);

      ESP_WML_LOGINFO3(F("ChkCrR:CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));

      if ( checkSum != readCheckSum)
      {
        return false;
      }

      return true;
    }

    //////////////////////////////////////////////

    bool EEPROM_getDynamicData()
    {
      int readCheckSum;
      int checkSum = 0;
      uint16_t offset = CONFIG_EEPROM_START + sizeof(ESP_WM_LITE_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;

      totalDataSize = sizeof(ESP_WM_LITE_config) + sizeof(readCheckSum);

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;

        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++, offset++)
        {
          *_pointer = EEPROM.read(offset);

          checkSum += *_pointer;
        }

        ESP_WML_LOGDEBUG3(F("CR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
      }

      EEPROM.get(offset, readCheckSum);

      ESP_WML_LOGINFO3(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));

      if ( checkSum != readCheckSum)
      {
        return false;
      }

      return true;
    }

    //////////////////////////////////////////////

    void EEPROM_putDynamicData()
    {
      int checkSum = 0;
      uint16_t offset = CONFIG_EEPROM_START + sizeof(ESP_WM_LITE_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        char* _pointer = myMenuItems[i].pdata;

        ESP_WML_LOGDEBUG3(F("CW:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);

        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++, _pointer++, offset++)
        {
          EEPROM.write(offset, *_pointer);

          checkSum += *_pointer;
        }
      }

      EEPROM.put(offset, checkSum);
      //EEPROM.commit();

      ESP_WML_LOGINFO1(F("CrWCSum=0x"), String(checkSum, HEX));
    }
#endif

    //////////////////////////////////////////////

    void saveConfigData()
    {
      int calChecksum = calcChecksum();
      ESP_WM_LITE_config.checkSum = calChecksum;
      ESP_WML_LOGINFO3(F("SaveEEPROM,sz="), EEPROM_SIZE, F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(CONFIG_EEPROM_START, ESP_WM_LITE_config);

      EEPROM.commit();
    }

    //////////////////////////////////////////////

    void saveAllConfigData()
    {
      int calChecksum = calcChecksum();
      ESP_WM_LITE_config.checkSum = calChecksum;
      ESP_WML_LOGINFO3(F("SaveEEPROM,sz="), EEPROM_SIZE, F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(CONFIG_EEPROM_START, ESP_WM_LITE_config);

#if USE_DYNAMIC_PARAMETERS
      EEPROM_putDynamicData();
#endif

      EEPROM.commit();
    }

    //////////////////////////////////////////////

    void loadAndSaveDefaultConfigData()
    {
      // Load Default Config Data from Sketch
      memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
      strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

      // Including config and dynamic data, and assume valid
      saveConfigData();

      ESP_WML_LOGINFO(F("======= Start Loaded Config Data ======="));
      displayConfigData(ESP_WM_LITE_config);
    }

    //////////////////////////////////////////////

    bool getConfigData()
    {
      bool dynamicDataValid = true;
      int calChecksum;

      hadConfigData = false;

      EEPROM.begin(EEPROM_SIZE);
      ESP_WML_LOGINFO1(F("EEPROMsz:"), EEPROM_SIZE);

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

        // Including config and dynamic data, and assume valid
        saveAllConfigData();

        ESP_WML_LOGINFO(F("======= Start Loaded Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        // Don't need Config Portal anymore
        return true;
      }
      else
      {
        // Load data from EEPROM
        EEPROM.get(CONFIG_EEPROM_START, ESP_WM_LITE_config);

        if ( !isWiFiConfigValid() )
        {
          // If SSID, PW ="blank" or NULL, stay in config mode forever until having config Data.
          return false;
        }

        ESP_WML_LOGINFO(F("======= Start Stored Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        calChecksum = calcChecksum();

        ESP_WML_LOGINFO3(F("CCSum=0x"), String(calChecksum, HEX),
                         F(",RCSum=0x"), String(ESP_WM_LITE_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS

        // Load dynamic data from EEPROM
        dynamicDataValid = EEPROM_getDynamicData();

        if (dynamicDataValid)
        {
          ESP_WML_LOGINFO(F("Valid Stored Dynamic Data"));
        }
        else
        {
          ESP_WML_LOGINFO(F("Invalid Stored Dynamic Data. Ignored"));
        }

#endif
      }

      if ( (strncmp(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE, strlen(ESP_WM_LITE_BOARD_TYPE)) != 0) ||
           (calChecksum != ESP_WM_LITE_config.checkSum) || !dynamicDataValid )
      {
        // Including Credentials CSum
        ESP_WML_LOGINFO3(F("InitEEPROM,sz="), EEPROM_SIZE, F(",DataSz="), totalDataSize);

        // doesn't have any configuration
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        }
        else
        {
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.board_name, WM_NO_CONFIG);

#if USE_DYNAMIC_PARAMETERS

          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }

#endif
        }

        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

#if USE_DYNAMIC_PARAMETERS

        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          ESP_WML_LOGDEBUG3(F("g:myMenuItems["), i, F("]="), myMenuItems[i].pdata );
        }

#endif

        // Don't need
        ESP_WM_LITE_config.checkSum = 0;

        saveAllConfigData();

        return false;
      }
      else if ( !isWiFiConfigValid() )
      {
        // If SSID, PW ="blank" or NULL, stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(ESP_WM_LITE_config);
      }

      return true;
    }

#endif    // #if ( USE_LITTLEFS || USE_SPIFFS )

    //////////////////////////////////////////////

    // New connectMultiWiFi() logic from v1.7.0
    // Max times to try WiFi per loop() iteration. To avoid blocking issue in loop()
    // Default 1 and minimum 1.
#if !defined(MAX_NUM_WIFI_RECON_TRIES_PER_LOOP)
#define MAX_NUM_WIFI_RECON_TRIES_PER_LOOP     1
#else
#if (MAX_NUM_WIFI_RECON_TRIES_PER_LOOP < 1)
#define MAX_NUM_WIFI_RECON_TRIES_PER_LOOP     1
#endif
#endif

    uint8_t connectMultiWiFi()
    {
#if ESP32
      // For ESP32, this better be 0 to shorten the connect time.
      // For ESP32-S2/C3, must be > 500
#if ( USING_ESP32_S2 || USING_ESP32_C3 )
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS           500L
#else
      // For ESP32 core v1.0.6, must be >= 500
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS           800L
#endif
#else
      // For ESP8266, this better be 2200 to enable connect the 1st time
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS             2200L
#endif

#define WIFI_MULTI_CONNECT_WAITING_MS                   500L

      uint8_t status;

      ESP_WML_LOGINFO(F("Connecting MultiWifi..."));

      WiFi.mode(WIFI_STA);

      setHostname();

      int i = 0;
      status = wifiMulti.run();
      delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

      uint8_t numWiFiReconTries = 0;

      while ( ( status != WL_CONNECTED ) && (numWiFiReconTries++ < MAX_NUM_WIFI_RECON_TRIES_PER_LOOP) )
      {
        status = WiFi.status();

        if ( status == WL_CONNECTED )
          break;
        else
          delay(WIFI_MULTI_CONNECT_WAITING_MS);
      }

      if ( status == WL_CONNECTED )
      {
        ESP_WML_LOGWARN1(F("WiFi connected after time: "), i);
        ESP_WML_LOGWARN3(F("SSID="), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
        ESP_WML_LOGWARN3(F("Channel="), WiFi.channel(), F(",IP="), WiFi.localIP() );
      }
      else
      {
        ESP_WML_LOGERROR(F("WiFi not connected"));

#if RESET_IF_NO_WIFI

#if USING_MRD
        // To avoid unnecessary MRD
        mrd->loop();
#else
        // To avoid unnecessary DRD
        drd->loop();
#endif

        resetFunc();

#endif
      }

      return status;
    }

    //////////////////////////////////////////////

    // NEW
    void createHTML(String& root_html_template)
    {
      String pitem;

      root_html_template = FPSTR(ESP_WM_LITE_HTML_HEAD_START);

#if USING_CUSTOMS_STYLE

      // Using Customs style when not NULL
      if (_CustomsHeadStyle)
        root_html_template += FPSTR(_CustomsHeadStyle);
      else
        root_html_template += FPSTR(ESP_WM_LITE_HTML_HEAD_STYLE);

#else
      root_html_template += FPSTR(ESP_WM_LITE_HTML_HEAD_STYLE);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT

      if (_CustomsHeadElement)
        root_html_template += _CustomsHeadElement;

#endif

#if SCAN_WIFI_NETWORKS

      ESP_WML_LOGDEBUG1(WiFiNetworksFound, F(" SSIDs found, generating HTML now"));
      // Replace HTML <input...> with <select...>, based on WiFi network scan in startConfigurationMode()

      ListOfSSIDs = "";

      for (int i = 0, list_items = 0; (i < WiFiNetworksFound) && (list_items < MAX_SSID_IN_LIST); i++)
      {
        if (indices[i] == -1)
          continue;     // skip duplicates and those that are below the required quality

        ListOfSSIDs += String(FPSTR(ESP_WM_LITE_OPTION_START)) + String(WiFi.SSID(indices[i])) + String(FPSTR(ESP_WM_LITE_OPTION_END));
        list_items++;   // Count number of suitable, distinct SSIDs to be included in list
      }

      ESP_WML_LOGDEBUG(ListOfSSIDs);

      if (ListOfSSIDs == "")    // No SSID found or none was good enough
        ListOfSSIDs = String(FPSTR(ESP_WM_LITE_OPTION_START)) + String(FPSTR(ESP_WM_LITE_NO_NETWORKS_FOUND)) + String(FPSTR(ESP_WM_LITE_OPTION_END));

      pitem = String(FPSTR(ESP_WM_LITE_HTML_HEAD_END));

#if MANUAL_SSID_INPUT_ALLOWED
      pitem.replace("[[input_id]]",  "<input id='id' list='SSIDs'>"  + String(FPSTR(ESP_WM_LITE_DATALIST_START)) + "'SSIDs'>" +
                    ListOfSSIDs + FPSTR(ESP_WM_LITE_DATALIST_END));
      ESP_WML_LOGDEBUG1(F("pitem:"), pitem);
      pitem.replace("[[input_id1]]", "<input id='id1' list='SSIDs'>" + String(FPSTR(ESP_WM_LITE_DATALIST_START)) + "'SSIDs'>" +
                    ListOfSSIDs + FPSTR(ESP_WM_LITE_DATALIST_END));
      ESP_WML_LOGDEBUG1(F("pitem:"), pitem);
#else
      pitem.replace("[[input_id]]",  "<select id='id'>"  + ListOfSSIDs + FPSTR(ESP_WM_LITE_SELECT_END));
      pitem.replace("[[input_id1]]", "<select id='id1'>" + ListOfSSIDs + FPSTR(ESP_WM_LITE_SELECT_END));
#endif

      root_html_template += pitem + FPSTR(ESP_WM_LITE_FLDSET_START);

#else

      pitem = String(FPSTR(ESP_WM_LITE_HTML_HEAD_END));
      pitem.replace("[[input_id]]",  FPSTR(ESP_WM_LITE_HTML_INPUT_ID));
      pitem.replace("[[input_id1]]", FPSTR(ESP_WM_LITE_HTML_INPUT_ID1));
      root_html_template += pitem + FPSTR(ESP_WM_LITE_FLDSET_START);

#endif    // SCAN_WIFI_NETWORKS

#if USE_DYNAMIC_PARAMETERS

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = String(FPSTR(ESP_WM_LITE_HTML_PARAM));

        pitem.replace("{b}", myMenuItems[i].displayName);
        pitem.replace("{v}", myMenuItems[i].id);
        pitem.replace("{i}", myMenuItems[i].id);

        root_html_template += pitem;
      }

#endif

      root_html_template += String(FPSTR(ESP_WM_LITE_FLDSET_END)) + FPSTR(ESP_WM_LITE_HTML_BUTTON) + FPSTR(ESP_WM_LITE_HTML_SCRIPT);

#if USE_DYNAMIC_PARAMETERS

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = String(FPSTR(ESP_WM_LITE_HTML_SCRIPT_ITEM));

        pitem.replace("{d}", myMenuItems[i].id);

        root_html_template += pitem;
      }

#endif

      root_html_template += String(FPSTR(ESP_WM_LITE_HTML_SCRIPT_END)) + FPSTR(ESP_WM_LITE_HTML_END);

      return;
    }

    //////////////////////////////////////////////

    void serverSendHeaders()
    {
      ESP_WML_LOGDEBUG3(F("serverSendHeaders:WM_HTTP_CACHE_CONTROL:"), FPSTR(WM_HTTP_CACHE_CONTROL), " : ", FPSTR(WM_HTTP_NO_STORE));
      server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
      // New from v1.2.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
      ESP_WML_LOGDEBUG3(F("serverSendHeaders:WM_HTTP_CORS:"), FPSTR(WM_HTTP_CORS), " : ", FPSTR(_CORS_Header));
      server->sendHeader(FPSTR(WM_HTTP_CORS), FPSTR(_CORS_Header));
#endif

      ESP_WML_LOGDEBUG3(F("serverSendHeaders:WM_HTTP_PRAGMA:"), FPSTR(WM_HTTP_PRAGMA), " : ", FPSTR(WM_HTTP_NO_CACHE));
      server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));

      ESP_WML_LOGDEBUG3(F("serverSendHeaders:WM_HTTP_EXPIRES:"), FPSTR(WM_HTTP_EXPIRES), " : ", "-1");
      server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
    }

    //////////////////////////////////////////////

    void handleRequest()
    {
      if (server)
      {
        String key    = server->arg("key");
        String value  = server->arg("value");

        static int number_items_Updated = 0;

        if (key == "" && value == "")
        {
          // New from v1.2.0
          serverSendHeaders();
          //////

          String result;
          createHTML(result);

          //ESP_WML_LOGDEBUG1(F("h:Repl:"), result);

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;

          if ( RFC952_hostname[0] != 0 )
          {
            // Replace only if Hostname is valid
            result.replace("ESP_WM_LITE", RFC952_hostname);
          }
          else if ( ESP_WM_LITE_config.board_name[0] != 0 )
          {
            // Or replace only if board_name is valid.  Otherwise, keep intact
            result.replace("ESP_WM_LITE", ESP_WM_LITE_config.board_name);
          }

          if (hadConfigData)
          {
            result.replace("[[id]]",     ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid);
            result.replace("[[pw]]",     ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw);
            result.replace("[[id1]]",    ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid);
            result.replace("[[pw1]]",    ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw);

#if USING_BOARD_NAME
            result.replace("[[nm]]",     ESP_WM_LITE_config.board_name);
#endif
          }
          else
          {
            result.replace("[[id]]",  "");
            result.replace("[[pw]]",  "");
            result.replace("[[id1]]", "");
            result.replace("[[pw1]]", "");

#if USING_BOARD_NAME
            result.replace("[[nm]]",  "");
#endif
          }

#if USE_DYNAMIC_PARAMETERS

          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            String toChange = String("[[") + myMenuItems[i].id + "]]";
            result.replace(toChange, myMenuItems[i].pdata);
          }

#endif

          ESP_WML_LOGDEBUG1(F("h:HTML page size:"), result.length());
          ESP_WML_LOGDEBUG1(F("h:HTML="), result);

          server->send(200, FPSTR(WM_HTTP_HEAD_TEXT_HTML), result);

          return;
        }

        if (number_items_Updated == 0)
        {
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));
          strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
        }

#if USE_DYNAMIC_PARAMETERS

        if (!menuItemUpdated)
        {
          // Don't need to free
          menuItemUpdated = new bool[NUM_MENU_ITEMS];

          if (menuItemUpdated)
          {
            for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
            {
              // To flag item is not yet updated
              menuItemUpdated[i] = false;
            }

            ESP_WML_LOGDEBUG(F("h: Init menuItemUpdated" ));
          }
          else
          {
            ESP_WML_LOGERROR(F("h: Error can't alloc memory for menuItemUpdated" ));
          }
        }

#endif

        static bool id_Updated  = false;
        static bool pw_Updated  = false;
        static bool id1_Updated = false;
        static bool pw1_Updated = false;

#if USING_BOARD_NAME
        static bool nm_Updated  = false;
#endif

        if (!id_Updated && (key == String("id")))
        {
          ESP_WML_LOGDEBUG(F("h:repl id"));
          id_Updated = true;

          number_items_Updated++;

          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid, value.c_str(),
                    sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) - 1);
        }
        else if (!pw_Updated && (key == String("pw")))
        {
          ESP_WML_LOGDEBUG(F("h:repl pw"));
          pw_Updated = true;

          number_items_Updated++;

          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw, value.c_str(), sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw) - 1);
        }
        else if (!id1_Updated && (key == String("id1")))
        {
          ESP_WML_LOGDEBUG(F("h:repl id1"));
          id1_Updated = true;

          number_items_Updated++;

          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid, value.c_str(),
                    sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) - 1);
        }
        else if (!pw1_Updated && (key == String("pw1")))
        {
          ESP_WML_LOGDEBUG(F("h:repl pw1"));
          pw1_Updated = true;

          number_items_Updated++;

          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw, value.c_str(), sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw) - 1);
        }

#if USING_BOARD_NAME
        else if (!nm_Updated && (key == String("nm")))
        {
          ESP_WML_LOGDEBUG(F("h:repl nm"));
          nm_Updated = true;

          number_items_Updated++;

          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.board_name) - 1)
            strcpy(ESP_WM_LITE_config.board_name, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.board_name, value.c_str(), sizeof(ESP_WM_LITE_config.board_name) - 1);
        }

#endif

#if USE_DYNAMIC_PARAMETERS
        else
        {
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            if ( !menuItemUpdated[i] && (key == myMenuItems[i].id) )
            {
              ESP_WML_LOGDEBUG3(F("h:"), myMenuItems[i].id, F("="), value.c_str() );

              menuItemUpdated[i] = true;

              number_items_Updated++;

              // Actual size of pdata is [maxlen + 1]
              memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

              if ((int) strlen(value.c_str()) < myMenuItems[i].maxlen)
                strcpy(myMenuItems[i].pdata, value.c_str());
              else
                strncpy(myMenuItems[i].pdata, value.c_str(), myMenuItems[i].maxlen);

              break;
            }
          }
        }

#endif

        ESP_WML_LOGDEBUG1(F("h:items updated ="), number_items_Updated);
        ESP_WML_LOGDEBUG3(F("h:key ="), key, ", value =", value);

        server->send(200, FPSTR(WM_HTTP_HEAD_TEXT_HTML), "OK");

#if USE_DYNAMIC_PARAMETERS

        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS + NUM_MENU_ITEMS)
#else
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
#endif
        {
#if USE_LITTLEFS
          ESP_WML_LOGERROR1(F("h:Updating LittleFS:"), CONFIG_FILENAME);
#elif USE_SPIFFS
          ESP_WML_LOGERROR1(F("h:Updating SPIFFS:"), CONFIG_FILENAME);
#else
          ESP_WML_LOGERROR(F("h:Updating EEPROM. Please wait for reset"));
#endif

          saveAllConfigData();

          // Done with CP, Clear CP Flag here if forced
          if (isForcedConfigPortal)
            clearForcedCP();

          ESP_WML_LOGERROR(F("h:Rst"));

          // TO DO : what command to reset
          // Delay then reset the board after save data
          resetFunc();
        }
      }   // if (server)
    }

    //////////////////////////////////////////////

#ifndef CONFIG_TIMEOUT
#warning Default CONFIG_TIMEOUT = 60s
#define CONFIG_TIMEOUT      60000L
#endif

    void startConfigurationMode()
    {
#if SCAN_WIFI_NETWORKS
      configTimeout = 0;  // To allow user input in CP

      WiFiNetworksFound = scanWifiNetworks(&indices);
#endif

#if USE_LED_BUILTIN
      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LED_ON);
#endif

      if ( (portal_ssid == "") || portal_pass == "" )
      {
        String chipID = String(ESP_getChipId(), HEX);
        chipID.toUpperCase();

        portal_ssid = "ESP_" + chipID;

        portal_pass = "MyESP_" + chipID;
      }

      WiFi.mode(WIFI_AP);

      // New
      delay(100);

      static int channel;

      // Use random channel if WiFiAPChannel == 0
      if (WiFiAPChannel == 0)
      {
        //channel = random(MAX_WIFI_CHANNEL) + 1;
        channel = (millis() % MAX_WIFI_CHANNEL) + 1;
      }
      else
        channel = WiFiAPChannel;

      // softAPConfig() must be put before softAP() for ESP8266 core v3.0.0+ to work.
      // ESP32 or ESP8266is core v3.0.0- is OK either way
      WiFi.softAPConfig(portal_apIP, portal_apIP, IPAddress(255, 255, 255, 0));

      WiFi.softAP(portal_ssid.c_str(), portal_pass.c_str(), channel);

      ESP_WML_LOGERROR3(F("\nstConf:SSID="), portal_ssid, F(",PW="), portal_pass);
      ESP_WML_LOGERROR3(F("IP="), portal_apIP.toString(), ",ch=", channel);

      delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428

      if (!server)
      {
#if ESP8266
        server = new ESP8266WebServer;
#else
        server = new WebServer;
#endif
      }

      if (!dnsServer)
      {
        dnsServer = new DNSServer();
      }

      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      if (server && dnsServer)
      {
        // CaptivePortal
        // if DNSServer is started with "*" for domain name, it will reply with provided IP to all DNS requests
        dnsServer->start(DNS_PORT, "*", portal_apIP);
        
        // reply to all requests with same HTML
        server->onNotFound([this]()
        {
          handleRequest();
        });

        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      // or SSID, PW, Server,Token ="nothing"
      if (hadConfigData)
      {
        configTimeout = millis() + CONFIG_TIMEOUT;

        ESP_WML_LOGDEBUG3(F("s:millis() = "), millis(), F(", configTimeout = "), configTimeout);
      }
      else
      {
        configTimeout = 0;
        ESP_WML_LOGDEBUG(F("s:configTimeout = 0"));
      }

      configuration_mode = true;
    }

#if SCAN_WIFI_NETWORKS

    // Source code adapted from https://github.com/khoih-prog/ESP_WiFiManager/blob/master/src/ESP_WiFiManager-Impl.h

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    bool          _removeDuplicateAPs     = true;

    //////////////////////////////////////////

    void swap(int *thisOne, int *thatOne)
    {
      int tempo;

      tempo    = *thatOne;
      *thatOne = *thisOne;
      *thisOne = tempo;
    }

    //////////////////////////////////////////

    void setMinimumSignalQuality(const int& quality)
    {
      _minimumQuality = quality;
    }

    //////////////////////////////////////////

    //if this is true, remove duplicate Access Points - default true
    void setRemoveDuplicateAPs(const bool& removeDuplicates)
    {
      _removeDuplicateAPs = removeDuplicates;
    }

    //////////////////////////////////////////

    //Scan for WiFiNetworks in range and sort by signal strength
    //space for indices array allocated on the heap and should be freed when no longer required
    int scanWifiNetworks(int **indicesptr)
    {
      ESP_WML_LOGDEBUG(F("Scanning Network"));

      int n = WiFi.scanNetworks();

      ESP_WML_LOGDEBUG1(F("scanWifiNetworks: Done, Scanned Networks n = "), n);

      //KH, Terrible bug here. WiFi.scanNetworks() returns n < 0 => malloc( negative == very big ) => crash!!!
      //In .../esp32/libraries/WiFi/src/WiFiType.h
      //#define WIFI_SCAN_RUNNING   (-1)
      //#define WIFI_SCAN_FAILED    (-2)
      //if (n == 0)
      if (n <= 0)
      {
        ESP_WML_LOGDEBUG(F("No network found"));
        return (0);
      }
      else
      {
        // Allocate space off the heap for indices array.
        // This space should be freed when no longer required.
        int* indices = (int *)malloc(n * sizeof(int));

        if (indices == NULL)
        {
          ESP_WML_LOGDEBUG(F("ERROR: Out of memory"));
          *indicesptr = NULL;
          return (0);
        }

        *indicesptr = indices;

        //sort networks
        for (int i = 0; i < n; i++)
        {
          indices[i] = i;
        }

        ESP_WML_LOGDEBUG(F("Sorting"));

        // RSSI SORT
        // old sort
        for (int i = 0; i < n; i++)
        {
          for (int j = i + 1; j < n; j++)
          {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
            {
              //std::swap(indices[i], indices[j]);
              // Using locally defined swap()
              swap(&indices[i], &indices[j]);
            }
          }
        }

        ESP_WML_LOGDEBUG(F("Removing Dup"));

        // remove duplicates ( must be RSSI sorted )
        if (_removeDuplicateAPs)
        {
          String cssid;

          for (int i = 0; i < n; i++)
          {
            if (indices[i] == -1)
              continue;

            cssid = WiFi.SSID(indices[i]);

            for (int j = i + 1; j < n; j++)
            {
              if (cssid == WiFi.SSID(indices[j]))
              {
                ESP_WML_LOGDEBUG1("DUP AP:", WiFi.SSID(indices[j]));
                indices[j] = -1; // set dup aps to index -1
              }
            }
          }
        }

        for (int i = 0; i < n; i++)
        {
          if (indices[i] == -1)
            continue; // skip dups

          int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

          if (!(_minimumQuality == -1 || _minimumQuality < quality))
          {
            indices[i] = -1;
            ESP_WML_LOGDEBUG(F("Skipping low quality"));
          }
        }

        ESP_WML_LOGWARN(F("WiFi networks found:"));

        for (int i = 0; i < n; i++)
        {
          if (indices[i] == -1)
            continue; // skip dups
          else
            ESP_WML_LOGWARN5(i + 1, ": ", WiFi.SSID(indices[i]), ", ", WiFi.RSSI(i), "dB");
        }

        return (n);
      }
    }

    //////////////////////////////////////////

    int getRSSIasQuality(const int& RSSI)
    {
      int quality = 0;

      if (RSSI <= -100)
      {
        quality = 0;
      }
      else if (RSSI >= -50)
      {
        quality = 100;
      }
      else
      {
        quality = 2 * (RSSI + 100);
      }

      return quality;
    }

    //////////////////////////////////////////

#endif
};


#endif    //ESP_WiFiManager_Lite_h
