## ESP_MultiResetDetector

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_MultiResetDetector.svg?)](https://www.ardu-badge.com/ESP_MultiResetDetector)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP_MultiResetDetector.svg)](https://github.com/khoih-prog/ESP_MultiResetDetector/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP_MultiResetDetector/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP_MultiResetDetector.svg)](http://github.com/khoih-prog/ESP_MultiResetDetector/issues)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>

---
---

## Table of Contents

* [Why do we need this ESP_MultiResetDetector library](#why-do-we-need-this-esp_multiresetdetector-library)
  * [Features](#features)
  * [Currently supported Boards](#currently-supported-boards)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Usage](#howto-usage)
* [Examples](#examples)
  * [ 1. ConfigOnMultiReset](examples/ConfigOnMultiReset)
  * [ 2. ConfigOnMRD_ESP32_minimal](examples/ConfigOnMRD_ESP32_minimal)
  * [ 3. ConfigOnMRD_ESP8266_minimal](examples/ConfigOnMRD_ESP8266_minimal)
  * [ 4. minimal](examples/minimal)
  * [ 5. checkWaitingMRD](examples/checkWaitingMRD) **New**
 * [Examples from other libraries](#examples-from-other-libraries)
  * [ 1. ESP_WiFiManager Library](https://github.com/khoih-prog/ESP_WiFiManager)
    * [ 1. ConfigOnDoubleReset](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDoubleReset)
    * [ 2. ConfigOnDRD_FS_MQTT_Ptr](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_FS_MQTT_Ptr)
    * [ 3. ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP32_FSWebServer_DRD)
    * [ 4. ESP_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP_FSWebServer_DRD)
    * [ 5. ConfigOnDRD_ESP32_minimal](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_ESP32_minimal)
    * [ 6. ConfigOnDRD_ESP8266_minimal](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_ESP8266_minimal)
    * [ 7. ConfigOnDRD_FS_MQTT_Ptr_Complex](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_FS_MQTT_Ptr_Complex)
    * [ 8. ConfigOnDRD_FS_MQTT_Ptr_Medium](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_FS_MQTT_Ptr_Medium)
  * [ 2. ESPAsync_WiFiManager Library](https://github.com/khoih-prog/ESPAsync_WiFiManager)
    * [ 1. Async_ConfigOnDoubleReset](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDoubleReset)
    * [ 2. Async_ConfigOnDRD_FS_MQTT_Ptr](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_FS_MQTT_Ptr)
    * [ 3. Async_ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ESP32_FSWebServer_DRD)
    * [ 4. Async_ESP_FSWebServer_DRD](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ESP_FSWebServer_DRD)
    * [ 5. Async_ConfigOnDRD_ESP32_minimal](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_ESP32_minimal)
    * [ 6. Async_ConfigOnDRD_ESP8266_minimal](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_ESP8266_minimal)
    * [ 7. Async_ConfigOnDRD_FS_MQTT_Ptr_Complex](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_FS_MQTT_Ptr_Complex)
    * [ 8. Async_ConfigOnDRD_FS_MQTT_Ptr_Medium](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_FS_MQTT_Ptr_Medium)
  * [Many other libraries are depending on this library's DRD and MRD feature](#many-other-libraries-are-depending-on-this-librarys-drd-and-mrd-feature)
    * [ 1. Blynk_WM](https://github.com/khoih-prog/Blynk_WM)
    * [ 2. Blynk_Async_WM](https://github.com/khoih-prog/Blynk_Async_WM)
    * [ 3. BlynkEthernet_WM](https://github.com/khoih-prog/BlynkEthernet_WM)
    * [ 4. BlynkESP32_BT_WF](https://github.com/khoih-prog/BlynkESP32_BT_WF)
    * [ 5. BlynkGSM_Manager](https://github.com/khoih-prog/BlynkEthernet_WM)
    * [ 6. Blynk_Async_ESP32_BT_WF](https://github.com/khoih-prog/Blynk_Async_ESP32_BT_WF)
    * [ 7. Blynk_Async_GSM_Manager](https://github.com/khoih-prog/Blynk_Async_GSM_Manager)
    * [ 8. Ethernet_Manager](https://github.com/khoih-prog/Ethernet_Manager)
* [Example checkWaitingMRD](#Example-checkWaitingMRD)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ESP32_FSWebServer_DRD on ESP32_DEV](#1-esp32_fswebserver_drd-on-esp32_dev)
  * [2. minimal on ESP32_DEV](#2-minimal-on-esp32_dev)
    * [ 2.1 Data Corrupted => reset to 0](#21-data-corrupted--reset-to-0)
    * [ 2.2 Reset Detected => Reporting 1](#22-reset-detected--reporting-1)
    * [ 2.3 Reset Detected => Reporting 2](#23-reset-detected--reporting-2)
    * [ 2.4 Reset Detected => Reporting 3](#24-reset-detected--reporting-3)
    * [ 2.5 Reset Detected => Reporting 4](#25-reset-detected--reporting-4)
    * [ 2.6 Reset Detected => Reporting 5. Multi Reset Detected](#26-reset-detected--reporting-5-multi-reset-detected)
    * [ 2.7 Timed out => reset to 1](#27-timed-out--reset-to-1)
    * [ 2.8 Reset Detected => Reporting 1](#28-reset-detected--reporting-1)
  * [3. minimal on ESP8266_NODEMCU](#3-minimal-on-esp8266_nodemcu)
    * [ 3.1 Data Corrupted => reset to 0](#31-data-corrupted--reset-to-0)
    * [ 3.2 Reset Detected => Reporting 1](#32-reset-detected--reporting-1)
    * [ 3.3 Reset Detected => Reporting 2](#33-reset-detected--reporting-2)
    * [ 3.4 Reset Detected => Reporting 3](#34-reset-detected--reporting-3)
    * [ 3.5 Reset Detected => Reporting 4](#35-reset-detected--reporting-4)
    * [ 3.6 Reset Detected => Reporting 5. Multi Reset Detected](#36-reset-detected--reporting-5-multi-reset-detected)
    * [ 3.7 Timed out => reset to 1](#37-timed-out--reset-to-1)
    * [ 3.8 Reset Detected => Reporting 1](#38-reset-detected--reporting-1)
  * [4. ESPAsync_WiFi using LittleFS on ESP32S3_DEV](#4-ESPAsync_WiFi-using-LittleFS-on-ESP32S3_DEV)
* [Libraries using ESP_MultiResetDetector, ESP_DoubleResetDetector or DoubleResetDetector_Generic library](#libraries-using-esp_multiresetdetector-esp_doubleresetdetector-or-doubleresetdetector_generic-library)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Why do we need this [ESP_MultiResetDetector library](https://github.com/khoih-prog/ESP_MultiResetDetector)

#### Features

[**ESP_MultiResetDetector**](https://github.com/khoih-prog/ESP_MultiResetDetector) is a library for the **ESP8266 and ESP32** boards to detects a **configurable multi reset**, within configurable timeout (default 10s) seconds, so that an alternative start-up mode can be used. Example use cases are to allow re-configuration of a device's WiFi / MQTT / Blynk credentials or to count the number of resets within a pre-determined timed.

This library is based on, modified, bug-fixed and improved from [`Stephen Denne's DoubleResetDetector`](https://github.com/datacute/DoubleResetDetector) to add support to ESP8266 and ESP32 using EEPROM, SPIFFS and LittleFS besides original RTC.

Currently, [`DoubleResetDetector`](https://github.com/datacute/DoubleResetDetector) only supports ESP8266 using RTC memory.
 
This library can be used to detect a **multi reset within a predetermined time to force the program to enter a special operation** such as Config Portal, Clear Default Data, etc., using :

1. EEPROM, SPIFFS or LittleFS for ESP8266 and ESP32 boards.
2. RTC memory for ESP8266 boards (unadvised).

#### Currently supported Boards

This [**ESP_MultiResetDetector** library](https://github.com/khoih-prog/ESP_MultiResetDetector) currently supports these following boards:

 1. **ESP32, ESP32_C3, ESP32_S2 and ESP32_S3 boards, using EEPROM, SPIFFS or LittleFS**.
 2. **ESP8266 boards RTC memory, EEPROM, SPIFFS or LittleFS**
 
---
---


## Prerequisites

1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
2. [`ESP32 Core 2.0.4+`](https://github.com/espressif/arduino-esp32) for ESP32-based boards. [![Latest release](https://img.shields.io/github/release/espressif/arduino-esp32.svg)](https://github.com/espressif/arduino-esp32/releases/latest/)
3. [`ESP8266 Core 3.0.2+`](https://github.com/esp8266/Arduino) for ESP8266-based boards. [![Latest release](https://img.shields.io/github/release/esp8266/Arduino.svg)](https://github.com/esp8266/Arduino/releases/latest/). SPIFFS is deprecated from ESP8266 core 2.7.1+, to use LittleFS. 
4. [`LittleFS_esp32 v1.0.6+`](https://github.com/lorol/LITTLEFS) for ESP32-based boards using LittleFS with ESP32 core **v1.0.5-**. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/LittleFS_esp32.svg?)](https://www.ardu-badge.com/LittleFS_esp32). **Notice**: This [`LittleFS_esp32 library`](https://github.com/lorol/LITTLEFS) has been integrated to Arduino [ESP32 core v1.0.6+](https://github.com/espressif/arduino-esp32/tree/master/libraries/LITTLEFS) and **you don't need to install it if using ESP32 core v1.0.6+**

---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `ESP_MultiResetDetector`, then select / install the latest version. You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_MultiResetDetector.svg?)](https://www.ardu-badge.com/ESP_MultiResetDetector) for more detailed instructions.

### Manual Install

1. Navigate to [ESP_MultiResetDetector](https://github.com/khoih-prog/ESP_MultiResetDetector) page.
2. Download the latest release `ESP_MultiResetDetector-main.zip`.
3. Extract the zip file to `ESP_MultiResetDetector-main` directory 
4. Copy the whole `ESP_MultiResetDetector-main` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**ESP_MultiResetDetector** library](https://registry.platformio.org/libraries/khoih-prog/ESP_MultiResetDetector) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/ESP_MultiResetDetector/installation). Search for **ESP_MultiResetDetector** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

#### HOWTO Usage

How to use

```cpp
// These defines must be put before #include <ESP_MultiResetDetector.h>
// to select where to store MultiResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM, LITTLEFS or SPIFFS)
// Otherwise, library will use default EEPROM storage

#ifdef ESP8266
  #define ESP8266_MRD_USE_RTC     false   //true
#endif

#define ESP_MRD_USE_LITTLEFS    true
#define ESP_MRD_USE_SPIFFS      false
#define ESP_MRD_USE_EEPROM      false

#define MULTIRESETDETECTOR_DEBUG       true  //false

// These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
// Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
// Number of subsequent resets during MRD_TIMEOUT to activate
#define MRD_TIMES               5

// Number of seconds after reset during which a 
// subsequent reset will be considered a multi reset.
#define MRD_TIMEOUT             10

// RTC/EEPROM Memory Address for the MultiResetDetector to use
#define MRD_ADDRESS             0

#include <ESP_MultiResetDetector.h>      //https://github.com/khoih-prog/ESP_MultiResetDetector

MultiResetDetector* mrd;

#ifdef ESP32

  // For ESP32
  #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #endif

  #define LED_OFF     LOW
  #define LED_ON      HIGH

#else

  // For ESP8266
  #define LED_ON      LOW
  #define LED_OFF     HIGH

#endif

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial); 
  delay(200);
  
  Serial.print(F("\nStarting ESP_MultiResetDetector minimal on ")); Serial.print(ARDUINO_BOARD);

#if ESP_MRD_USE_LITTLEFS
  Serial.println(F(" using LittleFS"));
#elif ESP_MRD_USE_SPIFFS
  Serial.println(F(" using SPIFFS"));
#else
  Serial.println(F(" using EEPROM"));
#endif
  
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
  
  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);

  if (mrd->detectMultiReset()) 
  {
    Serial.println("Multi Reset Detected");
    digitalWrite(LED_BUILTIN, LED_ON);
  } 
  else 
  {
    Serial.println("No Multi Reset Detected");
    digitalWrite(LED_BUILTIN, LED_OFF);
  }

}

void loop()
{
  // Call the multi reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call mrd.stop() when you wish to no longer
  // consider the next reset as a multi reset.
  mrd->loop();
}
```

---
---

### Examples

1. [ConfigOnMultiReset](examples/ConfigOnMultiReset)
2. [ConfigOnMRD_ESP32_minimal](examples/ConfigOnMRD_ESP32_minimal)
3. [ConfigOnMRD_ESP8266_minimal](examples/ConfigOnMRD_ESP8266_minimal)
4. [minimal](examples/minimal)

### Examples from other libraries

#### 1.[ESP_WiFiManager Library](https://github.com/khoih-prog/ESP_WiFiManager)

* [ 1. ConfigOnDoubleReset](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDoubleReset)
* [ 2. ConfigOnDRD_FS_MQTT_Ptr](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_FS_MQTT_Ptr)
* [ 3. ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP32_FSWebServer_DRD)
* [ 4. ESP_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP_FSWebServer_DRD)
* [ 5. ConfigOnDRD_ESP32_minimal](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_ESP32_minimal)
* [ 6. ConfigOnDRD_ESP8266_minimal](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_ESP8266_minimal)
* [ 7. ConfigOnDRD_FS_MQTT_Ptr_Complex](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_FS_MQTT_Ptr_Complex)
* [ 8. ConfigOnDRD_FS_MQTT_Ptr_Medium](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnDRD_FS_MQTT_Ptr_Medium)
  
    
#### 2. [ESPAsync_WiFiManager Library](https://github.com/khoih-prog/ESPAsync_WiFiManager)

* [ 1. Async_ConfigOnDoubleReset](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDoubleReset)
* [ 2. Async_ConfigOnDRD_FS_MQTT_Ptr](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_FS_MQTT_Ptr)
* [ 3. Async_ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ESP32_FSWebServer_DRD)
* [ 4. Async_ESP_FSWebServer_DRD](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ESP_FSWebServer_DRD)
* [ 5. Async_ConfigOnDRD_ESP32_minimal](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_ESP32_minimal)
* [ 6. Async_ConfigOnDRD_ESP8266_minimal](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_ESP8266_minimal)
* [ 7. Async_ConfigOnDRD_FS_MQTT_Ptr_Complex](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_FS_MQTT_Ptr_Complex)
* [ 8. Async_ConfigOnDRD_FS_MQTT_Ptr_Medium](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ConfigOnDRD_FS_MQTT_Ptr_Medium)

and there are many more.

#### Many other libraries are depending on this library's DRD and MRD feature

  All examples of these following libraries are using DRD/MRD feature of this [ESP_DoubleResetDetector Library](https://github.com/khoih-prog/ESP_DoubleResetDetector) or [ESP_MultiResetDetector Library](https://github.com/khoih-prog/ESP_MultiResetDetector)


* [ 1. Blynk_WM](https://github.com/khoih-prog/Blynk_WM)
* [ 2. Blynk_Async_WM](https://github.com/khoih-prog/Blynk_Async_WM)
* [ 3. BlynkEthernet_WM](https://github.com/khoih-prog/BlynkEthernet_WM)
* [ 4. BlynkESP32_BT_WF](https://github.com/khoih-prog/BlynkESP32_BT_WF)
* [ 5. BlynkGSM_Manager](https://github.com/khoih-prog/BlynkEthernet_WM)
* [ 6. Blynk_Async_ESP32_BT_WF](https://github.com/khoih-prog/Blynk_Async_ESP32_BT_WF)
* [ 7. Blynk_Async_GSM_Manager](https://github.com/khoih-prog/Blynk_Async_GSM_Manager)
* [ 8. Ethernet_Manager](https://github.com/khoih-prog/Ethernet_Manager)


---
---

### Example [checkWaitingMRD](examples/checkWaitingMRD)

https://github.com/khoih-prog/ESP_MultiResetDetector/blob/390a11e842b689ab3277dc42c617344416aaa559/examples/checkWaitingMRD/checkWaitingMRD.ino#L22-L148


---
---

### Debug Terminal Output Samples

#### 1. ESP32_FSWebServer_DRD on ESP32_DEV

This is terminal debug output when running [ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP32_FSWebServer_DRD) on  ***ESP32 ESP32_DEV.***. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi AP **HueNet1** using new Static IP successfully. WiFi AP **HueNet1** is then lost, and board **autoreconnects** itself to backup WiFi AP **HueNet2**.

```cpp
Starting ESP32_FSWebServer_DRD with DoubleResetDetect using SPIFFS on ESP32_DEV
ESP_WiFiManager v1.11.0
ESP_MultiResetDetector v1.3.2
FS File: /ConfigSW.json, size: 150B
FS File: /CanadaFlag_1.png, size: 40.25KB
FS File: /CanadaFlag_2.png, size: 8.12KB
FS File: /CanadaFlag_3.jpg, size: 10.89KB
FS File: /edit.htm.gz, size: 4.02KB
FS File: /favicon.ico, size: 1.12KB
FS File: /graphs.js.gz, size: 1.92KB
FS File: /index.htm, size: 3.63KB
FS File: /drd.dat, size: 4B
FS File: /wifi_cred.dat, size: 192B

[WM] RFC925 Hostname = ESP32-FSWebServerDRD
[WM] setAPStaticIPConfig
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
Stored: SSID = HueNet2, Pass = 12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
Got stored Credentials. Timeout 120s for Config Portal
SPIFFS Flag read = 0xd0d04321
No multiResetDetected
Saving config file...
Saving config file OK
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet2 , Router_Pass =  12345678
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -27
[WM] Channel: 2 ,IP address: 192.168.2.232
After waiting 3.16 secs more in setup(), connection result is connected. Local IP: 192.168.2.232
HTTP server started @ 192.168.2.232
Open http://esp32-fs-browser.local/edit to see the file browser
[WM] freeing allocated params!
Stop multiResetDetecting
Saving config file...
Saving config file OK

WiFi lost. Call connectMultiWiFi in loop
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet2 , Router_Pass =  12345678
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  3
[WM] SSID: HueNet2 ,RSSI= -59
[WM] Channel: 4 ,IP address: 192.168.2.232
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
```

---

#### 2. minimal on ESP32_DEV

This is terminal debug output when running [minimal](examples/minimal) on ***ESP32 ESP32_DEV using LittleFS.***. Config Portal was requested by MRD to input and save Credentials.


#### 2.1 Data Corrupted => reset to 0

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
/home/kh/Arduino/libraries/LITTLEFS-master/src/lfs.c:1003:error: Corrupted dir pair at {0x0, 0x1}
E (241) esp_littlefs: mount failed,  (-84)
E (245) esp_littlefs: Failed to initialize LittleFS
multiResetDetectorFlag = 0xFEFEFEFE
lowerBytes = 0xFEFE, upperBytes = 0x0101
lowerBytes = 0xFEFE, upperBytes = 0x0101
detectRecentlyResetFlag: Data corrupted. Reset to 0
No multiResetDetected, number of times = 0
Saving config file...
Saving config file OK
No Multi Reset Detected
Stop multiResetDetecting
Saving config file...
Saving config file OK
```

#### 2.2 Reset Detected => Reporting 1

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFE0001
multiResetDetectorFlag = 0xFFFE0001
lowerBytes = 0x0001, upperBytes = 0x0001
No multiResetDetected, number of times = 1
LittleFS Flag read = 0xFFFE0001
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 2.3 Reset Detected => Reporting 2

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFD0002
multiResetDetectorFlag = 0xFFFD0002
lowerBytes = 0x0002, upperBytes = 0x0002
No multiResetDetected, number of times = 2
LittleFS Flag read = 0xFFFD0002
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 2.4 Reset Detected => Reporting 3

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFC0003
multiResetDetectorFlag = 0xFFFC0003
lowerBytes = 0x0003, upperBytes = 0x0003
No multiResetDetected, number of times = 3
LittleFS Flag read = 0xFFFC0003
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 2.5 Reset Detected => Reporting 4

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFB0004
multiResetDetectorFlag = 0xFFFB0004
lowerBytes = 0x0004, upperBytes = 0x0004
No multiResetDetected, number of times = 4
LittleFS Flag read = 0xFFFB0004
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 2.6 Reset Detected => Reporting 5. Multi Reset Detected

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFA0005
multiResetDetectorFlag = 0xFFFA0005
lowerBytes = 0x0005, upperBytes = 0x0005
multiResetDetected, number of times = 5
Saving config file...
Saving config file OK
Multi Reset Detected
```

#### 2.7 Timed out => reset to 1

```
Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFB0004
multiResetDetectorFlag = 0xFFFB0004
lowerBytes = 0x0004, upperBytes = 0x0004
No multiResetDetected, number of times = 4
LittleFS Flag read = 0xFFFB0004
Saving config file...
Saving config file OK
No Multi Reset Detected
Stop multiResetDetecting
Saving config file...
Saving config file OK
```

#### 2.8 Reset Detected => Reporting 1

```

Starting ESP_MultiResetDetector minimal on ESP32_DEV using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFE0001
multiResetDetectorFlag = 0xFFFE0001
lowerBytes = 0x0001, upperBytes = 0x0001
No multiResetDetected, number of times = 1
LittleFS Flag read = 0xFFFE0001
Saving config file...
Saving config file OK
No Multi Reset Detected

```

---

#### 3. minimal on ESP8266_NODEMCU

This is terminal debug output when running [minimal](examples/minimal) on ***ESP8266_NODEMCU using LittleFS.***. Config Portal was requested by MRD to input and save Credentials.


#### 3.1 Data Corrupted => reset to 0

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
multiResetDetectorFlag = 0x00000000
lowerBytes = 0x0000, upperBytes = 0xFFFF
lowerBytes = 0x0000, upperBytes = 0xFFFF
detectRecentlyResetFlag: Data corrupted. Reset to 0
No multiResetDetected, number of times = 0
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 3.2 Reset Detected => Reporting 1

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFE0001
multiResetDetectorFlag = 0xFFFE0001
lowerBytes = 0x0001, upperBytes = 0x0001
No multiResetDetected, number of times = 1
LittleFS Flag read = 0xFFFE0001
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 3.3 Reset Detected => Reporting 2

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFD0002
multiResetDetectorFlag = 0xFFFD0002
lowerBytes = 0x0002, upperBytes = 0x0002
No multiResetDetected, number of times = 2
LittleFS Flag read = 0xFFFD0002
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 3.4 Reset Detected => Reporting 3

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFC0003
multiResetDetectorFlag = 0xFFFC0003
lowerBytes = 0x0003, upperBytes = 0x0003
No multiResetDetected, number of times = 3
LittleFS Flag read = 0xFFFC0003
Saving config file...
Saving config file OK
No Multi Reset Detected
```

#### 3.5 Reset Detected => Reporting 4

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFB0004
multiResetDetectorFlag = 0xFFFB0004
lowerBytes = 0x0004, upperBytes = 0x0004
No multiResetDetected, number of times = 4
LittleFS Flag read = 0xFFFB0004
Saving config file...
Saving config file OK
```

#### 3.6 Reset Detected => Reporting 5. Multi Reset Detected

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFA0005
multiResetDetectorFlag = 0xFFFA0005
lowerBytes = 0x0005, upperBytes = 0x0005
multiResetDetected, number of times = 5
Saving config file...
Saving config file OK
Multi Reset Detected
```

#### 3.7 Timed out => reset to 1

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFB0004
multiResetDetectorFlag = 0xFFFB0004
lowerBytes = 0x0004, upperBytes = 0x0004
No multiResetDetected, number of times = 4
LittleFS Flag read = 0xFFFB0004
Saving config file...
Saving config file OK
No Multi Reset Detected
Stop multiResetDetecting
Saving config file...
Saving config file OK
```

#### 3.8 Reset Detected => Reporting 1

```
Starting ESP_MultiResetDetector minimal on ESP8266_NODEMCU using LittleFS
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFE0001
multiResetDetectorFlag = 0xFFFE0001
lowerBytes = 0x0001, upperBytes = 0x0001
No multiResetDetected, number of times = 1
LittleFS Flag read = 0xFFFE0001
Saving config file...
Saving config file OK
No Multi Reset Detected
```

---

#### 4. ESPAsync_WiFi using LittleFS on ESP32S3_DEV

This is terminal debug output when running [ESPAsync_WiFi](https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite/tree/main/examples/ESPAsync_WiFi) on  ***ESP32 ESP32S3_DEV.***. Config Portal was requested by MRD to input and save Credentials.


```
Starting ESPAsync_WiFi using LittleFS on ESP32S3_DEV
ESPAsync_WiFiManager_Lite v1.9.0
ESP_MultiResetDetector v1.3.2
LittleFS Flag read = 0xFFFC0003
multiResetDetectorFlag = 0xFFFC0003
lowerBytes = 0x0003, upperBytes = 0x0003
multiResetDetected, number of times = 3
Saving config file...
Saving config file OK
[WML] Hdr=ESP_WM_LITE,SSID=HueNet1,PW=12345678
[WML] SSID1=HueNet2,PW1=12345678
[WML] BName=ESP32_S3
[WML] Hdr=ESP_WM_LITE,SSID=HueNet1,PW=12345678
[WML] SSID1=HueNet2,PW1=12345678
[WML] BName=ESP32_S3
[WML] WiFi networks found:
[WML] 1: HueNetTek, -30dB
[WML] 2: HueNet, -31dB
[WML] 3: HueNet1, -36dB
[WML] 4: HueNet2, -55dB
[WML] 5: SmartRG-02a2, -75dB
[WML] 6: AT_301_WLREL6325F_f66d, -75dB
[WML] 7: Linksys00043, -77dB
[WML] 8: El khoury, -79dB
[WML] 9: house, -79dB
[WML] 10: Waterhome, -87dB
[WML] 11: Offline5, -89dB
[WML] 12: Access, -90dB
[WML] 13: ESP151CD5, -90dB
[WML] 14: Jessie, -90dB
[WML] 15: AdeyemiHome, -90dB
[WML] 16: BELL627, -90dB
[WML] 17: JJ Realestate Investments, -90dB
[WML] 18: BELL042, -92dB
[WML] 19: VIRGIN874, -93dB
[WML] 20: BELL905, -95dB
[WML] 
stConf:SSID=ESP_E1A1DF7C,PW=MyESP_E1A1DF7C
[WML] IP=192.168.4.1,ch=6
F
Your stored Credentials :
Blynk Server1 = account.duckdns.org
Token1 = token1
Blynk Server2 = account.ddns.net
Token2 = token2
Port = 8080
MQTT Server = mqtt.duckdns.org
```

---
---

### Libraries using ESP_MultiResetDetector, ESP_DoubleResetDetector or DoubleResetDetector_Generic library

You can also see how [`ESP_MultiResetDetector`](https://github.com/khoih-prog/ESP_MultiResetDetector) and [`DoubleResetDetector_Generic`](https://github.com/khoih-prog/DoubleResetDetector_Generic) are applied in many other libraries, such as:

 1. [Blynk_WM](https://github.com/khoih-prog/Blynk_WM)
 2. [BlynkEthernet_WM](https://github.com/khoih-prog/BlynkEthernet_WM)
 3. [WiFiManager_NINA_Lite](https://github.com/khoih-prog/WiFiManager_NINA_Lite)
 4. [BlynkESP32_BT_WF](https://github.com/khoih-prog/BlynkESP32_BT_WF), 
 5. [Blynk_GSM_Manager](https://github.com/khoih-prog/Blynk_GSM_Manager),
 6. [Blynk_Esp8266AT_WM](https://github.com/khoih-prog/Blynk_Esp8266AT_WM), 
 7. [Blynk_WiFiNINA_WM](https://github.com/khoih-prog/Blynk_WiFiNINA_WM), 
 8. [Blynk_Async_WM](https://github.com/khoih-prog/Blynk_Async_WM),
 9. [Blynk_Async_ESP32_BT_WF](https://github.com/khoih-prog/Blynk_Async_ESP32_BT_WF), 
10. [Blynk_Async_GSM_Manager](https://github.com/khoih-prog/Blynk_Async_GSM_Manager),
11. [ESP_WiFiManager](https://github.com/khoih-prog/ESP_WiFiManager)
12. [ESPAsync_WiFiManager](https://github.com/khoih-prog/ESPAsync_WiFiManager)
13. [WiFiManager_NINA_Lite](https://github.com/khoih-prog/WiFiManager_NINA_Lite)
14. [BlynkEthernet_STM32_WM](https://github.com/khoih-prog/BlynkEthernet_STM32_WM),
15. [ESP_AT_WM_Lite](https://github.com/khoih-prog/ESP_AT_WM_Lite)
16. [WIOTerminal_WiFiManager](https://github.com/khoih-prog/WIOTerminal_WiFiManager)
17. [Ethernet_Manager](https://github.com/khoih-prog/Ethernet_Manager)
18. [Ethernet_Manager_STM32](https://github.com/khoih-prog/Ethernet_Manager_STM32)

and the list is growing fast.

---
---

### Debug

Debug is disabled by default. To enable debug:

```cpp
// Use this to output debug msgs to Serial
#define MULTIRESETDETECTOR_DEBUG       true
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the `ESP32 / ESP8266` core for Arduino.

Sometimes, the library will only work if you update the `ESP32 / ESP8266` core to the latest version because I am using some newly added function.

---
---


### Issues ###

Submit issues to: [ESP_MultiResetDetector issues](https://github.com/khoih-prog/ESP_MultiResetDetector/issues)

---
---

### TO DO

1. Search for bug and improvement.
2. Similar features for Arduino (UNO, Mega, SAM DUE, SAMD21/SAMD51, nRF52, STM32, Teensy, etc.). Look at [**DoubleResetDetector_Generic**](https://github.com/khoih-prog/DoubleResetDetector_Generic)


### DONE

1. Multi Reset Detector for ESP32 (EEPROM, SPIFFS and LittleFS) and ESP8266 (RTC, EEPROM, SPIFFS and LittleFS).
2. Add support to `ESP32_C3`, `ESP32_S2`
3. Add support to `ESP32_S3` using ESP32 core v2.0.2+
4. Add waitingForMRD() function to signal in MRD wating period.
6. Fix ESP32 chipID for example`ConfigOnMultiReset`
7. Remove dependency on `LittleFS_esp32` library to prevent PIO error when using new ESP32 core v1.0.6+

---
---

### Contributions and thanks

1. Thanks to [kbssa](https://github.com/kbssa) for request enhancement in [Issue 9: Not an issue, but a question](https://github.com/khoih-prog/ESP_DoubleResetDetector/issues/9), leading to this new [ESP_MultiResetDetector Library](https://github.com/khoih-prog/ESP_MultiResetDetector)
2. Thanks to [Tochi Moreno](https://github.com/tochimoreno) for enhancement request in [DRD is waiting for a double reset? #14](https://github.com/khoih-prog/ESP_DoubleResetDetector/discussions/14) leading to v1.3.1 to add `waitingForMRD()` function to signal in MRD wating period

<table>
  <tr>
    <td align="center"><a href="https://github.com/kbssa"><img src="https://github.com/kbssa.png" width="100px;" alt="kbssa"/><br /><sub><b>kbssa</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/tochimoreno"><img src="https://github.com/tochimoreno.png" width="100px;" alt="tochimoreno"/><br /><sub><b>Tochi Moreno</b></sub></a><br /></td>
  </tr> 
</table>

---

### Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/ESP_MultiResetDetector/blob/main/LICENSE)

---

### Copyright

Copyright 2020- Khoi Hoang

