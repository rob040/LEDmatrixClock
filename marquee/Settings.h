/**
* Parts Copyright (c) 2018 David Payne
* Parts Copyright (c) 2025 rob040@users.github.com
* This code is licensed under MIT license (see LICENSE.txt for details)
*/

/******************************************************************************
 * This is designed for the Wemos D1 ESP8266, but other ESP8266 boards work as well.
 * Wemos D1 Mini:   https://www.wemos.cc/en/latest/d1/d1_mini.html
 * MAX7219 LED Dot Matrix Module 4-in-1 Display (FC16) For Arduino
 * Matrix Display:  https://www.electroduino.com/max7219-4-in-1-led-dot-matrix-display-module-functions/
 ******************************************************************************/
/******************************************************************************
 * NOTE: The settings here are the default settings for the first loading.
 * After loading you will manage changes to the settings via the Web Interface.
 * If you want to change settings again in the settings.h, you will need to
 * erase the file system on the Wemos or use the “Reset Settings” option in
 * the Web Interface.
 ******************************************************************************/

//Compilation options (can also be set as compiler commandline define)
#ifndef COMPILE_MQTT
#define COMPILE_MQTT 1  // tried and tested
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "LittleFS.h"
#define FS LittleFS
#include <SPI.h>
#include <Adafruit_GFX.h> // --> https://github.com/adafruit/Adafruit-GFX-Library
#include <Max72xxPanel.h> // --> https://github.com/markruys/arduino-Max72xxPanel, fixed locally lib/arduino-Max72xxPanel
#include <pgmspace.h>
#include "OpenWeatherMapClient.h"
#include "TimeNTP.h"
#include "TimeStr.h"
#if COMPILE_MQTT
#include "MqttClient.h"
#endif

// For ESP_WiFiManager_Lite, the include sequence does matter!
//#include <WiFiManager.h> // --> https://github.com/tzapu/WiFiManager
//#include <ESP_WiFiManager_Lite.h> // --> https://github.com/khoih-prog/ESP_WiFiManager_Lite, now updated local version
#include "WmlSettings.h"
#include "ESP_WiFiManager_Lite.h"
#include "DynamicParams.h"

//******************************
// Hard(-ware related) settings
//******************************
// Display Settings
// CLK -> D5 (SCK)  GPIO14 on ESP8266
// CS  -> D6        GPIO12 on ESP8266
// DIN -> D7 (MOSI) GPIO13 on ESP8266
const int pinCS = D6; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int displayIntensity = 1;  //(This can be set from 0 - 15)
int displayWidth = 4; // default 4 for standard 4 x 1 display Max size of 16
const int displayHeight = 1; // default 1 for a single row height (do not change, this SW does not support multiple lines, nor double hight chars)
// set ledRotation for LED Display panels, 0: no rotation, 1: 90 degrees clockwise, 2: 180 degrees, 3: 90 degrees counter clockwise (default)
const int ledRotation = 3;

// Change the LED_ONBOARD to the pin you wish to use if other than the Built-in LED
#define LED_ONBOARD  LED_BUILTIN // LED_BUILTIN is the on-board LED on the ESP12E module on the Wemos
#define LED_ON  LOW   // define polarity of LED_ONBOARD
#define LED_OFF HIGH  // define polarity of LED_ONBOARD

// uncomment define BUZZER pin when BUZZER is installed
// #define BUZZER_PIN  D2


// Configuration.ini file is stored in local filesystem
#define CONFIG "/conf.txt"



//******************************
// Configuration default Settings
// (no need to change; these can be set on configuration page)
//******************************

String owmApiKey = ""; // Your free API Key from http://openweathermap.org/ (registration required; use Free Access for everyone )
// Default GEO Location (use http://openweathermap.org/find to find location name being "cityname,countrycode" or "city ID" or GPS "latitude,longitude")
String geoLocation = "Breda,NL";
String marqueeMessage = "";

// Default Weather Settings
bool showTemperature = true;
bool showDate = false;
bool showCity = true;
bool showCondition = true;
bool showHumidity = true;
bool showWind = true;
bool showWindDir = true;
bool showPressure = false;
bool showHighLow = true;

