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
* Configurable number of LED-matrix tiles (4 to >8) (compile option) and
  types of Clock Displays on larger panels, e.g. also display seconds or temperature
* Video: https://youtu.be/DsThufRpoiQ
* Build Video by Chris Riley: https://youtu.be/KqBiqJT9_lE

## Feature Enhancements
Enhancements included in [THIS repository](https://github.com/rob040/LEDmatrixClock) [by rob040](https://github.com/rob040/) :
* Removed the TimeZoneDB.com registration requirement.
* Actual time zone information is used from OpenWeatherMap API.
* Added Time NTP and more efficient time strings instead.
* Update to new OpenWeatherMap.org API. Newly requested Free Service API-keys can no longer use the older call and structure.
* Reduce RAM usage. The ESP8266 is an older WiFi processor with limited RAM (80kB), especially when compared to its newer ESP32 members.
* Added MQTT support with basic Authentication, to get message to be displayed. Message is displayed immediately when display shows the time, and repeated every minute with default configuration.
* Use VScode IDE with PlatformIO / PIOarduino, for better development and maintenance experience and much better build environment and library version control.
* Improved start-up on time synchronisation and weather data update.
* Improved weather data display on webpage (minor changes).
* Automatic timezone (from Local weather), hence no need for TimeZoneDB.
* Added a favicon for easy recognition in your browser.
* Support for different display sizes without re-compilation, via configuration page (device will reboot upon change).
* Using the 'LittleFS' filesystem in stead of the deprecated 'SPIFFS' filesystem
* Using ArduinoJson upgraded to version 7
* Webpage now has switchable dark-mode view
* Webpage now has switchable automatic page update
* Instead of scrolling text, there is also a static display mode for short messages, date, temperature or humidity only next to the time display.
* Weather Location lookup has been made simpler; there is no longer the need to lookup the City-ID code; just enter a (valid) city name with optional 2-letter country code. Also GPS coordinates are now allowed as location input.
* Added a display QUIET time config option, where the display can be Off or Dimmed or Dimmed with no Motion (ie. no scrolling, no blinking)
* Replaced Wifi_manager with ESP_WiFiManager_Lite library, which allows multiple WiFi Accesspoints (SSID's) to be (pre-)configured, which is handy for a portable device. It also provides a fallback if one WiFi station goes down.

### known issues
* Webpage update does halt the scrolling display for a moment. See [issue #8](https://github.com/rob040/LEDmatrixClock/issues/8) for more details.
* Scrolling text appears to have some 'flex' in it. See [issue #9](https://github.com/rob040/LEDmatrixClock/issues/9) for more details.
* When using the LED display at lowest intensity, some pixel flicker might be visible. See [issue #10](https://github.com/rob040/LEDmatrixClock/issues/10) for more details.


## Required Parts:
* Wemos LOLIN D1 Mini ESP8266: https://www.wemos.cc/en/latest/d1/d1_mini.html
  Other EXP8266 boards based on [ESP12(-E,-F)](https://components101.com/sites/default/files/component_pin/ESP12E-Pinout.png) module, such as the NodeMCU, shall work equally well, only the 3D print models are more or less bound to the D1-mini physical dimensions.
* MAX7219 LED Dot Matrix Module 4-in-1 Display (FC16) for Arduino. Commonly available from Chinese webshops, or Ebay or Amazon.com.


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
That was caused by the fact that the original .stl can only fit a matrix display module of 128.4 mm wide. <br>
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
### Using Arduino 2.x IDE
It is no longer recommended to use the Arduino IDE. It might be difficult to get the right library versions together, especially when using it also for other Arduino projects.<br>
Still, the main sketch filename (marquee.ino) and source directory name (marquee) is kept such that this project can be build in the Arduino IDE. This might change in the future.
* Support for ESP8266 Boards is included in Arduino v2.x
* Select Board:  "ESP8266" --> "LOLIN(WEMOS) D1 R2 & mini"
* Set Flash Size: 4MB (FS:1MB OTA:~1019KB)
* Select the **Port** from the tools menu.

### Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to install and manage libraries https://www.arduino.cc/en/Guide/Libraries

**Packages** -- the following packages and libraries are used (download and install):
* <TimeLib.h> --> https://github.com/PaulStoffregen/Time v1.6.1+
* <Adafruit_GFX.h> --> https://github.com/adafruit/Adafruit-GFX-Library v1.12.1+ (and Adafruit BusIO at version 1.17.2+)
* <ArduinoJson.h> -->  https://github.com/bblanchon/ArduinoJson v7.4.2+
* <PubSubClient.h> --> https://github.com/hmueller01/pubsubclient3 v3.2.0+

**Local libraries** --
This project has local libraries in the lib directory.
Having project local libraries is not supported by Arduino IDE.
Therefore you must copy these manually to your Arduino sketchbook library directory. 
This is the directory where all libraries managed by the Arduino library manager are located.<br>
By default this is `C:\Users\<userName>\Documents\Arduino\libraries` on Windows machines.

Copy local libraries:
* in command terminal, change to marquee directory,
* `xcopy/s ..\lib\* C:\Users\<userName>\Documents\Arduino\libraries`

After copy do restart the Arduino IDE.

Assure there are no similarly named libaries (/packages) with higher version numbers.
Use library manager to check. At end of compilation in the IDE, a list of used libraries is shown. Check these.


## Building with PlatformIO.
Use [**VScode**](https://code.visualstudio.com/docs) with [**PlatformIO**](https://platformio.org/) or better, its descendant fork [**PIOarduino**](https://marketplace.visualstudio.com/items?itemName=pioarduino.pioarduino-ide) extension.

Please refer to the provided links on how to install VScode on your OS.

Then, open this project with `Visual Studio Code`, via File --> Open Folder: select folder containing `platformio.ini` file.

The `platformio.ini` file contains the references to the required external libraries and version numbers.

To build, open the `pioarduino` or `platformio` extension (icon on left hand side bar), then under *PROJECT TASKS* -> *Default* -> *General* : select **Build** and then **Upload**

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
