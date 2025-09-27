/*
 * Definitions for ESP_WiFiManager_lite.h
 *
 *
 */
#pragma once

// Wifi Manager Lite
// ESP Accesspoint hostname by ESP_WifiManager_Lite; The device MAC address will be added
#define AP_HOSTNAME_BASE                          "CLOCK-"
#define ESP_WM_LITE_DEBUG_OUTPUT                  Serial
#define _ESP_WM_LITE_LOGLEVEL_                    -1
#define USE_LED_BUILTIN                           true
#define USE_DYNAMIC_PARAMETERS                    false
#define USING_CUSTOM_STYLE                        false
#define USING_CUSTOM_HEAD_ELEMENT                 false
#define USING_CORS_FEATURE                        false
#define NUM_WIFI_CREDENTIALS                      4
#define TIMEOUT_RECONNECT_WIFI                    10000
// set to true for debug-test: FORCES default values to be loaded each run.
#define LOAD_DEFAULT_CONFIG_DATA                  false
// Permit running CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET times before reset hardware
// to permit user another chance to config. Only if Config Data is valid.
// If Config Data is invalid, this has no effect as Config Portal will persist
#define RESET_IF_CONFIG_TIMEOUT                   true
// Permitted range of user-defined CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET between 2-100
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET    5
// Config Timeout 120s (default 60s). Applicable only if Config Data is Valid
#define CONFIG_TIMEOUT                            120000
// Permit input only one set of WiFi SSID/PWD. The other can be "NULL or "blank"
// Default is false (if not defined) => must input ALL sets of SSID/PWD WIFI_CREDENTIALS
// TODO: remove this option and input check
#define REQUIRE_ONE_SET_SSID_PW                   true
// Max times to try WiFi per loop() iteration. To avoid blocking issue in loop()
// Default 1 if not defined, and minimum 1.
#define MAX_NUM_WIFI_RECON_TRIES_PER_LOOP         2
// Default no interval between recon WiFi if lost
// Max permitted interval will be 10mins
// Uncomment to use. Be careful, WiFi reconnect will be delayed if using this method
// Only use whenever urgent tasks in loop() can't be delayed. But if so, it's better you have to rewrite your code, e.g. using higher priority tasks.
//#define WIFI_RECON_INTERVAL                       30000
// Permit reset hardware if no WiFi to permit user another chance to access Config Portal.
#define RESET_IF_NO_WIFI                          false

// The next two defines determine the WiFi SSID input field on config html page;
// With SCAN_WIFI_NETWORKS=true and MANUAL_SSID_INPUT_ALLOWED=true, a free text input field with SSID proposal list filtered with text typed is presented.
// With SCAN_WIFI_NETWORKS=true and MANUAL_SSID_INPUT_ALLOWED=false, a traditional drop-downbox is presented.
// With SCAN_WIFI_NETWORKS=false, a text input field is presented without any aid of present WiFi stations.

// show list of possible AP to choose from; when false,
#define SCAN_WIFI_NETWORKS                        true
// To be able to manually input SSID, not from scanned SSID lists
#define MANUAL_SSID_INPUT_ALLOWED                 true
// List nearby SSID's found, From 2-15
#define MAX_SSID_IN_LIST                          8
// Optional, to use and modify Board Name in Menu
#define USING_BOARD_NAME                          true


// We are using MRD (Multi reset detector): after 3 resets within 10 seconds after startup, we enter Wifi Manager AP mode
#define MULTIRESETDETECTOR_DEBUG                  (_ESP_WM_LITE_LOGLEVEL_>1)
#define MRD_TIMES                                 3
#define MRD_TIMEOUT                               10

// WML configuration storage; Choose one and only one
#define USE_EEPROM                                true
#define USE_LITTLEFS                              false
#define USE_SPIFFS                                false

// For testing, create a default config
#define WML_DEFAULT_CONFIG {  "ESP8266", \
  "SSID1",  "password1",\
  "SSID2",  "password2",\
  "SSID3",  "password3",\
  "SSID4",  "password4",\
  "LED matrix Clock", 0 };

//#include <ESP_WiFiManager_Lite.h> // --> https://github.com/khoih-prog/ESP_WiFiManager_Lite (Archived) --> ../lib/ESP_WiFiManager_Lite.h
//#include "DynamicParams.h"