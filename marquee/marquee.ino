/**
 * Copyright (c) 2018 David Payne
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 */

#include "Settings.h"

#define VERSION "3.3.0"

// Refresh main web page every x seconds. The mainpage has button to activate its auto-refresh
#define WEBPAGE_AUTOREFRESH   30

// DARK mode: Add button to main page to toggle webpage dark mode
#define WEBPAGE_DARKMODE

// matrix fillscreen clear color is 0
#define CLEARSCREEN  0


// declaring local prototypes

void setup();
void loop();
void displayScrollMessage(const String &msg);
void displayScrollErrorMessage(const String &msg, bool showOnce);
void processEveryMinute();
void processEverySecond();
String hourMinutes(bool isRefresh);
char secondsIndicator(bool isRefresh);
bool authentication();
void handlePull();
void handleSaveMqtt();
void handleSaveConfig();
void handleSystemReset();
void restartEsp();
void handleForgetWifi();
void handleMqttConfigure();
void handleConfigure();
void handleDisplay();
void getWeatherData();
void webDisplayMessage(const String &message);
void redirectHome();
void sendHeader(bool isMainPage = false);
void sendFooter();
void webDisplayWeatherData();
void onBoardLed(bool on);
void flashLED(int number, int delayTime);
String getTempSymbol(bool forWeb = false);
String getSpeedSymbol();
String getPressureSymbol();
String getTimeTillUpdate();
int8_t getWifiQuality();
int getMinutesFromLastRefresh();
int getMinutesFromLastDisplay();
int getMinutesFromLastDisplayScroll();
void enableDisplay(bool enable);
void checkDisplay();
void writeConfiguration();
void readConfiguration();
void scrollMessageSetup(const String &msg);
void scrollMessageNext();
void scrollMessageWait(const String &msg);
bool staticDisplaySetupSingle(char * message);
void staticDisplaySetup(void);
void staticDisplayNext(void);
void centerPrint(const String &msg, bool extraStuff = false);
String EncodeHtmlSpecialChars(const char *msg);
String EncodeUrlSpecialChars(const char *msg);

// LED font constants; the font is a 5x7 pixels in a 6x8 space
const int font_space = 1;  // dots between letters
const int font_width = 5 + font_space; // The font width is 5 pixels + font_space

Max72xxPanel matrix = Max72xxPanel(pinCS, 0, 0); // will be re-instantiated later in setup()

// Time
int lastMinute;
int lastSecond;
//uint32_t proc1sLastTime;
uint32_t lastDisplayScrollTimestamp;
uint32_t lastRefreshDataTimestamp;
uint32_t firstTimeSync;
uint32_t displayOffTimestamp;
bool displayOn = true;
bool isDisplayTimeNew;
bool isDisplayMessageNew;
bool isDisplayScrollErrorMsgNew;
bool isDisplayScrollErrorMsgOnce;
bool isQuietPeriod;
bool isQuietPeriodNoBlinkNoscroll;
bool isMqttStatusPublishDone;
String displayTime;
char * newMqttMessage;
String displayScrollMessageStr;
String displayScrollErrorMsgStr;


// Scroll message variables
bool scrlBusy;
int scrlPixTotal;
int scrlPixIdx;
int scrlPixY;
int scrlMsgLen;
unsigned long scrlPixelLastTime;
String scrlMsg; // copy of scrolling message

//Static data display
String staticDisplay[8];
bool isStaticDisplayNew;
bool isStaticDisplayBusy;
int staticDisplayIdx;
int staticDisplayIdxOut;
unsigned long staticDisplayLastTime;


// loop() FSM statemachine
enum loopState_e {
  lStateIdle,
  lStateScrollMsgPix,
  lStateScrollNewMqttMsgPix,
  lStateScrollErrMsgPix,
  lStateDispStaticMsg,
};
enum loopState_e loopState, lastState;

// Scheduler macro
// SCHEDULE_INTERVAL(unsigned long work_variable, int intervaltime_ms, function)
//   will call the function at regular time intervals, when included in loop().
#define SCHEDULE_INTERVAL(_var,_interval_ms,_func) { if((millis()-(_var))>(unsigned)(_interval_ms)){_var=millis();_func();}}
#define SCHEDULE_INTERVAL_START(_var,_delay) {_var=millis()-(_delay);}

// Weather Client
OpenWeatherMapClient weatherClient(owmApiKey, isMetric);

#if COMPILE_MQTT
// Mqtt Client
MqttClient mqttClient(MqttServer, MqttPort, MqttTopic, MqttAuthUser, MqttAuthPass);
#endif

ESP8266WebServer server(WEBSERVER_PORT);
ESP8266HTTPUpdateServer serverUpdater;

ESP_WiFiManager_Lite* ESP_WiFiManager;

ESP_WM_LITE_Configuration defaultConfig = WML_DEFAULT_CONFIG;//{0};


static const char webHeaderHtml[] PROGMEM = "<!DOCTYPE HTML>"
  "<html><head>"
  "<title>Marquee LED matrix Clock</title>"
  "<meta charset='UTF-8'>"
  "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />"
  "<meta name='viewport' content='width=device-width, initial-scale=1'>"
  "<meta name='description' content='Clock and Weather Marquee displaying time and current weather information on LED matrix display.'>"
  "<meta name='keywords' content='clock, LED matrix display, weather, marquee, information'>"
  "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>"
  "<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-$COLOR$.css'>"
  "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.8.1/css/all.min.css'>"
  "<link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAACJUlEQVQ4T2NkwAUcHFj0LKPegKQvHV8mwn"
  "DgwB9sShlDQ0OZbyk5bf0iIKWEouA/A+N/RkYVBgZGIOPfHSD1H1me58Ozexfv7fNmdEgrFrmpE/z6uZIlin5GIA9FB5r1kvePM0hdXisKMUA3+PUXAWkG9m8fcHiIC"
  "Sj+Dy73k0uAgefDUwZ1ZAO4Pr9meCVjwCD++CzOYAFJvJQ1ZhB9coHhO48og/oVJBew/frG8FjZhkHi4Rm8BryQN2GQvXuE4RcrF9SAYqAXFIJfww14RMAAOXQDoGHA"
  "+/4xw2dBWQbuTy/gLngvqgpmC76+DRf7yifBAFYrIIvqhZeyRgwsv38iOf8/g8ydw2D+ExVbIAmKFwj4w8oODKtz0EBsAHpBGBiNiqjRCFLI//IuAwMwAj6KKiMZDIo"
  "NJgZQNKLEwj9GFgbWHx8ZmP//Y/jKL8XwRkoHb7qAG2CbWrHkK5+4ENPfvwzfeEUYGP/9BaaHjww/eITwxgbHt/fA8Hr5jtE2rXLev38M8xkYOIEavuPVhC7JzMSYwG"
  "iXXm7H8BfofiD4xclbddMg2Fn50qaTXF/fVcE0cAAZ77iF2u7q+ZmrX1i7l+375zawHPP/P/CgtU2v1Lqt43fhHws7s/qZFZaH53adQrbRNrnM7KZJxHGmvz//ql7eZ"
  "HB4Zvs1kDzcAJu06kVPlawDhV5dX3G2rzgVm1+Mi3pnvxPTjJC+d3T9kVmtcSA1AN+w4BmrADV4AAAAAElFTkSuQmCC'>"
  "<style>"
    "body{"
      "font-family:Arial, sans-serif;"
      "font-size:16px;"
    "}"
    ".w3-button{"
      "border:none;"
      "border-radius:5px;"
      "cursor:pointer;"
    "}"
    ".w3-button:hover{"
      "background-color:#ccc;"
    "}"
    ".w3-small{margin:4px 0 4px;}"
    ".dark-mode{"
      "background-color:#333;"
      "color:#fff;"
    "}"
    ".dark-mode *{"
      "background-color:inherit;"
      "color:inherit;"
    "}"
    ".dark-mode .w3-theme{"
      "background-color:#444;"
    "}"
    ".dark-mode .w3-theme-d2{"
      "background-color:#555;"
    "}"
    ".dark-mode i{"
      "color:#fff;"
    "}"
  "</style>"
  "</head>\n"
"<body>\n"
  "<header class='w3-top w3-bar w3-theme'>"
  "<button class='w3-bar-item w3-button w3-xxxlarge w3-hover-theme' onclick='openSidebar()'>"
  "<i class='fas fa-bars'></i>"
  "</button>"
  "<h3 class='w3-bar-item'>Weather Marquee</h3>"
  ;
static const char webHeaderMainPage[] PROGMEM =
  "<div class='w3-right'>"
    #if defined (WEBPAGE_AUTOREFRESH) && (WEBPAGE_AUTOREFRESH > 0)
    "<button id='autorefresh-button' class='w3-button w3-small' onclick='toggleAutoRefresh()' title='toggle Auto Refresh Mode'>"
       "<i id='autorefresh-icon' class='fas fa-sync'></i>"
    "</button><br>"
    #endif
    #if defined (WEBPAGE_DARKMODE)
    "<button id='darkmode-button' class='w3-button w3-small' onclick='toggleDarkMode()' title='toggle Dark Mode '>"
       "<i id='darkmode-icon' class='fas fa-moon'></i>"
    "</button>"
    #endif
  "</div>"
;

static const char webBody1[] PROGMEM =
  "</header>\n"
  "<nav id='mySidebar' class='w3-sidebar w3-bar-block w3-card'>"
  "<div class='w3-container w3-theme-d2'>"
  "<span onclick='closeSidebar()' class='w3-button w3-display-topright w3-large'><i class='fas fa-times'></i></span>"
  "<div class='w3-left'><img src='http://openweathermap.org/img/w/$ICO$.png' alt='$IDES$'></div>"
  "<div class='w3-padding'>Menu</div></div>";

static const char webBody2[] PROGMEM =
  "</nav>\n"
  "<script>"
  "function openSidebar(){document.getElementById('mySidebar').style.display='block'}"
  "function closeSidebar(){document.getElementById('mySidebar').style.display='none'}"
  "closeSidebar();"
  #if defined (WEBPAGE_DARKMODE)
  "function restoreDarkMode(){const darkModeEnabled=localStorage.getItem('darkModeEnabled')==='true';"
  "if(darkModeEnabled){document.body.classList.add('dark-mode');}}"
  "restoreDarkMode();"
  #endif
  "</script>\n"
  "<div class='w3-container w3-large' style='margin-top:88px'>\n";

