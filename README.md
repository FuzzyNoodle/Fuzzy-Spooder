## Important Notice
This library is work in progress, not ready for general usage. It is registered in PlatformIO library manger for development purpose only.

# Fuzzy-Spooder
An add-on filament autochanger for existing 3D printers, in duel-spool configuration.

---
## Getting Started

### Upload the NoWifi_Demo Example

#### Using Arduino IDE

1. [Install](https://arduino-esp8266.readthedocs.io/en/latest/installing.html) the ESP8266 Arduino Core.
2. Arduino IDE->Tools->Board->ESP8266 Boards, select NodeMCU 1.0 (ESP-12 Module). You may change the **Upload Speed** to higher baud rate. 
3. Download and [install](https://www.arduino.cc/en/Guide/Libraries) the [Fuzzy-Spooder](https://github.com/FuzzyNoodle/Fuzzy-Spooder) library.
4. Individually install the following library dependencies using the the Arduino [Library Manager](https://www.arduino.cc/en/guide/libraries#toc3):
    1. [ESP Rotary](https://github.com/LennartHennigs/ESPRotary) by Lennart Hennigs, version 1.4.2
    2. [Button2](https://github.com/LennartHennigs/Button2) by Lennart Hennigs, version 1.6.1
    3. [Blynk](https://github.com/blynkkk/blynk-library) by Volodymyr Shymanskyy, version 0.6.1
    4. [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://github.com/ThingPulse/esp8266-oled-ssd1306) by ThingPulse, Fabrice Weinberg, version 4.2.0
    5. [HX711_ADC](https://github.com/olkal/HX711_ADC) by Oolav Kallhovd, version 1.2.7
5. Open the File->Examples->Fuzzy Spooder->NoWifi_Demo sketch and upload to your board via USB connection.
6. Calibrate the sensors for the first time.

#### Using VSCode IDE + platformio extension

1. PIO Home-> New Project
    1. Name: **NoWifi_Demo** for example
    2. Board: Search "mcu" select **NodeMCU 1.0 (ESP-12E Module)**
    3. Framework: **Arduino**
2. PIO Home->Libraries->Search and goto "Fuzzy Spooder".
3. Add to Project -> Select the project you just created. (ex. Projects\NoWifi_Demo). The library dependencies should be automataclly downloaded and installed.
4. On the same page, select and copy all the sketch code from the NoWifi_Demo
5. Open VSCode->Explorer(Left/Top Icon)-><Your Project Name>->src->main.cpp. Paste and overwrite the example sketch code copied from previous step.
6. Upload the program(The right arrow located at the bottom toolbar).

Additional note:
The default platformio.ini configuration would be something like:

```
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps = georgychen/Fuzzy Spooder@^0.1.2
```

Modify the configuration section to:
```
[env:NoWifi_Demo Example - USB Serial]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200
upload_speed = 921600
monitor_filters = send_on_enter
lib_deps = georgychen/Fuzzy Spooder@^0.1.2

```
This increases upload speed and enables serial debug.


### Using the demo sketch

### Menu and Functions

- Spooder Home Page
- Info Page
- Menu Page

## Library Dependencies


### Using VSCode IDE + platformio extension
Specify the following dependencies in the **platformio.ini** configuration file. [PlatformIO Library Manager](https://docs.platformio.org/en/feature-test-docs/librarymanager/index.html) will automatically download and install the required libraries during compilation.
```
lib_deps = 
	lennarthennigs/ESP Rotary@^1.4.2
	lennarthennigs/Button2@^1.6.0
	blynkkk/Blynk@^0.6.7
	thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays@^4.2.0
  olkal/HX711_ADC@^1.2.7
```


## Development Roadmap
### Stage 1: Filament Weight Estimator
- A stand-alone filament weight estimator
- Displays estimated filament remaining weight
- Filters weight variation during printing
- Includes simple UI, using
  - Rotory-encoder with push button
  - 0.96 inch monochrome OLED
- May provide filament status/low warning to user through web page hosting
- May provide indication when the print job is done, throught non-existant of filament weight variation. 

### Stage 2: Multiple Weight Estimators + Controller
- Controller provides
  - Remaining filament weight for every units
  - Provides alarm when the filament is used below preset limit
- Controller can be
  - Central-less.  Information stored in every unit. User access info using browser.
  - A ESP device with HMI screen.
  - Hosted IOT web service. Accessed using browser.
  - Locally installed server. Accessed using browser.
### Stage 3: Filament Autochanger
- Two filament units in a pair
- Auto-change filament, then provide indications to user
