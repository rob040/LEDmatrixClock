/****************************************************************************************************************************
  ESP_WiFiManager_Lite.h
  For ESP8266 / ESP32 boards

  ESP_WiFiManager_Lite (https://github.com/khoih-prog/ESP_WiFiManager_Lite) is a library
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager_Lite
  Licensed under MIT license

  Version: 1.11.0

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
  1.11.0  rob040        20250722   Massive simplification to reset detector configuration, html corrections,
                                   Flexible number of wifi credentials NUM_WIFI_CREDENTIALS,
                                   Replaced "Customs" with "Custom" everywhere,
                                   Resolve the getRFC952_hostname issues, renamed to setWmlHostname(),
                                   Fix startup with erased flash: load default configuration, Have FS and EEPROM behave te same,
                                   Do LOAD_DEFAULT_CONFIG_DATA (restore default on EVERY startup) from 5 to 1 place: begin(),
                                   Rework isWiFiConfigValid(),
                                   Rename loadAndSaveDefaultConfigData() method to restoreDefaultConfiguration()
                                   Fix hadConfigData flag usage, and rename to present tense hasConfigData.
                                   Method setHostName now accepts a hostname.

 *****************************************************************************************************************************/

#pragma once

#ifndef ESP_WiFiManager_Lite_h
#define ESP_WiFiManager_Lite_h


//TODO: check if these exceptions on ESP32* are still valid! (they may be outdated)

#if !( defined(ESP8266) ||  defined(ESP32) )
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#elif ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_ESP32S2_THING_PLUS || ARDUINO_MICROS2 || \
        ARDUINO_METRO_ESP32S2 || ARDUINO_MAGTAG29_ESP32S2 || ARDUINO_FUNHOUSE_ESP32S2 || \
        ARDUINO_ADAFRUIT_FEATHER_ESP32S2_NOPSRAM )
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #warning Using ESP32_S2. To follow library instructions to install esp32-s2 core and WebServer Patch
  #endif
  #define USING_ESP32_S2        true
#elif ( ARDUINO_ESP32C3_DEV )
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      #warning Using ESP32_C3 using core v2.0.0+. Either LittleFS, SPIFFS or EEPROM OK.
    #else
      #warning Using ESP32_C3 using core v1.0.6-. To follow library instructions to install esp32-c3 core. Only SPIFFS and EEPROM OK.
    #endif
    #warning You have to select Flash size 2MB and Minimal APP (1.3MB + 700KB) for some boards
  #endif
  #define USING_ESP32_C3        true
#elif ( defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_ESP32_S3_BOX) || defined(ARDUINO_TINYS3) || \
        defined(ARDUINO_PROS3) || defined(ARDUINO_FEATHERS3) )
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #warning Using ESP32_S3. To install esp32-s3-support branch if using core v2.0.2-.
  #endif
  #define USING_ESP32_S3        true
#endif

///////////////////////////////////////////

#ifndef ESP_WIFI_MANAGER_LITE_VERSION
  #define ESP_WIFI_MANAGER_LITE_VERSION             "ESP_WiFiManager_Lite v1.11.0"

  #define ESP_WIFI_MANAGER_LITE_VERSION_MAJOR       1
  #define ESP_WIFI_MANAGER_LITE_VERSION_MINOR       11
  #define ESP_WIFI_MANAGER_LITE_VERSION_PATCH       0

  #define ESP_WIFI_MANAGER_LITE_VERSION_INT         1011000
#endif

///////////////////////////////////////////

#ifdef ESP8266

  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
  #include <ESP8266WebServer.h>

  #if ( USE_LITTLEFS || USE_SPIFFS )

    #if USE_LITTLEFS
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
      #include <LittleFS.h>
      #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
        #warning Using LittleFS in ESP_WiFiManager_Lite.h
      #endif
    #else
      #define FileFS        SPIFFS
      #define FS_Name       "SPIFFS"
      #include <SPIFFS.h>
      #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
        #warning Using SPIFFS in ESP_WiFiManager_Lite.h
      #endif
    #endif

    #include <FS.h>
  #elif USE_EEPROM
    #include <EEPROM.h>
    #define FS_Name         "EEPROM"
    #define EEPROM_SIZE     2048
    #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
      #warning Using EEPROM in ESP_WiFiManager_Lite.h
    #endif

  #else
    #error You must select one WM configuration storage: USE_EEPROM, USE_SPIFFS, USE_LITTLEFS
  #endif

#else   //ESP32

  #include <WiFi.h>
  #include <WiFiMulti.h>
  #include <WebServer.h>

  //TODO: check if these exceptions are still valid (they may be outdated); for the moment no config change, just warnings

  // To be sure no LittleFS for ESP32-C3 for core v1.0.6-
  #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
    // For core v2.0.0+, ESP32-C3 can use LittleFS, SPIFFS or EEPROM
    // LittleFS has higher priority than SPIFFS.
    // For core v2.0.0+, if not specified any, use better LittleFS
    #if ! (defined(USE_LITTLEFS) || defined(USE_SPIFFS) )
      #warning use #define USE_LITTLEFS      true
    #endif
  #elif defined(ARDUINO_ESP32C3_DEV)
    // For core v1.0.6-, ESP32-C3 only supporting SPIFFS and EEPROM. To use v2.0.0+ for LittleFS
    #if USE_LITTLEFS
      #warning use #define USE_SPIFFS              true
    #endif
  #else
    // For core v1.0.6-, if not specified any, use SPIFFS to not forcing user to install LITTLEFS library
    #if ! (defined(USE_LITTLEFS) || defined(USE_SPIFFS) )
      #warning use #define USE_SPIFFS      true
    #endif
  #endif

  #if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
    //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
        #warning Using ESP32 Core 1.0.6 or 2.0.0+
      #endif
      // The library has been merged into esp32 core from release 1.0.6
      #include <LittleFS.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS

      FS* filesystem =      &LittleFS;
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
    #else
      #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
        #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
      #endif
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
    #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
      #warning Using SPIFFS in ESP_WiFiManager_Lite.h
    #endif

  #elif USE_EEPROM
    #include <EEPROM.h>
    #define FS_Name         "EEPROM"
    #define EEPROM_SIZE     2048
    #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
      #warning Using EEPROM in ESP_WiFiManager_Lite.h
    #endif

  #else
    #error You must select one WM configuration storage: USE_EEPROM, USE_SPIFFS, USE_LITTLEFS
  #endif