static const char webBody2MainPage[] PROGMEM =
  "<script>"
  #if defined (WEBPAGE_AUTOREFRESH) && (WEBPAGE_AUTOREFRESH > 0)
  "var intervaltimer=0;"
  "function refreshPage(){if(document.getElementById('mySidebar').style.display==='none')window.location.reload();}"
  "function toggleAutoRefresh(){const autoRefreshEnabled=localStorage.getItem('autoRefreshEnabled')==='true';"
  "const newState=!autoRefreshEnabled;localStorage.setItem('autoRefreshEnabled',newState);updateAutoRefreshButton(newState);"
  "if(newState){startAutoRefresh();}else{stopAutoRefresh();}}"
  "function updateAutoRefreshButton(enabled){const autoRefreshIcon=document.getElementById('autorefresh-icon');"
  "if(enabled){autoRefreshIcon.classList.add('fa-spin');}else{autoRefreshIcon.classList.remove('fa-spin');}}"
  "function stopAutoRefresh(){clearInterval(intervaltimer);}"
  "function startAutoRefresh(){intervaltimer=setInterval(refreshPage," __STRINGIZE(WEBPAGE_AUTOREFRESH) "*1000);}"
  "function initAutoRefresh(){const autoRefreshEnabled=localStorage.getItem('autoRefreshEnabled')==='true';"
  "if(autoRefreshEnabled){startAutoRefresh();updateAutoRefreshButton(true);}"
  "else{stopAutoRefresh();updateAutoRefreshButton(false);}}"
  "initAutoRefresh();"
  #endif
  #if defined (WEBPAGE_DARKMODE)
  "function toggleDarkMode(){const darkModeEnabled=localStorage.getItem('darkModeEnabled')==='true';"
  "const newState=!darkModeEnabled;localStorage.setItem('darkModeEnabled',newState);updateDarkModeButton(newState);"
  "document.body.classList.toggle('dark-mode',newState);}"
  "function updateDarkModeButton(enabled){const darkModeIcon=document.getElementById('darkmode-icon');"
  "if(enabled){darkModeIcon.classList.replace('fa-moon','fa-sun');}"
  "else{darkModeIcon.classList.replace('fa-sun','fa-moon');}}"
  "function initDarkMode(){const darkModeEnabled=localStorage.getItem('darkModeEnabled')==='true';"
  "if(darkModeEnabled){document.body.classList.add('dark-mode');updateDarkModeButton(true);}"
  "else{updateDarkModeButton(false);}}"
  "initDarkMode();"
  #endif
  "</script>\n";

static const char webFooterHtml[] PROGMEM = "<br><br><br>"
  "</div>\n"
  "<footer class='w3-container w3-bottom w3-theme'>"
  "<i class='far fa-paper-plane'></i> Version: " VERSION " build " __DATE__ " " __TIME__ "<br>"
  "<i class='far fa-clock'></i> Next Data Update: $UPD$ <br>"
  "<i class='fas fa-rss'></i> Signal Strength: $RSSI$%"
  "</footer>\n"
  "</body></html>\n";

static const char webActions1[] PROGMEM =
  "<a class='w3-bar-item w3-button' href='/'><i class='fas fa-home'></i> Home</a>"
  "<a class='w3-bar-item w3-button' href='/configure'><i class='fas fa-cog'></i> Configure</a>"
  #if COMPILE_MQTT
  "<a class='w3-bar-item w3-button' href='/configuremqtt'><i class='fas fa-network-wired'></i> MQTT</a>"
  #endif
  "<a class='w3-bar-item w3-button' href='/pull'><i class='fas fa-cloud-download-alt'></i> Refresh Data</a>"
  "<a class='w3-bar-item w3-button' href='/display'>";

static const char webActions2_ON[] PROGMEM =
  "<i class='fas fa-eye-slash'></i> Turn Display OFF";
static const char webActions2_OFF[] PROGMEM =
  "<i class='fas fa-eye'></i> Turn Display ON";

static const char webActions3[] PROGMEM =
  "</a><a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default weather settings?\")'>"
  "<i class='fas fa-undo'></i> Reset Settings</a>"
  "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fas fa-wifi'></i> Forget WiFi</a>"
  "<a class='w3-bar-item w3-button' href='/restart'><i class='fas fa-sync'></i> Restart</a>"
  "<a class='w3-bar-item w3-button' href='/update'><i class='fas fa-wrench'></i> Firmware Update</a>"
  "<a class='w3-bar-item w3-button' href='https://github.com/rob040/LEDmatrixClock' target='_blank'><i class='fas fa-question-circle'></i> About</a>";

static const char webChangeForm1[] PROGMEM =
  "<form class='w3-container' action='/saveconfig' method='get'><h2>Configure:</h2>"
  "<fieldset><legend>OpenWeatherMap configuration</legend>"
  "<label>OpenWeatherMap API Key (get free access from <a href='https://openweathermap.org/price#freeaccess' target='_BLANK'>openweathermap.org</a>)</label>"
  "<input class='w3-input w3-border' type='text' name='openWeatherMapApiKey' value='%OWMKEY%' maxlength='70'>"
  "<p><label>Geo Location "
  "<small>Enter one of `City-name,2-letter-country-code` OR `City-ID` OR GPS `Latitude,Longitude`</small> "
  "(<a href='http://openweathermap.org/find' target='_BLANK'><i class='fas fa-search'></i> Search for Geo Location</a>) </label>"
  "<input class='w3-input w3-border' type='text' name='gloc' value='%GLOC%'>"
  "<label>%CTYNM%</label></p>"
  "</fieldset>\n"
  "<fieldset><legend>LED Display weather data options</legend>"
  "<p><input name='showtemp' class='w3-check' type='checkbox' %TEMP_CB%> Display Temperature</p>"
  "<p><input name='showdate' class='w3-check' type='checkbox' %DATE_CB%> Display Date</p>"
  "<p><input name='showcity' class='w3-check' type='checkbox' %CITY_CB%> Display City Name</p>"
  "<p><input name='showhighlow' class='w3-check' type='checkbox' %HILO_CB%> Display Current High/Low Temperatures</p>"
  "<p><input name='showcondition' class='w3-check' type='checkbox' %COND_CB%> Display Weather Condition</p>"
  "<p><input name='showhumidity' class='w3-check' type='checkbox' %HUM_CB%> Display Humidity</p>"
  "<p><input name='showwind' class='w3-check' type='checkbox' %WIND_CB%> Display Wind</p>"
  "<p><input name='showpressure' class='w3-check' type='checkbox' %PRES_CB%> Display Barometric Pressure</p>"
  "</fieldset>\n"
  "<fieldset><legend>LED Display scrolling message</legend>"
  "<p><label>Marquee Message (up to 80 chars)</label><input class='w3-input w3-border' type='text' name='marqueeMsg' value='%MSG%' maxlength='80'></p>"
  "</fieldset>\n";

static const char webChangeForm2[] PROGMEM =
  "<fieldset><legend>Data Display settings</legend>"
  // title='Any selected Display Scrolling Data is shown in short form without movement (scroll) when fitting within display width.'
  "<p><input name='statdisp' class='w3-check' type='checkbox' %STATDISP_CB%> Do not scroll weather data display</p>"
  "<p><input name='metric' class='w3-check' type='checkbox' %METRIC_CB%> Use Metric units (Celsius,kmh,mBar); Unchecked: use Imperial units (Farenheid,mph,inHg)</p>"
  "<p><input name='is24hour' class='w3-check' type='checkbox' %24HR_CB%> Use 24 Hour Clock; Unchecked: use 12 Hour clock</p>"
  "<p><input name='isPM' class='w3-check' type='checkbox' %PM_CB%> Show PM indicator (only on 12 Hour clock)</p>"
  "<p><input name='flashseconds' class='w3-check' type='checkbox' %FLASH_CB%> Blink \":\" in the time</p>"
  "</fieldset>\n"
  "<fieldset><legend>LED Display Quiet Times</legend>"
  "<p><label>Quiet time mode &nbsp;&nbsp;&nbsp;</label> <select class='w3-option w3-padding' name='qtmode'>%QTM_OPT%</select></p>"
  "<p><label>Quiet time dimlevel </label><input class='w3-border' name='qtlvl' type='number' min='0' max='15' value='%QTDIM%'></p>"
  "<p><label>Start quiet Time &nbsp;</label><input name='qtstrt' type='time' value='%QTST%'></p>"
  "<p><label>End quiet Time &nbsp;&nbsp;</label><input name='qtend' type='time' value='%QTEND%'></p>"
  "</fieldset>\n"
  "<fieldset><legend>LED Display settings</legend>"
  "<p>Display Brightness <input class='w3-border' name='ledintensity' type='number' min='0' max='15' value='%INTY_OPT%'></p>"
  "<p>Display Width (in 8x8 pixel tiles) <input class='w3-border' name='displaywidth' type='number' min='4' max='32' value='%DTW_OPT%'></p>"
  "<p>Display Format for &ge; 8 tiles <select class='w3-option w3-padding' name='wideclockformat' title='Format options for Display with 8 or more tiles' $WCLKDIS$>%WCLK_OPT%</select></p>"
  "<p>Display Scroll Speed <select class='w3-option w3-padding' name='scrollspeed'>%SCRL_OPT%</select></p>"
  "<p>Display Scrolling Data interval <input class='w3-border' name='refreshDisplay' type='number' min='1' max='10' value='%RFSH_DISP%'> (minutes)</p>"
  "<p>Data Refresh interval <select class='w3-option w3-padding' name='refresh'>%RFSH_OPT%</select> (minutes)</p>"
  "<p><input name='sysled' class='w3-check' type='checkbox' %SYSLED_CB%> Flash System LED on WiFi activity</p>"
  "</fieldset>\n";

static const char webChangeForm3[] PROGMEM =
  "<fieldset><legend>Web page settings</legend>"
  "<p>Theme Color <select class='w3-option w3-padding' name='theme'>%THEME_OPT%</select></p>"
  "<p><input name='isBasicAuth' class='w3-check' type='checkbox' %AUTH_CB%> Use Security Credentials for Configuration Changes</p>"
  "<p><label>Configure User ID (for this web interface)</label><input class='w3-input w3-border' type='text' name='userid' value='%CFGUID%' maxlength='20'></p>"
  "<p><label>Configure Password </label><input class='w3-input w3-border' type='password' name='stationpassword' value='%CFGPW%'></p>"
  "</fieldset>\n"
  "<br><button class='w3-button w3-block w3-green w3-section w3-padding' type='submit'>Save</button></form>"
  "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

