/****************************************************************************************************************************
  ESP_MultiResetDetector.h
  For ESP8266 / ESP32 boards

  ESP_MultiResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 twice.

  Based on and modified from
  1) DataCute    https://github.com/datacute/DoubleResetDetector
  2) Khoi Hoang  https://github.com/khoih-prog/ESP_DoubleResetDetector

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_MultiResetDetector
  Licensed under MIT license
  Version: 1.3.2

  [rob040] TODO suggestion to remove all Flash FS or EEPROM R/W and only use RAM / RTCRAM
  for ESP32 there is the
  `__NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;`
  or the
  `RTC_NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;`
  Either will work, but have to check availability on Arduino IDE and PlatformIO / PioArduino  version/release
  Likely it wasn't available when this library was created

  For Arduino ESP8266 there is at leaste the longer used
  `ESP.rtcUserMemoryRead(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));`
  and `ESP.rtcUserMemoryWrite()`

  With these, complexity will be reduced greatly!


  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.1.1   K Hoang      30/12/2020 Initial coding to support Multiple Reset Detection. Sync with ESP_DoubleResetDetector v1.1.1
  1.1.2   K Hoang      10/10/2021 Update `platform.ini` and `library.json`
  1.2.0   K Hoang      26/11/2021 Auto detect ESP32 core and use either built-in LittleFS or LITTLEFS library
  1.2.1   K Hoang      26/11/2021 Fix compile error for ESP32 core v1.0.5-
  1.3.0   K Hoang      10/02/2022 Add support to new ESP32-S3
  1.3.1   K Hoang      04/03/2022 Add waitingForMRD() function to signal in MRD wating period
  1.3.2   K Hoang      09/09/2022 Fix ESP32 chipID for example ConfigOnMultiReset
  2.0.0   rob040@users.github.com 2025-07-22 Massive reduction in (configuration) complexity: use RAM storage only (TODO FIXME)
*****************************************************************************************************************************/

#pragma once

#ifndef ESP_MultiResetDetector_H
#define ESP_MultiResetDetector_H

#ifndef MULTIRESETDETECTOR_DEBUG
  #define MULTIRESETDETECTOR_DEBUG       false
#endif

// todo: its safe to assume to support only Arduino 1.8 (2017) and later is to be used
#if defined(ARDUINO) && (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#ifndef ESP_MULTI_RESET_DETECTOR_VERSION
  #define ESP_MULTI_RESET_DETECTOR_VERSION             "ESP_MultiResetDetector v1.3.2"

  #define ESP_MULTI_RESET_DETECTOR_VERSION_MAJOR       1
  #define ESP_MULTI_RESET_DETECTOR_VERSION_MINOR       3
  #define ESP_MULTI_RESET_DETECTOR_VERSION_PATCH       2

  #define ESP_MULTI_RESET_DETECTOR_VERSION_INT         1003002
#endif

#define ESP_MULTIRESETDETECTOR_VERSION         ESP_MULTI_RESET_DETECTOR_VERSION

//#define ESP_MRD_USE_EEPROM      false
//#define ESP_MRD_USE_LITTLEFS    false
//#define ESP_MRD_USE_SPIFFS      false
//#define ESP8266_MRD_USE_RTC     false   //true

#ifdef ESP32
  #if (!ESP_MRD_USE_EEPROM && !ESP_MRD_USE_SPIFFS && !ESP_MRD_USE_LITTLEFS)
    #if (MULTIRESETDETECTOR_DEBUG)
      #warning Neither EEPROM, SPIFFS nor LittleFS selected. Default to EEPROM
    #endif

    #ifdef ESP_MRD_USE_EEPROM
      #undef ESP_MRD_USE_EEPROM
      #define ESP_MRD_USE_EEPROM      true
    #endif
  #endif
#endif

#ifdef ESP8266
  #if (!ESP8266_MRD_USE_RTC && !ESP_MRD_USE_EEPROM && !ESP_MRD_USE_SPIFFS && !ESP_MRD_USE_LITTLEFS)
    #if (MULTIRESETDETECTOR_DEBUG)
      #warning Neither RTC, EEPROM, LITTLEFS nor SPIFFS selected. Default to EEPROM
    #endif

    #ifdef ESP_MRD_USE_EEPROM
      #undef ESP_MRD_USE_EEPROM
      #define ESP_MRD_USE_EEPROM      true
    #endif
  #endif
