/** The MIT License (MIT)

  Copyright (c) 2018 David Payne

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/**********************************************
  Edit Settings.h for personalization
***********************************************/

#include "Settings.h"

#define VERSION "3.1.10"

#define HOSTNAME "CLOCK-"
#define CONFIG "/conf.txt"
// uncomment Define buzzer pin when installed
// #define BUZZER_PIN  D2


//declairing prototypes
void configModeCallback(WiFiManager *myWiFiManager);
int8_t getWifiQuality();

// LED Settings
const int offset = 1;
int refresh = 0;
int spacer = 1;  // dots between letters
int width = 5 + spacer; // The font width is 5 pixels + spacer
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
String Wide_Clock_Style = "1";  //1="hh:mm Temp", 2="hh:mm:ss", 3="hh:mm"

// Time
int lastMinute;
int displayRefreshCount = 1;
long lastEpoch = 0;
long firstEpoch = 0;
long displayOffEpoch = 0;
boolean displayOn = true;

// News Client
NewsApiClient newsClient(NEWS_API_KEY, NEWS_SOURCE);
int newsIndex = 0;

// Weather Client
OpenWeatherMapClient weatherClient(APIKEY, CityID, IS_METRIC);
// (some) Default Weather Settings
//FIXME TODO: do not use uppercase for variables
boolean SHOW_DATE = false;
boolean SHOW_CITY = true;
boolean SHOW_CONDITION = true;
boolean SHOW_HUMIDITY = true;
boolean SHOW_WIND = true;
boolean SHOW_WINDDIR = true;
boolean SHOW_PRESSURE = false;
boolean SHOW_HIGHLOW = true;

// OctoPrint Client
OctoPrintClient printerClient(OctoPrintApiKey, OctoPrintServer, OctoPrintPort, OctoAuthUser, OctoAuthPass);
int printerCount = 0;

// Pi-hole Client
PiHoleClient piholeClient;

ESP8266WebServer server(WEBSERVER_PORT);
ESP8266HTTPUpdateServer serverUpdater;

static const char HEADER1[] PROGMEM = "<!DOCTYPE HTML>"
"<html><head><title>Marquee Scroller</title><link rel='icon' href='data:;base64,='>"
"<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />"
"<meta name='viewport' content='width=device-width, initial-scale=1'>"
"<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>"
"<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-$COLOR$.css'>"
"<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.8.1/css/all.min.css'>"
"</head><body>"
"<nav class='w3-sidebar w3-bar-block w3-card' style='margin-top:88px' id='mySidebar'>"
"<div class='w3-container w3-theme-d2'>"
"<span onclick='closeSidebar()' class='w3-button w3-display-topright w3-large'><i class='fas fa-times'></i></span>"
"<div class='w3-left'><img src='http://openweathermap.org/img/w/$ICO$.png' alt='$IDES$'></div>"
"<div class='w3-padding'>Menu</div></div>";

static const char HEADER2[] PROGMEM = "</nav>"
  "<header class='w3-top w3-bar w3-theme'><button class='w3-bar-item w3-button w3-xxxlarge w3-hover-theme' onclick='openSidebar()'>"
  "<i class='fas fa-bars'></i></button><h2 class='w3-bar-item'>Weather Marquee</h2></header>"
  "<script>"
  "function openSidebar(){document.getElementById('mySidebar').style.display='block'}"
  "function closeSidebar(){document.getElementById('mySidebar').style.display='none'}"
  "closeSidebar();"
  //"function refreshPage(){if(document.getElementById('mySidebar').style.display==='none')window.location.reload();}"
  //"var intervaltimer = setInterval(refreshPage, 10000);"
  "</script>"
  "<br><div class='w3-container w3-large' style='margin-top:88px'>";

static const char FOOTER[] PROGMEM = "<br><br><br>"
  "</div>"
  "<footer class='w3-container w3-bottom w3-theme w3-margin-top'>"
  "<i class='far fa-paper-plane'></i> Version: " VERSION "<br>"
  "<i class='far fa-clock'></i> Next Update: $UPD$ <br>"
  "<i class='fas fa-rss'></i> Signal Strength: $RSSI$%"
  "</footer>"
  "</body></html>";

static const char WEB_ACTIONS1[] PROGMEM =
  "<a class='w3-bar-item w3-button' href='/'><i class='fas fa-home'></i> Home</a>"
  "<a class='w3-bar-item w3-button' href='/configure'><i class='fas fa-cog'></i> Configure</a>"
  "<a class='w3-bar-item w3-button' href='/configurenews'><i class='far fa-newspaper'></i> News</a>"
  "<a class='w3-bar-item w3-button' href='/configureoctoprint'><i class='fas fa-cube'></i> OctoPrint</a>";

static const char WEB_ACTIONS2[] PROGMEM =
  "<a class='w3-bar-item w3-button' href='/configurepihole'><i class='fas fa-network-wired'></i> Pi-hole</a>"
  "<a class='w3-bar-item w3-button' href='/pull'><i class='fas fa-cloud-download-alt'></i> Refresh Data</a>"
  "<a class='w3-bar-item w3-button' href='/display'>";

static const char WEB_ACTION3[] PROGMEM =
  "</a><a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default weather settings?\")'>"
  "<i class='fas fa-undo'></i> Reset Settings</a>"
  "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fas fa-wifi'></i> Forget WiFi</a>"
  "<a class='w3-bar-item w3-button' href='/update'><i class='fas fa-wrench'></i> Firmware Update</a>"
  "<a class='w3-bar-item w3-button' href='https://github.com/Qrome/marquee-scroller' target='_blank'><i class='fas fa-question-circle'></i> About</a>";

static const char CHANGE_FORM1[] PROGMEM =
  "<form class='w3-container' action='/saveconfig' method='get'><h2>Configure:</h2>"
  "<label>OpenWeatherMap API Key (get from <a href='https://openweathermap.org/' target='_BLANK'>here</a>)</label>"
  "<input class='w3-input w3-border w3-margin-bottom' type='text' name='openWeatherMapApiKey' value='%OWMKEY%' maxlength='70'>"
  "<p><label>%CTYNM% (<a href='http://openweathermap.org/find' target='_BLANK'><i class='fas fa-search'></i> Search for City ID</a>)</label>"
  "<input class='w3-input w3-border w3-margin-bottom' type='text' name='city' value='%CITY%' onkeypress='return isNumberKey(event)'></p>"
  "<p><input name='metric' class='w3-check w3-margin-top' type='checkbox' %METRIC_CB%> Use Metric (Celsius)</p>"
  "<p><input name='showdate' class='w3-check w3-margin-top' type='checkbox' %DATE_CB%> Display Date</p>"
  "<p><input name='showcity' class='w3-check w3-margin-top' type='checkbox' %CITY_CB%> Display City Name</p>"
  "<p><input name='showhighlow' class='w3-check w3-margin-top' type='checkbox' %HILO_CB%> Display Current High/Low Temperatures</p>"
  "<p><input name='showcondition' class='w3-check w3-margin-top' type='checkbox' %COND_CB%> Display Weather Condition</p>"
  "<p><input name='showhumidity' class='w3-check w3-margin-top' type='checkbox' %HUM_CB%> Display Humidity</p>"
  "<p><input name='showwind' class='w3-check w3-margin-top' type='checkbox' %WIND_CB%> Display Wind</p>"
  "<p><input name='showpressure' class='w3-check w3-margin-top' type='checkbox' %PRES_CB%> Display Barometric Pressure</p>"
  "<p><input name='is24hour' class='w3-check w3-margin-top' type='checkbox' %24HR_CB%> Use 24 Hour Clock (military time)</p>";