#if COMPILE_MQTT
static const char webMQTTform[] PROGMEM =
  "<form class='w3-container' action='/savemqtt' method='get'><h2>MQTT Configuration:</h2>"
  "<p><input name='displaymqtt' class='w3-check' type='checkbox' %MQTT_CB%> Enable MQTT</p>"
  "<label>MQTT Address (do not include http://)</label><input class='w3-input w3-border' type='text' name='mqttAddress' id='mqttAddress' value='%MQTT_ADR%' maxlength='60'>"
  "<label>MQTT Port</label><input class='w3-input w3-border' type='text' name='mqttPort' id='mqttPort' value='%MQTT_PRT%' maxlength='5'  onkeypress='return isNumberKey(event)'>"
  "<label>MQTT Topic</label><input class='w3-input w3-border' type='text' name='mqttTopic' id='mqttTopic' value='%MQTT_TOP%' maxlength='128'>"
  "<label>MQTT server User (leave empty when not required) </label><input class='w3-input w3-border' type='text' name='mqttUser' value='%MQTT_USR%' maxlength='30'>"
  "<label>MQTT server Password </label><input class='w3-input w3-border' type='password' name='mqttPass' value='%MQTT_PW%'>"
  "<button class='w3-button w3-block w3-green w3-section w3-padding' type='submit'>Save</button></form>"
  "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";
#endif

static const char webColorThemes[] PROGMEM =
  "<option>red</option>"
  "<option>pink</option>"
  "<option>purple</option>"
  "<option>deep-purple</option>"
  "<option>indigo</option>"
  "<option>blue</option>"
  "<option>light-blue</option>"
  "<option>cyan</option>"
  "<option>teal</option>"
  "<option>green</option>"
  "<option>light-green</option>"
  "<option>lime</option>"
  "<option>khaki</option>"
  "<option>yellow</option>"
  "<option>amber</option>"
  "<option>orange</option>"
  "<option>deep-orange</option>"
  "<option>blue-grey</option>"
  "<option>brown</option>"
  "<option>grey</option>"
  "<option>dark-grey</option>"
  "<option>black</option>"
  "<option>w3schools</option>";