#endif

//default to use EEPROM, otherwise, use LITTLEFS (higher priority), then SPIFFS
#if ESP_MRD_USE_EEPROM
  #include <EEPROM.h>

  #define  FLAG_DATA_SIZE     4

  #ifndef EEPROM_SIZE
    #define EEPROM_SIZE     512
  #endif

  #ifndef EEPROM_START
    #define EEPROM_START    256
  #endif

#elif ( ESP_MRD_USE_LITTLEFS || ESP_MRD_USE_SPIFFS )

#include <FS.h>

#ifdef ESP32

  #if ESP_MRD_USE_LITTLEFS
    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
    //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      #if (MULTIRESETDETECTOR_DEBUG)
        #warning Using ESP32 Core 1.0.6 or 2.0.0+
      #endif

      // The library has been merged into esp32 core from release 1.0.6
      #include <LittleFS.h>

      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
    #else
      #if (MULTIRESETDETECTOR_DEBUG)
        #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
      #endif

      // The library has been merged into esp32 core from release 1.0.6
      #include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS

      #define FileFS        LITTLEFS
      #define FS_Name       "LittleFS"
    #endif
  #else
    #include "SPIFFS.h"
    // ESP32 core 1.0.4 still uses SPIFFS
    #define FileFS   SPIFFS
  #endif

#else
  // From ESP8266 core 2.7.1
  #include <LittleFS.h>

  #if ESP_MRD_USE_LITTLEFS
    #define FileFS    LittleFS
  #else
    #define FileFS   SPIFFS
  #endif

#endif    // #if ESP_MRD_USE_EEPROM

#define  MRD_FILENAME     "/mrd.dat"

#endif    //#if ESP_MRD_USE_EEPROM

///////////////////
// Default values if not specified in sketch

#ifndef MRD_TIMES
  #define MRD_TIMES         3  // 3 means 2 resets within <MRD_TIMEOUT> seconds after restart, because it starts as 1 and is set to 2 after power-on before timeout
#endif

#ifndef MRD_TIMEOUT
  #define MRD_TIMEOUT       10
#endif

#ifndef MRD_ADDRESS
  #define MRD_ADDRESS       0  // only appliccable to EEPROM stotrage
#endif

///////////////////

// Flag clear to 0xFFFE0001 if no MRD within MRD_TIMEOUT. Flag will increase 1 for each reset within MRD_TIMEOUT
// So multiResetDetectorFlag_SET is not necessary.
// Will use upper 2 bytes to verify if corrupted data.

#define USING_INVERTED    true

#if USING_INVERTED
  #define multiResetDetectorFlag_BEGIN  0xFFFE0001     // Used when beginning a new cycle
  #define multiResetDetectorFlag_CLEAR  0xFFFF0000     // Used when data corrupted, such as reformat LittleFS/SPIFFS
#else
  #define multiResetDetectorFlag_BEGIN  0x00010001     // Used when beginning a new cycle
  #define multiResetDetectorFlag_CLEAR  0x00000000     // Used when data corrupted, such as reformat LittleFS/SPIFFS
#endif

class MultiResetDetector
{
  public:
    MultiResetDetector(int timeout, int address)
    {
      mrd_times = MRD_TIMES;

#if ESP_MRD_USE_EEPROM
#if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf("EEPROM size = %d, start = %d\n", EEPROM_SIZE, EEPROM_START);
#endif

      EEPROM.begin(EEPROM_SIZE);
#elif ( ESP_MRD_USE_LITTLEFS || ESP_MRD_USE_SPIFFS )
      // LittleFS / SPIFFS code. Format FileFS if not yet
  #ifdef ESP32
      if (!FileFS.begin(true))
  #else
      if (!FileFS.begin())
  #endif
      {
#if (MULTIRESETDETECTOR_DEBUG)

#if ESP_MRD_USE_LITTLEFS
        Serial.println(F("LittleFS failed!. Please use SPIFFS or EEPROM."));
#else
        Serial.println(F("SPIFFS failed!. Please use LittleFS or EEPROM."));
#endif

#endif
        multiResetDetectorFlag = multiResetDetectorFlag = 0;
      }
#else
  #ifdef ESP8266
      //RTC only for ESP8266
  #else
      // RAM storage for ESP32
      __NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;
  #endif
#endif

      this->timeout = timeout * 1000;
      this->address = address;
      multiResetDetected = false;
      waitingForMultiReset = false;
    };