static const char CHANGE_FORM2[] PROGMEM =
  "<p><input name='isPM' class='w3-check w3-margin-top' type='checkbox' %PM_CB%> Show PM indicator (only 12h format)</p>"
  "<p><input name='flashseconds' class='w3-check w3-margin-top' type='checkbox' %FLASH_CB%> Flash \":\" in the time</p>"
  "<p><label>Marquee Message (up to 60 chars)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='marqueeMsg' value='%MSG%' maxlength='60'></p>"
  "<p><label>Start Time </label><input name='startTime' type='time' value='%STRT_TM%'></p>"
  "<p><label>End Time </label><input name='endTime' type='time' value='%END_TM%'></p>"
  "<p>Display Brightness <input class='w3-border w3-margin-bottom' name='ledintensity' type='number' min='0' max='15' value='%INTY_OPT%'></p>"
  "<p>Display Scroll Speed <select class='w3-option w3-padding' name='scrollspeed'>%SCRL_OPT%</select></p>"
  "<p>Minutes Between Refresh Data <select class='w3-option w3-padding' name='refresh'>%RFSH_OPT%</select></p>"
  "<p>Minutes Between Scrolling Data <input class='w3-border w3-margin-bottom' name='refreshDisplay' type='number' min='1' max='10' value='%RFSH_DISP%'></p>"
  "<p>Theme Color <select class='w3-option w3-padding' name='theme'>%THEME_OPT%</select></p>";

static const char CHANGE_FORM3[] PROGMEM =
  "<hr><p><input name='isBasicAuth' class='w3-check w3-margin-top' type='checkbox' %AUTH_CB%> Use Security Credentials for Configuration Changes</p>"
  "<p><label>Configure User ID (for this web interface)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='userid' value='%CFGUID%' maxlength='20'></p>"
  "<p><label>Configure Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='stationpassword' value='%CFGPW%'></p>"
  "<p><button class='w3-button w3-block w3-green w3-section w3-padding' type='submit'>Save</button></p></form>"
  "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

static const char WIDECLOCK_FORM[] PROGMEM =
  "<form class='w3-container' action='/savewideclock' method='get'><h2>Wide Clock Configuration:</h2>"
  "<p>Wide Clock Display Format <select class='w3-option w3-padding' name='wideclockformat'>%WCLK_OPT%</select></p>"
  "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button></form>";

static const char PIHOLE_FORM[] PROGMEM =
  "<form class='w3-container' action='/savepihole' method='get'><h2>Pi-hole Configuration:</h2>"
  "<p><input name='displaypihole' class='w3-check w3-margin-top' type='checkbox' %PIHO_CB%> Show Pi-hole Statistics</p>"
  "<label>Pi-hole Address (do not include http://)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='piholeAddress' id='piholeAddress' value='%PIHO_ADR%' maxlength='60'>"
  "<label>Pi-hole Port</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='piholePort' id='piholePort' value='%PIHO_PRT%' maxlength='5'  onkeypress='return isNumberKey(event)'>"
  "<label>Pi-hole API Token (from Pi-hole &rarr; Settings &rarr; API/Web interface)</label>"
  "<input class='w3-input w3-border w3-margin-bottom' type='text' name='piApiToken' id='piApiToken' value='%PIHO_API%' maxlength='65'>"
  "<input type='button' value='Test Connection and JSON Response' onclick='testPiHole()'><p id='PiHoleTest'></p>"
  "<button class='w3-button w3-block w3-green w3-section w3-padding' type='submit'>Save</button></form>"
  "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

static const char PIHOLE_TEST[] PROGMEM =
  "<script>function testPiHole(){var e=document.getElementById(\"PiHoleTest\"),t=document.getElementById(\"piholeAddress\").value,"
 "n=document.getElementById(\"piholePort\").value,api=document.getElementById(\"piApiToken\").value;;"
 "if(e.innerHTML=\"\",\"\"==t||\"\"==n)return e.innerHTML=\"* Address and Port are required\","
 "void(e.style.background=\"\");var r=\"http://\"+t+\":\"+n;r+=\"/admin/api.php?summary=3&auth=\"+api,window.open(r,\"_blank\").focus()}</script>";

static const char NEWS_FORM1[] PROGMEM =
  "<form class='w3-container' action='/savenews' method='get'><h2>News Configuration:</h2>"
  "<p><input name='displaynews' class='w3-check w3-margin-top' type='checkbox' %NEWS_CB%> Display News Headlines</p>"
  "<label>News API Key (get from <a href='https://newsapi.org/' target='_BLANK'>here</a>)</label>"
  "<input class='w3-input w3-border w3-margin-bottom' type='text' name='newsApiKey' value='%NEWSKEY%' maxlength='60'>"
  "<p>Select News Source <select class='w3-option w3-padding' name='newssource' id='newssource'></select></p>"
  "<script>var s='%NEWSSRC%';var tt;var xmlhttp=new XMLHttpRequest();xmlhttp.open('GET','https://raw.githubusercontent.com/Qrome/marquee-scroller/master/sources.json',!0);"
  "xmlhttp.onreadystatechange=function(){if(xmlhttp.readyState==4){if(xmlhttp.status==200){var obj=JSON.parse(xmlhttp.responseText);"
  "obj.sources.forEach(t)}}};xmlhttp.send();function t(it){if(it!=null){if(s==it.id){se=' selected'}else{se=''}tt+='<option'+se+'>'+it.id+'</option>';"
  "document.getElementById('newssource').innerHTML=tt}}</script>"
  "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button></form>";

static const char OCTO_FORM[] PROGMEM =
  "<form class='w3-container' action='/saveoctoprint' method='get'><h2>OctoPrint Configuration:</h2>"
  "<p><input name='displayoctoprint' class='w3-check w3-margin-top' type='checkbox' %OCTO_CB%> Show OctoPrint Status</p>"
  "<p><input name='octoprintprogress' class='w3-check w3-margin-top' type='checkbox' %OCTPG_CB%> Show OctoPrint progress with clock</p>"
  "<label>OctoPrint API Key (get from your server)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoPrintApiKey' value='%OCTOKEY%' maxlength='60'>"
  "<label>OctoPrint Address (do not include http://)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoPrintAddress' value='%OCTOADR%' maxlength='60'>"
  "<label>OctoPrint Port</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoPrintPort' value='%OCTOPOR%' maxlength='5'  onkeypress='return isNumberKey(event)'>"
  "<label>OctoPrint User (only needed if you have haproxy or basic auth turned on)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoUser' value='%OCTOUSR%' maxlength='30'>"
  "<label>OctoPrint Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='octoPass' value='%OCTOPW%'>"
  "<button class='w3-button w3-block w3-green w3-section w3-padding' type='submit'>Save</button></form>"
  "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

static const char COLOR_THEMES[] PROGMEM =
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

//Replace checkbox marker text
//inline void formReplaceCheckboxMarker(String form, String marker, bool checked) {
//  form.replace(marker, (checked) ? "checked='checked'" : "");
//}

const int TIMEOUT = 500; // 500 = 1/2 second
int timeoutCount = 0;

// Change the externalLight to the pin you wish to use if other than the Built-in LED
int externalLight = LED_BUILTIN; // LED_BUILTIN is is the built in LED on the Wemos

