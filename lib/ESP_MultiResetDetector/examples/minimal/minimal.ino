/****************************************************************************************************************************
  minimal.ino
  For ESP8266 / ESP32 boards
  
  ESP_MultiResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 multiple times.
  
  Based on and modified from
  1) DataCute    https://github.com/datacute/MultiResetDetector
  2) Khoi Hoang  https://github.com/khoih-prog/ESP_MultiResetDetector
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_MultiResetDetector
  Licensed under MIT license
 *****************************************************************************************************************************/

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