    bool detectMultiReset()
    {
      multiResetDetected = detectRecentlyResetFlag();

      if (multiResetDetected)
      {
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.printf("multiResetDetected, count = %d (>%d)\n", (uint16_t) (multiResetDetectorFlag), MRD_TIMES);
#endif

        clearRecentlyResetFlag();
      }
      else
      {
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.printf("No multiResetDetected, count = %d\n", (uint16_t) (multiResetDetectorFlag) );
#endif

        setRecentlyResetFlag();
        waitingForMultiReset = true;
      }

      return multiResetDetected;

    };

    bool waitingForMRD()
    {
      return waitingForMultiReset;
    }

    void loop()
    {
      if (waitingForMultiReset && (millis() > timeout))
      {
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.println(F("Stop multiResetDetecting"));
#endif

        stop();
      }
    };

    void stop()
    {
      clearRecentlyResetFlag();
      waitingForMultiReset = false;
    };

    bool multiResetDetected;


  private:

    unsigned long mrd_times;

    unsigned long timeout;

    int address;
    bool waitingForMultiReset;

    uint32_t multiResetDetectorFlag;

    bool readRecentlyResetFlag()
    {
#if (ESP_MRD_USE_EEPROM)
      EEPROM.get(EEPROM_START, multiResetDetectorFlag);

  #if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf("EEPROM Flag read = 0x%08X\n", multiResetDetectorFlag);
  #endif
#elif ( ESP_MRD_USE_LITTLEFS || ESP_MRD_USE_SPIFFS )
      // LittleFS / SPIFFS code
      if (FileFS.exists(MRD_FILENAME))
      {
        // if config file exists, load
        File file = FileFS.open(MRD_FILENAME, "r");

        if (!file)
        {
  #if (MULTIRESETDETECTOR_DEBUG)
          Serial.println(F("Loading config file failed"));
  #endif
          return false;
        }

        file.readBytes((char *) &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));

  #if (MULTIRESETDETECTOR_DEBUG)

    #if ESP_MRD_USE_LITTLEFS
        Serial.printf("LittleFS Flag read = 0x%08X\n", multiResetDetectorFlag);
    #else
        Serial.printf("SPIFFS Flag read = 0x%08X\n", multiResetDetectorFlag);
    #endif

  #endif

        file.close();
      }
#else
  #ifdef ESP8266
      //RTC only for ESP8266
      ESP.rtcUserMemoryRead(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
  #else
      // ESP32 RAM storage
      multiResetDetectorFlag = noinit_multiResetDetectorFlag;
  #endif
  #if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf("MRD ReadFlag 0x%08X\n", multiResetDetectorFlag);
  #endif
#endif

      return true;
    }

    bool detectRecentlyResetFlag()
    {
      if (!readRecentlyResetFlag())
        return false;

      //multiResetDetected = (multiResetDetectorFlag == multiResetDetectorFlag_SET);
      // Check lower 2 bytes is > 0 and upper 2 bytes agrees

#if USING_INVERTED
      uint16_t upperBytes = (uint16_t) ~(multiResetDetectorFlag >> 16);
#else
      uint16_t upperBytes = multiResetDetectorFlag >> 16;
#endif

      uint16_t lowerBytes = (uint16_t) multiResetDetectorFlag;

#if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf("multiResetDetectorFlag = 0x%08X\n", multiResetDetectorFlag);
      Serial.printf("lowerBytes = 0x%04X, upperBytes = 0x%04X\n", lowerBytes, upperBytes);
#endif

      if ( ( lowerBytes >= MRD_TIMES ) && ( lowerBytes == upperBytes )  )
      {
        multiResetDetected = true;
      }
      else
      {
        multiResetDetected = false;

        if (lowerBytes != upperBytes)
        {
          // To reset if data corrupted
          //[rob040] FIXME note that this clear flag will not pass detection criteria above! -> done
          multiResetDetectorFlag = multiResetDetectorFlag_CLEAR;
#if (MULTIRESETDETECTOR_DEBUG)
          ///Serial.printf("lowerBytes = 0x%04X, upperBytes = 0x%04X\n", lowerBytes, upperBytes);
          Serial.println(F("detectRecentlyResetFlag: Data corrupted. Reset to 0"));
          Serial.printf("multiResetDetectorFlag = 0x%08X\n", multiResetDetectorFlag);
#endif
        }
      }

      return multiResetDetected;
    };

