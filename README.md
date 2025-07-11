# LED Matrix Clock a.k.a. Marquee Scroller (Clock, Weather, and More)

## NOTICE
This fork enhances the Qrome Marquee Scroller version 3.03 from february 2023, that was developed during 2018-2022 period.

For more information and development history visit [Qrome Marquee Scroller](https://github.com/Qrome/marquee-scroller) and read his [README](https://github.com/Qrome/marquee-scroller/blob/master/README.md).

Read the [feature enhancements below](#feature-enhancements)


## Features include:
* Accurate Clock refresh from Internet Time Servers
* Display Local Weather and conditions (refreshed every 10 - 30 minutes, configurable)
* <del>Display News Headlines from all the major sources</del> -- removed, not free, see #1
* <del>Display 3D print progress from your OctoPrint Server</del> -- removed, not well suited for scrolling text display; better use [Qrome printer monitor with 128x64 pixel OLED display](https://github.com/Qrome/printer-monitor)
* <del>Display Pi-hole status and graph (each pixel across is 10 minutes)</del> -- removed, does not work, needs upgrade, not well suited for scrolling text display; better use [Qrome Pi-hole monitor with 128x64 pixel OLED display](https://github.com/Qrome/Pi-hole-Monitor)
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
Enhancements included in [THIS repository](https://github.com/rob040/LEDmatrixClock) :
* Removed the TimeZoneDB.com registration requirement.
* Added Time NTP and more efficient time strings instead. Actual time zone information is used from OpenWeather API.
* Update to new OpenWeatherMap.org API. Newly requested Free Service API-keys can no longer use the older call and structure.
* Reduce RAM usage. The ESP8266 is an older WiFi processor with limited RAM (80kB), especially when compared to its newer ESP32 members.
* Added MQTT support with basic Authentication, to send message to be displayed. Message is displayed immediately when display shows the time, and repeated every minute with default configuration.
* Use VScode IDE with PlatformIO / PIOarduino, for better development and maintenance experience and much better build environment and library version control.
* Improved start-up on time synchronisation and weather data update,
* Improved weather data display on webpage (minor changes)
* Automatic timezone (from Local weather), hence no need for TimeZoneDB.
* Added a favicon for easy recognition in your browser.
* Support for different display sizes without re-compilation, via configuration page (device will reboot upon change).
* Using the 'LittleFS' filesystem in stead of the depecated 'SPIFFS' filesystem
* Using ArduinoJson upgraded to version 7
* Webpage now has switchable dark-mode view
* Webpage now has switchable automatic page update
* Instead of scrolling text, there is also a static display mode for short messages, date, temperature or humidity only next to the time display.
*


### known issues
* Webpage update does halt the scrolling display for a moment. This cannot be prevented, only shortened with optimizations, such as reduction of 'String' usage.
* Scrolling text appears to have some 'flex' in it, due to variations in display data update. This is being worked on.
* When using the LED display at lowest intensity, some pixel flicker might be visible. It varies with display module HW. This is being worked on.


## Required Parts:
* Wemos D1 Mini ESP8266: https://amzn.to/3tMl81U
* LED Dot Matrix Module: https://amzn.to/2HtnQlD

**Note:** Using the links provided here help to support [Qrome](https://github.com/Qrome) for his efforts. Thank you for the support.

## Wiring for the Wemos D1 Mini to the LED Dot Matrix Display

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
Be aware that the display in above picture is actually upside-down; when viewed from front, the data input DIN is at the right hand side; the text on the PCB is usually upside-down.


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



## Compiling and Loading to Wemos D1 Mini (ESP8266)
### Using Arduino 2.x IDE
It is no longer recommended to use the Arduino IDE. It might be difficult to get the right library versions together, especially when using it also for other Arduino projects.<br>
Still, the source directory structure is kept such that this project can be build with little to no effort. This might change in the future.
* Support for ESP8266 Boards is included in Arduino v2.x
* Select Board:  "ESP8266" --> "LOLIN(WEMOS) D1 R2 & mini"
* Set Flash Size: 4MB (FS:1MB OTA:~1019KB)
* Select the **Port** from the tools menu.

### Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to installing and manage libraries https://www.arduino.cc/en/Guide/Libraries

**Packages** -- the following packages and libraries are used (download and install):
* <WiFiManager.h> --> https://github.com/tzapu/WiFiManager (latest)
* <TimeLib.h> --> https://github.com/PaulStoffregen/Time
* <Adafruit_GFX.h> --> https://github.com/adafruit/Adafruit-GFX-Library
* <Max72xxPanel.h> --> https://github.com/markruys/arduino-Max72xxPanel
* <JsonStreamingParser.h> --> https://github.com/squix78/json-streaming-parser  (to be discarded soon)
* <ArduinoJson> -->  https://github.com/bblanchon/ArduinoJson v7.4.2+

## Building with PlatformIO.
Use [**VScode**](https://code.visualstudio.com/docs) with [**PlatformIO**](https://platformio.org/) or better, its desendant fork [**PIOarduino**](https://marketplace.visualstudio.com/items?itemName=pioarduino.pioarduino-ide) extension.

Please refer to the provided links on how to install VScode on your OS.

Then, open this project with `Visual Studio Code`, via File --> Open Folder: select folder containing `platformio.ini` file.

The `platformio.ini` file contains the references to the required external libraries and version numbers.

To build, open the `pioarduino` or `platformio` extension (icon on left hand side bar), then under *PROJECT TASKS* -> *Default* -> *General* : select **Build** and then **Upload**

## Initial Configuration
Editing the **Settings.h** file is totally optional and not required.
All settings and API Keys are managed from the Web Interface.
* Open Weather Map free service API key: http://openweathermap.org/  -- this is used to get weather data and the current time zone from the selected City. This API key is required for correct time.
* <del>TimeZoneDB free registration for API key: https://timezonedb.com/register -- this is used for setting the time and getting the correct time zone as well as managing time changes due to Day Light Savings time by regions.  This key is set and managed only through the web interface. TimeZoneDB key is required for correct time display.</del><br> This is no longer needed nor requested!
* <del>Your OctoPrint API Key -- optional if you use the OctoPrint status.</del> This function has been removed, see above.
* <del>Your Pi-hole API Key -- optional if you use the Pi-hole status.</del> This function has been removed, see above.

**NOTE:** The settings in the Settings.h are the default settings for the first loading. After loading you will manage changes to the settings via the Web Interface. If you want to change settings again in the settings.h, you will need to erase the file system on the Wemos or use the `Reset Settings` option in the Web Interface.

## Web Interface
The Marquee Scroller uses the **WiFiManager** so when it can't find the last network it was connected to,
it will become an **Access Point Hotspot** -- connect to it with your phone at http://192.168.4.1/ and you can then enter your WiFi connection information.

After connected to your WiFi network it will display the IP address assigned to it and that can be
used to open a browser to the Web Interface.  You will be able to manage your API Keys through the web interface.

The default user / password for the configuration page is: admin / password

The Clock will display the local time of the City selected for the weather after a short synchronization period.

<p align="left" title="the (old) webinterface on a phone">
  <img src="/images/20180419_065805.png" width="200"/>
  <img src="/images/20180419_065815.png" width="200"/>
  <img src="/images/20180419_065832.png" width="200"/>
  <img src="/images/20180419_065858.png" width="200"/>
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
