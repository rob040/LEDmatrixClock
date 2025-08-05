/****************************************************************************************************************************
  ESP_MultiResetDetector.h
  For ESP8266 / ESP32 boards

  ESP_MultiResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 twice.

  Based on and modified from
  1) DataCute    https://github.com/datacute/DoubleResetDetector
  2) Khoi Hoang  https://github.com/khoih-prog/ESP_DoubleResetDetector
  3) rob040      https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector (version 2.0+)

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_MultiResetDetector (Archived)
  Licensed under MIT license
  Version: 2.0.0

  [rob040]  https://github.com/rob040/ESP_WiFiManagerLite2
  For version 2.0, the configuration complexity is massively reduced, by only using (uninitialized) RAM storage,
  and remove all Flash FS or EEPROM R/W functions.
  Also the similarity with ESP_DoubleResetDetector is so much that the MRD can do the job of DRD, just
  set MRD_TIMES to 2 and you have DRD so that DRD library can be removed.

----
  for ESP32 there is the
  `__NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;`
  or the
  `RTC_NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;`
  Either will work, but have to check availability on Arduino IDE and PlatformIO / PioArduino  version/release
  Likely it wasn't available when this library was created

  For Arduino ESP8266 there is at least the longer used
  `ESP.rtcUserMemoryRead(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));`
  and `ESP.rtcUserMemoryWrite()`

  With these, complexity will be reduced greatly!
----

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.1.1   K Hoang      30/12/2020 Initial coding to support Multiple Reset Detection. Sync with ESP_DoubleResetDetector v1.1.1
  1.1.2   K Hoang      10/10/2021 Update `platform.ini` and `library.json`
  1.2.0   K Hoang      26/11/2021 Auto detect ESP32 core and use either built-in LittleFS or LITTLEFS library
  1.2.1   K Hoang      26/11/2021 Fix compile error for ESP32 core v1.0.5-
  1.3.0   K Hoang      10/02/2022 Add support to new ESP32-S3
  1.3.1   K Hoang      04/03/2022 Add waitingForMRD() function to signal in MRD waiting period
  1.3.2   K Hoang      09/09/2022 Fix ESP32 chipID for example ConfigOnMultiReset
  2.0.0   rob040       2025-07-22 Massive reduction in (configuration) complexity: use RAM storage only
  2.0.0   rob040       2025-07-30 Renamed "Bytes" to "Word", place debug strings into PROGMEM
*****************************************************************************************************************************/

#pragma once

#ifndef ESP_MultiResetDetector_H
#define ESP_MultiResetDetector_H

#ifndef MULTIRESETDETECTOR_DEBUG
  #define MULTIRESETDETECTOR_DEBUG       false
#endif

#include <Arduino.h>

#ifndef ESP_MULTI_RESET_DETECTOR_VERSION
  #define ESP_MULTI_RESET_DETECTOR_VERSION             "ESP_MultiResetDetector v2.0.0"

  #define ESP_MULTI_RESET_DETECTOR_VERSION_MAJOR       2
  #define ESP_MULTI_RESET_DETECTOR_VERSION_MINOR       0
  #define ESP_MULTI_RESET_DETECTOR_VERSION_PATCH       0

  #define ESP_MULTI_RESET_DETECTOR_VERSION_INT         2000000
#endif

#define ESP_MULTIRESETDETECTOR_VERSION         ESP_MULTI_RESET_DETECTOR_VERSION



///////////////////
// Default values if not specified in sketch

#ifndef MRD_TIMES
  #define MRD_TIMES         3  // 3 means 2 resets within <MRD_TIMEOUT> seconds after power-on, or 3 resets when application is running longer than <MRD_TIMEOUT>
#endif

#ifndef MRD_TIMEOUT
  #define MRD_TIMEOUT       10  // Seconds within resets are detected
#endif

#ifndef MRD_ADDRESS
  #define MRD_ADDRESS       0  // only applicable to ESP8266 RTC storage offset
#endif

///////////////////

// Flag clear to 0xFFFE0001 if no MRD within MRD_TIMEOUT. Flag will increase 1 for each reset within MRD_TIMEOUT
// Will use the inverted upper 2 bytes to check for valid data.
#define multiResetDetectorFlag_BEGIN  0xFFFE0001     // Used when beginning a new cycle