#endif

#define DNS_PORT      53

///////////////////////////////////////////

#include <DNSServer.h>
#include <memory>
#undef min
#undef max
#include <algorithm>

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


// These defines must be put before #include <ESP_MultiResetDetector.h>
// The MultiResetDetector's variable is stored in Volatile memory that is not
// erased by chip reset or application start; it is never stored in
// non-volatile flash storage, like it was before version 1.11.

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

// RTC Memory Address for the MultiResetDetector to use
#ifndef MRD_ADDRESS
  #define MRD_ADDRESS 0
#endif

#define MRD_ALLOCATE_STATIC_DATA

#include <ESP_MultiResetDetector.h>      // https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector

#undef MRD_ALLOCATE_STATIC_DATA

//MultiResetDetector mrd(MRD_TIMEOUT, MRD_ADDRESS);
MultiResetDetector* mrd;


///////////////////////////////////////////

#define MAX_ID_LEN                5
#define MAX_DISPLAY_NAME_LEN      16

///////////////////////////////////////////

// Dynamic parameter menu descriptor
typedef struct MenuItem_s
{
  char id             [MAX_ID_LEN + 1]; // id name must be unique (is used in html)
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
} MenuItem;

///////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS
  #if (_ESP_WM_LITE_LOGLEVEL_ > 3)
    #warning Using Dynamic Parameters
  #endif

  extern uint16_t NUM_MENU_ITEMS;
  extern MenuItem myMenuItems[];
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

typedef struct Credentials_s
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

#ifndef NUM_WIFI_CREDENTIALS
#define NUM_WIFI_CREDENTIALS      2  // (select 1...4, but actually no limit)
#endif

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

typedef struct Configuration_s
{
  char header         [HEADER_MAX_LEN];
  WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
  char board_name     [BOARD_NAME_MAX_LEN];
  int  checkSum;
} ESP_WM_LITE_Configuration;

// CONFIG_DATA_SIZE for NUM_WIFI_CREDENTIALS==2  =   236  = (16 + 96 * 2 + 24 + 4)
#define CONFIG_DATA_SIZE  sizeof(ESP_WM_LITE_Configuration)

///////////////////////////////////////////

extern ESP_WM_LITE_Configuration defaultConfig;

///////////////////////////////////////////

// -- HTML page fragments

const char ESP_WM_LITE_HTML_HEAD_START[] PROGMEM =
  "<!DOCTYPE html><html><head><title>ESP_WM_LITE</title><meta name='viewport' content='width=device-width, initial-scale=1'>";

const char ESP_WM_LITE_HTML_HEAD_STYLE[] PROGMEM =
  "<style>"
  "div,input{padding:5px;font-size:1em;}"
  "input{width:95%;}"
  "body{text-align: center;}"
  "button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}"
  "fieldset{border-radius:0.3rem;margin:0px;}"
  "</style>";

const char ESP_WM_LITE_HTML_HEAD_END[]   PROGMEM =
  "</head><div style='text-align:left;display:inline-block;min-width:260px;'>";


const char ESP_WM_LITE_HTML_INPUT_WIFI_IN[]   PROGMEM =
  "<div><label>*WiFi SSID #[n]</label><div><input value='[v]' id='id[n]'></div></div>";
const char ESP_WM_LITE_HTML_INPUT_WIFI_SEL[]   PROGMEM =
  "<div><label>*WiFi SSID #[n]</label><div><select id='id[n]'>[o]</select></div></div>";
const char ESP_WM_LITE_HTML_INPUT_WIFI_LIST[]   PROGMEM =
  "<div><label>*WiFi SSID #[n]</label><div><input value='[v]' id='id[n]' list='SSIDs[n]'><datalist id='SSIDs[n]'>[o]</datalist></div></div>";
const char ESP_WM_LITE_HTML_INPUT_WIFI_PW[]   PROGMEM =
  "<div><label>*PWD #[n]</label><input value='[p]' id='pw[n]'><div></div></div>";

#if USING_BOARD_NAME
const char ESP_WM_LITE_HTML_INPUT_BOARD_NAME[]   PROGMEM =
  "<fieldset><div><label>Board Name</label><input value='[nm]' id='nm'><div></div></div></fieldset>";
#endif



const char ESP_WM_LITE_FLDSET_START[]  PROGMEM = "<fieldset>";
const char ESP_WM_LITE_FLDSET_END[]    PROGMEM = "</fieldset>";
const char ESP_WM_LITE_HTML_PARAM[]    PROGMEM = "<div><label>[b]</label><input value='[v]' id='[i]'><div></div></div>";

const char ESP_WM_LITE_HTML_SCRIPT[]   PROGMEM =
  "<button onclick=\"sv()\">Save</button></div>"
  "<script id=\"jsbin-javascript\">"
  "function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);"
  "request.open('GET',url,false);request.send(null);}"
  "function sv(){";

const char ESP_WM_LITE_HTML_SCRIPT_ITEM[]  PROGMEM = "udVal('[d]',document.getElementById('[d]').value);";
const char ESP_WM_LITE_HTML_SCRIPT_END[]   PROGMEM = "alert('Updated');}</script></html>";

#if SCAN_WIFI_NETWORKS
  //n.u. const char ESP_WM_LITE_SELECT_START[]      PROGMEM = "<select id=";
  //n.u. const char ESP_WM_LITE_SELECT_END[]        PROGMEM = "</select>";
  //n.u. const char ESP_WM_LITE_DATALIST_START[]    PROGMEM = "<datalist id=";
  //n.u. const char ESP_WM_LITE_DATALIST_END[]      PROGMEM = "</datalist>";
  const char ESP_WM_LITE_OPTION_START[]      PROGMEM = "<option>";
  //n.u. const char ESP_WM_LITE_OPTION_END[]        PROGMEM = "";      // "</option>"; is not required
  const char ESP_WM_LITE_NO_NETWORKS_FOUND[] PROGMEM = "No suitable WiFi networks available!";
