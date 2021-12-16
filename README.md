# Fuzzy-Spooder
An add-on filament estimator for existing 3D printers. It provides filament remaining weight, track printer status, and send notifications to your mobile device. A WiFi environment is required for most functions.

---
## Getting Started
### Upload the Spooder Example
<details>
<summary>#### Using Arduino IDE</summary>

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


6. Open the ```File->Examples->Fuzzy Spooder->Spooder``` sketch and upload to your board via USB connection. Correct port needs to be selected.
7. [Upload](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) the data files in the \data folder too. See below.
</details>

<details>
<summary>#### Using VSCode IDE + platformio extension</summary>

1. ```PIO Home-> New Project```
    1. Name: **Spooder** for example
    2. Board: Search "mcu" select **NodeMCU 1.0 (ESP-12E Module)**
    3. Framework: **Arduino**
2. ```PIO Home->Libraries->Search``` and go to **"Fuzzy Spooder"**.
3. Add to Project -> Select the project you just created. (ex. ```Projects\Spooder```). The library dependencies should be automataclly downloaded and installed.
4. On the same page, select and copy all the sketch code from the Spooder 
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
lib_deps = georgychen/Fuzzy Spooder@^1.0.0
```

Modify the configuration section to:
```
[env:Spooder Example - USB Serial]
platform = espressif8266
board = nodemcuv2
framework = arduino
board_build.f_cpu = 160000000L
board_build.filesystem = littlefs
board_build.ldscript = eagle.flash.4m1m.ld
build_flags = -w
monitor_speed = 115200
upload_speed = 921600
monitor_filters = send_on_enter
lib_deps = georgychen/Fuzzy Spooder@^1.0.0

