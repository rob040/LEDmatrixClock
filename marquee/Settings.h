/** The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/******************************************************************************
 * This is designed for the Wemos D1 ESP8266
 * Wemos D1 Mini:  https://amzn.to/2qLyKJd
 * MAX7219 Dot Matrix Module 4-in-1 Display For Arduino
 * Matrix Display:  https://amzn.to/2HtnQlD
 ******************************************************************************/
/******************************************************************************
 * NOTE: The settings here are the default settings for the first loading.
 * After loading you will manage changes to the settings via the Web Interface.
 * If you want to change settings again in the settings.h, you will need to
 * erase the file system on the Wemos or use the “Reset Settings” option in
 * the Web Interface.
 ******************************************************************************/

//Compilation options (can also be set as compiler commandline define)
#ifndef COMPILE_NEWS
#define COMPILE_NEWS 0  // this newsservice is not free but rather expensive to use
#endif
#ifndef COMPILE_PIHOLE
#define COMPILE_PIHOLE 0 // current implementation does not work anymore, needs updating
#endif
#ifndef COMPILE_OCTOPRINT
#define COMPILE_OCTOPRINT 0  // unsure if it works, untested
#endif
#ifndef COMPILE_MQTT
#define COMPILE_MQTT 1  // tried and tested
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h> // --> https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "LittleFS.h"
#define FS LittleFS
#include <SPI.h>
#include <Adafruit_GFX.h> // --> https://github.com/adafruit/Adafruit-GFX-Library
#include <Max72xxPanel.h> // --> https://github.com/markruys/arduino-Max72xxPanel
#include <pgmspace.h>
#include "OpenWeatherMapClient.h"
#include "TimeNTP.h"
#include "TimeStr.h"
#if COMPILE_NEWS
#include "NewsApiClient.h"
#endif
#if COMPILE_OCTOPRINT
#include "OctoPrintClient.h"
#endif
#if COMPILE_PIHOLE
#include "PiHoleClient.h"
#endif
#if COMPILE_MQTT
#include "MqttClient.h"
#endif

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
/* set ledRotation for LED Display panels (3 is default)
0: no rotation
1: 90 degrees clockwise
2: 180 degrees
3: 90 degrees counter clockwise (default)
*/
const int ledRotation = 3;

// Change the LED_ONBOARD to the pin you wish to use if other than the Built-in LED
#define LED_ONBOARD  LED_BUILTIN // LED_BUILTIN is the on-board LED on the ESP12E module on the Wemos
#define LED_ON  LOW   // define polarity of LED_ONBOARD
#define LED_OFF HIGH  // define polarity of LED_ONBOARD

// uncomment define BUZZER pin when BUZZER is installed
// #define BUZZER_PIN  D2

// ESP Accesspoint hostname by WifiManager; The device MAC address will be added
#define AP_HOSTNAME_BASE "CLOCK-"
// Configuration.ini file is stored in local filesystem
#define CONFIG "/conf.txt"



//******************************
// Configuration default Settings
// (no need to change; these can be set on configuration page)
//******************************

String APIKEY = ""; // Your API Key from http://openweathermap.org/
// Default City Location (use http://openweathermap.org/find to find city ID)
int CityID = 2759794;
String marqueeMessage = "";

// (some) Default Weather Settings
//FIXME TODO: do not use uppercase for variables
//FIXME: inconsistent use of "Show" and "Display"; sometimes it refers to LED display, and sometimes to WebPage
boolean SHOW_TEMPERATURE = true;
boolean SHOW_DATE = false;
boolean SHOW_CITY = true;
boolean SHOW_CONDITION = true;
boolean SHOW_HUMIDITY = true;
boolean SHOW_WIND = true;
boolean SHOW_WINDDIR = true;
boolean SHOW_PRESSURE = false;
boolean SHOW_HIGHLOW = true;

const int staticDisplayTime = 5000;  // static display time per item, in ms
boolean isStaticDisplay = false; // static display above SHOW_* items
boolean IS_METRIC = true; // false = Imperial and true = Metric
boolean IS_24HOUR = true; // 24 hour clock is displayed, false = 12 hour clock (for configuration, 24h time is always used)
boolean IS_PM = true; // Show PM indicator on Clock when in AM/PM mode
boolean isSysLed = true; // flash onboard LED on system actions
const int WEBSERVER_PORT = 80; // The port you can access this device on over HTTP
const boolean WEBSERVER_ENABLED = true;  // Device will provide a web interface via http://[ip]:[port]/
boolean IS_BASIC_AUTH = false;  // Use Basic Authorization for Configuration security on Web Interface
char www_username[32] = {"admin"};  // User account for the Web Interface
char www_password[32] = {"password"};  // Password for the Web Interface
int refreshDataInterval = 15;  // Time in minutes between data refresh (default 15 minutes)
int displayScrollingInterval = 1; // Time in minutes between scrolling data (default 1 minutes and max is 10)
int displayScrollSpeed = 25; // In milliseconds -- Configurable by the web UI (slow = 35, normal = 25, fast = 15, very fast = 5)
boolean flashOnSeconds = true; // when true the ':' character in the time will blink as a seconds indicator

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

#if COMPILE_NEWS
boolean NEWS_ENABLED = false;
String NEWS_API_KEY = ""; // Get your News API Key from https://newsapi.org
String NEWS_SOURCE = "reuters";  // https://newsapi.org/sources to get full list of news sources available
#endif

String timeDisplayTurnsOn = "06:30";  // 24 Hour Format HH:MM -- Leave blank for always on. (ie 05:30)
String timeDisplayTurnsOff = "23:00"; // 24 Hour Format HH:MM -- Leave blank for always on. Both must be set to work.

#if COMPILE_OCTOPRINT
// OctoPrint Monitoring -- Monitor your 3D printer OctoPrint Server
boolean OCTOPRINT_ENABLED = false;
boolean OCTOPRINT_PROGRESS = true;
String OctoPrintApiKey = "";  // ApiKey from your User Account on OctoPrint
String OctoPrintServer = "";  // IP or Address of your OctoPrint Server (DO NOT include http://)
int OctoPrintPort = 80;       // the port you are running your OctoPrint server on (usually 80);
String OctoAuthUser = "";     // only used if you have HAproxy or basic authentication turned on (not default)
String OctoAuthPass = "";     // only used with HAproxy or basic auth (only needed if you must authenticate)
#endif

#if COMPILE_PIHOLE
// Pi-hole Client -- monitor basic stats from your Pi-hole server (see http://pi-hole.net)
boolean USE_PIHOLE = false;   // Set true to display your Pi-hole details
String PiHoleServer = "";     // IP or Address only (DO NOT include http://)
int PiHolePort = 80;          // Port of your Pi-hole address (default 80)
String PiHoleApiKey = "";   // Optional -- only needed to see top blocked clients
#endif

#if COMPILE_MQTT
// Mqtt add scrolling messages with Mqtt
boolean USE_MQTT = false;             // Set true to display mqtt messages
String MqttServer = "";               // IP or Address only (DO NOT include http://)
int MqttPort = 1883;                  // Port of your mqtt server (default 1883)
String MqttTopic = "display/message"; // Topic on which to listen
String  MqttAuthUser, MqttAuthPass;   // mqtt server authentication
#endif

const boolean ENABLE_OTA = true;    // this will allow you to load firmware to the device over WiFi (see OTA for ESP8266)
String OTA_Password = "";     // Set an OTA password here -- leave blank if you don't want to be prompted for password

//blue-grey
String themeColor = "blue-grey"; // this can be changed later in the web interface.

//******************************
// End Settings
//******************************