#endif

//////////////////////////////////////////

// repeatedly used consts

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

// alternately use IPAddress.toString() or WiFi.localIP().toString() or localIP()
/*String IPAddressToString(const IPAddress& _address)
{
  String str = String(_address[0]);
  str += ".";
  str += String(_address[1]);
  str += ".";
  str += String(_address[2]);
  str += ".";
  str += String(_address[3]);
  return str;
}*/

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

    // Caveat: this begin method has completely different effect than the begin(const char*), AFAIK this is against every Cpp/OOD rule
    void begin(const char* ssid,
               const char* pass )
    {
      ESP_WML_LOGINFO(F("conW"));
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

      mrd = new MultiResetDetector();
      bool noConfigPortal = true;

      if (mrd->detectMultiReset())
      {
        ESP_WML_LOGINFO(F("Multi Reset Detected"));
        noConfigPortal = false;
      }

      WiFi.mode(WIFI_STA);

      if (iHostname == NULL || iHostname[0] == 0)
      {
        String _hostname = "ESP_" + String(ESP_getChipId(), HEX);
        _hostname.toUpperCase();

        setWmlHostname(_hostname.c_str());
      }
      else
      {
        // Prepare and store the hostname only not NULL
        setWmlHostname(iHostname);
      }

      ESP_WML_LOGINFO1(F("Hostname="), wmlHostname);

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
         // Special case: Force default configuration for DEBUG TEST
        restoreDefaultConfiguration();

        // Don't need Config Portal anymore, when defaultConfig is properly set
        // (the user could chose to just declare a zero defaultConfig, in which case the CP should be entered)
        hasConfigData = isWiFiConfigValid();
      }
      else
      {
        // Normal case: Get Config data from non-volatile storage
        hasConfigData = getConfigData();
      }

      isForcedConfigPortal = isForcedCP();

      //  noConfigPortal when getConfigData() OK and not MRD'ed
      if (hasConfigData && noConfigPortal && (!isForcedConfigPortal) )
      {
        //hasConfigData = true; // was already true

        //ESP_WML_LOGDEBUG1(F("bg:noConfigPortal="), noConfigPortal); // is always true

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
        ESP_WML_LOGDEBUG1(F("bg:isForcedConfigPortal="), isForcedConfigPortal);

        // If not persistent => clear the flag so that after reset. no more CP, even CP not entered and saved
        if (persForcedConfigPortal)
        {
          ESP_WML_LOGINFO1(F("bg:Stay forever in CP:"),
                           isForcedConfigPortal ? F("Forced-Persistent") : (noConfigPortal ? F("No ConfigDat") : F("MRD")));
        }
        else
        {
          ESP_WML_LOGINFO1(F("bg:Stay forever in CP:"),
                           isForcedConfigPortal ? F("Forced-non-Persistent") : (noConfigPortal ? F("No ConfigDat") : F("MRD")));
          clearForcedCP();

        }
        hasConfigData = isForcedConfigPortal && !noConfigPortal ? true : false;

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
#warning TIMEOUT_RECONNECT_WIFI too low. Resetting to 10000
#undef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   10000L
#elif (TIMEOUT_RECONNECT_WIFI > 60000L)
#warning TIMEOUT_RECONNECT_WIFI too high. Resetting to 60000
#undef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   60000L
#endif
#endif

#ifndef RETRY_TIMES_RECONNECT_WIFI
#define RETRY_TIMES_RECONNECT_WIFI   2
#else
    // Force range of user-defined RETRY_TIMES_RECONNECT_WIFI between 2-5 times
#if (RETRY_TIMES_RECONNECT_WIFI < 2)
#warning RETRY_TIMES_RECONNECT_WIFI too low. Resetting to 2
#undef RETRY_TIMES_RECONNECT_WIFI
#define RETRY_TIMES_RECONNECT_WIFI   2
#elif (RETRY_TIMES_RECONNECT_WIFI > 5)
#warning RETRY_TIMES_RECONNECT_WIFI too high. Resetting to 5
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
#warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too low. Resetting to 2
#undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   2
#elif (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET > 100)
#warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too high. Resetting to 100
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

      // Call the multi reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call mrd.stop() when you wish to no longer
      // consider the next reset as a multi reset.
      mrd->loop();

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
            ESP_WML_LOGINFO(F("r:Check&WLost"));
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
            delay(1); // yield() to other processes, if any
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

              ESP_WML_LOGINFO(F("r:WLost.ReconW"));

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

    void setHostname(const char *iHostname = "")
    {
      if (iHostname && iHostname[0] != 0)
      {
        setWmlHostname(iHostname);
      }

      if (wmlHostname[0] != 0)
      {
#if ESP8266
        WiFi.hostname(wmlHostname);
#else


        // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
#if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
        WiFi.setHostname(wmlHostname);
#else

        // Still have bug in ESP32_S2 for old core. If using WiFi.setHostname() => WiFi.localIP() always = 255.255.255.255
        if ( String(ARDUINO_BOARD) != "ESP32S2_DEV" )
        {
          // See https://github.com/espressif/arduino-esp32/issues/2537
          WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
          WiFi.setHostname(wmlHostname);
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

    String getWiFiSSID(uint8_t index)
    {
      if (!hasConfigData)
        getConfigData();

      if ((index >= NUM_WIFI_CREDENTIALS) || !hasConfigData)
        return "";  // even after getConfigData, still no (valid) configdata

      return (String(ESP_WM_LITE_config.WiFi_Creds[index].wifi_ssid));
    }

    //////////////////////////////////////////////

    String getWiFiPW(uint8_t index)
    {
      if (!hasConfigData)
        getConfigData();

      if ((index >= NUM_WIFI_CREDENTIALS) || !hasConfigData)
        return "";  // even after getConfigData, still no (valid) configdata

      return (String(ESP_WM_LITE_config.WiFi_Creds[index].wifi_pw));
    }

    //////////////////////////////////////////////

    String getBoardName()
    {
      if (!hasConfigData)
        getConfigData();

      if (!hasConfigData)
        return "";  // even after getConfigData, still no (valid) configdata

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
      if (!hasConfigData)
        getConfigData();

      if (!hasConfigData)
        return NULL;  // even after getConfigData, still no (valid) configdata

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_Configuration));

      return (configData);
    }

    //////////////////////////////////////////////

    String localIP()
    {
      //ipAddress = IPAddressToString(WiFi.localIP());
      //return ipAddress;
      return WiFi.localIP().toString();
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
      return hasConfigData;
    }

    //////////////////////////////////////////////

    bool isConfigMode()
    {
      return configuration_mode;
    }

    //////////////////////////////////////////////

    // Forced CP => Flag = 0xBEEFBEEF. Else => No forced CP
    // Flag to be stored at (EEPROM_START + CONFIG_DATA_SIZE)
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

    // Add custom headers from v1.2.0

    // New from v1.2.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
    // New from v1.11.0: all "Customs" are renamed "Custom". This spelling error should have been fixed sooner
#if USING_CUSTOMS_STYLE
    #error "Please rename all your 'Customs' style macros and functions to 'Custom' "
#endif
#if USING_CUSTOMS_HEAD_ELEMENT
    #error "Please rename all your 'Customs' element macros and functions to 'Custom' "
#endif
#if USING_CUSTOM_STYLE
    //sets a custom style, such as color
    // "<style>div,input{padding:5px;font-size:1em;}
    // input{width:95%;}body{text-align: center;}
    // button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}
    // fieldset{border-radius:0.3rem;margin:0px;}</style>";
    void setCustomStyle(PGM_P CustomStyle = ESP_WM_LITE_HTML_HEAD_STYLE)
    {
      _CustomHeadStyle = CustomStyle;
      ESP_WML_LOGDEBUG1(F("Set CustomStyle to : "), FPSTR(_CustomHeadStyle));
    }

    //////////////////////////////////////

    PGM_P getCustomStyle()
    {
      ESP_WML_LOGDEBUG1(F("Get CustomStyle = "), FPSTR(_CustomHeadStyle));
      return _CustomHeadStyle;
    }
#endif

    //////////////////////////////////////

#if USING_CUSTOM_HEAD_ELEMENT
    //sets a custom element to add to head, like a new style tag
    void setCustomHeadElement(PGM_P CustomHeadElement = NULL)
    {
      _CustomHeadElement = CustomHeadElement;
      ESP_WML_LOGDEBUG1(F("Set CustomHeadElement to : "), _CustomHeadElement);
    }

    //////////////////////////////////////

    PGM_P getCustomHeadElement()
    {
      ESP_WML_LOGDEBUG1(F("Get CustomHeadElement = "), _CustomHeadElement);
      return _CustomHeadElement;
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
        ESP_WML_LOGWARN(F("SPIFFS/LittleFS failed!"));
        return false;
      }

#endif   // #if ( USE_LITTLEFS || USE_SPIFFS )

      return loadDynamicData();

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
        ESP_WML_LOGWARN(F("SPIFFS/LittleFS failed!"));
        return;
      }

#endif   // #if ( USE_LITTLEFS || USE_SPIFFS )

      saveDynamicData();

    }