void setup() {
  Serial.begin(115200);
  FS.begin();
  //FS.remove(CONFIG);
  delay(10);

  // Initialize digital pin for LED
  pinMode(externalLight, OUTPUT);

  //New Line to clear from start garbage
  Serial.println();

  readConfiguration();

  Serial.println(F("Number of LED Displays: ") + String(numberOfHorizontalDisplays));
  // initialize dispaly
  matrix.setIntensity(0); // Use a value between 0 and 15 for brightness

  int maxPos = numberOfHorizontalDisplays * numberOfVerticalDisplays;
  for (int i = 0; i < maxPos; i++) {
    matrix.setRotation(i, ledRotation);
    matrix.setPosition(i, maxPos - i - 1, 0);
  }

  Serial.println(F("matrix created"));
  matrix.fillScreen(LOW); // show black
  centerPrint("hello");

#ifdef BUZZER_PIN
  tone(BUZZER_PIN, 415, 500);
  delay(500 * 1.3);
  tone(BUZZER_PIN, 466, 500);
  delay(500 * 1.3);
  tone(BUZZER_PIN, 370, 1000);
  delay(1000 * 1.3);
  noTone(BUZZER_PIN);
#endif

  for (int inx = 0; inx <= 15; inx++) {
    matrix.setIntensity(inx);
    delay(100);
  }
  for (int inx = 15; inx >= 0; inx--) {
    matrix.setIntensity(inx);
    delay(60);
  }
  delay(1000);
  matrix.setIntensity(displayIntensity);
#ifdef BUZZER_PIN
  //noTone(BUZZER_PIN);
#endif

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);

  //Custom Station (client) Static IP Configuration - Set custom IP for your Network (IP, Gateway, Subnet mask)
  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  if (!wifiManager.autoConnect((const char *)hostname.c_str())) {// new addition
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }

  // print the received signal strength:
  Serial.print(F("Signal Strength (RSSI): "));
  Serial.print(getWifiQuality());
  Serial.println("%");

  if (ENABLE_OTA) {
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.setHostname((const char *)hostname.c_str());
    if (OTA_Password != "") {
      ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
    }
    ArduinoOTA.begin();
  }

  if (WEBSERVER_ENABLED) {
    server.on("/", displayWeatherData);
    server.on("/pull", handlePull);
    server.on("/saveconfig", handleSaveConfig);
    server.on("/savewideclock", handleSaveWideClock);
    server.on("/savenews", handleSaveNews);
    server.on("/saveoctoprint", handleSaveOctoprint);
    server.on("/savepihole", handleSavePihole);
    server.on("/systemreset", handleSystemReset);
    server.on("/forgetwifi", handleForgetWifi);
    server.on("/configure", handleConfigure);
    server.on("/configurewideclock", handleWideClockConfigure);
    server.on("/configurenews", handleNewsConfigure);
    server.on("/configureoctoprint", handleOctoprintConfigure);
    server.on("/configurepihole", handlePiholeConfigure);
    server.on("/display", handleDisplay);
    server.onNotFound(redirectHome);
    serverUpdater.setup(&server, "/update", www_username, www_password);
    // Start the server
    server.begin();
    Serial.println("Server started");
    // Print the IP address
    String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(WEBSERVER_PORT) + "/";
    Serial.println("Use this URL : " + webAddress);
    scrollMessage(" v" + String(VERSION) + "  IP: " + WiFi.localIP().toString() + "  ");

    timeNTPsetup();
  } else {
    Serial.println("Web Interface is Disabled");
    scrollMessage("Web Interface is Disabled");
  }

  flashLED(1, 500);
}

//************************************************************
// Main Looop
//************************************************************
void loop() {
  //Get some Weather Data to serve
  if ((getMinutesFromLastRefresh() >= minutesBetweenDataRefresh) || lastEpoch == 0) {
    getWeatherData();
  }
  checkDisplay(); // this will see if we need to turn it on or off for night mode.

  if (lastMinute != minute()) {
    lastMinute = minute();

    if (weatherClient.getErrorMessage() != "") {
      scrollMessage(weatherClient.getErrorMessage());
      return;
    }

    if (displayOn) {
      matrix.shutdown(false);
    }
    matrix.fillScreen(LOW); // show black
    if (OCTOPRINT_ENABLED) {
      if (displayOn && ((printerClient.isOperational() || printerClient.isPrinting()) || printerCount == 0)) {
        // This should only get called if the printer is actually running or if it has been 2 minutes since last check
        printerClient.getPrinterJobResults();
      }
      printerCount += 1;
      if (printerCount > 2) {
        printerCount = 0;
      }
    }

    displayRefreshCount --;
    // Check to see if we need to Scroll some Data
    if (displayRefreshCount <= 0) {
      displayRefreshCount = minutesBetweenScrolling;
      String temperature = String(weatherClient.getTemperature(),0);
      String weatherDescription = weatherClient.getWeatherDescription();
      weatherDescription.toUpperCase();
      String msg;
      msg += " ";

      if (SHOW_DATE) {
        msg += getDayName(weekday()) + ", ";
        msg += getMonthName(month()) + " " + day() + "  ";
      }
      if (SHOW_CITY) {
        msg += weatherClient.getCity() + "  ";
      }
      msg += temperature + getTempSymbol() + "  ";

      //show high/low temperature
      if (SHOW_HIGHLOW) {
        msg += "High/Low:" + String(weatherClient.getTemperatureHigh(),0) + "/" + String(weatherClient.getTemperatureLow(),0) + " " + getTempSymbol() + "  ";
      }

      if (SHOW_CONDITION) {
        msg += weatherDescription + "  ";
      }
      if (SHOW_HUMIDITY) {
        msg += F("Humidity:") + String(weatherClient.getHumidity()) + "%  ";
      }
      if (SHOW_WIND) {
        msg += F("Wind: ") + weatherClient.getWindDirectionText() + " @ " + String(weatherClient.getWindSpeed(),0) + " " + getSpeedSymbol() + "  ";
      }
      //line to show barometric pressure
      if (SHOW_PRESSURE) {
        msg += F("Pressure:") + String(weatherClient.getPressure()) + getPressureSymbol() + "  ";
      }

      msg += marqueeMessage + " ";

      if (NEWS_ENABLED) {
        msg += "  " + NEWS_SOURCE + ": " + newsClient.getTitle(newsIndex) + "  ";
        newsIndex += 1;
        if (newsIndex > 9) {
          newsIndex = 0;
        }
      }
      if (OCTOPRINT_ENABLED && printerClient.isPrinting()) {
        msg += "  " + printerClient.getFileName() + " ";
        msg += "(" + printerClient.getProgressCompletion() + "%)  ";
      }
      if (USE_PIHOLE) {
        piholeClient.getPiHoleData(PiHoleServer, PiHolePort, PiHoleApiKey);
        piholeClient.getGraphData(PiHoleServer, PiHolePort, PiHoleApiKey);
        if (piholeClient.getPiHoleStatus() != "") {
          msg += "    Pi-hole (" + piholeClient.getPiHoleStatus() + "): " + piholeClient.getAdsPercentageToday() + "% ";
        }
      }

      scrollMessage(msg);
      drawPiholeGraph();
    }
  }

  String currentTime = hourMinutes(false);

  if (numberOfHorizontalDisplays >= 8) {
    if (Wide_Clock_Style == "1") {
      // On Wide Display -- show the current temperature as well
      String currentTemp = String(weatherClient.getTemperature(),0);
      currentTime += " " + currentTemp + getTempSymbol();
    }
    if (Wide_Clock_Style == "2") {
      currentTime = currentTime + secondsIndicator(false) + zeroPad(second());
      matrix.fillScreen(LOW); // show black
    }
    if (Wide_Clock_Style == "3") {
      // No change this is normal clock display
    }
  }
  matrix.fillScreen(LOW);
  centerPrint(currentTime, true);

  if (WEBSERVER_ENABLED) {
    server.handleClient();
  }
  if (ENABLE_OTA) {
    ArduinoOTA.handle();
  }
}


String hourMinutes(boolean isRefresh) {
  if (IS_24HOUR) {
    return String(hour()) + secondsIndicator(isRefresh) + zeroPad(minute());
  } else {
    return String(hourFormat12()) + secondsIndicator(isRefresh) + zeroPad(minute());
  }
}

String secondsIndicator(boolean isRefresh) {
  String rtnValue = ":";
  if (isRefresh == false && (flashOnSeconds && (second() % 2) == 0)) {
    rtnValue = " ";
  }
  return rtnValue;
}

boolean authentication() {
  if (IS_BASIC_AUTH) {
    return server.authenticate(www_username, www_password);
  }
  return true; // Authentication not required
}

void handlePull() {
  getWeatherData(); // this will force a data pull for new weather
  displayWeatherData();
}

void handleSaveWideClock() {
  // test that important args are present to accept new config
  if (server.hasArg(F("wideclockformat"))) {
    if (!authentication()) {
      return server.requestAuthentication();
    }
    if (numberOfHorizontalDisplays >= 8) {
      Wide_Clock_Style = server.arg(F("wideclockformat"));
      writeConfiguration();
      matrix.fillScreen(LOW); // show black
    }
  }
  redirectHome();
}

