/****************************************************************************************************************************
  minimal.ino
  For ESP8266 / ESP32 boards
  
  ESP_MultiResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 multiple times.
  
  Based on and modified from
  1) DataCute    https://github.com/datacute/MultiResetDetector
  2) Khoi Hoang  https://github.com/khoih-prog/ESP_MultiResetDetector (archived)
  3) rob040      https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector (version 2.0+)
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_MultiResetDetector
  Licensed under MIT license
 *****************************************************************************************************************************/


// These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
// Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
// Number of subsequent resets during MRD_TIMEOUT to activate
#define MRD_TIMES               5

// Number of seconds after reset during which a 
// subsequent reset will be considered a multi reset.
#define MRD_TIMEOUT             10

// RTC Memory Address for the MultiResetDetector to use (onlt appliccable to ESP8266)
#define MRD_ADDRESS             0

// Generate some debug output
#define MULTIRESETDETECTOR_DEBUG       true  //false

#include <ESP_MultiResetDetector.h>            // https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector

MultiResetDetector* mrd;

#ifdef ESP32

  // For ESP32
  #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #endif

  #define LED_ON      HIGH
  #define LED_OFF     LOW

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
  
  Serial.print(F("\nStarting ESP_MultiResetDetector minimal on ")); 
  Serial.print(ARDUINO_BOARD);

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
