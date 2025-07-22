/****************************************************************************************************************************
  ConfigOnMRD_ESP32_minimal.ino
  For ESP32 boards

  ESP_MultiResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 multiple times.

  Based on and modified from
  1) DataCute    https://github.com/datacute/MultiResetDetector
  2) Khoi Hoang  https://github.com/khoih-prog/ESP_MultiResetDetector (archived)
  3) rob040      https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector (version 2.0+)

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_MultiResetDetector
  Licensed under MIT license
 *****************************************************************************************************************************/

#include <ESP_WiFiManager.h>                    // https://github.com/khoih-prog/ESP_WiFiManager

// These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
// Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
// Number of subsequent resets during MRD_TIMEOUT to activate
#define MRD_TIMES                     3

// Number of seconds after reset during which a 
// subsequent reset will be considered a multi reset.
#define MRD_TIMEOUT                   10

// RTC Memory Address for the MultiResetDetector to use (onlt appliccable to ESP8266)
#define MRD_ADDRESS                   0

// Generate some debug output
#define MULTIRESETDETECTOR_DEBUG       true  //false

#include <ESP_MultiResetDetector.h>            // https://github.com/rob040/LEDmatrixClock/lib/ESP_MultiResetDetector

MultiResetDetector* mrd;
const int PIN_LED = 2;
#define LED_ON      HIGH
#define LED_OFF     LOW

bool      initialConfig = false;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
  delay(200);

  Serial.print(F("\nStarting ConfigOnMRD_ESP32_minimal on "));
  Serial.print(ARDUINO_BOARD);
  Serial.println(ESP_WIFIMANAGER_VERSION);
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);

  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);

  if (mrd->detectMultiReset()) {
    Serial.println(F("MRD"));
    initialConfig = true;
  }

  ESP_WiFiManager ESP_wifiManager("ConfigOnMRD_ESP32_minimal");
  ESP_wifiManager.setConfigPortalTimeout(0);

  if (ESP_wifiManager.WiFi_SSID() == "") {
    Serial.println(F("No AP credentials"));
    initialConfig = true;
  }

  if (initialConfig) {
    Serial.println(F("Starting Config Portal"));
    digitalWrite(PIN_LED, LED_ON);
    if (!ESP_wifiManager.startConfigPortal()) {
      Serial.println(F("Not connected to WiFi"));
    }
    else {
      Serial.println(F("connected"));
    }
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }
  digitalWrite(PIN_LED, LED_OFF);

  Serial.print(F("After waiting "));

  unsigned long startedAt = millis();
  int connRes = WiFi.waitForConnectResult();
  int waited = (millis() - startedAt);

  Serial.print(waited / 1000.0);
  Serial.print(F(" secs , Connection result is "));
  Serial.println(connRes);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Failed to connect"));
  }
  else {
    Serial.print(F("Local IP: "));
    Serial.println(WiFi.localIP());
  }
}

void loop()
{
  mrd->loop();
}