#endif

    //////////////////////////////////////


  private:
    //n.u. String ipAddress = "0.0.0.0";

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
    bool hasConfigData = false;
    bool hasDynamicData = false;

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

    // Add custom headers from v1.2.0

#if USING_CUSTOM_STYLE
    PGM_P _CustomHeadStyle = nullptr;
#endif

#if USING_CUSTOM_HEAD_ELEMENT
    PGM_P _CustomHeadElement = nullptr;
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

    // updated_flags is an internal structure to keep track of which key-value pairs
    // are already handled in handleRequest()
    struct updated_flags_s {
      uint8_t number_items;
      struct wificreds_s{
        bool wfidn;
        bool wfpwn;
      } wificreds[NUM_WIFI_CREDENTIALS];
      bool nm;
    } updated_flags;

    //////////////////////////////////////

#define WML_HOSTNAME_MAXLEN      24

    char wmlHostname[WML_HOSTNAME_MAXLEN + 1];

    //[rob040] fixed all remarks
    char* setWmlHostname(const char* iHostname)
    {
      memset(wmlHostname, 0, sizeof(wmlHostname));

      int len = strlen(iHostname);
      if (len > WML_HOSTNAME_MAXLEN) len = WML_HOSTNAME_MAXLEN;

      int j = 0;

      for (int i = 0; i < len; i++)
      {
        char ch = iHostname[i];
        if ((ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') ||
            ((ch == '-' || ch == '.' || ch == '_') && (i > 0 && i < len - 1)))
        {
          wmlHostname[j] = ch;
          j++;
        }
      }

      return wmlHostname;
    }
    // for backwards compatibility:
    // not needed; its private
    //char* getRFC952_hostname(const char* iHostname) { return setWmlHostname(iHostname); }

    //////////////////////////////////////

    void displayConfigData(const ESP_WM_LITE_Configuration& configData)
    {
      ESP_WML_LOGINFO1(F("Hdr="), configData.header);
      for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
        ESP_WML_LOGINFO5(F("SSID"), i, "=", configData.WiFi_Creds[i].wifi_ssid, F(",PW.len="), strlen(configData.WiFi_Creds[i].wifi_pw));
      }

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
      ESP_WML_LOGINFO3(F("SSID="), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
      ESP_WML_LOGINFO1(F("IP="), WiFi.localIP() );
    }

    //////////////////////////////////////

#define ESP_WM_LITE_BOARD_TYPE   "ESP_WM_LITE"
#define WM_NO_CONFIG             "blank"

    int calcChecksum()
    {
      int checkSum = 0;

      for (uint16_t index = 0; index < (sizeof(ESP_WM_LITE_config) - sizeof(ESP_WM_LITE_config.checkSum)); index++)
      {
        checkSum += * ( ( (uint8_t*) &ESP_WM_LITE_config ) + index);
      }

      return checkSum;
    }

    //////////////////////////////////////////////

    void restoreDefaultConfiguration()
    {
      // Load Default Config Data from Sketch
      memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
      strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

      // Including config and dynamic data, and assume valid
      saveAllConfigData();

      ESP_WML_LOGINFO(F("== Restore Default Config =="));
      displayConfigData(ESP_WM_LITE_config);
    }

    //////////////////////////////////////////////

    bool isWiFiConfigValid()
    {
      int cnt = 0;
      for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
        // If SSID ="blank" or NULL, or PWD length < 8 (as required by standard) => invalid set
        if (  (ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid[0] != 0) &&
              ((uint8_t)ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid[0] != 0xFF) &&
              (strlen(ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw) >= PASSWORD_MIN_LEN ) &&
              (strcmp(ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid, WM_NO_CONFIG) != 0) )
        {
          // this entry is good
          cnt++;
        }
      }
#if REQUIRE_ONE_SET_SSID_PW
      // at least one set was valid (advised setting)
      return (cnt > 0);
#else
      // all sets were valid (not advised setting for NUM_WIFI_CREDENTIALS > 2 )
      // User must have entered valid settings for all possible Wifi Credentials
      return (cnt == NUM_WIFI_CREDENTIALS);
#endif
   }

    //////////////////////////////////////////////