///////////////////
#ifdef MRD_ALLOCATE_STATIC_DATA
#ifdef ESP8266
//RTC only for ESP8266
#else
// RAM storage for ESP32, not affected by application reset (?)
__NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;
#endif
#else
extern __NOINIT_ATTR uint32_t noinit_multiResetDetectorFlag;
#endif

///////////////////



class MultiResetDetector
{
  public:
    MultiResetDetector(int timeout = MRD_TIMEOUT, int address = MRD_ADDRESS)
    {

      this->timeout = timeout * 1000;
      this->address = address;
      multiResetDetected = false;
      waitingForMultiReset = false;
    }

    bool detectMultiReset()
    {
      readRecentlyResetFlag();

      uint16_t upperWord = (uint16_t) ~(multiResetDetectorFlag >> 16);
      uint16_t lowerWord = (uint16_t) multiResetDetectorFlag;

#if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf_P(PSTR("multiResetDetectorFlag = 0x%08X\n"), multiResetDetectorFlag);
      Serial.printf_P(PSTR("lowerWord = 0x%04X, upperWord = 0x%04X\n"), lowerWord, upperWord);
#endif

      if ( ( lowerWord > MRD_TIMES ) && ( lowerWord == upperWord )  )
      {
        multiResetDetected = true;
#if (MULTIRESETDETECTOR_DEBUG)
        Serial.printf_P(PSTR("multiResetDetected, count = %d (>%d)\n"), (uint16_t) (multiResetDetectorFlag), MRD_TIMES);
#endif
        clearRecentlyResetFlag();
      }
      else
      {
        multiResetDetected = false;
        waitingForMultiReset = true;

        if (lowerWord != upperWord)
        {
          // To reset if data corrupted
#if (MULTIRESETDETECTOR_DEBUG)
          Serial.println(F("detectMultiReset: Data corrupted, Clear Flag"));
#endif
          clearRecentlyResetFlag();
        }
        else
        {
#if (MULTIRESETDETECTOR_DEBUG)
          Serial.printf_P(PSTR("No multiResetDetected, count = %d\n"), (uint16_t) (multiResetDetectorFlag)+1 );
#endif
          setRecentlyResetFlag();
        }
      }

      return multiResetDetected;
    }

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
    }

    bool multiResetDetected;

  private:
    unsigned long timeout;
    int address;
    bool waitingForMultiReset;
    uint32_t multiResetDetectorFlag;

    bool readRecentlyResetFlag()
    {
  #ifdef ESP8266
      //RTC only for ESP8266
      ESP.rtcUserMemoryRead(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
  #else
      // ESP32 RAM storage
      multiResetDetectorFlag = noinit_multiResetDetectorFlag;
  #endif
  #if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf_P(PSTR("MRD ReadFlag 0x%08X\n"), multiResetDetectorFlag);
  #endif

      return true;
    }


    void setRecentlyResetFlag()
    {
      // Add 1 every time detecting a reset
      // To read first, increase and update 2 checking bytes
      //readRecentlyResetFlag();

      multiResetDetectorFlag += 1;
      multiResetDetectorFlag = ((~multiResetDetectorFlag) << 16) | (multiResetDetectorFlag & 0xFFFF);

#if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf_P(PSTR("setRecentlyResetFlag = 0x%08X\n"), multiResetDetectorFlag);
      Serial.flush();
#endif
  #ifdef ESP8266
      //RTC only for ESP8266
      ESP.rtcUserMemoryWrite(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
  #else
      // ESP32 RAM storage
      noinit_multiResetDetectorFlag = multiResetDetectorFlag;
  #endif
    }


    void clearRecentlyResetFlag()
    {
      multiResetDetectorFlag = multiResetDetectorFlag_BEGIN;

  #if (MULTIRESETDETECTOR_DEBUG)
      Serial.printf_P(PSTR("clearRecentlyResetFlag = 0x%08X\n"), multiResetDetectorFlag);
      Serial.flush();
  #endif
  #ifdef ESP8266
      //RTC only for ESP8266
      ESP.rtcUserMemoryWrite(address, &multiResetDetectorFlag, sizeof(multiResetDetectorFlag));
  #else
      // ESP32 RAM storage
      noinit_multiResetDetectorFlag = multiResetDetectorFlag;
  #endif
    }
};
#endif // ESP_MultiResetDetector_H