void setup() {
  Serial.begin(115200);
  FS.begin();
  // uncomment for testing, comment for release!
  //FS.remove(CONFIG);
  delay(10);

  // Initialize digital pin for LED
  pinMode(LED_ONBOARD, OUTPUT);

  //New Line to clear from start garbage
  Serial.println();

  readConfiguration();

  Serial.printf_P(PSTR("LED Display tiles wide: %d\n"), displayWidth);
  // initialize display
  matrix = Max72xxPanel(pinCS, displayWidth, displayHeight);
  matrix.setIntensity(0); // Use a value between 0 and 15 for brightness
  matrix.cp437(true); // use true CP437 (IBM-PC) character font

  int maxPos = displayWidth * displayHeight;
  for (int i = 0; i < maxPos; i++) {
    matrix.setRotation(i, ledRotation);
    matrix.setPosition(i, maxPos - i - 1, 0);
  }

  Serial.println(F("matrix created"));
  matrix.fillScreen(CLEARSCREEN);
  centerPrint(F("hello"));
  // welcome continued later

#ifdef BUZZER_PIN
  tone(BUZZER_PIN, 415, 500);
  delay(500 * 1.3);
  tone(BUZZER_PIN, 466, 500);
  delay(500 * 1.3);
  tone(BUZZER_PIN, 370, 1000);
  delay(1000 * 1.3);
  noTone(BUZZER_PIN);
#endif

  // WiFiManager
  // ESP_WiFiManager_Lite (multiWifi, Activation on multi reset detect):

  ESP_WiFiManager = new ESP_WiFiManager_Lite();
  // Setup Config Portal hostname, without access password
  String hostname(AP_HOSTNAME_BASE);
  hostname += String(ESP.getChipId(), HEX);
  hostname.toUpperCase();
  ESP_WiFiManager->setConfigPortal(hostname);
  // Set customized DHCP HostName
  ESP_WiFiManager->begin(hostname.c_str());

  // print the received signal strength:
  Serial.printf_P(PSTR("Signal Strength (RSSI): %d%%\n"), getWifiQuality());

  if (isOTAenabled) {
    ArduinoOTA.onStart([]() {
      Serial.println(F("Start OTA"));
    });
    ArduinoOTA.onEnd([]() {
      Serial.println(F("\nEnd OTA"));
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf_P(PSTR("Progress: %u%%\r"), (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf_P(PSTR("OTA Error[%u]: "), error);
      if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
      else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
      else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
      else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
      else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
    });
    ArduinoOTA.setHostname((const char *)hostname.c_str());
    if (OTA_Password != "") {
      ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
    }
    ArduinoOTA.begin();
  }

  if (!ESP_WiFiManager->isConfigMode()) {
    // Continue welcome 'hello' for 4.5 to 6 seconds
    centerPrint(F("hello"));
    for (int inx = 0; inx <= 15; inx++) {
      matrix.setIntensity(inx);
      delay(100);
    }
    delay(200);
    for (int inx = 15; inx >= 0; inx--) {
      matrix.setIntensity(inx);
      delay(90);
    }
    delay(400);
    for (int inx = 0; inx <= displayIntensity; inx++) {
      matrix.setIntensity(inx);
      delay(100);
    }
    delay(800);
  }
  matrix.setIntensity(displayIntensity);

  if (isWebserverEnabled) {
    server.on("/", webDisplayWeatherData);
    server.on("/pull", handlePull);
    server.on("/saveconfig", handleSaveConfig);
    #if COMPILE_MQTT
    server.on("/savemqtt", handleSaveMqtt);
    #endif
    server.on("/systemreset", handleSystemReset);
    server.on("/forgetwifi", handleForgetWifi);
    server.on("/restart", restartEsp);
    server.on("/configure", handleConfigure);
    #if COMPILE_MQTT
    server.on("/configuremqtt", handleMqttConfigure);
    #endif
    server.on("/display", handleDisplay);
    server.onNotFound(redirectHome);
    serverUpdater.setup(&server, "/update", www_username, www_password);
    // Start the server
    server.begin();
    if (ESP_WiFiManager->isConfigMode()) {
      Serial.print(F("Config portal started "));
    } else if (WiFi.status() == WL_CONNECTED) {
      Serial.print(F("Server started "));
    } else {
      Serial.print(F("Server started but NO WIFI "));
    }
    // Print the IP address
    char webAddress[32];
    sprintf_P(webAddress, PSTR("v" VERSION "  IP: %s  "),
      (ESP_WiFiManager->isConfigMode()) ? WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str());
    Serial.println(webAddress);
    scrollMessageWait(webAddress);
    if (ESP_WiFiManager->isConfigMode()) {
      String msg = F("Wifi Manager Started... Please Connect to AP: ") + WiFi.softAPSSID() + F(" password: My") + WiFi.softAPSSID();
      Serial.println(msg);
      scrollMessageWait(msg);
      centerPrint(F("wifi"));
    }
    // Start NTP , although it can't do anything while in config mode or when no WiFi AP connected
    timeNTPsetup();

  } else {
    String msg = F("Web Interface is Disabled");
    Serial.println(msg);
    scrollMessageWait(msg);
  }
  SCHEDULE_INTERVAL_START(scrlPixelLastTime, 2000);
  flashLED(1, 500);
}

//************************************************************
// Main Loop
//************************************************************
void loop() {

  if (lastSecond != second()) {
    lastSecond = second();

    if (!ESP_WiFiManager->isConfigMode()) {
      static int scrlBusyCnt = 0;
      if (!scrlBusy) {
        uint32_t t = millis();
        if (scrlBusyCnt != 0) {
          Serial.printf_P(PSTR("scroll busy for %d s\n"), scrlBusyCnt);
        }
        scrlBusyCnt = 0;
        processEverySecond();
        t = millis() - t;
        if (t > 100) Serial.printf_P(PSTR("proc1s took %u ms\n"), t);

        if (lastMinute != minute()) {
          uint32_t t = millis();
          lastMinute = minute();
          processEveryMinute();
          t = millis() - t;
          if (t > 100) Serial.printf_P(PSTR("proc1m took %u ms\n"), t);
        }
      }
      else {
        if (++scrlBusyCnt > 60) {
          Serial.print(F("scroll busy too long, resetting\n"));
          scrlBusyCnt = 0;
        }
      }
    }
  }
  //SCHEDULE_INTERVAL(proc1sLastTime, 1000, processEverySecond);

  SCHEDULE_INTERVAL(scrlPixelLastTime, displayScrollSpeed, scrollMessageNext);

  SCHEDULE_INTERVAL(staticDisplayLastTime, staticDisplayTime, staticDisplayNext);

  if (loopState != lastState) {
    #if DEBUG
      Serial.printf_P(PSTR("[%u] loopstate %d -> %d\n"), millis()&0xFFFF, lastState, loopState);
    #endif
    lastState = loopState;
  }
  switch (loopState) {
  default:
  case lStateIdle: // Idle: Show time on display, when ON
      if (isDisplayScrollErrorMsgNew) {
        scrollMessageSetup(displayScrollErrorMsgStr);
        if (isDisplayScrollErrorMsgOnce) {
          isDisplayScrollErrorMsgNew = false;
          isDisplayScrollErrorMsgOnce = false;
        }
        loopState = lStateScrollErrMsgPix;
      } else
      if (newMqttMessage != 0 && newMqttMessage[0] != 0) {
        // if static display mode enabled and message length fits screen
        if (staticDisplaySetupSingle(newMqttMessage)) {
          newMqttMessage = NULL;
          loopState = lStateDispStaticMsg;
        } else {
          scrollMessageSetup(newMqttMessage);
          newMqttMessage = NULL;
          loopState = lStateScrollNewMqttMsgPix;
        }
      } else
      if (isDisplayMessageNew) {
        scrollMessageSetup(displayScrollMessageStr);
        isDisplayMessageNew = false;
        loopState = lStateScrollMsgPix;
      } else
      if (isStaticDisplayNew) {
        staticDisplaySetup();
        loopState = lStateDispStaticMsg;
      } else
      if (isDisplayTimeNew) {
        isDisplayTimeNew = false;
        if (displayOn) {
          matrix.fillScreen(CLEARSCREEN);
          centerPrint(displayTime, true);
        }
      }
      break;
  case lStateScrollMsgPix:
      if (!scrlBusy) loopState = lStateIdle;
      break;
  case lStateScrollNewMqttMsgPix:
      if (!scrlBusy) loopState = lStateIdle;
      break;
  case lStateScrollErrMsgPix:
      if (!scrlBusy) loopState = lStateIdle;
      break;
  case lStateDispStaticMsg:
      if (!isStaticDisplayNew)  loopState = lStateIdle;
      break;
  }
  if (loopState != lastState) {
    #if DEBUG
      Serial.printf_P(PSTR("[%u] loopstate -> %d\n"), millis()&0xFFFF, loopState);
    #endif
  }


  if (isWebserverEnabled) {
    server.handleClient();
  }
  if (isOTAenabled) {
    ArduinoOTA.handle();
  }

  ESP_WiFiManager->run();
}

void displayScrollMessage(const String &msg)
{
  displayScrollMessageStr = msg;
  isDisplayMessageNew = true;
}

void displayScrollErrorMessage(const String &msg, bool showOnce)
{
  Serial.printf_P(PSTR("setDispERRmsg: %s\n"), msg.c_str());
  displayScrollErrorMsgStr = msg;
  isDisplayScrollErrorMsgNew = true;
  isDisplayScrollErrorMsgOnce = showOnce;
}

// This is called every minute, synchronized with the internal clock minute change
void processEveryMinute()
{

    // Get some Weather Data to serve
    if ((getMinutesFromLastRefresh() >= refreshDataInterval) || lastRefreshDataTimestamp == 0) {
      getWeatherData();
      lastRefreshDataTimestamp = now();
    }

    if (weatherClient.getErrorMessage() != "") {
      displayScrollErrorMessage(weatherClient.getErrorMessage(), true);
      return;
    }
    isMqttStatusPublishDone = false; // allow mqtt status publish again next minute

    // Check to see if we need to Scroll some weather Data
    if ((getMinutesFromLastDisplayScroll() >= displayScrollingInterval) && weatherClient.getWeatherDataValid() && (weatherClient.getErrorMessage().length() == 0)) {
      String msg = " ";
      String temperature = String(weatherClient.getTemperature(),0);
      String weatherDescription = weatherClient.getWeatherDescription();
      weatherDescription.toUpperCase();
      staticDisplayIdx = 0;

      if (showDate) {
        if (!isStaticDisplay) {
          msg += getDayName(weekday()) + ", ";
          msg += getMonthName(month()) + " " + day() + "  ";
        } else {
          if (isMetric) {
            staticDisplay[staticDisplayIdx] = zeroPad(month()) + "-" + zeroPad(day());
          } else {
            staticDisplay[staticDisplayIdx] = zeroPad(day()) + "," + zeroPad(month());
          }
          staticDisplayIdx++;
        }
      }
      if (showCity && !isStaticDisplay) {
        msg += weatherClient.getCity() + "  ";
      }
      if (showTemperature) {
        if (!isStaticDisplay) {
          msg += temperature + getTempSymbol() + "  ";
        } else {
          staticDisplay[staticDisplayIdx] = temperature + getTempSymbol();
          staticDisplayIdx++;
        }
      }

      // show high/low temperature
      if (showHighLow && !isStaticDisplay) {
        msg += F("High/Low:") + String(weatherClient.getTemperatureHigh(),0) + "/" + String(weatherClient.getTemperatureLow(),0) + "  ";
      }
      if (showCondition && !isStaticDisplay) {
        msg += weatherDescription + "  ";
      }
      if (showHumidity) {
        if (!isStaticDisplay) {
          msg += F("Humidity:") + String(weatherClient.getHumidity()) + "%  ";
        } else {
          staticDisplay[staticDisplayIdx] = String(weatherClient.getHumidity()) + ((displayWidth>=6)?"%RH":"%");
          staticDisplayIdx++;
        }
      }
      if (showWind) {
        String windspeed = String(weatherClient.getWindSpeed(),0);
        windspeed.trim();
        if (!isStaticDisplay) {
          msg += F("Wind:") + weatherClient.getWindDirectionText() + " " + windspeed + getSpeedSymbol() + "  ";
        } else {
          // 4 tile display can fit up to "99kmh"
          staticDisplay[staticDisplayIdx] = windspeed + getSpeedSymbol();
          staticDisplayIdx++;
        }
      }
      if (showPressure) {
        if (!isStaticDisplay) {
          msg += F("Pressure:") + String(weatherClient.getPressure()) + getPressureSymbol() + "  ";
        } else {
          // 4 tile display can just fit "999mb", ie. low pressure, Imperial inHg will only fit on 8 tiles
          staticDisplay[staticDisplayIdx] = String(weatherClient.getPressure()) + getPressureSymbol();
          staticDisplayIdx++;
        }
      }
      if (marqueeMessage.length() > 0) {
        msg += marqueeMessage + "  ";
      }

      #if COMPILE_MQTT
      if (isMqttEnabled) {
        char * mqttmsg = mqttClient.getLastMqttMessage();
        if (strlen(mqttmsg)> 0) {
          if (isStaticDisplay)
          {
            if ((int)strlen(mqttmsg) <= ((displayWidth * 8) / font_width)) {
              // only show mqtt message in static display mode when it fits on screen
              staticDisplay[staticDisplayIdx] = mqttmsg;
              staticDisplayIdx++;
            }
          } else {
            // add mqtt message to scrolling message
            msg += mqttmsg;
          }
        }
      }
      #endif

      if (isStaticDisplay) {
        if (staticDisplayIdx != 0) {
          isStaticDisplayNew = true;
          staticDisplayIdxOut = 0;
        }
      }

      if ((msg.length() > 3) && !isQuietPeriodNoBlinkNoscroll) {
        displayScrollMessage(msg);
        lastDisplayScrollTimestamp = now();
      }
    }

}

void processEverySecond()
{

  #if COMPILE_MQTT
  // allow the mqtt client to do its thing
  if (isMqttEnabled) {
    mqttClient.loop();
    newMqttMessage = mqttClient.getNewMqttMessage();

    // after first connection (and every minute), when time and weather are also connected, publish one time
    // the current values of hostname and IP address
    // this is useful for home automation systems to identify the device
    if (mqttClient.connected() && !isMqttStatusPublishDone && (now() > TIME_VALID_MIN) && weatherClient.getWeatherDataValid()) {
        // Post to unique status topic, containing hostname with value IP address
      char msg[128];
      String datetime = String(year()) + zeroPad(month()) + zeroPad(day()) + "T" + zeroPad(hour()) + zeroPad(minute()) + zeroPad(second());
      String pubtopic(MqttTopic);
      if (pubtopic.lastIndexOf('/') > 0)
        pubtopic = pubtopic.substring(0, pubtopic.lastIndexOf('/'));
      pubtopic += "/";
      pubtopic += WiFi.getHostname();
      pubtopic += "/status";
      // {"device":"ESP123456","ip":"
      snprintf(msg, sizeof(msg), "{\"device\":\"%s\",\"ip\":\"%s\",\"time\":\"%s\",\"temp\":%.1f,\"hum\":%d,\"cond\":\"%s\"}",
        WiFi.getHostname(),
        WiFi.localIP().toString().c_str(),
        datetime.c_str(),
        weatherClient.getTemperature(),
        weatherClient.getHumidity(),
        weatherClient.getWeatherDescription().c_str() );

      if (mqttClient.publish(pubtopic.c_str(), msg)) {
        Serial.printf_P(PSTR("MQTT publish to %s: %s\n"), pubtopic.c_str(), msg);
        isMqttStatusPublishDone = true;
      } else {
        Serial.printf_P(PSTR("MQTT publish to %s FAILED\n"), pubtopic.c_str());
      }
    }
  }
  #endif

  checkDisplay(); // check if we need to turn display on or off for night mode.

  displayTime = hourMinutes(false);
  isDisplayTimeNew = true;
  if (displayWidth >= 8) {
    // wide clock style config, different screen formats for 8+ tiles: HH:MM, HH:MM:SS, HH:MM *CF, HH:MM %RH, mm dd HH:MM,  HH:MM Www DD (12 chars! 8 tiles fit 10 chars! ,
    // 1=HH:MM, 2=HH:MM:SS, 3=HH:MM *CF, 4=HH:MM %RH, 5=mm dd HH:MM, 6=HH:MM mmdd, 7=HH:MM ddmm, 8=HH:MMWwwDD (or HH:MM Www DD on >= 10 tile display)
    String add;

    switch (wideClockStyle) {
    default:
        /* fall through */
    case WIDE_CLOCK_STYLE_HHMM:
        // No change this is normal clock display
        break;
    case WIDE_CLOCK_STYLE_HHMMSS:
        displayTime += secondsIndicator(false) + zeroPad(second());
        break;
    case WIDE_CLOCK_STYLE_HHMM_CF:
        // On Wide Display -- show the current temperature as well
        add = String(weatherClient.getTemperature(),0);
        displayTime += " " + add + getTempSymbol();
        break;
    case WIDE_CLOCK_STYLE_HHMM_RH:
        displayTime += " " + String(weatherClient.getHumidity()) + "%";
        break;
    case WIDE_CLOCK_STYLE_MMDD_HHMM:
        add = zeroPad(month())+zeroPad(day());
        displayTime = add + " " + displayTime;
        break;
    case WIDE_CLOCK_STYLE_HHMM_MMDD:
        add = zeroPad(month())+zeroPad(day());
        displayTime += " " + add;
        break;
    case WIDE_CLOCK_STYLE_HHMM_DDMM:
        add = zeroPad(day())+zeroPad(month());
        displayTime += " " + add;
        break;
    case WIDE_CLOCK_STYLE_HHMM_WWWDD:
        String add = getDayName(weekday());
        if (displayWidth >= 10) {
          add.remove(3);
          add = " " + add + " ";
        } else {
          add.remove(3);
        }
        add += spacePad(day());
        displayTime += add;
        break;
    }
  }
}


String hourMinutes(bool isRefresh) {
  if (is24hour) {
    return spacePad(hour()) + secondsIndicator(isRefresh) + zeroPad(minute());
  } else {
    return spacePad(hourFormat12()) + secondsIndicator(isRefresh) + zeroPad(minute());
  }
}

char secondsIndicator(bool isRefresh) {
  char rtnValue = ':';
  if (!isRefresh && !isQuietPeriodNoBlinkNoscroll && flashOnSeconds && ((second() % 2) == 0)) {
    rtnValue = ' ';
  }
  return rtnValue;
}

bool authentication() {
  if (isBasicAuth) {
    return server.authenticate(www_username, www_password);
  }
  return true; // Authentication not required
}

void handlePull() {
  getWeatherData(); // this will force a data pull for new weather
  webDisplayWeatherData();
}

#if COMPILE_MQTT
void handleSaveMqtt() {
  // test that some important args are present to accept new config
  if (server.hasArg(F("mqttAddress")) &&
      server.hasArg(F("mqttPort")) &&
      server.hasArg(F("mqttTopic"))
    ) {

    if (!authentication()) {
      return server.requestAuthentication();
    }
    isMqttEnabled = server.hasArg(F("displaymqtt"));
    MqttServer = server.arg(F("mqttAddress"));
    MqttPort = server.arg(F("mqttPort")).toInt();
    MqttTopic = server.arg(F("mqttTopic"));
    MqttAuthUser = server.arg(F("mqttUser"));
    MqttAuthPass = server.arg(F("mqttPass"));

    writeConfiguration();
  }
  redirectHome();
}
#endif

void handleSaveConfig() {
  bool configChangedMustRestart = false;
  // test that some important args are present to accept new config
  if (server.hasArg(F("openWeatherMapApiKey")) &&
      server.hasArg(F("gloc")) &&
      server.hasArg(F("marqueeMsg")) &&
      server.hasArg(F("displaywidth")) &&
      //server.hasArg(F("startTime")) &&
      server.hasArg(F("userid")) &&
      server.hasArg(F("stationpassword")) &&
      server.hasArg(F("theme")) &&
      server.hasArg(F("scrollspeed"))
      ) {

    if (!authentication()) {
      return server.requestAuthentication();
    }
    owmApiKey = server.arg(F("openWeatherMapApiKey"));
    geoLocation = server.arg(F("gloc"));
    flashOnSeconds = server.hasArg(F("flashseconds")); // flashOnSeconds means blinking ':' on clock
    is24hour = server.hasArg(F("is24hour"));
    isPmIndicator = server.hasArg(F("isPM"));
    showTemperature = server.hasArg(F("showtemp"));
    showDate = server.hasArg(F("showdate"));
    showCity = server.hasArg(F("showcity"));
    showCondition = server.hasArg(F("showcondition"));
    showHumidity = server.hasArg(F("showhumidity"));
    showWind = server.hasArg(F("showwind"));
    showPressure = server.hasArg(F("showpressure"));
    showHighLow = server.hasArg(F("showhighlow"));
    isStaticDisplay = server.hasArg(F("statdisp"));
    isSysLed = server.hasArg(F("sysled"));
    isMetric = server.hasArg(F("metric"));
    marqueeMessage = server.arg(F("marqueeMsg"));
    quietTimeMode = server.arg(F("qtmode")).toInt();
    quietTimeDimlevel = server.arg(F("qtlvl")).toInt();
    String s = server.arg(F("qtstrt"));
    quietTimeStart = (s.length() > 0) ? TIME_HHMM(s.toInt(),s.substring(s.indexOf(':')+1).toInt()) : -1;
    s = server.arg(F("qtend"));
    quietTimeEnd   = (s.length() > 0) ? TIME_HHMM(s.toInt(),s.substring(s.indexOf(':')+1).toInt()) : -1;
    displayIntensity = server.arg(F("ledintensity")).toInt();
    int n = server.arg(F("displaywidth")).toInt();
    if ((displayWidth != n) && (n >= 4) && (n <= 32)) {
      displayWidth = n;
      configChangedMustRestart = true;
    }
    refreshDataInterval = server.arg(F("refresh")).toInt();
    themeColor = server.arg(F("theme"));
    displayScrollingInterval = server.arg(F("refreshDisplay")).toInt();
    displayScrollSpeed = server.arg(F("scrollspeed")).toInt();
    wideClockStyle = server.arg(F("wideclockformat")).toInt();

    isBasicAuth = server.hasArg(F("isBasicAuth"));
    String temp = server.arg(F("userid"));
    temp.trim();
    temp.toCharArray(www_username, sizeof(www_username));
    temp = server.arg(F("stationpassword"));
    temp.trim();
    temp.toCharArray(www_password, sizeof(www_password));
    weatherClient.setMetric(isMetric);
    weatherClient.setGeoLocation(geoLocation);
    matrix.fillScreen(CLEARSCREEN);
    writeConfiguration();
    Serial.println(F("handleSaveConfig: saved"));
    getWeatherData(); // this will force a data pull for new weather
  }
  redirectHome();
  if (configChangedMustRestart) restartEsp();
}

void handleSystemReset() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  Serial.println(F("Reset System Configuration"));
  if (FS.remove(CONFIG)) {
    redirectHome();
    ESP.restart();
  }
}

void restartEsp() {
  redirectHome();
  ESP.restart();
}

void handleForgetWifi() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  //WiFiManager
  //Local initialization. Once its business is done, there is no need to keep it around
  redirectHome();
  //WiFiManager wifiManager;
  //wifiManager.resetSettings();
  //ESP.restart();
  ESP_WiFiManager->resetAndEnterConfigPortal();
}