void handleSaveNews() {
  // test that important args are present to accept new config
  if (server.hasArg(F("newsApiKey")) &&
      server.hasArg(F("newssource"))) {
    if (!authentication()) {
      return server.requestAuthentication();
    }
    NEWS_ENABLED = server.hasArg(F("displaynews"));
    NEWS_API_KEY = server.arg(F("newsApiKey"));
    NEWS_SOURCE = server.arg(F("newssource"));
    matrix.fillScreen(LOW); // show black
    writeConfiguration();
    newsClient.updateNews();
  }
  redirectHome();
}

void handleSaveOctoprint() {
  // test that important args are present to accept new config
  if (server.hasArg(F("octoPrintApiKey")) &&
      server.hasArg(F("octoUser"))
      ) {
    if (!authentication()) {
      return server.requestAuthentication();
    }
    OCTOPRINT_ENABLED = server.hasArg(F("displayoctoprint"));
    OCTOPRINT_PROGRESS = server.hasArg(F("octoprintprogress"));
    OctoPrintApiKey = server.arg(F("octoPrintApiKey"));
    OctoPrintServer = server.arg(F("octoPrintAddress"));
    OctoPrintPort = server.arg(F("octoPrintPort")).toInt();
    OctoAuthUser = server.arg(F("octoUser"));
    OctoAuthPass = server.arg(F("octoPass"));
    matrix.fillScreen(LOW); // show black
    writeConfiguration();
    if (OCTOPRINT_ENABLED) {
      printerClient.getPrinterJobResults();
    }
  }
  redirectHome();
}

void handleSavePihole() {
  // test that some important args are present to accept new config
  if (server.hasArg(F("piholeAddress")) &&
      server.hasArg(F("piholePort")) &&
      server.hasArg(F("piApiToken"))
      ) {

    if (!authentication()) {
      return server.requestAuthentication();
    }
    USE_PIHOLE = server.hasArg(F("displaypihole"));
    PiHoleServer = server.arg(F("piholeAddress"));
    PiHolePort = server.arg(F("piholePort")).toInt();
    PiHoleApiKey = server.arg(F("piApiToken"));
    Serial.println(F("PiHoleApiKey from save: ") + PiHoleApiKey);
    writeConfiguration();
    if (USE_PIHOLE) {
      piholeClient.getPiHoleData(PiHoleServer, PiHolePort, PiHoleApiKey);
      piholeClient.getGraphData(PiHoleServer, PiHolePort, PiHoleApiKey);
    }
  }
  redirectHome();
}

void handleSaveConfig() {
  // test that some important args are present to accept new config
  if (server.hasArg(F("openWeatherMapApiKey")) &&
      server.hasArg(F("city1")) &&
      server.hasArg(F("marqueeMsg")) &&
      server.hasArg(F("startTime")) &&
      server.hasArg(F("userid")) &&
      server.hasArg(F("stationpassword")) &&
      server.hasArg(F("theme")) &&
      server.hasArg(F("scrollspeed"))
      ) {

    if (!authentication()) {
      return server.requestAuthentication();
    }
    APIKEY = server.arg(F("openWeatherMapApiKey"));
    CityID = server.arg(F("city")).toInt();
    flashOnSeconds = server.hasArg(F("flashseconds")); // means blinking ":" on clock
    IS_24HOUR = server.hasArg(F("is24hour"));
    IS_PM = server.hasArg(F("isPM"));
    SHOW_DATE = server.hasArg(F("showdate"));
    SHOW_CITY = server.hasArg(F("showcity"));
    SHOW_CONDITION = server.hasArg(F("showcondition"));
    SHOW_HUMIDITY = server.hasArg(F("showhumidity"));
    SHOW_WIND = server.hasArg(F("showwind"));
    SHOW_PRESSURE = server.hasArg(F("showpressure"));
    SHOW_HIGHLOW = server.hasArg(F("showhighlow"));
    IS_METRIC = server.hasArg(F("metric"));
    marqueeMessage = decodeHtmlString(server.arg(F("marqueeMsg")));
    timeDisplayTurnsOn = decodeHtmlString(server.arg(F("startTime")));
    timeDisplayTurnsOff = decodeHtmlString(server.arg(F("endTime")));
    displayIntensity = server.arg(F("ledintensity")).toInt();
    minutesBetweenDataRefresh = server.arg(F("refresh")).toInt();
    themeColor = server.arg(F("theme"));
    minutesBetweenScrolling = server.arg(F("refreshDisplay")).toInt();
    displayScrollSpeed = server.arg(F("scrollspeed")).toInt();
    IS_BASIC_AUTH = server.hasArg(F("isBasicAuth"));
    String temp = server.arg(F("userid"));
    temp.toCharArray(www_username, sizeof(temp));
    temp = server.arg(F("stationpassword"));
    temp.toCharArray(www_password, sizeof(temp));
    weatherClient.setMetric(IS_METRIC);
    matrix.fillScreen(LOW); // show black
    writeConfiguration();
    getWeatherData(); // this will force a data pull for new weather
  }
  redirectHome();
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

void handleForgetWifi() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  redirectHome();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
}

void handleWideClockConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");

  sendHeader();

  if (numberOfHorizontalDisplays >= 8) {
    // Wide display options
    String form = FPSTR(WIDECLOCK_FORM);
    String clockOptions = F("<option value='1'>HH:MM Temperature</option><option value='2'>HH:MM:SS</option><option value='3'>HH:MM</option>");
    clockOptions.replace(Wide_Clock_Style + "'", Wide_Clock_Style + F("' selected"));
    form.replace(F("%WCLK_OPT%"), clockOptions);
    server.sendContent(form);
  }

  sendFooter();

  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void handleNewsConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");

  sendHeader();

  String form = FPSTR(NEWS_FORM1);
  form.replace(F("%NEWS_CB%"), (NEWS_ENABLED) ? "checked='checked'" : "");
  form.replace(F("%NEWSKEY%"), NEWS_API_KEY);
  form.replace(F("%NEWSSRC%"), NEWS_SOURCE);
  server.sendContent(form); //Send news form

  sendFooter();

  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void handleOctoprintConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");

  sendHeader();

  String form = FPSTR(OCTO_FORM);
  form.replace(F("%OCTO_CB%"), (OCTOPRINT_ENABLED) ? "checked='checked'" : "");
  form.replace(F("%OCTPG_CB%"), (OCTOPRINT_PROGRESS) ? "checked='checked'" : "");
  form.replace(F("%OCTOKEY%"), OctoPrintApiKey);
  form.replace(F("%OCTOADR%"), OctoPrintServer);
  form.replace(F("%OCTOPOR%"), String(OctoPrintPort));
  form.replace(F("%OCTOUSR%"), OctoAuthUser);
  form.replace(F("%OCTOPW%"), OctoAuthPass);
  server.sendContent(form);

  sendFooter();

  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void handlePiholeConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");

  sendHeader();

  server.sendContent(FPSTR(PIHOLE_TEST));

  String form = FPSTR(PIHOLE_FORM);
  form.replace("%PIHO_CB%", (USE_PIHOLE) ? "checked='checked'" : "");
  form.replace("%PIHO_ADR%", PiHoleServer);
  form.replace("%PIHO_PRT%", String(PiHolePort));
  form.replace("%PIHO_API%", PiHoleApiKey);

  server.sendContent(form);
  form = "";

  sendFooter();

  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void handleConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);
  String html = "";

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");

  sendHeader();

  String form = FPSTR(CHANGE_FORM1);
  form.replace(F("%OWMKEY%"), APIKEY);
  form.replace(F("%CTYNM%"), (weatherClient.getCity() != "") ? weatherClient.getCity() + ", " + weatherClient.getCountry() : "");
  form.replace(F("%CITY%"), String(CityID));
  form.replace(F("%DATE_CB%"), (SHOW_DATE) ? "checked='checked'" : "");
  form.replace(F("%CITY_CB%"), (SHOW_CITY) ? "checked='checked'" : "");
  form.replace(F("%COND_CB%"), (SHOW_CONDITION) ? "checked='checked'" : "");
  form.replace(F("%HUM_CB%"), (SHOW_HUMIDITY) ? "checked='checked'" : "");
  form.replace(F("%WIND_CB%"), (SHOW_WIND) ? "checked='checked'" : "");
  form.replace(F("%PRES_CB%"), (SHOW_PRESSURE) ? "checked='checked'" : "");
  form.replace(F("%HILO_CB%"), (SHOW_HIGHLOW) ? "checked='checked'" : "");
  form.replace(F("%24HR_CB%"), (IS_24HOUR) ? "checked='checked'" : "");
  form.replace(F("%METRIC_CB%"), (IS_METRIC) ? "checked='checked'" : "");
  server.sendContent(form);

  form = FPSTR(CHANGE_FORM2);
  form.replace(F("%PM_CB%"), (IS_PM) ? "checked='checked'" : "");
  form.replace(F("%FLASH_CB%"), (flashOnSeconds) ? "checked='checked'" : "");
  form.replace(F("%MSG%"), marqueeMessage);
  form.replace(F("%STRT_TM%"), timeDisplayTurnsOn);
  form.replace(F("%END_TM%"), timeDisplayTurnsOff);
  form.replace(F("%INTY_OPT%"), String(displayIntensity));
  String dSpeed = String(displayScrollSpeed);
  String scrollOptions = F("<option value='35'>Slow</option><option value='25'>Normal</option><option value='15'>Fast</option><option value='10'>Very Fast</option>");
  scrollOptions.replace(dSpeed + "'", dSpeed + "' selected");
  form.replace(F("%SCRL_OPT%"), scrollOptions);
  String minutes = String(minutesBetweenDataRefresh);
  String options = F("<option>5</option><option>10</option><option>15</option><option>20</option><option>30</option><option>60</option>");
  options.replace(">" + minutes + "<", " selected>" + minutes + "<");
  form.replace(F("%RFSH_OPT%"), options);
  form.replace(F("%RFSH_DISP%"), String(minutesBetweenScrolling));
  String themeOptions = FPSTR(COLOR_THEMES);
  themeOptions.replace(">" + String(themeColor) + "<", " selected>" + String(themeColor) + "<");
  form.replace(F("%THEME_OPT%"), themeOptions);
  server.sendContent(form); //Send another chunk of the form

  form = FPSTR(CHANGE_FORM3);
  form.replace(F("%AUTH_CB%"), (IS_BASIC_AUTH) ? "checked='checked'" : "");
  form.replace(F("%CFGUID%"), String(www_username));
  form.replace(F("%CFGPW%"), String(www_password));
  server.sendContent(form); // Send the second chunk of Data

  sendFooter();

  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void handleDisplay() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  enableDisplay(!displayOn);
  displayMessage(F("Display is now ") + String((displayOn) ? "ON" : "OFF"));
}

//***********************************************************************
void getWeatherData() //client function to send/receive GET request data.
{
  digitalWrite(externalLight, LOW);
  matrix.fillScreen(LOW); // show black
  Serial.println();

  if (displayOn) {
    // only pull the weather data if display is on
    if (firstEpoch != 0) {
      centerPrint(hourMinutes(true), true);
    } else {
      centerPrint("...");
    }
    matrix.drawPixel(0, 7, HIGH);
    matrix.drawPixel(0, 6, HIGH);
    matrix.drawPixel(0, 5, HIGH);
    matrix.write();

    weatherClient.updateWeather();
    if (weatherClient.getErrorMessage() != "") {
      scrollMessage(weatherClient.getErrorMessage());
    } else {
      // Set current timezone (adapts to DST when regeon supports that)
      set_timeZoneSec(weatherClient.getTimeZoneSeconds());
    }
  }

  Serial.println(F("Updating Time..."));
  //Update the Time
  matrix.drawPixel(0, 4, HIGH);
  matrix.drawPixel(0, 3, HIGH);
  matrix.drawPixel(0, 2, HIGH);
  //Serial.println(F("matrix Width:") + String(matrix.width()));
  matrix.write();

  lastEpoch = now();
  if (timeStatus() != timeNotSet) {
    if (firstEpoch == 0) {
      firstEpoch = now();
      setSyncInterval(300);
      Serial.println(F("firstEpoch is: ") + String(firstEpoch));
    }
  }

  if (NEWS_ENABLED && displayOn) {
    matrix.drawPixel(0, 2, HIGH);
    matrix.drawPixel(0, 1, HIGH);
    matrix.drawPixel(0, 0, HIGH);
    matrix.write();
    //Serial.println(F("Getting News Data for ") + NEWS_SOURCE);
    newsClient.updateNews();
  }

  Serial.println(F("Version: " VERSION));
  Serial.println();
  digitalWrite(externalLight, HIGH);
}

