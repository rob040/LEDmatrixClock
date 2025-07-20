/****************************************************************************************************************************
  ConfigOnMRD_ES8266_minimal.ino
  For ESP8266 / ESP32 boards
  
  ESP_MultiResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 multiple times.
  
  Based on and modified from
  1) DataCute    https://github.com/datacute/MultiResetDetector
  2) Khoi Hoang  https://github.com/khoih-prog/ESP_MultiResetDetector
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_MultiResetDetector
  Licensed under MIT license
 *****************************************************************************************************************************/
 
#if defined(ESP8266)
  #define USE_LITTLEFS            true
  #define ESP_MRD_USE_LITTLEFS    true
  #define ESP_MRD_USE_SPIFFS      false
  #define ESP_MRD_USE_EEPROM      false
#else  
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.  
#endif
#include <ESP_WiFiManager.h>                    //https://github.com/khoih-prog/ESP_WiFiManager
#define MRD_TIMES                     3
#define MRD_TIMEOUT                   10
#define MRD_ADDRESS                   0
#define MULTIRESETDETECTOR_DEBUG      true 
#include <ESP_MultiResetDetector.h>            //https://github.com/khoih-prog/ESP_MultiResetDetector
MultiResetDetector* mrd;
const int PIN_LED       = 2;
bool      initialConfig = false;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200); while (!Serial); delay(200);
  Serial.print(F("\nStarting ConfigOnMRD_ES8266_minimal on ")); Serial.print(ARDUINO_BOARD);
#if ESP_MRD_USE_LITTLEFS
  Serial.println(F(" using LittleFS"));
#elif ESP_MRD_USE_SPIFFS
  Serial.println(F(" using SPIFFS"));
#else
  Serial.println(F(" using EEPROM"));
#endif    
  Serial.println(ESP_WIFIMANAGER_VERSION); Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
  if (WiFi.SSID() == "") { Serial.println(F("No AP credentials")); initialConfig = true; }
  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
  if (mrd->detectMultiReset()) { Serial.println(F("MRD")); initialConfig = true; }
  if (initialConfig) {
    Serial.println(F("Starting Config Portal")); digitalWrite(PIN_LED, LOW);
    ESP_WiFiManager ESP_wifiManager("ConfigOnMRD_ES8266_minimal");
    ESP_wifiManager.setConfigPortalTimeout(0);
    if (!ESP_wifiManager.startConfigPortal()) { Serial.println(F("Not connected to WiFi")); }
    else { Serial.println(F("connected")); }
  }
  digitalWrite(PIN_LED, HIGH); Serial.print(F("After waiting ")); //WiFi.mode(WIFI_STA);
  unsigned long startedAt = millis();
  int connRes = WiFi.waitForConnectResult();
  float waited = (millis() - startedAt);
  Serial.print(waited / 1000); Serial.print(F(" secs , Connection result is ")); Serial.println(connRes);
  if (WiFi.status() != WL_CONNECTED) { Serial.println(F("Failed to connect")); }
  else { Serial.print(F("Local IP: ")); Serial.println(WiFi.localIP()); }
}
void loop() { mrd->loop(); }