#if COMPILE_MQTT
void handleMqttConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  onBoardLed(LED_ON);

  sendHeader();

  String form = FPSTR(webMQTTform);
  form.replace(F("%MQTT_CB%"), (isMqttEnabled) ? "checked" : "");
  form.replace(F("%MQTT_ADR%"), MqttServer);
  form.replace(F("%MQTT_PRT%"), String(MqttPort));
  form.replace(F("%MQTT_TOP%"), MqttTopic);
  form.replace(F("%MQTT_USR%"), MqttAuthUser);
  form.replace(F("%MQTT_PW%"), MqttAuthPass);

  server.sendContent(form);
  form.clear();

  sendFooter();

  server.sendContent("");
  server.client().stop();
  onBoardLed(LED_OFF);
}
#endif

void handleConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  onBoardLed(LED_ON);

  sendHeader();

  String form = FPSTR(webChangeForm1);
  form.replace(F("%OWMKEY%"), owmApiKey);
  form.replace(F("%CTYNM%"), (weatherClient.getCity() != "") ?
      weatherClient.getCity() + ", " + weatherClient.getCountry() + " @ " + String(weatherClient.getLat(),6) + "," + String(weatherClient.getLon(),6)  : "");
  form.replace(F("%GLOC%"), geoLocation);
  form.replace(F("%MSG%"), EncodeHtmlSpecialChars(marqueeMessage.c_str()));
  form.replace(F("%TEMP_CB%"), (showTemperature) ? "checked" : "");
  form.replace(F("%DATE_CB%"), (showDate) ? "checked" : "");
  form.replace(F("%CITY_CB%"), (showCity) ? "checked" : "");
  form.replace(F("%COND_CB%"), (showCondition) ? "checked" : "");
  form.replace(F("%HUM_CB%"), (showHumidity) ? "checked" : "");
  form.replace(F("%WIND_CB%"), (showWind) ? "checked" : "");
  form.replace(F("%PRES_CB%"), (showPressure) ? "checked" : "");
  form.replace(F("%HILO_CB%"), (showHighLow) ? "checked" : "");
  server.sendContent(form);

  form = FPSTR(webChangeForm2);
  form.replace(F("%24HR_CB%"), (is24hour) ? "checked" : "");
  form.replace(F("%METRIC_CB%"), (isMetric) ? "checked" : "");
  form.replace(F("%PM_CB%"), (isPmIndicator) ? "checked" : "");
  form.replace(F("%FLASH_CB%"), (flashOnSeconds) ? "checked" : "");
  form.replace(F("%STATDISP_CB%"), (isStaticDisplay) ? "checked" : "");
  form.replace(F("%END_TM%"), String(quietTimeStart));
  String qtmode = String(quietTimeMode);
  String qtmOptions = F("<option value='0'>Disabled</option><option value='1'>Display Off</option><option value='2'>Dimmed</option><option value='3'>Dimmed and No Motion</option>");
  qtmOptions.replace(qtmode + "'", qtmode + "' selected");
  form.replace(F("%QTM_OPT%"), String(qtmOptions));
  form.replace(F("%QTDIM%"), String(quietTimeDimlevel));
  form.replace(F("%QTST%"), (quietTimeStart<0)? "--:--" : String(zeroPad(quietTimeStart/3600)+':'+zeroPad((quietTimeStart/60)%60)));
  form.replace(F("%QTEND%"), (quietTimeEnd<0)? "--:--" : String(zeroPad(quietTimeEnd/3600)+':'+zeroPad((quietTimeEnd/60)%60)));
  form.replace(F("%INTY_OPT%"), String(displayIntensity));
  form.replace(F("%DTW_OPT%"), String(displayWidth));
  String dSpeed = String(displayScrollSpeed);
  String scrollOptions = F("<option value='35'>Slow</option><option value='25'>Normal</option><option value='15'>Fast</option><option value='10'>Very Fast</option>");
  scrollOptions.replace(dSpeed + "'", dSpeed + "' selected");
  form.replace(F("%SCRL_OPT%"), scrollOptions);
  String minutes = String(refreshDataInterval);
  String options = F("<option>5</option><option>10</option><option>15</option><option>20</option><option>30</option><option>60</option>");
  options.replace(">" + minutes + "<", " selected>" + minutes + "<");
  form.replace(F("%RFSH_OPT%"), options);
  form.replace(F("%RFSH_DISP%"), String(displayScrollingInterval));
  form.replace(F("%SYSLED_CB%"), (isSysLed) ? "checked" : "");
  // Wide display options: 1=HH:MM, 2=HH:MM:SS, 3=HH:MM *CF, 4=HH:MM %RH, 5=mm dd HH:MM, 6=HH:MM mmdd, 7=HH:MM ddmm, 8=HH:MM WwwDD,
  String clockOptions = F("<option value=1>HH:MM</option><option value=2>HH:MM:SS</option><option value=3>HH:MM *CF</option><option value=4>HH:MM %RH</option><option value=5>mmdd HH:MM</option><option value=6>HH:MM mmdd</option><option value=7>HH:MM ddmm</option><option value=8>HH:MMWwwDD</option>");
  clockOptions.replace(String(wideClockStyle) + ">", String(wideClockStyle) + F(" selected>"));
  form.replace(F("%WCLK_OPT%"), clockOptions);
  if (displayWidth < 8) {
    form.replace(F("$WCLKDIS$"),F("disabled"));
  } else {
    form.replace(F("$WCLKDIS$"),"");
  }

  server.sendContent(form); //Send another chunk of the form

  form = FPSTR(webChangeForm3);
  String themeOptions = FPSTR(webColorThemes);
  themeOptions.replace(">" + String(themeColor) + "<", " selected>" + String(themeColor) + "<");
  form.replace(F("%THEME_OPT%"), themeOptions);
  form.replace(F("%AUTH_CB%"), (isBasicAuth) ? "checked" : "");
  form.replace(F("%CFGUID%"), String(www_username));
  form.replace(F("%CFGPW%"), String(www_password));
  server.sendContent(form); // Send the second chunk of Data

  sendFooter();

  server.sendContent("");
  server.client().stop();
  onBoardLed(LED_OFF);
}