#if ( USE_LITTLEFS || USE_SPIFFS )

    // Use LittleFS/InternalFS for ESP
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

    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent ? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA :
                                            FORCED_CONFIG_PORTAL_FLAG_DATA;

      ESP_WML_LOGINFO(isPersistent ? F("setForcedCP Persistent") : F("setForcedCP non-Persistent"));

      saveForcedCP(readForcedConfigPortalFlag);
    }

    //////////////////////////////////////////////

    void clearForcedCP()
    {
      uint32_t readForcedConfigPortalFlag = 0;

      ESP_WML_LOGINFO(F("clearForcedCP"));

      saveForcedCP(readForcedConfigPortalFlag);
    }

    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

      ESP_WML_LOGINFO(F("Check if isForcedCP"));

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

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + CONFIG_DATA_SIZE)
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
          ESP_WML_LOGINFO(F("ChkCrR: Error can't allocate buffer."));
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
      if (hasDynamicData)
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

      hasDynamicData = true;
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

      return true;
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

    // Return false if init new FileFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid = true;
      bool configDataValid;
      int calChecksum;

      hasConfigData = false;

#if ESP8266

      // Format SPIFFS if not yet
      if (!FileFS.begin())
      {
        FileFS.format();
#else

      // Format SPIFFS if not yet
      if (!FileFS.begin(true))
      {
#endif
        ESP_WML_LOGWARN(F("SPIFFS/LittleFS failed! Formatting."));

        if (!FileFS.begin())
        {
#if USE_LITTLEFS
          ESP_WML_LOGWARN(F("LittleFS failed!. Please use SPIFFS or EEPROM."));
#else
          ESP_WML_LOGWARN(F("SPIFFS failed!. Please use LittleFS or EEPROM."));
#endif
          return false;
        }
      }

#if USE_DYNAMIC_PARAMETERS
      if ( ( FileFS.exists(CONFIG_FILENAME)      || FileFS.exists(CONFIG_FILENAME_BACKUP) ) &&
           ( FileFS.exists(CREDENTIALS_FILENAME) || FileFS.exists(CREDENTIALS_FILENAME_BACKUP) ) )
#else
      if ( FileFS.exists(CONFIG_FILENAME) || FileFS.exists(CONFIG_FILENAME_BACKUP) )
#endif
      {
        // Load config data from LittleFS
        // (when the file(s) exist, we should expect the load to succeed; the content has still to be validated)
        // Get config data. If "blank" or NULL, set false flag and exit
        configDataValid = loadConfigData();

        ESP_WML_LOGINFO1( (configDataValid) ? "":"In", F("valid Stored Config Data"));
        ESP_WML_LOGINFO(F("== Start Stored Config =="));
        displayConfigData(ESP_WM_LITE_config);

        calChecksum = calcChecksum();

        ESP_WML_LOGINFO3(F("CCSum=0x"), String(calChecksum, HEX),
                         F(",RCSum=0x"), String(ESP_WM_LITE_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS
        // Load dynamic data
        dynamicDataValid = loadDynamicData();

        ESP_WML_LOGINFO1( (dynamicDataValid) ? "":"In", F("valid Stored Dynamic Data"));
#endif
      }
      else
      {
        // config files not present: clean start: use default config, when set by user
        restoreDefaultConfiguration();

        // perform basic check on configuration. If valid, return and continue without Config Portal else return false and enter CP
        return isWiFiConfigValid();
      }

      if ( (strcmp(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE) != 0) ||
           (calChecksum != ESP_WM_LITE_config.checkSum) ||
            !dynamicDataValid ||
            !configDataValid     )
      {
        // loaded config data validation failed: init to "blank"
        // [rob040] why not also do restoreDefaultConfiguration ?

        ESP_WML_LOGINFO1(F("InitCfgFile,sz="), sizeof(ESP_WM_LITE_config));

          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

          for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
            strcpy(ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid,   WM_NO_CONFIG);
            strcpy(ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw,     WM_NO_CONFIG);
          }
          strcpy(ESP_WM_LITE_config.board_name, WM_NO_CONFIG);

#if USE_DYNAMIC_PARAMETERS

          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }

#endif

        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

#if USE_DYNAMIC_PARAMETERS

        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          ESP_WML_LOGDEBUG3(F("g:myMenuItems["), i, F("]="), myMenuItems[i].pdata );
        }

#endif

        // Checksum will be recalculated
        ESP_WM_LITE_config.checkSum = 0;

        saveAllConfigData();

        ESP_WML_LOGINFO(F("== Initialized Config =="));
        displayConfigData(ESP_WM_LITE_config);

        return false;
      }
      else if ( !isWiFiConfigValid() )
      {
        // If SSID, PW ="blank" or NULL, stay in config mode forever until having config Data.
        return false;
      }

      ESP_WML_LOGINFO(F("== Retrieved Config =="));
      displayConfigData(ESP_WM_LITE_config);

      hasConfigData = true;
      return true;
    }

    //////////////////////////////////////////////