const int staticDisplayTime = 5000;  // static display time per item, in ms
bool isStaticDisplay = false; // static display above SHOW_* items
bool isMetric = true; // false = Imperial and true = Metric
bool is24hour = true; // 24 hour clock is displayed, false = 12 hour clock (for configuration, 24h time is always used)
bool isPmIndicator = true; // Show PM indicator on Clock when in AM/PM mode
bool isSysLed = true; // flash onboard LED on system actions
const int WEBSERVER_PORT = 80; // The port you can access this device on over HTTP
const bool isWebserverEnabled = true;  // Device will provide a web interface via http://[ip]:[port]/
bool isBasicAuth = false;  // Use Basic Authorization for Configuration security on Web Interface
char www_username[32] = {"admin"};  // User account for the Web Interface
char www_password[32] = {"password"};  // Password for the Web Interface
int refreshDataInterval = 15;  // Time in minutes between data refresh (default 15 minutes)
int displayScrollingInterval = 1; // Time in minutes between scrolling data (default 1 minutes and max is 10)
int displayScrollSpeed = 25; // In milliseconds -- Configurable by the web UI (slow = 35, normal = 25, fast = 15, very fast = 5)
bool flashOnSeconds = true; // when true the ':' character in the time will blink as a seconds indicator

// New wide clock style config, different screen formats for 8+ tiles:
int wideClockStyle = 1; // 1=HH:MM, 2=HH:MM:SS, 3=HH:MM *CF, 4=HH:MM %RH, 5=mm dd HH:MM, 6=HH:MM mmdd, 7=HH:MM ddmm, 8=HH:MM WwwDD (or HH:MM Www DD on >= 10 tile display)

#define  WIDE_CLOCK_STYLE_HHMM      1
#define  WIDE_CLOCK_STYLE_HHMMSS    2
#define  WIDE_CLOCK_STYLE_HHMM_CF   3
#define  WIDE_CLOCK_STYLE_HHMM_RH   4
#define  WIDE_CLOCK_STYLE_MMDD_HHMM 5
#define  WIDE_CLOCK_STYLE_HHMM_MMDD 6
#define  WIDE_CLOCK_STYLE_HHMM_DDMM 7
#define  WIDE_CLOCK_STYLE_HHMM_WWWDD 8
#define  WIDE_CLOCK_STYLE_LAST      WIDE_CLOCK_STYLE_HHMM_WWWDD
#define  WIDE_CLOCK_STYLE_FIRST     WIDE_CLOCK_STYLE_HHMM

/* Quiet period setting*/
#define TIME_HHMM(hh,mm) ((hh)*3600+(mm)*60)
#define QTM_DISABLED        0
#define QTM_DISPLAYOFF      1
#define QTM_DIMMED          2
#define QTM_DIMMED_NOSCROLL 3

int quietTimeStart = TIME_HHMM(23,00); // 24 Hour Format HH:MM -- make negative for always on. Both must be set to work.
int quietTimeEnd = TIME_HHMM(06,30);   // 24 Hour Format HH:MM -- make negative for always on.
int quietTimeMode = QTM_DIMMED_NOSCROLL;
int quietTimeDimlevel = 0;

#if COMPILE_MQTT
// Mqtt add scrolling messages with Mqtt
bool isMqttEnabled = false;             // Set true to display mqtt messages
String MqttServer = "";               // IP or Address only (DO NOT include http://)
int MqttPort = 1883;                  // Port of your mqtt server (default 1883)
String MqttTopic = "display/message"; // Topic on which to listen
String  MqttAuthUser, MqttAuthPass;   // mqtt server authentication
#endif

const bool isOTAenabled = true;    // this will allow you to load firmware to the device over WiFi (see OTA for ESP8266)
String OTA_Password = "";     // Set an OTA password here -- leave blank if you don't want to be prompted for password

//blue-grey
String themeColor = "blue-grey"; // this can be changed later in the web interface.

//******************************
// End Settings
//******************************