void handleDisplay() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  enableDisplay(!displayOn);
  webDisplayMessage(F("Display is now ") + String((displayOn) ? "ON" : "OFF"));
  delay(1000);
  redirectHome();
}

//***********************************************************************
void getWeatherData() //client function to send/receive GET request data.
{
  onBoardLed(LED_ON);
  matrix.fillScreen(CLEARSCREEN);
  Serial.println();

  if (displayOn) {
    // only pull the weather data if display is on
    if (firstTimeSync != 0) {
      centerPrint(displayTime, true);
    } else {
      centerPrint("...");
    }
    matrix.drawPixel(0, 7, HIGH);
    matrix.drawPixel(0, 6, HIGH);
    matrix.drawPixel(0, 5, HIGH);
    matrix.write();

    weatherClient.updateWeather();
    if (weatherClient.getErrorMessage() != "") {
      displayScrollErrorMessage(weatherClient.getErrorMessage(), true);
    } else {
      // Set current timezone (adapts to DST when region supports that)
      // when time was potentially changed, stop quick auto sync
      if (set_timeZoneSec(weatherClient.getTimeZoneSeconds())) {
        // Stop automatic NTP sync and do it explicitly below
        setSyncProvider(NULL);
      }
    }
  }

// With time sync provider (timeNTP) set, the time sync may happen at inconvenient moments, like
// when scrolling the display causing that to stall.
// Solution: cancel setup sync provider, and call getNtpTime() here explicitly,
// then the time update is visualized on the LED display.
Serial.printf_P(PSTR("Timestatus=%d\n"), timeStatus());  // status timeNeedsSync(1) is NEVER set
  if (1) { //ALWAYS;   (timeStatus() != timeSet || updateTime) { // when timeNotSet OR timeNeedsSync
    Serial.println(F("Updating Time..."));
    //Update the Time
    matrix.drawPixel(0, 4, HIGH);
    matrix.drawPixel(0, 3, HIGH);
    matrix.drawPixel(0, 2, HIGH);
    matrix.write();

    // Explicitly get the NTP time
    time_t t = getNtpTime();
    if (t > TIME_VALID_MIN) {
      // warning: adding ctime() causes 5kB extra codesize!
      //Serial.printf_P(PSTR("setTime %u=%s"), uint32_t(t), ctime(&t));
      Serial.printf_P(PSTR("setTime %u\n"), uint32_t(t));
      setTime(t);
    }
    if (firstTimeSync == 0) {
      firstTimeSync = now();
      if (firstTimeSync > TIME_VALID_MIN) {
        setSyncInterval(222); // used for testing, value doesn't really matter
        Serial.printf_P(PSTR("firstTimeSync is: %d\n"), firstTimeSync);
      } else {
        // on a failed ntp sync we have seen that firstTimeSync was set to a low value: reset firstTimeSync
        firstTimeSync = 0;
      }
    }
  }

  Serial.println(F("Version: " VERSION));
  Serial.println();
  onBoardLed(LED_OFF);
}

void webDisplayMessage(const String &message) {
  onBoardLed(LED_ON);

  sendHeader();
  server.sendContent(message);
  sendFooter();
  server.sendContent("");
  server.client().stop();

  onBoardLed(LED_OFF);
}

void redirectHome() {
  // Send them back to the Root Directory
  server.sendHeader(F("Location"), "/", true);
  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.send(302, F("text/plain"), "");
  server.client().stop();
  delay(1000);
}

void sendHeader(bool isMainPage) {
  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");

  String html = FPSTR(webHeaderHtml);
  html.replace(F("$COLOR$"), themeColor);
  server.sendContent(html);
  if (isMainPage) {
    server.sendContent(FPSTR(webHeaderMainPage));
  }
  html = FPSTR(webBody1);
  html.replace(F("$ICO$"), weatherClient.getIcon());
  html.replace(F("$IDES$"), weatherClient.getWeatherDescription());
  server.sendContent(html);

  server.sendContent(FPSTR(webActions1));
  server.sendContent(FPSTR((displayOn)? webActions2_ON:webActions2_OFF));
  server.sendContent(FPSTR(webActions3));

  server.sendContent(FPSTR(webBody2));
  if (isMainPage) {
    server.sendContent(FPSTR(webBody2MainPage));
  }
}

void sendFooter() {
  int8_t rssi = getWifiQuality();
  Serial.printf_P(PSTR("Signal Strength (RSSI): %d%%, %d db\n"), rssi, WiFi.RSSI());
  String html = FPSTR(webFooterHtml);

  html.replace(F("$UPD$"), getTimeTillUpdate());
  html.replace(F("$RSSI$"), String(rssi));
  server.sendContent(html);
}

void webDisplayWeatherData() {
  onBoardLed(LED_ON);
  String html;

  sendHeader(true);

  String temperature = String(weatherClient.getTemperature(),1);

  String dtstr;
  if (is24hour) {
    // UK date+time presentation: MSB to LSB
    dtstr = getDayName(weekday()) + ", " + String(year()) + " " + getMonthName(month()) + " " + day() + " " + zeroPad(hour()) + ":" + zeroPad(minute());
  } else {
    // US date+time presentation
    dtstr = getDayName(weekday()) + ", " + getMonthName(month()) + " " + day() + ", " + hourFormat12() + ":" + zeroPad(minute()) + ", " + getAmPm(isPM());
  }

  Serial.print(F("Main page update "));
  Serial.println(dtstr);
  //Serial.println(temperature);
  //Serial.println(weatherClient.getCity());
  //Serial.println(weatherClient.getWeatherCondition());
  //Serial.println(weatherClient.getWeatherDescription());
  Serial.print(F("UpdateTime: "));
  Serial.println(get24HrColonMin(weatherClient.getReportTimestamp() + weatherClient.getTimeZoneSeconds()));
  //Serial.print(F("SunRiseTime: "));
  //Serial.println(get24HrColonMin(weatherClient.getSunRise() + weatherClient.getTimeZoneSeconds()));
  //Serial.print(F("SunSetTime: "));
  //Serial.println(get24HrColonMin(weatherClient.getSunSet() + weatherClient.getTimeZoneSeconds()));

  if (timeStatus() == timeNotSet) {
    html += F("<p>waiting for first time sync...</p>");
  }
  if (weatherClient.getCity().length() == 0) {
    html += F("<p>Waiting for first weather report... , or</p>"
      "<p>Please <a href='/configure'>Configure Weather</a> API</p>");
    if (weatherClient.getErrorMessage() != "") {
      html += F("<p>Weather Error: <strong>") + weatherClient.getErrorMessage() + F("</strong></p>");
    }
  } else {
    html.reserve(512);
    html += F(
      "<div class='w3-cell-row' style='width:100%'><h2>") + weatherClient.getCity() + ", " + weatherClient.getCountry() + F("</h2></div><div class='w3-cell-row'>"
      "<div class='w3-cell w3-left w3-medium' style='width:120px'>"
      "<img src='http://openweathermap.org/img/wn/") + weatherClient.getIcon() + "@2x.png' alt='" + weatherClient.getWeatherDescription() + "'><br>" +
      weatherClient.getHumidity() + F("% <span class='w3-tiny'>RH</span><br>") +
      weatherClient.getPressure() + F(" <span class='w3-tiny'>") + getPressureSymbol() + F("</span><br>"
      "Wind ") +
      weatherClient.getWindDirectionText() + " /<br>&nbsp;&nbsp;" + String(weatherClient.getWindSpeed(),1) + F("&nbsp;<span class='w3-tiny'>") + getSpeedSymbol() + F("</span><br>"
      "</div>"
      "<div class='w3-cell w3-container' style='width:100%'><p>");
    server.sendContent(html);
    String clouds;
    if ((weatherClient.getCloudCoverage() > 0) && (weatherClient.getWeatherDescription().indexOf("louds")>0)) {
       clouds = String(weatherClient.getCloudCoverage()) + " %";
    }
    html.clear();
    html.reserve(1024);
    html +=
      weatherClient.getWeatherCondition() + " (" + weatherClient.getWeatherDescription() + ") " + clouds + "<br>" +
      temperature + " " + getTempSymbol(true) + "<br>" +
      String(weatherClient.getTemperatureHigh(),1) + "/" + String(weatherClient.getTemperatureLow(),1) + " " + getTempSymbol(true) + "<br>"
      "SunRise " + get24HrColonMin(weatherClient.getSunRise() + weatherClient.getTimeZoneSeconds()) + "<br>"
      "SunSet " + get24HrColonMin(weatherClient.getSunSet() + weatherClient.getTimeZoneSeconds()) + "<br>"
      "Updated " + get24HrColonMin(weatherClient.getReportTimestamp() + weatherClient.getTimeZoneSeconds()) +
      F("<br>"
        "<a href='https://www.google.com/maps/@") + weatherClient.getLat() + "," + weatherClient.getLon() + F(",10000m/data=!3m1!1e3' target='_BLANK'><i class='fas fa-map-marker' style='color:red'></i> Map It!</a><br>"
      "</p></div></div>"
      "<div class='w3-cell-row' style='width:100%'><h3>") + dtstr  + F("</h3></div><hr>");
  }


  server.sendContent(html); // spit out what we got
  html.clear(); // fresh start

  #if COMPILE_MQTT
  if (isMqttEnabled) {
    if (mqttClient.getError().length() == 0) {
      html = F("<div class='w3-cell-row'><b>MQTT</b><br>"
             "Last Message: <b>") + EncodeHtmlSpecialChars(mqttClient.getLastMqttMessage()) + F("</b><br>"
             "</div><br><hr>");
    } else {
      html = F("<div class='w3-cell-row'><b>MQTT Error</b><br>"
             "Please <a href='/configuremqtt' title='Configure'>Configure</a> for MQTT <a href='/configuremqtt' title='Configure'><i class='fas fa-cog'></i></a><br>"
             "Status: Error Connecting<br>"
             "Reason: ") + mqttClient.getError() + F("<br></div><br><hr>");
    }
    server.sendContent(html);
    html.clear();
  }
  #endif

  sendFooter();
  server.sendContent("");
  server.client().stop();
  onBoardLed(LED_OFF);
}


void onBoardLed(bool on) {
  if (isSysLed) {
    digitalWrite(LED_ONBOARD, on);
  }
}

void flashLED(int number, int delayTime) {
  for (int inx = 0; inx < number; inx++) {
#ifdef BUZZER_PIN
    tone(BUZZER_PIN, 440, delayTime);
#endif
    delay(delayTime);
    onBoardLed(LED_ON);
    delay(delayTime);
    onBoardLed(LED_OFF);
    delay(delayTime);
  }
#ifdef BUZZER_PIN
  noTone(BUZZER_PIN);
#endif
}

