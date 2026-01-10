# LED Matrix Clock a.k.a. Marquee Scroller (Clock, Weather, and More)

## NOTICE
This fork enhances the Qrome Marquee Scroller version 3.03 from february 2023, that was developed during 2018-2022 period.

For more information and development history visit [Qrome Marquee Scroller](https://github.com/Qrome/marquee-scroller) and read his [README](https://github.com/Qrome/marquee-scroller/blob/master/README.md).

Read the [feature enhancements below](#feature-enhancements)


## Features include:
* Accurate Clock refresh from Internet Time Servers
* Display Local Weather and conditions (refreshed every 10 - 30 minutes, configurable)
* Configured through Web Interface
* Basic Authorization to access Configuration web interface
* Update firmware through web interface over WiFi (OTA)
* Configurable scroll speed and LED brightness
* Configurable data display and data update interval frequency
* Configurable sleep / wake times
* Configurable number of LED-matrix tiles (4 to >8) and
  types of Clock Displays on larger panels, e.g. also display seconds or temperature
* Video: https://youtu.be/DsThufRpoiQ
* Build Video by Chris Riley: https://youtu.be/KqBiqJT9_lE

## Feature Enhancements
Enhancements included in [THIS repository](https://github.com/rob040/LEDmatrixClock) [by rob040](https://github.com/rob040/) :

* Updated to new OpenWeatherMap.org API. Newly requested Free Service API-keys can no longer use the older call and structure.
* **Automatic timezone** is obtained from OpenWeatherMap API, using your configured location.
* It is no longer required to register for TimeZoneDB.com. This provider has been removed from the code.
* Add **Time NTP** to obtain regular time updates (original did not use NTP Network Time Protocol).
* Use more efficient time strings. Also, all text strings are stored in program flash (PROGMEM).
* Reduce RAM usage. The ESP8266 is an older WiFi processor with limited RAM (80kB), especially when compared to its newer ESP32 members.
* Add **MQTT support** with basic Authentication, to get message to be displayed. Message is displayed immediately when display shows the time, and repeated every minute with default configuration.
* Use **VS-Code IDE** with **PlatformIO**, for better development and maintenance experience and much better build environment and library version control.
* Improved start-up on time synchronisation and weather data update.
* Improved weather data display on webpage (minor changes).
* Add a **favicon** for easy recognition in your browser.
* Support for **different matrix display sizes** without re-compilation, via configuration page (device will reboot upon change).
* Using the 'LittleFS' filesystem in stead of the deprecated 'SPIFFS' filesystem.
* Using ArduinoJson upgraded to version 7.
* Webpage now has switchable **dark-mode view**.
* Webpage now has switchable **automatic page update**.
* Instead of scrolling text, there is also a **static display** mode for short messages, date, temperature or humidity, next to the time display.
* Weather **Location** lookup has been made simpler; there is no longer the need to lookup the City-ID code; just enter a (valid) **city name** with optional 2-letter state or country code. Also **GPS coordinates** are now allowed as location input.
* Added a display **QUIET time** config option, where the display can be Off or Dimmed or Dimmed with no Motion (ie. no scrolling, no blinking)
* Replaced Wifi_manager with **ESP_WiFiManager_Lite** library, which allows **multiple WiFi Access Points** (SSID's) to be (pre-)configured, which is handy for a portable device. It also provides a fallback if one WiFi station goes down.
* User **configurable units** for temperature (°C,°F,K), wind speed (m/s,kmh,mph,kn,Bft) and air pressure (mb,hPa,mmHg,inHg,psi,atm).
* Removed the Imperial / Metric choice , with more customization freedom to the user.
* The LED Display character font has been overhauled for more consistency and readability.
* The hostname is now configurable. This hostname was "CLOCK-XXXXXX", where "XXXXXX" are device specific hex characters, which is still the default. An empty entry will return to the default hostname. A change will cause a reboot.
* The hostname is used for mDNS to access the webpage at <br>`  http://hostname.local`. This is shown at power-up.
* The MQTT status message was changed and the publish frequency was reduced to once every hour, and after startup and after connection error recovery.
* The used WiFi Access Point is shown at start-up and on main webpage.
* The list of configured alternative WiFi Access Points, there can be 4, is shown on the main webpage.
* The 'Forget Wifi' menu entry will enter the WifiManager, but the configured WiFi Access Points will NOT be forgotten, nor erased, meaning that when nothing is changed and all is still valid, a power-cycle will connect to the best available WiFi Access Point.

* As of version 3.4.0, there is **multiple language support** for texts on the LED Matrix display. The web server pages will remain in English.
* There is Language support for following languages:
  * => English
  * => Nederlands  (Dutch)
  * => Deutsch  (German)
  * => Français  (French)
  * => Italiano  (Italian)
  * => Español  (Spanish)
  * => Português  (Portuguese)
  * => Dansk  (Danish)
  * => Norsk  (Norwegian)
  * => Svenska  (Swedish)
  * => Polski  (Polish)
  * => Čeština   (Czech)
  * => Slovenčina  (Slovak)


### known issues
* Webpage update does halt the scrolling display for a moment. See [discussion #34](https://github.com/rob040/LEDmatrixClock/discussions/34) for more details.
* Scrolling text appears to have some 'flex' in it. See [discussion #35](https://github.com/rob040/LEDmatrixClock/discussions/35) for more details.
* When using the LED display at lowest intensity, some pixel flicker might be visible. See [discussion #36](https://github.com/rob040/LEDmatrixClock/discussions/36) for more details.


## Required Parts:
* Wemos LOLIN D1 Mini ESP8266: https://www.wemos.cc/en/latest/d1/d1_mini.html <br>
  Other ESP8266 boards based on [ESP12(-E,-F)](https://components101.com/sites/default/files/component_pin/ESP12E-Pinout.png) module, such as the NodeMCU, shall work equally well, only the 3D print models are more or less bound to the D1-mini physical dimensions. <br>
  Get the D1 mini board from reputable source!
  There are clones around with sub-standard voltage regulators on board, that cannot supply the required 500mA. This can lead to unexpected hangups or resets when WiFi transmission is initiated. [See this](https://www.letscontrolit.com/forum/viewtopic.php?t=6603) and [this](https://github.com/bbqkees/Wemos-the-Clone-Wars#underpowered-ldo)
* MAX7219 LED Dot Matrix Module 4-in-1 Display (FC16) for Arduino. Commonly available from Chinese webshops, or Ebay or Amazon.
* A good 5V USB power supply
* Optional Electrolytic Capacitor on the 5V rail of LED display. Capacity: 100 uF -- 1000 uF, Voltage: > 6 Volts, Low ESR.<br>
  This can be salvaged from disused electronic equipment, such as a power-supply or PC mother board. It must be of good quality though; at minimum, the top should not be bulged or broken.


## Wiring for the Wemos D1 Mini to the MAX7218 LED Dot Matrix Display

| LED | D1 mini   |
| --- | ---       |
| VCC | 5V        |
| GND | G (GND)   |
| CLK | D5 (SCK)  |
| CS  | D6        |
| DIN | D7 (MOSI) |

The Connections in a picture:
<p align="left" title="Wires from D1-mini to LED display">
  <img src="/images/marquee_scroller_pins.png" width="400"/>
</p>
Be aware that the display in above picture is actually upside-down. When viewed from front, the data input DIN is at the right hand side; the text on the PCB is usually upside-down.

Solder the Electrolytic Capacitor directly on the back of the LED display board, with (+) on VCC and (-) on GND.


## 3D Printed Case by David Payne:
Original Single Panel version: https://www.thingiverse.com/thing:2867294 <br>
Double Wide LED version: https://www.thingiverse.com/thing:2989552

<p align="left" title="STL models from Thingiverse">
  <img src="/images/Thingiverse-stl-base.png" width="400"/>
  <img src="/images/Thingiverse-stl-back.png" width="400"/>
</p>

<p align="left" title="Marquee Scroller parts">
  <img src="/images/marquee-allparts.jpg" width="270"/>
  <img src="/images/marquee-backplate.jpg" width="270"/>
  <img src="/images/marquee-lit.jpg" width="260"/>
</p>
<p align="left" title="Makes from Thingiverse">
  <img src="/images/20171105_124635.jpg" width="200"/>
  <img src="/images/20171017_075241.jpg" width="200"/>
  <img src="/images/20171105_125913.jpg" width="200"/>
  <img src="/images/20171022_214044.jpg" width="200"/>
</p>
<p align="left" title="Makes from Thingiverse">
  <img src="/images/20180128_091534.jpg" width="200"/>
  <img src="/images/20180128_091524.jpg" width="200"/>
  <img src="/images/20180128_091552.jpg" width="200"/>
  <img src="/images/20180127_135828.jpg" width="200"/>
</p>

From the .stl files posted on Thingiverse.com, a SketchUp model was re-created and new .stl with wider dotmatrix display module area was made. <br>
Many [complained](https://www.thingiverse.com/thing:2867294/comments) that their dotmatrix display module didn't fit; some enlarged the X-axis in their slicer program to make it fit. Just too bad they needed to print it at least twice. <br>
That was caused by the fact that the original .stl can only fit a matrix display module of 128.4 mm wide. (4x 32mm tight together) <br>
The models available here will fit up to 132.0 mm wide display module. <br>
Most other characteristics, such as the loosely fitting backplate, were left unchanged; the unmatched screw holes were fixed though.

New / Updated SketchUp model and STL models are available here:
* [SketchUp model](/Models/LED_matrix_Clock_Marquee.skp)
* [STL base](/Models/LED_matrix_Clock_Marquee-base.stl) <img src="/Models/LED_matrix_Clock_Marquee-stl-base.png" height="40"/>
* [STL back](/Models/LED_matrix_Clock_Marquee-back.stl) <img src="/Models/LED_matrix_Clock_Marquee-stl-back.png" height="40"/>

Pictures:
<p align="left" title="Updated STL models">
  <img src="/Models/LED_matrix_Clock_Marquee-display_module_size.png" width="270"/>
  <img src="/Models/LED_matrix_Clock_Marquee+electronics-front_xray.png" width="260"/>
  <img src="/Models/LED_matrix_Clock_Marquee+electronics-back_xray.png" width="260"/>
</p>


## Compiling and Loading to Wemos D1 Mini (ESP8266)

## Building with VScode PlatformIO.
Use [**VScode**](https://code.visualstudio.com/docs) with [**PlatformIO**](https://platformio.org/).

Please refer to the provided links on how to install VScode on your OS.

Then, open this project with `Visual Studio Code`, via File --> Open Folder: select folder containing `platformio.ini` file.

The `platformio.ini` file contains the references to the required external libraries and version numbers.

To build, open the `platformio` extension (icon on left hand side bar), then under *PROJECT TASKS* -> *Default* -> *General* : select **Build** and then **Upload**

## Initial Configuration
Editing the **Settings.h** file is not required but optional to get different default values.
All settings and API Keys are managed from the Web Interface.

* Open Weather Map free service API key: http://openweathermap.org/  -- this is used to get weather data and the current time zone from the set Geo Location. This API key is required for correct time. Registration is required to obtain the API key.

**NOTE:** The settings in the Settings.h are the default settings for the first loading. After loading you will manage changes to the settings via the Web Interface. If you want to change settings again in the settings.h, you will need to erase the file system on the Wemos or use the `Reset Settings` option in the Web Interface.

## Web Interface
The Marquee Scroller uses the **WiFiManager** when it can't find the last network it was connected to,
it will become an **Access Point Hotspot** -- connect to it with your phone at http://192.168.4.1/ and you can then enter your WiFi connection information.<br>
When the **WiFiManager** is activated after power-up, the scrolling LED display shows the following message:<br>
`   v1.2.3  IP: 192.168.4.1  Wifi Manager Started... Please Connect to AP: ESP-XXXXXX password: MyESP-XXXXXX`<br>
And finally it displays:<br>
`   wifi`<br>
While the WiFiManager Access Point is active.<br>
The above "XXXXXX" are device specific hex characters.

After connected to your WiFi network it will, after power-up, display the IP address assigned to it and that can be
used to open a browser to the Web Interface.  You will be able to manage your API Keys through the web interface.

The default user / password for the configuration page is: admin / password

The Clock will display the local time of the Geo Location selected for the weather after a short synchronization period.

<p align="left" title="The new webinterface on a phone">
  <img src="/images/Screenshot_main.jpg" width="133"/>
  <img src="/images/Screenshot_main_menu.jpg" width="133"/>
  <img src="/images/Screenshot_cfg_mqtt.jpg" width="133"/>
  <img src="/images/Screenshot_cfg1.jpg" width="133"/>
  <img src="/images/Screenshot_cfg2.jpg" width="133"/>
  <img src="/images/Screenshot_cfg3.jpg" width="133"/>
</p>
<p align="left" title="The new webinterface in DARK MODE on a phone">
  <img src="/images/Screenshot_cfg4.jpg" width="133"/>
  <img src="/images/Screenshot_cfg5.jpg" width="133"/>
  <img src="/images/Screenshot_cfg6.jpg" width="133"/>
  <img src="/images/Screenshot_dark_main.jpg" width="133"/>
  <img src="/images/Screenshot_dark_main_menu.jpg" width="133"/>
  <img src="/images/Screenshot_dark_cfg.jpg" width="133"/>
</p>


## Contributors
David Payne <br>
Nathan Glaus <br>
Daniel Eichhorn -- Author of the TimeClient class (in older versions) <br>
yanvigdev <br>
nashiko-s <br>
magnum129 <br>
rob040  --  author of this repo and these [feature enhancements listed above](#feature-enhancements)<br>


Contributing to this software is warmly welcomed. You can do this basically by forking from master, committing modifications and then making a pulling requests against the latest DEV branch to be reviewed (follow the links above for operating guide). Detailed comments are encouraged. Adding change log and your contact into file header is encouraged. Thanks for your contribution.

When considering making a code contribution, please keep in mind the following goals for the project:
* User should not be required to edit the Settings.h file to compile and run.  This means the feature should be simple enough to manage through the web interface.
* Changes should always support the recommended hardware (links above).

<p title="Marquee Scroller">
  <img src="/images/ScrollingClock.jpg" width="400"/>
</p>
