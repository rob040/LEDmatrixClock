/****************************************************************************************************************************
  ESP_WiFiManager_Lite_Debug.h
  For ESP8266 / ESP32 boards

  ESP_WiFiManager_Lite (https://github.com/khoih-prog/ESP_WiFiManager_Lite) is a library
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager_Lite
  Licensed under MIT license

  Version: 1.10.5

  Version Modified By   Date        Comments
  ------- -----------  ----------   -----------
  1.0.0   K Hoang      04/02/2021  Initial coding for ESP32/ESP8266
  ...
  1.8.1   K Hoang      11/02/2022  Add LittleFS support to ESP32-C3. Use core LittleFS instead of Lorol's LITTLEFS for v2.0.0+
  1.8.2   K Hoang      21/02/2022  Optional Board_Name in Menu. Optimize code by using passing by reference
  1.9.0   K Hoang      09/09/2022  Fix ESP32 chipID and add ESP_getChipOUI()
  1.10.0  K Hoang      10/01/2023  Add Captive-Portal feature
  1.10.1  K Hoang      12/01/2023  Added public methods to load and save dynamic data
  1.10.2  K Hoang      15/01/2023  Add Config Portal scaling support to mobile devices
  1.10.3  K Hoang      19/01/2023  Fix compiler error if EEPROM is used
  1.10.4  K Hoang      27/01/2023  Using PROGMEM for HTML strings
  1.10.5  K Hoang      28/01/2023  Using PROGMEM for strings in examples
 *****************************************************************************************************************************/

#ifndef ESP_WiFiManager_Lite_Debug_h
#define ESP_WiFiManager_Lite_Debug_h

#ifdef ESP_WM_LITE_DEBUG_OUTPUT
  #define DBG_PORT_ESP_WML       ESP_WM_LITE_DEBUG_OUTPUT
#else
  #define DBG_PORT_ESP_WML       Serial
#endif

// Change _ESP_WM_LITE_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _ESP_WM_LITE_LOGLEVEL_
  #define _ESP_WM_LITE_LOGLEVEL_       0
#endif

const char ESP_WML_MARK[] = "[WML] ";

#define ESP_WML_PRINT_MARK   DBG_PORT_ESP_WML.print(ESP_WML_MARK)

#define ESP_WML_PRINT        DBG_PORT_ESP_WML.print
#define ESP_WML_PRINTLN      DBG_PORT_ESP_WML.println


///////////////////////////////////////////

#define ESP_WML_LOGERROR0(x)     if(_ESP_WM_LITE_LOGLEVEL_>0) { ESP_WML_PRINT(x); }
#define ESP_WML_LOGERROR(x)      if(_ESP_WM_LITE_LOGLEVEL_>0) { ESP_WML_PRINT_MARK; ESP_WML_PRINTLN(x); }
#define ESP_WML_LOGERROR1(x,y)   if(_ESP_WM_LITE_LOGLEVEL_>0) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINTLN(y); }
#define ESP_WML_LOGERROR2(x,y,z) if(_ESP_WM_LITE_LOGLEVEL_>0) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINTLN(z); }
#define ESP_WML_LOGERROR3(x,y,z,w) if(_ESP_WM_LITE_LOGLEVEL_>0) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINTLN(w); }
#define ESP_WML_LOGERROR5(x,y,z,w,xx,yy) if(_ESP_WM_LITE_LOGLEVEL_>0) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINT(w); ESP_WML_PRINT(xx); ESP_WML_PRINTLN(yy); }

///////////////////////////////////////////

#define ESP_WML_LOGWARN0(x)     if(_ESP_WM_LITE_LOGLEVEL_>1) { ESP_WML_PRINT(x); }
#define ESP_WML_LOGWARN(x)      if(_ESP_WM_LITE_LOGLEVEL_>1) { ESP_WML_PRINT_MARK; ESP_WML_PRINTLN(x); }
#define ESP_WML_LOGWARN1(x,y)   if(_ESP_WM_LITE_LOGLEVEL_>1) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINTLN(y); }
#define ESP_WML_LOGWARN2(x,y,z) if(_ESP_WM_LITE_LOGLEVEL_>1) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINTLN(z); }
#define ESP_WML_LOGWARN3(x,y,z,w) if(_ESP_WM_LITE_LOGLEVEL_>1) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINTLN(w); }
#define ESP_WML_LOGWARN5(x,y,z,w,xx,yy) if(_ESP_WM_LITE_LOGLEVEL_>1) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINT(w); ESP_WML_PRINT(xx); ESP_WML_PRINTLN(yy); }

///////////////////////////////////////////

#define ESP_WML_LOGINFO0(x)     if(_ESP_WM_LITE_LOGLEVEL_>2) { ESP_WML_PRINT(x); }
#define ESP_WML_LOGINFO(x)      if(_ESP_WM_LITE_LOGLEVEL_>2) { ESP_WML_PRINT_MARK; ESP_WML_PRINTLN(x); }
#define ESP_WML_LOGINFO1(x,y)   if(_ESP_WM_LITE_LOGLEVEL_>2) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINTLN(y); }
#define ESP_WML_LOGINFO2(x,y,z) if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINTLN(z); }
#define ESP_WML_LOGINFO3(x,y,z,w) if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINTLN(w); }
#define ESP_WML_LOGINFO5(x,y,z,w,xx,yy) if(_ESP_WM_LITE_LOGLEVEL_>2) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINT(w); ESP_WML_PRINT(xx); ESP_WML_PRINTLN(yy); }

///////////////////////////////////////////

#define ESP_WML_LOGDEBUG0(x)     if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT(x); }
#define ESP_WML_LOGDEBUG(x)      if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINTLN(x); }
#define ESP_WML_LOGDEBUG1(x,y)   if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINTLN(y); }
#define ESP_WML_LOGDEBUG2(x,y,z) if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINTLN(z); }
#define ESP_WML_LOGDEBUG3(x,y,z,w) if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINTLN(w); }
#define ESP_WML_LOGDEBUG5(x,y,z,w,xx,yy) if(_ESP_WM_LITE_LOGLEVEL_>3) { ESP_WML_PRINT_MARK; ESP_WML_PRINT(x); ESP_WML_PRINT(y); ESP_WML_PRINT(z); ESP_WML_PRINT(w); ESP_WML_PRINT(xx); ESP_WML_PRINTLN(yy); }

///////////////////////////////////////////

#endif    //ESP_WiFiManager_Lite_Debug_h