String getTempSymbol(bool forWeb) {
  // Note: The forWeb degrees character is an UTF8 double byte character!
  return String((forWeb) ? "°" : String(char(248))) + String((isMetric) ? 'C' : 'F');
}

String getSpeedSymbol() {
  return String((isMetric) ? "kmh" : "mph");
}

String getPressureSymbol()
{
  return String((isMetric) ? "mb" : "inHg");
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  // found out that RSSI returns 31 when wifi connection is lost
  // we return -1 in that case
  if (dbm > 0)
  {
    return -1; // wifi error
  }
  else if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}

String getTimeTillUpdate() {
  char hms[10];
  long timeToUpdate = (((refreshDataInterval * 60) + lastRefreshDataTimestamp) - now());

  int hours = numberOfHours(timeToUpdate);
  int minutes = numberOfMinutes(timeToUpdate);
  int seconds = numberOfSeconds(timeToUpdate);
  sprintf_P(hms, PSTR("%d:%02d:%02d"), hours, minutes, seconds);

  return String(hms);
}

int getMinutesFromLastRefresh() {
  int minutes = (now() - lastRefreshDataTimestamp) / 60;
  if (minutes < 0) { // system time must have changed
    minutes = 0;
    lastRefreshDataTimestamp = now();
  }
  return minutes;
}

int getMinutesFromLastDisplay() {
  int minutes = (now() - displayOffTimestamp) / 60;
   if (minutes < 0) { // system time must have changed
    minutes = 0;
    lastRefreshDataTimestamp = now();
  }
 return minutes;
}

int getMinutesFromLastDisplayScroll() {
  int minutes = (now() - lastDisplayScrollTimestamp) / 60;
  if (minutes < 0) { // system time must have changed
    minutes = 0;
    lastDisplayScrollTimestamp = now();
  }
  return minutes;
}

void enableDisplay(bool enable) {
  displayOn = enable;
  if (enable) {
    if (getMinutesFromLastDisplay() >= refreshDataInterval) {
      // The display has been off longer than the minutes between refresh -- need to get fresh data
      lastRefreshDataTimestamp = 0; // this should force a data pull of the weather
      displayOffTimestamp = 0;  // reset
    }
    matrix.shutdown(false);
    matrix.fillScreen(CLEARSCREEN);
    Serial.println(F("Display was turned ON: ") + get24HrColonMin(now()));
  } else {
    matrix.shutdown(true);
    Serial.println(F("Display was turned OFF: ") + get24HrColonMin(now()));
    displayOffTimestamp = lastRefreshDataTimestamp;
  }
}

// Toggle on and off the display if user defined times
void checkDisplay()
{
  if ((quietTimeMode == QTM_DISABLED) || (quietTimeStart <= 0) || (quietTimeEnd <= 0)) {
    return; // nothing to do
  }
  int currentTime = TIME_HHMM(hour(), minute());
  if (quietTimeMode == QTM_DISPLAYOFF) {
    if (currentTime == quietTimeEnd && !displayOn) {
      Serial.print(F("Time to turn display on: ")); Serial.println(currentTime);
      flashLED(1, 500);
      enableDisplay(true);
    }

    if (currentTime == quietTimeStart && displayOn) {
      Serial.print(F("Time to turn display off: ")); Serial.println(currentTime);
      flashLED(2, 500);
      enableDisplay(false);
    }
  }
  else { //if (quietTimeMode == QTM_DIMMED || quietTimeMode == QTM_DIMMED_NOSCROLL)
    if (((quietTimeStart > quietTimeEnd) && ((currentTime>=quietTimeStart)||(currentTime<=quietTimeEnd))) ||
        ((quietTimeEnd > quietTimeStart) && ((currentTime>=quietTimeStart)&&(currentTime<=quietTimeEnd))) )
    {
      // We are in the quiet period
      matrix.setIntensity(quietTimeDimlevel);
      isQuietPeriod = true;
      if (quietTimeMode == QTM_DIMMED_NOSCROLL) {
        isQuietPeriodNoBlinkNoscroll = true;
      }
    } else {
      // We are outside the quiet period
      matrix.setIntensity(displayIntensity);
      isQuietPeriod = false;
      isQuietPeriodNoBlinkNoscroll = false;
    }
  }

}

void writeConfiguration() {
  // Save decoded message to FS file for playback on power up.
  File f = FS.open(CONFIG, "w");
  if (!f) {
    Serial.println(F("File open failed!"));
  } else {
    Serial.println(F("Saving settings now..."));
    f.println(F("APIKEY=") + owmApiKey);
    f.println(F("CityID=") + geoLocation); // using CityID for backwards compatibility
    f.println(F("marqueeMessage=") + marqueeMessage);
    f.println(F("quietTimeStart=") + String(quietTimeStart));
    f.println(F("quietTimeEnd=") + String(quietTimeEnd));
    f.println(F("quietTimeMode=") + String(quietTimeMode));
    f.println(F("quietTimeDimlevel=") + String(quietTimeDimlevel));
    f.println(F("ledIntensity=") + String(displayIntensity));
    f.println(F("scrollSpeed=") + String(displayScrollSpeed));
    f.println(F("isFlash=") + String(flashOnSeconds));
    f.println(F("is24hour=") + String(is24hour));
    f.println(F("isPM=") + String(isPmIndicator));
    f.println(F("isMetric=") + String(isMetric));
    f.println(F("isStatDisp=") + String(isStaticDisplay));
    f.println(F("isSysLed=") + String(isSysLed));
    f.println(F("refreshRate=") + String(refreshDataInterval));
    f.println(F("dispInterval=") + String(displayScrollingInterval));
    f.println(F("displayWidth=") + String(displayWidth));
    f.println(F("wideClockStyle=") + String(wideClockStyle));
    f.println(F("www_username=") + String(www_username));
    f.println(F("www_password=") + String(www_password));
    f.println(F("IS_BASIC_AUTH=") + String(isBasicAuth));
    f.println(F("SHOW_CITY=") + String(showCity));
    f.println(F("SHOW_CONDITION=") + String(showCondition));
    f.println(F("SHOW_HUMIDITY=") + String(showHumidity));
    f.println(F("SHOW_WIND=") + String(showWind));
    f.println(F("SHOW_PRESSURE=") + String(showPressure));
    f.println(F("SHOW_HIGHLOW=") + String(showHighLow));
    f.println(F("SHOW_DATE=") + String(showDate));
    f.println(F("SHOW_TEMP=") + String(showTemperature));
    #if COMPILE_MQTT
    f.println(F("USE_MQTT=") + String(isMqttEnabled));
    f.println(F("MqttServer=") + MqttServer);
    f.println(F("MqttPort=") + String(MqttPort));
    f.println(F("MqttTopic=") + MqttTopic);
    f.println(F("MqttUser=") + MqttAuthUser);
    f.println(F("MqttPass=") + MqttAuthPass);
    #endif
    f.println(F("themeColor=") + themeColor);
  }
  f.close();

  readConfiguration();
  weatherClient.setGeoLocation(geoLocation);
}