void displayMessage(String message) {
  digitalWrite(externalLight, LOW);

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  sendHeader();
  server.sendContent(message);
  sendFooter();
  server.sendContent("");
  server.client().stop();

  digitalWrite(externalLight, HIGH);
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

void sendHeader() {
  String html = FPSTR(HEADER1);
  html.replace(F("$COLOR$"), themeColor);
  html.replace(F("$ICO$"), weatherClient.getIcon());
  html.replace(F("$IDES$"), weatherClient.getWeatherDescription());
  server.sendContent(html);

  server.sendContent(FPSTR(WEB_ACTIONS1));
  //Serial.println("Displays: " + String(numberOfHorizontalDisplays));
  if (numberOfHorizontalDisplays >= 8) {
    server.sendContent(F("<a class='w3-bar-item w3-button' href='/configurewideclock'><i class='far fa-clock'></i> Wide Clock</a>"));
  }
  server.sendContent(FPSTR(WEB_ACTIONS2));
  if (displayOn) {
    server.sendContent(F("<i class='fas fa-eye-slash'></i> Turn Display OFF"));
  } else {
    server.sendContent(F("<i class='fas fa-eye'></i> Turn Display ON"));
  }
  server.sendContent(FPSTR(WEB_ACTION3));

  server.sendContent(FPSTR(HEADER2));
}

void sendFooter() {
  int8_t rssi = getWifiQuality();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println("%");
  String html = FPSTR(FOOTER);

  html.replace(F("$UPD$"), getTimeTillUpdate());
  html.replace(F("$RSSI$"), String(rssi));
  server.sendContent(html);
}

void displayWeatherData() {
  digitalWrite(externalLight, LOW);
  String html = "";

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  sendHeader();

  String temperature = String(weatherClient.getTemperature(),1);

  String dtstr;
  if (IS_24HOUR) {
    // UK date+time presentation: MSB to LSB
    dtstr = String(year()) + " " + getMonthName(month()) + " " + day() + " " + zeroPad(hour()) + ":" + zeroPad(minute()) + ", " + getDayName(weekday());
  } else {
    // US date+time presentation
    dtstr = getDayName(weekday()) + ", " + getMonthName(month()) + " " + day() + ", " + hourFormat12() + ":" + zeroPad(minute()) + ", " + getAmPm(isPM());
  }

  Serial.println(weatherClient.getCity());
  Serial.println(weatherClient.getWeatherCondition());
  Serial.println(weatherClient.getWeatherDescription());
  Serial.print(F("UpdateTime: "));
  Serial.println(get24HrColonMin(weatherClient.getReportTimestamp() + weatherClient.getTimeZoneSeconds()));
  Serial.print(F("SunRiseTime: "));
  Serial.println(get24HrColonMin(weatherClient.getSunRise() + weatherClient.getTimeZoneSeconds()));
  Serial.print(F("SunSetTime: "));
  Serial.println(get24HrColonMin(weatherClient.getSunSet() + weatherClient.getTimeZoneSeconds()));
  Serial.println(temperature);
  Serial.println(dtstr);

  if (weatherClient.getCity() == "") {
    if (timeStatus() == timeNotSet) {
      html += F("<p>waiting for first time sync...</p>");
    }
    html += F("<p>Please <a href='/configure'>Configure Weather</a> API</p>");
    if (weatherClient.getErrorMessage() != "") {
      html += F("<p>Weather Error: <strong>") + weatherClient.getErrorMessage() + F("</strong></p>");
    }
  } else {
    html = F(
      "<script>"
      "function refreshPage(){if(document.getElementById('mySidebar').style.display==='none')window.location.reload();}"
      "var intervaltimer = setInterval(refreshPage, 10000);"
      "</script>"
      "<div class='w3-cell-row' style='width:100%'><h2>") + weatherClient.getCity() + ", " + weatherClient.getCountry() + F("</h2></div><div class='w3-cell-row'>"
      "<div class='w3-cell w3-left w3-medium' style='width:120px'>"
      "<img src='http://openweathermap.org/img/w/") + weatherClient.getIcon() + ".png' alt='" + weatherClient.getWeatherDescription() + "'><br>" +
      weatherClient.getHumidity() + F("% Humidity<br>") +
      weatherClient.getWindDirectionText() + " / " + String(weatherClient.getWindSpeed(),1) + F(" <span class='w3-tiny'>") + getSpeedSymbol() + F("</span> Wind<br>") +
      weatherClient.getPressure() + F(" Pressure<br>"
        "</div>"
        "<div class='w3-cell w3-container' style='width:100%'><p>") +
      weatherClient.getWeatherCondition() + " (" + weatherClient.getWeatherDescription() + ")<br>" +
      temperature + " " + getTempSymbol(true) + "<br>" +
      String(weatherClient.getTemperatureHigh(),1) + "/" + String(weatherClient.getTemperatureLow(),1) + " " + getTempSymbol(true) + "<br>"
      "SunRise " + get24HrColonMin(weatherClient.getSunRise() + weatherClient.getTimeZoneSeconds()) + "<br>"
      "SunSet " + get24HrColonMin(weatherClient.getSunSet() + weatherClient.getTimeZoneSeconds()) + "<br>"
      "Updated " + get24HrColonMin(weatherClient.getReportTimestamp() + weatherClient.getTimeZoneSeconds()) +
      F("<br>""<br>"
        "<a href='https://www.google.com/maps/@") + weatherClient.getLat() + "," + weatherClient.getLon() + F(",10000m/data=!3m1!1e3' target='_BLANK'><i class='fas fa-map-marker' style='color:red'></i> Map It!</a><br>"
        "</p></div></div><hr>"
        "<div class='w3-cell-row' style='width:100%'><h3>") + dtstr  + F("</h3></div>");
  }


  server.sendContent(String(html)); // spit out what we got
  html = ""; // fresh start


  if (OCTOPRINT_ENABLED) {
    html = F("<div class='w3-cell-row'><b>OctoPrint Status:</b> ");
    if (printerClient.isPrinting()) {
      int val = printerClient.getProgressPrintTimeLeft().toInt();
      int hours = numberOfHours(val);
      int minutes = numberOfMinutes(val);
      int seconds = numberOfSeconds(val);
      html += F("Online and Printing</br>Est. Print Time Left: ") + zeroPad(hours) + ":" + zeroPad(minutes) + ":" + zeroPad(seconds) + "<br>";

      val = printerClient.getProgressPrintTime().toInt();
      hours = numberOfHours(val);
      minutes = numberOfMinutes(val);
      seconds = numberOfSeconds(val);
      html += "Printing Time: " + zeroPad(hours) + ":" + zeroPad(minutes) + ":" + zeroPad(seconds) + "<br>" +
        printerClient.getState() + " " + printerClient.getFileName() + F("</br>"
        "<style>#myProgress {width: 100%;background-color: #ddd;}#myBar {width: ") + printerClient.getProgressCompletion() + F("%;height: 30px;background-color: #4CAF50;}</style>"
        "<div id=\"myProgress\"><div id=\"myBar\" class=\"w3-medium w3-center\">") + printerClient.getProgressCompletion() + F("%</div></div>");
    } else if (printerClient.isOperational()) {
      html += printerClient.getState();
    } else if (printerClient.getError() != "") {
      html += printerClient.getError();
    } else {
      html += F("Not Connected");
    }
    html += F("</div><br><hr>");
    server.sendContent(String(html));
    html = "";
  }

  if (USE_PIHOLE) {
    if (piholeClient.getError() == "") {
      html = F("<div class='w3-cell-row'><b>Pi-hole</b><br>"
        "Total Queries (") + piholeClient.getUniqueClients() + F(" clients): <b>") + piholeClient.getDnsQueriesToday() + F("</b><br>"
        "Queries Blocked: <b>") + piholeClient.getAdsBlockedToday() + F("</b><br>"
        "Percent Blocked: <b>") + piholeClient.getAdsPercentageToday() + F("%</b><br>"
        "Domains on Blocklist: <b>") + piholeClient.getDomainsBeingBlocked() + F("</b><br>"
        "Status: <b>") + piholeClient.getPiHoleStatus() + F("</b><br>"
        "</div><br><hr>");
    } else {
      html = F("<div class='w3-cell-row'>Pi-hole Error"
        "Please <a href='/configurepihole' title='Configure'>Configure</a> for Pi-hole <a href='/configurepihole' title='Configure'><i class='fas fa-cog'></i></a><br>"
        "Status: Error Getting Data<br>"
        "Reason: ") + piholeClient.getError() + F("<br></div><br><hr>");
    }
    server.sendContent(html);
    html = "";
  }

  if (NEWS_ENABLED) {
    html = F("<div class='w3-cell-row' style='width:100%'><h2>News (") + NEWS_SOURCE + F(")</h2></div>");
    if (newsClient.getTitle(0) == "") {
      html += F("<p>Please <a href='/configurenews'>Configure News</a> API</p>");
      server.sendContent(html);
      html = "";
    } else {
      for (int inx = 0; inx < 10; inx++) {
        html = F("<div class='w3-cell-row'><a href='") + newsClient.getUrl(inx) + F("' target='_BLANK'>") + newsClient.getTitle(inx) + "</a></div>" +
          newsClient.getDescription(inx) + "<br/><br/>";
        server.sendContent(html);
        html = "";
      }
    }
  }

  sendFooter();
  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void configModeCallback(WiFiManager* myWiFiManager) {
  Serial.println(F("Entered config mode"));
  Serial.println(WiFi.softAPIP());
  Serial.println(F("Wifi Manager"));
  Serial.println(F("Please connect to AP"));
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println(F("To setup Wifi Configuration"));
  scrollMessage(F("Please Connect to AP: ") + String(myWiFiManager->getConfigPortalSSID()));
  centerPrint("wifi");
}

void flashLED(int number, int delayTime) {
  for (int inx = 0; inx < number; inx++) {
#ifdef BUZZER_PIN
    tone(BUZZER_PIN, 440, delayTime);
#endif
    delay(delayTime);
    digitalWrite(externalLight, LOW);
    delay(delayTime);
    digitalWrite(externalLight, HIGH);
    delay(delayTime);
  }
#ifdef BUZZER_PIN
  noTone(BUZZER_PIN);
#endif
}

String getTempSymbol() {
  return getTempSymbol(false);
}

String getTempSymbol(bool forWeb) {
  //return = ((forWeb) ? "°" : String(char(247))) + String((IS_METRIC) ? 'C' : 'F');
  String rtnValue = (IS_METRIC) ? "C" : "F";
  if (forWeb) {
    rtnValue = "°" + rtnValue;
  } else {
    rtnValue = char(247) + rtnValue;
  }
  return rtnValue;
}


String getSpeedSymbol() {
  return (IS_METRIC) ? "kmh" : "mph";
}

String getPressureSymbol()
{
  return (IS_METRIC) ? "mb":"inHg";
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}

String getTimeTillUpdate() {
  String rtnValue = "";

  long timeToUpdate = (((minutesBetweenDataRefresh * 60) + lastEpoch) - now());

  int hours = numberOfHours(timeToUpdate);
  int minutes = numberOfMinutes(timeToUpdate);
  int seconds = numberOfSeconds(timeToUpdate);

  rtnValue += String(hours) + ":";
  if (minutes < 10) {
    rtnValue += "0";
  }
  rtnValue += String(minutes) + ":";
  if (seconds < 10) {
    rtnValue += "0";
  }
  rtnValue += String(seconds);

  return rtnValue;
}

int getMinutesFromLastRefresh() {
  int minutes = (now() - lastEpoch) / 60;
  return minutes;
}

int getMinutesFromLastDisplay() {
  int minutes = (now() - displayOffEpoch) / 60;
  return minutes;
}

void enableDisplay(boolean enable) {
  displayOn = enable;
  if (enable) {
    if (getMinutesFromLastDisplay() >= minutesBetweenDataRefresh) {
      // The display has been off longer than the minutes between refresh -- need to get fresh data
      lastEpoch = 0; // this should force a data pull of the weather
      displayOffEpoch = 0;  // reset
    }
    matrix.shutdown(false);
    matrix.fillScreen(LOW); // show black
    Serial.println(F("Display was turned ON: ") + get24HrColonMin(now()));
  } else {
    matrix.shutdown(true);
    Serial.println(F("Display was turned OFF: ") + get24HrColonMin(now()));
    displayOffEpoch = lastEpoch;
  }
}

// Toggle on and off the display if user defined times
void checkDisplay() {
  if (timeDisplayTurnsOn == "" || timeDisplayTurnsOff == "") {
    return; // nothing to do
  }
  String currentTime = zeroPad(hour()) + ":" + zeroPad(minute());

  if (currentTime == timeDisplayTurnsOn && !displayOn) {
    Serial.println("Time to turn display on: " + currentTime);
    flashLED(1, 500);
    enableDisplay(true);
  }

  if (currentTime == timeDisplayTurnsOff && displayOn) {
    Serial.println("Time to turn display off: " + currentTime);
    flashLED(2, 500);
    enableDisplay(false);
  }
}

void writeConfiguration() {
  // Save decoded message to FS file for playback on power up.
  File f = FS.open(CONFIG, "w");
  if (!f) {
    Serial.println(F("File open failed!"));
  } else {
    Serial.println(F("Saving settings now..."));
    f.println(F("APIKEY=") + APIKEY);
    f.println(F("CityID=") + String(CityID));
    f.println(F("marqueeMessage=") + marqueeMessage);
    f.println(F("newsSource=") + NEWS_SOURCE);
    f.println(F("timeDisplayTurnsOn=") + timeDisplayTurnsOn);
    f.println(F("timeDisplayTurnsOff=") + timeDisplayTurnsOff);
    f.println(F("ledIntensity=") + String(displayIntensity));
    f.println(F("scrollSpeed=") + String(displayScrollSpeed));
    f.println(F("isNews=") + String(NEWS_ENABLED));
    f.println(F("newsApiKey=") + NEWS_API_KEY);
    f.println(F("isFlash=") + String(flashOnSeconds));
    f.println(F("is24hour=") + String(IS_24HOUR));
    f.println(F("isPM=") + String(IS_PM));
    f.println(F("wideclockformat=") + Wide_Clock_Style);
    f.println(F("isMetric=") + String(IS_METRIC));
    f.println(F("refreshRate=") + String(minutesBetweenDataRefresh));
    f.println(F("minutesBetweenScrolling=") + String(minutesBetweenScrolling));
    f.println(F("isOctoPrint=") + String(OCTOPRINT_ENABLED));
    f.println(F("isOctoProgress=") + String(OCTOPRINT_PROGRESS));
    f.println(F("octoKey=") + OctoPrintApiKey);
    f.println(F("octoServer=") + OctoPrintServer);
    f.println(F("octoPort=") + String(OctoPrintPort));
    f.println(F("octoUser=") + OctoAuthUser);
    f.println(F("octoPass=") + OctoAuthPass);
    f.println(F("www_username=") + String(www_username));
    f.println(F("www_password=") + String(www_password));
    f.println(F("IS_BASIC_AUTH=") + String(IS_BASIC_AUTH));
    f.println(F("SHOW_CITY=") + String(SHOW_CITY));
    f.println(F("SHOW_CONDITION=") + String(SHOW_CONDITION));
    f.println(F("SHOW_HUMIDITY=") + String(SHOW_HUMIDITY));
    f.println(F("SHOW_WIND=") + String(SHOW_WIND));
    f.println(F("SHOW_PRESSURE=") + String(SHOW_PRESSURE));
    f.println(F("SHOW_HIGHLOW=") + String(SHOW_HIGHLOW));
    f.println(F("SHOW_DATE=") + String(SHOW_DATE));
    f.println(F("USE_PIHOLE=") + String(USE_PIHOLE));
    f.println(F("PiHoleServer=") + PiHoleServer);
    f.println(F("PiHolePort=") + String(PiHolePort));
    f.println(F("PiHoleApiKey=") + String(PiHoleApiKey));
    f.println(F("themeColor=") + themeColor);
  }
  f.close();

  readConfiguration();
  weatherClient.setCityId(CityID);
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
    //print each line read
    Serial.println(line);
    if ((idx = line.indexOf(F("APIKEY="))) >= 0) {
      APIKEY = line.substring(idx + 7);
      APIKEY.trim();
    }
    if ((idx = line.indexOf(F("CityID="))) >= 0) {
      CityID = line.substring(idx + 7).toInt();
    }
    if ((idx = line.indexOf(F("newsSource="))) >= 0) {
      NEWS_SOURCE = line.substring(idx + 11);
      NEWS_SOURCE.trim();
    }
    if ((idx = line.indexOf(F("isNews="))) >= 0) {
      NEWS_ENABLED = line.substring(idx + 7).toInt();
    }
    if ((idx = line.indexOf(F("newsApiKey="))) >= 0) {
      NEWS_API_KEY = line.substring(idx + 11);
      NEWS_API_KEY.trim();
    }
    if ((idx = line.indexOf(F("isFlash="))) >= 0) {
      flashOnSeconds = line.substring(idx + 8).toInt();
    }
    if ((idx = line.indexOf(F("is24hour="))) >= 0) {
      IS_24HOUR = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("isPM="))) >= 0) {
      IS_PM = line.substring(idx + 5).toInt();
    }
    if ((idx = line.indexOf(F("wideclockformat="))) >= 0) {
      Wide_Clock_Style = line.substring(idx + 16);
      Wide_Clock_Style.trim();
    }
    if ((idx = line.indexOf(F("isMetric="))) >= 0) {
      IS_METRIC = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("refreshRate="))) >= 0) {
      minutesBetweenDataRefresh = line.substring(idx + 12).toInt();
      if (minutesBetweenDataRefresh == 0) {
        minutesBetweenDataRefresh = 15; // can't be zero
      }
    }
    if ((idx = line.indexOf(F("minutesBetweenScrolling="))) >= 0) {
      displayRefreshCount = 1;
      minutesBetweenScrolling = line.substring(idx + 24).toInt();
    }
    if ((idx = line.indexOf(F("marqueeMessage="))) >= 0) {
      marqueeMessage = line.substring(idx + 15);
      marqueeMessage.trim();
    }
    if ((idx = line.indexOf(F("timeDisplayTurnsOn="))) >= 0) {
      timeDisplayTurnsOn = line.substring(idx + 19);
      timeDisplayTurnsOn.trim();
    }
    if ((idx = line.indexOf(F("timeDisplayTurnsOff="))) >= 0) {
      timeDisplayTurnsOff = line.substring(idx + 20);
      timeDisplayTurnsOff.trim();
    }
    if ((idx = line.indexOf(F("ledIntensity="))) >= 0) {
      displayIntensity = line.substring(idx + 13).toInt();
    }
    if ((idx = line.indexOf(F("scrollSpeed="))) >= 0) {
      displayScrollSpeed = line.substring(idx + 12).toInt();
    }
    if ((idx = line.indexOf(F("isOctoPrint="))) >= 0) {
      OCTOPRINT_ENABLED = line.substring(idx + 12).toInt();
    }
    if ((idx = line.indexOf(F("isOctoProgress="))) >= 0) {
      OCTOPRINT_PROGRESS = line.substring(idx + 15).toInt();
    }
    if ((idx = line.indexOf(F("octoKey="))) >= 0) {
      OctoPrintApiKey = line.substring(idx + 8);
      OctoPrintApiKey.trim();
    }
    if ((idx = line.indexOf(F("octoServer="))) >= 0) {
      OctoPrintServer = line.substring(idx + 11);
      OctoPrintServer.trim();
    }
    if ((idx = line.indexOf(F("octoPort="))) >= 0) {
      OctoPrintPort = line.substring(idx + 9).toInt();
    }
    if ((idx = line.indexOf(F("octoUser="))) >= 0) {
      OctoAuthUser = line.substring(idx + 9);
      OctoAuthUser.trim();
    }
    if ((idx = line.indexOf(F("octoPass="))) >= 0) {
      OctoAuthPass = line.substring(idx + 9);
      OctoAuthPass.trim();
    }
    if ((idx = line.indexOf(F("www_username="))) >= 0) {
      String temp = line.substring(idx + 13);
      temp.trim();
      temp.toCharArray(www_username, sizeof(temp));
    }
    if ((idx = line.indexOf(F("www_password="))) >= 0) {
      String temp = line.substring(idx + 13);
      temp.trim();
      temp.toCharArray(www_password, sizeof(temp));
    }
    if ((idx = line.indexOf(F("IS_BASIC_AUTH="))) >= 0) {
      IS_BASIC_AUTH = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_CITY="))) >= 0) {
      SHOW_CITY = line.substring(idx + 10).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_CONDITION="))) >= 0) {
      SHOW_CONDITION = line.substring(idx + 15).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_HUMIDITY="))) >= 0) {
      SHOW_HUMIDITY = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_WIND="))) >= 0) {
      SHOW_WIND = line.substring(idx + 10).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_PRESSURE="))) >= 0) {
      SHOW_PRESSURE = line.substring(idx + 14).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_HIGHLOW="))) >= 0) {
      SHOW_HIGHLOW = line.substring(idx + 13).toInt();
    }
    if ((idx = line.indexOf(F("SHOW_DATE="))) >= 0) {
      SHOW_DATE = line.substring(idx + 10).toInt();
    }
    if ((idx = line.indexOf(F("USE_PIHOLE="))) >= 0) {
      USE_PIHOLE = line.substring(idx + 11).toInt();
    }
    if ((idx = line.indexOf(F("PiHoleServer="))) >= 0) {
      PiHoleServer = line.substring(idx + 13);
      PiHoleServer.trim();
    }
    if ((idx = line.indexOf(F("PiHolePort="))) >= 0) {
      PiHolePort = line.substring(idx + 11).toInt();
    }
    if ((idx = line.indexOf(F("PiHoleApiKey="))) >= 0) {
      PiHoleApiKey = line.substring(idx + 13);
      PiHoleApiKey.trim();
    }
    if ((idx = line.indexOf(F("themeColor="))) >= 0) {
      themeColor = line.substring(idx + 11);
      themeColor.trim();
    }
  }
  fr.close();
  Serial.println(F("ReadConfigFile EOF"));
  matrix.setIntensity(displayIntensity);
  newsClient.updateNewsClient(NEWS_API_KEY, NEWS_SOURCE);
  weatherClient.setWeatherApiKey(APIKEY);
  weatherClient.setMetric(IS_METRIC);
  weatherClient.setCityId(CityID);
  printerClient.updateOctoPrintClient(OctoPrintApiKey, OctoPrintServer, OctoPrintPort, OctoAuthUser, OctoAuthPass);
}

