/****************************************************************************************************************************
  checkWaitingMRD.ino
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

// These defines must be put before #include <ESP_MultiResetDetector.h>
// to select where to store MultiResetDetector's variable.

// This example demonstrates how to use new function waitingForMRD() to signal the stage of MRD
// waitingForMRD() returns true if in MRD_TIMEOUT, false when out of MRD_TIMEOUT
// In this example, LED_BUILTIN will blink in MRD_TIMEOUT period, ON when DR has been detected, OFF otherwise


// Uncomment to have debug
//#define MULTIRESETDETECTOR_DEBUG       true

// These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
// Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
// Number of subsequent resets during MRD_TIMEOUT to activate
#define MRD_TIMES               3

// Number of seconds after reset during which a
// subsequent reset will be considered a multi reset.
#define MRD_TIMEOUT             10

// RTC/EEPROM Memory Address for the MultiResetDetector to use
#define MRD_ADDRESS             0

#include <ESP_MultiResetDetector.h>      // https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector

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

bool MRD_Detected = false;

void check_status()
{
  static ulong checkstatus_timeout  = 0;
  static bool LEDState = LED_OFF;

  static ulong current_millis;

#define MRD_CHECK_INTERVAL    500L

  current_millis = millis();

  // If MRD_Detected, don't need to blink, just keep LED_BUILTIN ON
  if ( !MRD_Detected && ((current_millis > checkstatus_timeout) || (checkstatus_timeout == 0)) )
  {
    // If in MRD checking loop, blinking the LED_BUILTIN
    if ( mrd->waitingForMRD() )
    {
      digitalWrite(LED_BUILTIN, LEDState);

      LEDState = !LEDState;
    }
    else
    {
      digitalWrite(LED_BUILTIN, LED_OFF);
    }

    checkstatus_timeout = current_millis + MRD_CHECK_INTERVAL;
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print("\nStarting checkWaitingMRD on"); Serial.println(ARDUINO_BOARD);

  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);

  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);

  if (mrd->detectMultiReset())
  {
    Serial.println("Multi Reset Detected");
    digitalWrite(LED_BUILTIN, LED_ON);
    MRD_Detected = true;
  }
  else
  {
    Serial.println("No Multi Reset Detected");
    digitalWrite(LED_BUILTIN, LED_OFF);
  }
}

void loop()
{
  // Call the double reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call mrd.stop() when you wish to no longer
  // consider the next reset as a double reset.
  mrd->loop();

  check_status();
}