    void setRecentlyResetFlag()
    {
      // Add 1 every time detecting a reset
      // To read first, increase and update 2 checking bytes
      readRecentlyResetFlag();

#if USING_INVERTED
      // 2 lower bytes
      //multiResetDetectorFlag = (multiResetDetectorFlag & 0x0000FFFF) + 1;

      // 2 upper bytes
      //uint16_t upperBytes = ~multiResetDetectorFlag;
      //multiResetDetectorFlag = (upperBytes << 16) | multiResetDetectorFlag;

      // Simpler?
      multiResetDetectorFlag += 1;
      multiResetDetectorFlag = (~multiResetDetectorFlag << 16) | (multiResetDetectorFlag & 0xFFFF);
#else
      // 2 lower bytes
      multiResetDetectorFlag = (multiResetDetectorFlag & 0x0000FFFF) + 1;

      // 2 upper bytes
      multiResetDetectorFlag = (multiResetDetectorFlag << 16) | multiResetDetectorFlag;
#endif

      multiResetDetectorFlag  = multiResetDetectorFlag;

#if (ESP_MRD_USE_EEPROM)
      EEPROM.put(EEPROM_START, multiResetDetectorFlag);
      EEPROM.commit();

#if (MULTIRESETDETECTOR_DEBUG)
      delay(1000);
      EEPROM.get(EEPROM_START, multiResetDetectorFlag);

      Serial.printf("SetFlag write = 0x%08X\n", multiResetDetectorFlag);
#endif
#elif ( ESP_MRD_USE_LITTLEFS || ESP_MRD_USE_SPIFFS )
      // LittleFS / SPIFFS code
      File file = FileFS.open(MRD_FILENAME, "w");
#if (MULTIRESETDETECTOR_DEBUG)
      Serial.println(F("Saving config file..."));
#endif

      if (file)
      {
        file.write((uint8_t *) &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
        file.close();
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.println(F("Saving config file OK"));
#endif
      }
      else
      {
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.println(F("Saving config file failed"));
#endif
      }
#else
  #if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf_P(PSTR("MRD SetFlag write 0x%08X\n"), multiResetDetectorFlag);
  #endif
  #ifdef ESP8266
      //RTC only for ESP8266
      ESP.rtcUserMemoryWrite(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
  #else
      // ESP32 RAM storage
      noinit_multiResetDetectorFlag = multiResetDetectorFlag;
  #endif
#endif
    };


    void clearRecentlyResetFlag()
    {
      multiResetDetectorFlag = multiResetDetectorFlag_BEGIN;

#if (ESP_MRD_USE_EEPROM)
      //multiResetDetectorFlag = multiResetDetectorFlag_BEGIN;
      EEPROM.put(EEPROM_START, multiResetDetectorFlag);
      EEPROM.commit();

#if (MULTIRESETDETECTOR_DEBUG)
      delay(1000);
      EEPROM.get(EEPROM_START, multiResetDetectorFlag);

      Serial.printf("ClearFlag write = 0x%08X\n", multiResetDetectorFlag);
#endif
#elif ( ESP_MRD_USE_LITTLEFS || ESP_MRD_USE_SPIFFS )
      // LittleFS / SPIFFS code
      File file = FileFS.open(MRD_FILENAME, "w");
#if (MULTIRESETDETECTOR_DEBUG)
      Serial.println(F("Saving config file..."));
#endif

      if (file)
      {
        file.write((uint8_t *) &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
        file.close();
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.println(F("Saving config file OK"));
#endif
      }
      else
      {
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.println(F("Saving config file failed"));
#endif
      }

#else
  #ifdef ESP8266
      //RTC only for ESP8266
      ESP.rtcUserMemoryWrite(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
  #else
      // ESP32 RAM storage
      noinit_multiResetDetectorFlag = multiResetDetectorFlag;
  #endif
#endif
    };
};
#endif // ESP_MultiResetDetector_H