```
This creates a more user friendly env:name, increases upload speed and enables serial debug. The [LittleFS filesystem](https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/) is used in this project.

```build_flags = -w``` prevents some compile warnings/errors.

---
</details>

### Upload the data folder to the file system
The data folder contains all the files to be uploaded to the ESP8266 flash memory. Sketch(program) upload and data upload are independent operations.

For example, the data folder contains:
- The **logo.bmp** shown during booting. It can be replaced by user.
- The **config.json** file, where all configurations are edited and stored:
  - Your WiFi SSID/PASSWORD.
  - Blynk authorizatio code.
  - Spool holder name/weight presets.

- The **index.html** file for the browser file manager

<details>
<summary>#### Using Arduino IDE</summary>

1. Copy the 'data' folder from the 
  - library (ex: ```arduino\libraies\Fuzzy_Spooder```) to your 
  - sketch folder (ex: ```Arduino\Spooder```).
2. Edit the config.json file in the data folder as required. 
3. Select the ```Arduino IDE -> Tools ->Flash Size 4MB (FS:1MB OTA:~1019KB)```. This setup is required for OTA.
3. Use the ```Arduino IDE -> Tools -> ESP8266 LittleFS Data Upload``` command to uplod the file image to esp8266 flash memory. 
</details>

<details>
<summary>#### Using VSCode IDE + platformio extension</summary>

1. Copy the 'data' folder from the 
  - library (ex: ```platformio_project_folder\.pio\libdeps\Spooder\Fuzzy_Spooder```) to your 
  - project folder (ex: ```platformio_project_folder\Spooder```).
2. Edit the config.json file in the data folder as required. 
3. Use the ```PlatformIO->PROJECT TASKS->Spooder->Platform->Upload Filesystem Image``` command to uplod the file image. Make SURE you have the ```board_build.filesystem = littlefs``` configurtion setting in the platformio.ini file.
</details>


### OTA Upload
Once the OTA (over-the-air) codes are in place, and the device is connected to the local WiFi enviromnet, the ESP can be uploaded wirelessly.

<details>
<summary>#### Using Arduino IDE</summary>

Follow the IDE upload part in this [tutorial](https://randomnerdtutorials.com/esp8266-ota-updates-with-arduino-ide-over-the-air/) to upload the sketch wirelessly.
</details>

<details>
<summary>#### Using VSCode IDE + platformio extension</summary>

Add two lines in your platformio.ini:
- upload_protocol = espota
- upload_port = spooderA1.local

Creating a [env] is recommended. The whole section would look like this:

```
[env:Spooder Example - OTA]
platform = espressif8266
board = nodemcuv2
framework = arduino
board_build.f_cpu = 160000000L
board_build.filesystem = littlefs
board_build.ldscript = eagle.flash.4m1m.ld
build_flags = -w
monitor_speed = 115200
upload_speed = 921600
monitor_filters = send_on_enter
upload_protocol = espota
;upload_port = 192.168.0.133  ;use ip address if the mDNS hostname is not resolved
upload_port = spooderA1.local ;using mDNS, replace with actual name
lib_deps = georgychen/Fuzzy Spooder@^1.0.0
```

After setting the OTA environment in the platformio.ini file, select the intended project environment on the bottom toolbar. Then, the upload command will conduct an OTA upload. If the mDNS hostname is not resolved, you can use the IP address.
</details>
---

## Using the Spooder

### UI Pages

- **Spooder Home Page**:
  - Single click: Cycles the display mode:
    - Filament weight: an estimated weight (total - spool holder weight). Displays "Empty" when the value is below a negative value.
    - Spool holder weight: an user input value, adjustable in sketch.
    - Total weight: measured (spool holder + filament) weight.
- **Info Page**:
  - Displays additional information
- **Menu Page**: 
  - Tare: Perform a tare manually. The current tare offset value is saved into EEPROM. A manual tare needs to bo done at least once with the spool removed.
  - Calibrate: Perform a calibration. Calibrated value is saved into EEPROM. A calibration needs to be done at least once with known weight.
  - Spool Holder Weight: Set spool holder weight, or load preset values. This is used for the filament weight estimation.
  - Set Spooder ID: Set the spooder ID. An unique Spooder ID needs to be set for each spooder.
  - Low Filament Setup: Adjust the threshold value for the low filament notification. Default value is 80 g.
  - Notification: User option to enable or disable each notification type. 
  - Firmware Update: Update firmware from github repository directly.
  - Options: User enable/disable the following options:
    - WiFi
    - mDNS
    - Blynk
    - Web server
    - Spooder client
    - Spooder server
    - Arduino OTA
    - Auto Homepage 
  - Debug: Various debugging functions.

#### Spool Holder Weight

Spool holder weight is a user input value in grams. This weight is used to estimate remaining filament weight. The default spool holder weight can be 
- Adjusted in the spooder UI. 
- Loaded from preset values.

There are additional slots (up to 32 maximum) of preset spool holders, each with its name and weight. They are defined in the `\data\config.json` file. These preset spool holders can be selected in the spooder UI.

#### Network Functions
To enable the network functions, user needs to provide wifi ssid/password, and Blynk Authorization Token in the config.json file. Install the Blynk app on your tab/phone. The browser file manager works locally, but the Blynk Notification works globally. Which means, your phone doesn't have to be in your local wifi network to receive notifications.

#### Spooder ID and mDNS

The user needs to set an unique Spooder ID in the UI for each unit. The ID consists of a letter (from A to Z) and a number (from 1 to 99). For example, A1, B13, C3, etc... 

The unit's [mDNS](https://en.wikipedia.org/wiki/Multicast_DNS) hostname is prefixed with "spooder". For example, spooderA1, spooderB13, spooderC3, etc... The spooder can be accessed simply using "**spooderA1.local**", "**spooderB13.local**", etc. Note that the mDNS hostname is case-insensitive, which means "**SPOODERA1.LOCAL**" also works. If the mDNS isn't availble, IP address can always be used.

#### Notification

There are several conditions that will trigger the notification:

- **Print job started**: When the filament is constantly being pulled by the extruder. There will be some ~30 seconds lag for the detection.
- **Print job completed**: When the weight measured is being at rest after print job started.
- **Low filament warning**: When the remaining filament weight is less than a preset value. (default value is 50 g). This threshold can be adjusted in the menu. This notification can only be triggered once during each print.
- **Fall off rack warning**: When the detected weight is less than spool holder weight (minus a threshold) during printing. This notification can only be triggered once during each print, and inhibits low filament warning.

User has the option to enable or disable each notification type in the UI **Notification** page. These options are stored in EEPROM and enabled by default.

The nofification is sent through the [Blynk App](https://docs.blynk.cc/).

#### Firmware Update from Github

Each spooder can update its own firmware over-the-air from this [github repository](https://github.com/FuzzyNoodle/Fuzzy-Spooder) manually or automatically, provided that the device is connected to the internet through WiFi. Current onboard version and latest version on github are displayed on the **Firmware Update** page. 

Manual **Check Now**, and manual **Update Now** can be performed anytime. The version check would normally take around 10~15 seconds.

If the **Auto Update** option is set to **On**, the device will check the latest github release periodically, and will update the firmware if there is a newer version.  Due to the github api [60 requests per hour rate limit](https://docs.github.com/en/rest/overview/resources-in-the-rest-api#rate-limiting), the interval between automatic version checks is set between 60 to 120 minutes. Automatic update is inhibited for 10 minutes if any filament movements are detected. The next check interval is also extended right after a successful automatic firmware update.


#### Browser File Manager

After configuring wifi, the spooder file system can be accessed using a web browser. Type for example, http://spooderA1.local/edit in the URL of your browser. This function is directly imported from the excellent [FSBrowser example](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/FSBrowser) by Hristo Gochkov.

Files can be uploaded, downloaded, or edited (for example, config.json) directly from the web browser.

---

### Additional Information

#### Bonjour Browser

The following apps can be used to browse active **spooders** (and other mDNS devices) in your local network. 
- [Bonjour Browser](https://play.google.com/store/apps/details?id=de.wellenvogel.bonjourbrowser) for Android
- [Bonjour Browser](https://hobbyistsoftware.com/bonjourbrowser) for Windows
- [Discovery - DNS-SD Browser](https://apps.apple.com/us/app/discovery-dns-sd-browser/id1381004916?mt=12) for macOS
- [Discovery - DNS-SD Browser](https://apps.apple.com/us/app/discovery-dns-sd-browser/id305441017) for iOS


#### Installing Blynk App for Push Notification 
(free for limited Widget usage)
1. Install "Blynk (legacy)" App (iOS or Android)
2. Register a Blynk account
3. New Project 
  - Name: ex. "Spooder"
  - Device: "ESP8266"
  - Connection Type: "WiFi"
4. An unique Authorization Token (per project) will be sent to your registered email. The code needs to be copied into the config.json under \data folder.   
5. Touch the design screen, a Widgex Box appear. Place a "Notification $400" widget.
6. Press "Play" icon on the top right corner. Done. App doesn't need to be active for the notification to work.

[Limitations:](https://github.com/blynkkk/blynkkk.github.io/blob/master/Widgets.md)

- Maximum allowed body length is 120 symbols;
- Every device can send only 1 notification every 5 seconds;