#endif // ( USE_LITTLEFS || USE_SPIFFS )

#if USE_EEPROM

#ifndef EEPROM_SIZE  // Must agree with application if that also uses EEPROM
#define EEPROM_SIZE     2048
#endif
#ifndef EEPROM_START  // Adapt start to not conflict with application if that also uses EEPROM
#define EEPROM_START     0
#else
#if (EEPROM_START + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE)
#error EPROM_START + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE. Please adjust.
#endif
#endif

// Starting positon to store ESP_WM_LITE_config
#define CONFIG_EEPROM_START    (EEPROM_START)

    //////////////////////////////////////////////

    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent ? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA :
                                            FORCED_CONFIG_PORTAL_FLAG_DATA;

      ESP_WML_LOGINFO(isPersistent ? F("setForcedCP Persistent") : F("setForcedCP non-Persistent"));

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

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      EEPROM.get(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + CONFIG_DATA_SIZE)
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
      //    ?? That is not what is coded here [rob040]
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        if (myMenuItems[i].maxlen > BUFFER_LEN)
        {
          // Size too large, abort and flag false
          ESP_WML_LOGWARN(F("ChkCrR: Error Small Buffer."));
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

    bool loadDynamicData()
    {
      if (hasDynamicData)
      { // already loaded
        return true;
      }
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

      hasDynamicData = true;
      return true;
    }

    //////////////////////////////////////////////

    void saveDynamicData()
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

    // Load config data from nonvolatile storage
    // Returns false when data was not present / was lost
    // Returns true when present ; caller should check checksum
    bool loadConfigData()
    {
      // Load data from EEPROM
      EEPROM.get(CONFIG_EEPROM_START, ESP_WM_LITE_config);

      // Check if EEPROM was erased (0xFF)
      uint32_t * lp = (uint32_t *) &ESP_WM_LITE_config;
      int lz = ((int) sizeof(ESP_WM_LITE_config))/4;
      while (lz>0) {
        if (*lp++ != 0xFFFFFFFF) break;
        lz--;
      }
      if (lz == 0) {
        ESP_WML_LOGINFO(F("EEPROM: EMPTY!"));
        // set everything to zero, to get empty strings
        memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

        return false;
      }

      return true;
    }


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
      saveConfigData();

#if USE_DYNAMIC_PARAMETERS
      saveDynamicData();
      EEPROM.commit();
#endif
    }

    //////////////////////////////////////////////

    // Return false if init new EEPROM . No more need to try to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid = true;
      bool configDataValid;
      int calChecksum;

      hasConfigData = false;

      EEPROM.begin(EEPROM_SIZE);

      ESP_WML_LOGINFO3(F("EEPROMsz:"), EEPROM_SIZE, F(",DataSz="), totalDataSize);

      // Load config data from EEPROM
      configDataValid = loadConfigData();

      ESP_WML_LOGINFO1( (configDataValid) ? "":"In", F("valid Stored Config Data"));

      if (configDataValid)
      {
        ESP_WML_LOGINFO(F("== Start Stored Config =="));
        displayConfigData(ESP_WM_LITE_config);

        calChecksum = calcChecksum();

        ESP_WML_LOGINFO3(F("CCSum=0x"), String(calChecksum, HEX),
                         F(",RCSum=0x"), String(ESP_WM_LITE_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS

        // Load dynamic data from EEPROM
        dynamicDataValid = loadDynamicData();

        ESP_WML_LOGINFO1( (dynamicDataValid) ? "":"In", F("valid Stored Dynamic Data"));

#endif
      }
      else
      {

        // config data not present: clean start: use default config, when set by user
        restoreDefaultConfiguration();

        // perform basic check on configuration. If valid, return and continue without Config Portal else return false and enter CP
        return isWiFiConfigValid();
      }

      if ( (strcmp(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE) != 0) ||
           (calChecksum != ESP_WM_LITE_config.checkSum) || !dynamicDataValid )
      {
        // Including Credentials CSum
        ESP_WML_LOGINFO(F("InitEEPROM"));

      // If config data is empty or "blank", restore with defaultConfig
        // If there was no config data stored, restore with defaultConfig
        // TODO: also when CSum fail?
       // restoreDefaultConfiguration();

        // doesn't have any configuration
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));

          for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
            strcpy(ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid,   WM_NO_CONFIG);
            strcpy(ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw,     WM_NO_CONFIG);
          }
          strcpy(ESP_WM_LITE_config.board_name, WM_NO_CONFIG);

#if USE_DYNAMIC_PARAMETERS

          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }

#endif

        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);

#if USE_DYNAMIC_PARAMETERS

        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          ESP_WML_LOGDEBUG3(F("g:myMenuItems["), i, F("]="), myMenuItems[i].pdata );
        }

#endif

        // Checksum will be recalculated
        ESP_WM_LITE_config.checkSum = 0;

        saveAllConfigData();

        ESP_WML_LOGINFO(F("== Initialized Config =="));
        displayConfigData(ESP_WM_LITE_config);

        return false;
      }
      else if ( !isWiFiConfigValid() )
      {
        // If SSID, PW ="blank" or NULL, stay in config mode forever until having config Data.
        return false;
      }

      ESP_WML_LOGINFO(F("== Retrieved Config =="));
      displayConfigData(ESP_WM_LITE_config);
      hasConfigData = true;
      return true;
    }