void readConfiguration() {
  if (FS.exists(CONFIG) == false) {
    Serial.println(F("Settings File does not yet exists."));
    writeConfiguration();
    return;
  }
  Serial.println(F("ReadConfigFile:"));
  File fr = FS.open(CONFIG, "r");
  String line;
  int idx;
  while (fr.available()) {
    line = fr.readStringUntil('\n');
    line.trim();
    //print each line read
    if (1) {
      idx = line.indexOf("Key");
      if (idx < 0) idx = line.indexOf("KEY");
      if (idx < 0) idx = line.indexOf("Pass");
      if (idx > 0) idx = line.indexOf("=");
      if ((idx > 0) && (line.substring(idx+1).length() > 0)) {
        // do not print keys or passwords
        Serial.print(line.substring(0,idx+1));
        Serial.println(F("***"));
      } else {
        Serial.println(line);
      }
    }
    if ((idx = line.indexOf(F("APIKEY="))) >= 0) {
      owmApiKey = line.substring(idx + 7);
    }
    if ((idx = line.indexOf(F("CityID="))) >= 0) {
       // using CityID for backwards compatibility
      geoLocation = line.substring(idx + 7);
    }
    if ((idx = line.indexOf(F("isFlash="))) >= 0) {
      flashOnSeconds = line.substring(idx + 8).toInt();
    }
    if ((idx = line.indexOf(F("is24hour="))) >= 0) {
      is24hour = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("isPM="))) >= 0) {
      isPmIndicator = line.substring(idx + 5).toInt();
    }
    if ((idx = line.indexOf(F("wideclockformat="))) >= 0) {
      /* for backwards compatibility settings migration */
      //OLD: "1"="hh:mm Temp", "2"="hh:mm:ss", "3"="hh:mm"
      int n = line.substring(idx + 16).toInt();
      if (n == 1) wideClockStyle = WIDE_CLOCK_STYLE_HHMM_CF;
      if (n == 2) wideClockStyle = WIDE_CLOCK_STYLE_HHMMSS;
      if (n == 3) wideClockStyle = WIDE_CLOCK_STYLE_HHMM;
      // else: keep default
    }
    if ((idx = line.indexOf(F("wideClockStyle="))) >= 0) {
      int n = line.substring(idx + 15).toInt();
      if (n < WIDE_CLOCK_STYLE_FIRST || n > WIDE_CLOCK_STYLE_LAST) n = 1;
      if (wideClockStyle != n) {
        Serial.printf_P(PSTR("wideClockStyle changed %d->%d\n"), wideClockStyle, n);
        wideClockStyle = n;
      }
    }
    if ((idx = line.indexOf(F("isMetric="))) >= 0) {
      isMetric = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("isStatDisp="))) >= 0) {
      isStaticDisplay = line.substring(idx + 11).toInt();
    }
    if ((idx = line.indexOf(F("isSysLed="))) >= 0) {
      isSysLed = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("refreshRate="))) >= 0) {
      refreshDataInterval = line.substring(idx + 12).toInt();
      if (refreshDataInterval == 0) {
        refreshDataInterval = 15; // can't be zero
      }
    }
    if ((idx = line.indexOf(F("minutesBetweenScrolling="))) >= 0) {  /* for backwards compatibility settings migration */
      displayScrollingInterval = line.substring(idx + 24).toInt();
    }
    if ((idx = line.indexOf(F("dispInterval="))) >= 0) {
      displayScrollingInterval = line.substring(idx + 13).toInt();
      if (now() > TIME_VALID_MIN)
        lastDisplayScrollTimestamp = now() - displayScrollingInterval*60 + 5;
    }
    if ((idx = line.indexOf(F("displayWidth="))) >= 0) {
      int n = line.substring(idx + 13).toInt();
      if ((n < 4) && (n > 32)) n = 4;
      if (displayWidth != n) {
        Serial.printf_P(PSTR("displayWidth changed %d->%d\n"), displayWidth, n);
        displayWidth = n;
      }
    }
    if ((idx = line.indexOf(F("marqueeMessage="))) >= 0) {
      marqueeMessage = line.substring(idx + 15);
    }
    if ((idx = line.indexOf(F("quietTimeMode="))) >= 0) {
      quietTimeMode = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("quietTimeDimlevel="))) >= 0) {
      quietTimeDimlevel = line.substring(idx + 18).toInt();
    }
    if ((idx = line.indexOf(F("quietTimeStart="))) >= 0) {
      quietTimeStart = line.substring(idx + 15).toInt();
    }
    if ((idx = line.indexOf(F("quietTimeEnd="))) >= 0) {
      quietTimeEnd = line.substring(idx + 13).toInt();
    }
    if ((idx = line.indexOf(F("ledIntensity="))) >= 0) {
      displayIntensity = line.substring(idx + 13).toInt();
    }
    if ((idx = line.indexOf(F("scrollSpeed="))) >= 0) {
      displayScrollSpeed = line.substring(idx + 12).toInt();
    }
    if ((idx = line.indexOf(F("www_username="))) >= 0) {
      String temp = line.substring(idx + 13);
      temp.toCharArray(www_username, sizeof(www_username));
    }
    if ((idx = line.indexOf(F("www_password="))) >= 0) {
      String temp = line.substring(idx + 13);
      temp.toCharArray(www_password, sizeof(www_password));
    }
    if ((idx = line.indexOf(F("IS_BASIC_AUTH="))) >= 0) {
      isBasicAuth = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_CITY="))) >= 0) {
      showCity = line.substring(idx + 10).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_CONDITION="))) >= 0) {
      showCondition = line.substring(idx + 15).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_HUMIDITY="))) >= 0) {
      showHumidity = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_WIND="))) >= 0) {
      showWind = line.substring(idx + 10).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_PRESSURE="))) >= 0) {
      showPressure = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_HIGHLOW="))) >= 0) {
      showHighLow = line.substring(idx + 13).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_DATE="))) >= 0) {
      showDate = line.substring(idx + 10).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_TEMP="))) >= 0) {
      showTemperature = line.substring(idx + 10).toInt();
    }
    #if COMPILE_MQTT
    if ((idx = line.indexOf(F("USE_MQTT="))) >= 0) {
      isMqttEnabled = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("MqttServer="))) >= 0) {
      MqttServer = line.substring(idx + 11);
    }
    if ((idx = line.indexOf(F("MqttPort="))) >= 0) {
      MqttPort = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("MqttTopic="))) >= 0) {
      MqttTopic = line.substring(idx + 10);
    }
    if ((idx = line.indexOf(F("MqttUser="))) >= 0) {
      MqttAuthUser = line.substring(idx + 9);
    }
    if ((idx = line.indexOf(F("MqttPass="))) >= 0) {
      MqttAuthPass = line.substring(idx + 9);
    }
    #endif
    if ((idx = line.indexOf(F("themeColor="))) >= 0) {
      themeColor = line.substring(idx + 11);
    }
  }
  fr.close();
  Serial.println(F("ReadConfigFile EOF"));
  matrix.setIntensity(displayIntensity);
  weatherClient.setWeatherApiKey(owmApiKey);
  weatherClient.setMetric(isMetric);
  weatherClient.setGeoLocation(geoLocation);
  #if COMPILE_MQTT
  mqttClient.updateMqttClient(MqttServer, MqttPort, MqttTopic, MqttAuthUser, MqttAuthPass);
  #endif
}

void scrollMessageSetup(const String &msg) {
  scrlMsg = msg + " "; // add one more space at the end
  scrlMsgLen = (int)msg.length();
  scrlPixTotal = (font_width * (int)msg.length() + (matrix.width() - 1) - font_space);
  scrlPixY = (matrix.height() - 8) / 2; // center the text vertically
  scrlPixIdx = 0;
  scrlBusy = true;
  SCHEDULE_INTERVAL_START(scrlPixelLastTime,100);
}

// Call this every <displayScrollSpeed> ms
void scrollMessageNext() {
  if (scrlBusy) {
    int msgIdx = scrlPixIdx / font_width;
    int x = (matrix.width() - 1) - scrlPixIdx % font_width;
    matrix.fillScreen(CLEARSCREEN);
    while (((x + font_width - font_space) >= 0) && msgIdx >= 0) {
      if (msgIdx < scrlMsgLen) {
        matrix.drawChar(x, scrlPixY, scrlMsg[msgIdx], 1/*FGcolor*/, 0/*BGcolor*/, 1/*sizefactor*/);
      }
      msgIdx--;
      x -= font_width;
    }
    // TODO: perform matrix.write() on HW timer interrupt for more exact timings
    matrix.write(); // Send bitmap to display
    scrlPixIdx++;
  }
  scrlBusy = scrlPixIdx < scrlPixTotal;
}

void scrollMessageWait(const String &msg) {
  for (int i = 0; i < (font_width * (int)msg.length() + (matrix.width())); i++) {
    if (isWebserverEnabled) {
      server.handleClient();
    }
    if (isOTAenabled) {
      ArduinoOTA.handle();
    }
    matrix.fillScreen(CLEARSCREEN);

    int letter = i / font_width;
    int x = (matrix.width() - 1) - i % font_width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while (((x + font_width - font_space) >= 0) && letter >= 0) {
      if (letter < (int)msg.length()) {
        matrix.drawChar(x, y, msg[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= font_width;
    }

    matrix.write(); // Send bitmap to display
    delay(displayScrollSpeed);
  }
  matrix.setCursor(0, 0);
}

bool staticDisplaySetupSingle(char * message) {
  if (isStaticDisplay &&
      ((int)strlen(message) > 0) &&
      ((int)strlen(message) <= ((displayWidth * 8) / font_width)))
  {
    // msg fits on one screen : no scroll necessary
    matrix.fillScreen(CLEARSCREEN);
    centerPrint(String(message), false);
    SCHEDULE_INTERVAL_START(staticDisplayLastTime,0);
    isStaticDisplayBusy = true;
    isStaticDisplayNew = true;
    return true;
  } else {
    return false;
  }
}

void staticDisplaySetup(void) {
  if (isStaticDisplayNew && !isStaticDisplayBusy) {
    isStaticDisplayBusy = true;
    SCHEDULE_INTERVAL_START(staticDisplayLastTime,0);
    staticDisplayNext();
  }
}

void staticDisplayNext(void) {
  if (isStaticDisplayBusy) {
    int maxMsgLen = (displayWidth * 8) / font_width;
    // find (next) fitting message (too long messages will not be shown)
    while (staticDisplayIdxOut < staticDisplayIdx) {
      int len = staticDisplay[staticDisplayIdxOut].length();
      if (len > 0 && len <= maxMsgLen) break;
      staticDisplayIdxOut++;
    }

    if (staticDisplayIdxOut < staticDisplayIdx) {
      // msg fits on one screen : no scroll necessary
      matrix.fillScreen(CLEARSCREEN);
      centerPrint(staticDisplay[staticDisplayIdxOut], false);
      staticDisplayIdxOut++;
    } else {
      // at end of list: reset and continue
      staticDisplayIdx = 0;
      staticDisplayIdxOut = 0;
      isStaticDisplayNew = false;
      isStaticDisplayBusy = false;
    }
  }
}

void centerPrint(const String &msg, bool extraStuff) {
  int x = (matrix.width() - (msg.length() * font_width)) / 2;
  if (x < 0) {
    Serial.printf_P(PSTR("Error: centerPrint msg too large! len=%u:%s\n"), msg.length(), msg);
  }

  // Print the static portions of the display before the main Message
  if (extraStuff) {
    if (!is24hour && isPmIndicator && isPM()) {
      // Place PM indicator pixel at right edge, bottom line of digits
      matrix.drawPixel(matrix.width() - 1, 6, HIGH);
    }
  }

  matrix.setCursor(x, 0);
  matrix.print(msg);

  matrix.write();
}


String EncodeHtmlSpecialChars(const char *msg)
{
  String encoded;
  int inIdx;
  char ch;
  const int inLen = strlen(msg);
  //Serial.printf_P(PSTR("EncodeHTML in:  %s\n"), msg);
  encoded.reserve(inLen+128);
  for (inIdx=0; inIdx < inLen; inIdx++) {
    ch = msg[inIdx];
    if (ch < ' ') continue; // skip all non printable chars
    if ( ch == '\'' || ch == '"' || ch == '<' || ch == '>')
    {
      // convert character to "&#<decimal>;"
      encoded += '&';
      encoded += '#';
      encoded += (int)ch;
      encoded += ';';
    }
    else {
      encoded += ch;
    }
  }
  //Serial.printf_P(PSTR("EncodeHTML out: %s\n"), encoded.c_str());
  return encoded;
}


String EncodeUrlSpecialChars(const char *msg)
{
const static char special[] = {'\x20','\x22','\x23','\x24','\x25','\x26','\x2B','\x3B','\x3C','\x3D','\x3E','\x3F','\x40'};
  String encoded;
  int inIdx;
  char ch, hex;
  bool convert;
  const int inLen = strlen(msg);
  //Serial.printf_P(PSTR("EncodeURL in:  %s\n"), msg);
  encoded.reserve(inLen+128);

  for (inIdx=0; inIdx < inLen; inIdx++) {
    ch = msg[inIdx];
    convert = false;
    if (ch < ' ') {
      convert = true; // this includes 0x80-0xFF !
    }
    // find ch in table
    for (int i=0; i < (int)sizeof(special) && !convert; i++) {
      if (special[i] == ch) convert = true;
    }
    if (convert) {
      // convert character to "%HEX"
      encoded += '%';
      hex = (ch >> 4) & 0x0F;
      hex += '0';
      if (hex > '9') hex += 7;
      encoded += hex;
      hex = ch & 0x0F;
      hex += '0';
      if (hex > '9') hex += 7;
      encoded += hex;
    }
    else {
      encoded += ch;
    }
  }
  //Serial.printf_P(PSTR("EncodeURL out: %s\n"), encoded.c_str());
  return encoded;
}
