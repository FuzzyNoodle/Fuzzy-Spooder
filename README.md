## Important Notice
This library is work in progress, not ready for general usage. It is registered in library manger for development purpose only.

# Fuzzy-Spooder
An add-on filament autochanger for existing 3D printers, in duel-spool configuration.

- Firmware version: 0.5.0
- Config version: 0.3.0


---
## Getting Started

### Upload the NoWifi_Demo Example

#### Using Arduino IDE

1. [Install](https://arduino-esp8266.readthedocs.io/en/latest/installing.html) the ESP8266 Arduino Core.
2. Follow this [tutorial](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) to install the file system uploader, but use [this LittleFS plugin](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) instead of SPIFFS. 
3. ```Arduino IDE->Tools->Board->ESP8266 Boards```, select NodeMCU 1.0 (ESP-12 Module). You may change the **Upload Speed** to higher baud rate. 
4. Install the [Fuzzy Spooder](https://github.com/FuzzyNoodle/Fuzzy-Spooder) using the the [Arduino Library Manager](https://www.arduino.cc/en/guide/libraries#toc3). 
5. Starting with IDE v1.8.10, the following library dependencies will be prompted to install: (If not being prompted, please install them maually.)
    1. [ESP Rotary](https://github.com/LennartHennigs/ESPRotary) by Lennart Hennigs, version 1.4.2
    2. [Button2](https://github.com/LennartHennigs/Button2) by Lennart Hennigs, version 1.6.1
    3. [Blynk](https://github.com/blynkkk/blynk-library) by Volodymyr Shymanskyy, version 0.6.1
    4. [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://github.com/ThingPulse/esp8266-oled-ssd1306) by ThingPulse, Fabrice Weinberg, version 4.2.0
    5. [HX711_ADC](https://github.com/olkal/HX711_ADC) by Oolav Kallhovd, version 1.2.7
    6. [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoit Blanchon, version 6.17.3


6. Open the ```File->Examples->Fuzzy Spooder->NoWifi_Demo``` sketch and upload to your board via USB connection. Correct port needs to be selected.
7. [Upload](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) the data files in the \data folder too. See below.


#### Using VSCode IDE + platformio extension

1. ```PIO Home-> New Project```
    1. Name: **NoWifi_Demo** for example
    2. Board: Search "mcu" select **NodeMCU 1.0 (ESP-12E Module)**
    3. Framework: **Arduino**
2. ```PIO Home->Libraries->Search``` and go to **"Fuzzy Spooder"**.
3. Add to Project -> Select the project you just created. (ex. ```Projects\NoWifi_Demo```). The library dependencies should be automataclly downloaded and installed.
4. On the same page, select and copy all the sketch code from the NoWifi_Demo
5. Open ```VSCode->Explorer(Left/Top Icon)->[Your Project Name]->src->main.cpp```. Paste and overwrite the example sketch code copied from previous step.
6. Upload the program(The right arrow located at the bottom toolbar) via USB connection. Upload port should be auto-detected.
7. [Upload](https://diyprojects.io/esp8266-upload-data-folder-spiffs-littlefs-platformio) the data files in the \data folder too. See below.

Additional note:
The default platformio.ini configuration would be something like:

```
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps = georgychen/Fuzzy Spooder@^0.4.0
```

Modify the configuration section to:
```
[env:NoWifi_Demo Example - USB Serial]
platform = espressif8266
board = nodemcuv2
framework = arduino
board_build.filesystem = littlefs
board_build.ldscript = eagle.flash.4m1m.ld
monitor_speed = 115200
upload_speed = 921600
monitor_filters = send_on_enter
lib_deps = georgychen/Fuzzy Spooder@^0.4.0

```
This creates a more user friendly env:name, increases upload speed and enables serial debug. The [LittleFS filesystem](https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/) is used in this project.

---

### Upload the data folder to the file system
The data folder contains all the files to be uploaded to the ESP8266 flash memory. Sketch(program) upload and data upload are independent operations.

For example, the data folder contains:
- The **logo.bmp** shown during booting. It can be replaced by user.
- The **config.json** file, where all configurations are edited and stored.
- The **index.html** file for the browser file manager

#### Using Arduino IDE
1. Copy the 'data' folder from the 
  - library (ex: ```arduino\libraies\Fuzzy_Spooder```) to your 
  - sketch folder (ex: ```Arduino\NoWifi_Demo```).
2. Edit the config.json file in the data folder as required. 
3. Select the ```Arduino IDE -> Tools ->Flash Size 4MB (FS:1MB OTA:~1019KB)``` to save upload time.
3. Use the ```Arduino IDE -> Tools -> ESP8266 LittleFS Data Upload``` command to uplod the file image to esp8266 flash memory. 

#### Using VSCode IDE + platformio extension
1. Copy the 'data' folder from the 
  - library (ex: ```platformio_project_folder\.pio\libdeps\NoWifi_Demo\Fuzzy_Spooder```) to your 
  - project folder (ex: ```platformio_project_folder\NoWifi_Demo```).
2. Edit the config.json file in the data folder as required. 
3. Use the ```PlatformIO->PROJECT TASKS->NoWifi_Demo->Platform->Upload Filesystem Image``` command to uplod the file image. Make you have the ```board_build.filesystem = littlefs``` configurtion setting in the platformio.ini file.

---
### OTA Upload
#### Using Arduino IDE
Follow the IDE upload part in this [tutorial](https://randomnerdtutorials.com/esp8266-ota-updates-with-arduino-ide-over-the-air/) to upload the sketch wirelessly.

#### Using VSCode IDE + platformio extension
Add two lines in your platformio.ini:
- upload_protocol = espota
- upload_port = spooderA1.local

Creating a [env] is recommended. The whole section would look like this:

```
[env:Wifi_Standlone Example Example - OTA]
platform = espressif8266
board = nodemcuv2
framework = arduino
board_build.filesystem = littlefs
board_build.ldscript = eagle.flash.4m1m.ld
monitor_speed = 115200
upload_speed = 921600
monitor_filters = send_on_enter
upload_protocol = espota
;upload_port = 192.168.0.133  ;use ip address if the mDNS hostname is not resolved
upload_port = spooderA1.local ;using mDNS, replace with actual name
lib_deps = georgychen/Fuzzy Spooder@^0.4.0
```

After setting the OTA environment in the platformio.ini file, select the intended project environment on the bottom toolbar. Then, the upload command will conduct an OTA upload. If the mDNS hostname is not resolved, you can use the IP address.

---

### Using the NoWifi_Demo Example

An automatic tare is initiated upon power on. Tare should be done with the printed rack, but without spool holder/filament. 

A calibration needs to be done at least once with known weight.

There are three main pages:
- **Spooder Home Page**:
  - Single click: Cycles the display mode:
    - Filament weight: an estimated weight (total - spool holder weight). Displays "Empty" when the value is below a negative value.
    - Spool holder weight: an user input value, adjustable in sketch.
    - Total weight: measured (spool holder + filament) weight.
  - Long click: Perform a tare. 
- **Info Page**:
  - Displays additional information
- **Menu Page**: Rotate and click the menu items. Currently available menu items are:
  - Tare: Perform a tare.
  - Calibrate: Perform a calibration. Calibrated Value will be saved to EEPROM.
  - Spool Holder Weight: Set spool holder weight, or load preset values.
  - Set Spooder ID: Set the unique spooder ID.
  - Low Filament Setup: Adjust the threshold value for the low filament notification. Default value is 50 g.
  - Debug: Various debugging functions.

#### Spool Holder Weight

Spool holder weight is a user input value in grams. This weight is used to estimate remaining filament weight. The default spool holder weight can be 
- Set in the sketh using the `setCurrentSpoolHolderWeight(weight)` method. This method only works for the first time, where there is no previous data in the EEPROM, to prevent user set values being overridden upon reboot.
- Adjusted in the spooder UI. 
- Loaded from preset values.

There are additional slots (up to 32 maximum) of preset spool holders, each with its name and weight. They are defined in the `\data\config.json` file. These preset spool holders can be selected in the spooder UI.

### Using the Wifi_Stndalone Example
Add this line in your sketch setup:

 ```spooder.setWifi(true);``` 

 to enable the wifi function. User needs to provide wifi ssid/password, and Blynk Authorization Token in the config.json file. Install the Blynk app on your tab/phone. The browser file system works locally, but the Blynk Notification works globally. Which means, your phone doesn't have to be in your local wifi network to receive notifications.

#### Spooder ID and mDNS

The user needs to set an unique Spooder ID in the UI for each unit. The ID consists of a letter (from A to Z) and a number (from 1 to 99). For example, A1, B13, C3, etc... 

The unit's [mDNS](https://en.wikipedia.org/wiki/Multicast_DNS) hostname is prefixed with "spooder". For example, spooderA1, spooderB13, spooderC3, etc... The spooder can be accessed simply using "**spooderA1.local**", "**spooderB13.local**", etc. Note that the mDNS hostname is case-insensitive, which means "**SPOODERA1.LOCAL**" also works. If the mDNS isn't availble, IP address can always be used.

#### Notification

There are several conditions that will trigger the notification:

- Print job started: When the filament is constantly being pulled by the extruder. There will be some ~30 seconds lag for the detection.
- Print job completed: When the weight measured is being at rest after print job started.
- Low filament warning: When the remaining filament weight is less than a preset value. (default value is 50 g). This threshold can be adjusted in the menu. This notification can only be triggered once during each print.

The nofification is sent through the [Blynk App](https://docs.blynk.cc/).


#### Browser File System

After configuring wifi, the spooder file system can be accessed using a web browser. Type for example, http://spooderA1.local/edit in the URL of your browser. This function is directly imported from the excellent [FSBrowser example](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/FSBrowser) by Hristo Gochkov.

---


#### Bonjour Browser

The following apps can be used to browse active **spooders** (and other mDNS devices) in your local network. 
- [Bonjour Browser](https://play.google.com/store/apps/details?id=de.wellenvogel.bonjourbrowser) for Android
- [Bonjour Browser](https://hobbyistsoftware.com/bonjourbrowser) for Windows
- [Discovery - DNS-SD Browser](https://apps.apple.com/us/app/discovery-dns-sd-browser/id1381004916?mt=12) for macOS
- [Discovery - DNS-SD Browser](https://apps.apple.com/us/app/discovery-dns-sd-browser/id305441017) for iOS


#### Installing Blynk App for Push Notification 
(free for limited Widget usage)
1. Install "Blynk" App (iOS or Android)
2. Register a Blynk account
3. New Project 
  - Name: ex. "Spooder"
  - Device: "ESP8266"
  - Connection Type: "WiFi"
4. An unique Authorization Token (per project) will be sent to your registered email. Need to copy the code into the config.json under \data folder.   
5. Touch the design screen, a Widgex Box appear. Place a "Notification $400" widget.
6. Press "Play" icon on the top right corner. Done. App doesn't need to be active for the notification to work.

[Limitations:](https://github.com/blynkkk/blynkkk.github.io/blob/master/Widgets.md)

- Maximum allowed body length is 120 symbols;
- Every device can send only 1 notification every 5 seconds;

#### Printing Status

Printing status based on weight change pattern is defined as follow:

- STATUS_EMPTY
  - filamnetWeight < emptyThreshold
  
- STATUS_IDLE
- STATUS_PRINTING