#endif // USE_EEPROM

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
        ESP_WML_LOGWARN1(F("WiFi connected after retries: "), numWiFiReconTries);
        ESP_WML_LOGWARN3(F("SSID="), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
        ESP_WML_LOGWARN3(F("Channel="), WiFi.channel(), F(",IP="), WiFi.localIP() );
      }
      else
      {
        ESP_WML_LOGINFO(F("WiFi not connected"));

#if RESET_IF_NO_WIFI
        // To avoid unnecessary MRD
        mrd->loop();

        resetFunc();
#endif
      }

      return status;
    }

    //////////////////////////////////////////////

    void createHTML(String& root_html_template)
    {
      String pitem;
      pitem.reserve(512);

      pitem = FPSTR(ESP_WM_LITE_HTML_HEAD_START);

#if USING_BOARD_NAME
      if ( (ESP_WM_LITE_config.board_name[0] != 0) && (strcmp(ESP_WM_LITE_config.board_name, WM_NO_CONFIG) != 0) )
      {
        // Replace html title only if configurable board_name is valid.  Otherwise, keep intact
        pitem.replace("ESP_WM_LITE", ESP_WM_LITE_config.board_name);
      } else
#endif
      if ( wmlHostname[0] != 0 )
      {
        // Replace html title only if Hostname is valid
        pitem.replace("ESP_WM_LITE", wmlHostname);
      }

      root_html_template += pitem;

#if USING_CUSTOM_STYLE

      // Using Custom style when not NULL
      if (_CustomHeadStyle)
        root_html_template += FPSTR(_CustomHeadStyle);
      else
        root_html_template += FPSTR(ESP_WM_LITE_HTML_HEAD_STYLE);

#else
      root_html_template += FPSTR(ESP_WM_LITE_HTML_HEAD_STYLE);
#endif

#if USING_CUSTOM_HEAD_ELEMENT

      if (_CustomHeadElement)
        root_html_template += _CustomHeadElement;

#endif

      root_html_template += FPSTR(ESP_WM_LITE_HTML_HEAD_END);

#if SCAN_WIFI_NETWORKS

      ESP_WML_LOGDEBUG1(WiFiNetworksFound, F(" SSIDs found, generating HTML now"));
      // Replace HTML <input...> with <select...>, based on WiFi network scan in startConfigurationMode()
/*
      ListOfSSIDs = "";

      for (int i = 0, list_items = 0; (i < WiFiNetworksFound) && (list_items < MAX_SSID_IN_LIST); i++)
      {
        if (indices[i] == -1)
          continue;     // skip duplicates and those that are below the required quality

        ListOfSSIDs += String(FPSTR(ESP_WM_LITE_OPTION_START)) + String(WiFi.SSID(indices[i]));
        list_items++;   // Count number of suitable, distinct SSIDs to be included in list
      }

      ESP_WML_LOGDEBUG(ListOfSSIDs);

      if (ListOfSSIDs == "")    // No SSID found or none was good enough
        ListOfSSIDs = String(FPSTR(ESP_WM_LITE_OPTION_START)) + String(FPSTR(ESP_WM_LITE_NO_NETWORKS_FOUND));
*/
#endif

      root_html_template += FPSTR(ESP_WM_LITE_FLDSET_START);
      char nr[4] = "0";
      for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
        nr[0]='0'+i;
#if SCAN_WIFI_NETWORKS
        // on selection list, make sure the earlier inputted SSID is at top of the list
        ListOfSSIDs = "";
        String wifiSSID = ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid;
        if ( (wifiSSID.length() <= 4) || (wifiSSID == WM_NO_CONFIG) )
        {
          wifiSSID.clear();
        } else {
          ListOfSSIDs += String(FPSTR(ESP_WM_LITE_OPTION_START)) + wifiSSID;
        }
        for (int i = 0, list_items = 0; (i < WiFiNetworksFound) && (list_items < MAX_SSID_IN_LIST); i++)
        {
          if ((indices[i] >= 0) && (WiFi.SSID(indices[i]) != wifiSSID))
          {
            ListOfSSIDs += String(FPSTR(ESP_WM_LITE_OPTION_START)) + String(WiFi.SSID(indices[i]));
            list_items++;   // Count number of suitable, distinct SSIDs to be included in list
          }
        }
        if (ListOfSSIDs == "")    // No SSID found or none was good enough
          ListOfSSIDs = String(FPSTR(ESP_WM_LITE_OPTION_START)) + String(FPSTR(ESP_WM_LITE_NO_NETWORKS_FOUND));

#if MANUAL_SSID_INPUT_ALLOWED
        pitem = FPSTR(ESP_WM_LITE_HTML_INPUT_WIFI_LIST);
#else
        pitem = FPSTR(ESP_WM_LITE_HTML_INPUT_WIFI_SEL);
#endif
        pitem.replace("[o]", ListOfSSIDs);
#else
        pitem = FPSTR(ESP_WM_LITE_HTML_INPUT_WIFI_IN);
#endif   // SCAN_WIFI_NETWORKS
        pitem += FPSTR(ESP_WM_LITE_HTML_INPUT_WIFI_PW);
        pitem.replace("[n]", nr);
        pitem.replace("[v]", ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid);
        pitem.replace("[p]", ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw);
        ESP_WML_LOGDEBUG1(F("pitem:"), pitem);
        root_html_template += pitem;
      }
      root_html_template += FPSTR(ESP_WM_LITE_FLDSET_END);
#if USING_BOARD_NAME
      pitem = FPSTR(ESP_WM_LITE_HTML_INPUT_BOARD_NAME);
      pitem.replace("[nm]", ESP_WM_LITE_config.board_name);
      root_html_template += pitem;
#endif

#if USE_DYNAMIC_PARAMETERS
      root_html_template += FPSTR(ESP_WM_LITE_FLDSET_START);

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = FPSTR(ESP_WM_LITE_HTML_PARAM);

        pitem.replace("[b]", myMenuItems[i].displayName);
        pitem.replace("[v]", myMenuItems[i].pdata);
        pitem.replace("[i]", myMenuItems[i].id);

        root_html_template += pitem;
      }
      root_html_template += FPSTR(ESP_WM_LITE_FLDSET_END);

