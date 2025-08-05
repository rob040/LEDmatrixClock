/****************************************************************************************************************************
  defines.h
  For ESP8266 / ESP32 boards

  ESP_WiFiManager_Lite (https://github.com/rob040/ESP_WiFiManagerLite2) is a library
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager_Lite
  Licensed under MIT license
 *****************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#if !( ESP8266 || ESP32)
  #error This code is intended to run only on the ESP8266/ESP32 boards ! Please check your Tools->Board setting.
#endif

/* Comment this out to disable prints and save space */
#define ESP_WM_LITE_DEBUG_OUTPUT      Serial

#define _ESP_WM_LITE_LOGLEVEL_        2

// use builtin LED to show configuration mode
#define USE_LED_BUILTIN               true

#define MULTIRESETDETECTOR_DEBUG      true

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define MRD_TIMEOUT                   10

// RTC Memory Address for the DoubleResetDetector to use
#define MRD_ADDRESS                   0


/////////////////////////////////////////////

// WML configuration storage; Choose one and only one
#define USE_EEPROM                                true
#define USE_LITTLEFS                              false
#define USE_SPIFFS                                false

/////////////////////////////////////////////

// Add custom headers from v1.2.0
#define USING_CUSTOM_STYLE           true
#define USING_CUSTOM_HEAD_ELEMENT    true
#define USING_CORS_FEATURE            true

/////////////////////////////////////////////

// Force some params
#define TIMEOUT_RECONNECT_WIFI                    10000L

// set to true for debug-test: FORCES default values to be loaded each run.
#define LOAD_DEFAULT_CONFIG_DATA                  false

// Permit running CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET times before reset hardware
// to permit user another chance to config. Only if Config Data is valid.
// If Config Data is invalid, this has no effect as Config Portal will persist
#define RESET_IF_CONFIG_TIMEOUT                   true

// Permitted range of user-defined CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET between 2-100
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET    5

// Config Timeout 120s (default 60s). Applicable only if Config Data is Valid
#define CONFIG_TIMEOUT                            120000L

/////////////////////////////////////////////

// Permit input only one set of WiFi SSID/PWD. The other can be "NULL or "blank"
// Default is false (if not defined) => must input 2 sets of SSID/PWD
#define REQUIRE_ONE_SET_SSID_PW               true    //false

// Max times to try WiFi per loop() iteration. To avoid blocking issue in loop()
// Default 1 if not defined, and minimum 1.
#define MAX_NUM_WIFI_RECON_TRIES_PER_LOOP     2

// Default no interval between reconnect WiFi if lost
// Max permitted interval will be 10mins
// Uncomment to use. Be careful, WiFi reconnect will be delayed if using this method
// Only use whenever urgent tasks in loop() can't be delayed. But if so, it's better you have to rewrite your code, e.g. using higher priority tasks.
//#define WIFI_RECON_INTERVAL                   30000

/////////////////////////////////////////////

// Permit reset hardware if no WiFi to permit user another chance to access Config Portal.
#define RESET_IF_NO_WIFI              false

/////////////////////////////////////////////

#define USE_DYNAMIC_PARAMETERS        true

/////////////////////////////////////////////

#define SCAN_WIFI_NETWORKS                  true

// To be able to manually input SSID, not from a scanned SSID lists
#define MANUAL_SSID_INPUT_ALLOWED           true

// From 2-15
  #define MAX_SSID_IN_LIST                  8

/////////////////////////////////////////////

// Optional, to use Board Name in Menu
#define USING_BOARD_NAME                    true

/////////////////////////////////////////////

#include <ESP_WiFiManager_Lite.h>

#if ESP8266
  #define HOST_NAME   "ESP8266-Controller"
#else
  #define HOST_NAME   "ESP32-Controller"
#endif

#ifdef LED_BUILTIN
  #define LED_PIN     LED_BUILTIN
#else
  #define LED_PIN     13
#endif

#endif      //defines_h