void scrollMessage(String msg) {
  msg += " "; // add a space at the end
  for (int i = 0; i < (width * (int)msg.length() + (matrix.width() - 1) - spacer); i++) {
    if (WEBSERVER_ENABLED) {
      server.handleClient();
    }
    if (ENABLE_OTA) {
      ArduinoOTA.handle();
    }
    if (refresh == 1) i = 0;
    refresh = 0;
    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while (((x + width - spacer) >= 0) && letter >= 0) {
      if (letter < (int)msg.length()) {
        matrix.drawChar(x, y, msg[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display
    delay(displayScrollSpeed);
  }
  matrix.setCursor(0, 0);
}

void drawPiholeGraph() {
  if (!USE_PIHOLE || piholeClient.getBlockedCount() == 0) {
    return;
  }
  int count = piholeClient.getBlockedCount();
  int high = 0;
  int row = matrix.width() - 1;
  int yval = 0;

  int totalRows = count - matrix.width();

  if (totalRows < 0) {
    totalRows = 0;
  }

  // get the high value for the sample that will be on the screen
  for (int inx = count; inx >= totalRows; inx--) {
    if (piholeClient.getBlockedAds()[inx] > high) {
      high = (int)piholeClient.getBlockedAds()[inx];
    }
  }

  int currentVal = 0;
  for (int inx = (count - 1); inx >= totalRows; inx--) {
    currentVal = (int)piholeClient.getBlockedAds()[inx];
    yval = map(currentVal, 0, high, 7, 0);
    //Serial.println("Value: " + String(currentVal));
    //Serial.println("x: " + String(row) + " y:" + String(yval) + " h:" + String(8-yval));
    matrix.drawFastVLine(row, yval, 8 - yval, HIGH);
    if (row == 0) {
      break;
    }
    row--;
  }
  matrix.write();
  for (int wait = 0; wait < 500; wait++) {
    if (WEBSERVER_ENABLED) {
      server.handleClient();
    }
    if (ENABLE_OTA) {
      ArduinoOTA.handle();
    }
    delay(20);
  }
}

void centerPrint(String msg) {
  centerPrint(msg, false);
}

void centerPrint(String msg, boolean extraStuff) {
  int x = (matrix.width() - (msg.length() * width)) / 2;

  // Print the static portions of the display before the main Message
  if (extraStuff) {
    if (!IS_24HOUR && IS_PM && isPM()) {
      matrix.drawPixel(matrix.width() - 1, 6, HIGH);
    }

    if (OCTOPRINT_ENABLED && OCTOPRINT_PROGRESS && printerClient.isPrinting()) {
      int numberOfLightPixels = (printerClient.getProgressCompletion().toFloat() / float(100)) * (matrix.width() - 1);
      matrix.drawFastHLine(0, 7, numberOfLightPixels, HIGH);
    }

  }

  matrix.setCursor(x, 0);
  matrix.print(msg);

  matrix.write();
}

String decodeHtmlString(String msg) {
  String decodedMsg = msg;
  // Restore special characters that are misformed to %char by the client browser
  decodedMsg.replace("+", " ");
  decodedMsg.replace("%21", "!");
  decodedMsg.replace("%22", "");
  decodedMsg.replace("%23", "#");
  decodedMsg.replace("%24", "$");
  decodedMsg.replace("%25", "%");
  decodedMsg.replace("%26", "&");
  decodedMsg.replace("%27", "'");
  decodedMsg.replace("%28", "(");
  decodedMsg.replace("%29", ")");
  decodedMsg.replace("%2A", "*");
  decodedMsg.replace("%2B", "+");
  decodedMsg.replace("%2C", ",");
  decodedMsg.replace("%2F", "/");
  decodedMsg.replace("%3A", ":");
  decodedMsg.replace("%3B", ";");
  decodedMsg.replace("%3C", "<");
  decodedMsg.replace("%3D", "=");
  decodedMsg.replace("%3E", ">");
  decodedMsg.replace("%3F", "?");
  decodedMsg.replace("%40", "@");
  decodedMsg.toUpperCase();
  decodedMsg.trim();
  return decodedMsg;
}