#endif

      root_html_template +=  FPSTR(ESP_WM_LITE_HTML_SCRIPT);

      char id[4] = "id0";
      char pw[4] = "pw0";
      for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
        pitem = FPSTR(ESP_WM_LITE_HTML_SCRIPT_ITEM);
        id[2]='0'+i;
        pitem.replace("[d]", id);
        pitem += FPSTR(ESP_WM_LITE_HTML_SCRIPT_ITEM);
        pw[2]='0'+i;
        pitem.replace("[d]", pw);
        root_html_template += pitem;
      }
#if USING_BOARD_NAME
        pitem = FPSTR(ESP_WM_LITE_HTML_SCRIPT_ITEM);
        pitem.replace("[d]", "nm");
        root_html_template += pitem;
#endif

#if USE_DYNAMIC_PARAMETERS

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = FPSTR(ESP_WM_LITE_HTML_SCRIPT_ITEM);

        pitem.replace("[d]", myMenuItems[i].id);

        root_html_template += pitem;
      }

#endif

      root_html_template += FPSTR(ESP_WM_LITE_HTML_SCRIPT_END);
      ESP_WML_LOGDEBUG1(F("createHTML:html size="),root_html_template.length());
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

        if (key == "" && value == "")
        {
          serverSendHeaders();

          ESP_WML_LOGDEBUG1(F("HEAP before createHTML:"),ESP.getFreeHeap());

          String result;
          result.reserve(4096);
          createHTML(result);
          ESP_WML_LOGDEBUG1(F("HEAP after createHTML:"),ESP.getFreeHeap());

          //ESP_WML_LOGDEBUG1(F("h:Repl:"), result);

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;

          ESP_WML_LOGDEBUG1(F("h:HTML page size:"), result.length());
          ESP_WML_LOGDEBUG1(F("h:HTML="), result);

          server->send(200, FPSTR(WM_HTTP_HEAD_TEXT_HTML), result);

          if (server->uri() == "/") {
            // upon requesting the main page, the update flags will be cleared, to be used during save
            memset(&updated_flags, 0, sizeof(updated_flags));
          }

          return;
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
            ESP_WML_LOGWARN(F("h: Error can't alloc memory for menuItemUpdated" ));
          }
        }

#endif

        char wfidnkey[4] = "id0";
        char wfpwnkey[4] = "pw0";
        // reset all flags -> no need
        //memset(updated_flags, 0, sizeof(updated_flags));

        // handle incoming key-value pairs
        for (int n = 0; n < NUM_WIFI_CREDENTIALS; n++) {
          wfidnkey[2] = '0'+n;
          wfpwnkey[2] = '0'+n;
          if (!updated_flags.wificreds[n].wfidn && (key == String(wfidnkey))) {
            ESP_WML_LOGDEBUG1(F("h:repl "), wfidnkey);
            updated_flags.wificreds[n].wfidn = true;
            updated_flags.number_items++;
            strncpy(ESP_WM_LITE_config.WiFi_Creds[n].wifi_ssid, value.c_str(), SSID_MAX_LEN - 1);
          }
          if (!updated_flags.wificreds[n].wfpwn && (key == String(wfpwnkey))) {
            ESP_WML_LOGDEBUG1(F("h:repl "), wfpwnkey);
            updated_flags.wificreds[n].wfpwn = true;
            updated_flags.number_items++;
            strncpy(ESP_WM_LITE_config.WiFi_Creds[n].wifi_pw, value.c_str(), PASS_MAX_LEN - 1);
          }

        }

#if USING_BOARD_NAME
        if (!updated_flags.nm && (key == String("nm")))
        {
          ESP_WML_LOGDEBUG(F("h:repl nm"));
          updated_flags.nm = true;
          updated_flags.number_items++;

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

              updated_flags.number_items++;

              // Actual size of pdata is [maxlen + 1]
              memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
              strncpy(myMenuItems[i].pdata, value.c_str(), myMenuItems[i].maxlen);

              break;
            }
          }
        }

#endif

        ESP_WML_LOGDEBUG1(F("h:items updated ="), updated_flags.number_items);
        ESP_WML_LOGDEBUG3(F("h:key ="), key, ", value =", value);

        server->send(200, FPSTR(WM_HTTP_HEAD_TEXT_HTML), "OK");

#if USE_DYNAMIC_PARAMETERS

        if (updated_flags.number_items == NUM_CONFIGURABLE_ITEMS + NUM_MENU_ITEMS)
#else
        if (updated_flags.number_items == NUM_CONFIGURABLE_ITEMS)
#endif
        {
#if USE_LITTLEFS
          ESP_WML_LOGINFO1(F("h:Updating LittleFS:"), CONFIG_FILENAME);
#elif USE_SPIFFS
          ESP_WML_LOGINFO1(F("h:Updating SPIFFS:"), CONFIG_FILENAME);
#else
          ESP_WML_LOGINFO(F("h:Updating EEPROM. Please wait for reset"));
#endif

          saveAllConfigData();
          ESP_WML_LOGINFO(F("== Saved Config =="));
          displayConfigData(ESP_WM_LITE_config);

          // Done with CP, Clear CP Flag here if forced
          if (isForcedConfigPortal)
            clearForcedCP();

          ESP_WML_LOGWARN(F("h:Rst"));

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
      // ESP32 or ESP8266 is core v3.0.0- is OK either way
      WiFi.softAPConfig(portal_apIP, portal_apIP, IPAddress(255, 255, 255, 0));

      WiFi.softAP(portal_ssid.c_str(), portal_pass.c_str(), channel);

      ESP_WML_LOGINFO3(F("stConf:SSID="), portal_ssid, F(",PW="), portal_pass);
      ESP_WML_LOGINFO3(F("IP="), portal_apIP.toString(), ",ch=", channel);

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
          ESP_WML_LOGDEBUG1(F("Server onNotFound uri "), server->uri());
          handleRequest();
        });

        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      // or SSID, PW, Server,Token ="nothing"
      if (hasConfigData)
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

    int getRSSIasQuality(int RSSI)
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
