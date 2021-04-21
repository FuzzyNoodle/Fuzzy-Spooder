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
3. Download the [zip file](https://github.com/FuzzyNoodle/Fuzzy-Spooder/archive/refs/tags/v0.1.0.zip) and [install](https://www.arduino.cc/en/Guide/Libraries) the [Fuzzy-Spooder](https://github.com/FuzzyNoodle/Fuzzy-Spooder) library.
4. Individually install the following library dependencies using the the [Arduino Library Manager](https://www.arduino.cc/en/guide/libraries#toc3):
    1. [ESP Rotary](https://github.com/LennartHennigs/ESPRotary) by Lennart Hennigs, version 1.4.2
    2. [Button2](https://github.com/LennartHennigs/Button2) by Lennart Hennigs, version 1.6.1
    3. [Blynk](https://github.com/blynkkk/blynk-library) by Volodymyr Shymanskyy, version 0.6.1
    4. [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://github.com/ThingPulse/esp8266-oled-ssd1306) by ThingPulse, Fabrice Weinberg, version 4.2.0
    5. [HX711_ADC](https://github.com/olkal/HX711_ADC) by Oolav Kallhovd, version 1.2.7
5. Open the File->Examples->Fuzzy Spooder->NoWifi_Demo sketch and upload to your board via USB connection. Correct port needs to be selected.


#### Using VSCode IDE + platformio extension

1. PIO Home-> New Project
    1. Name: **NoWifi_Demo** for example
    2. Board: Search "mcu" select **NodeMCU 1.0 (ESP-12E Module)**
    3. Framework: **Arduino**
2. PIO Home->Libraries->Search and go to **"Fuzzy Spooder"**.
3. Add to Project -> Select the project you just created. (ex. **Projects\NoWifi_Demo**). The library dependencies should be automataclly downloaded and installed.
4. On the same page, select and copy all the sketch code from the NoWifi_Demo
5. Open VSCode->Explorer(Left/Top Icon)->[Your Project Name]->src->**main.cpp**. Paste and overwrite the example sketch code copied from previous step.
6. Upload the program(The right arrow located at the bottom toolbar) via USB connection. Upload port should be auto-detected.

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
This creates a more user friendly env:name, increases upload speed and enables serial debug.


### Using the NoWifi_Demo Example
An automatic tare is initiated upon power on. Tare should be done with the printed rack, but without spool holder/filament. 

A calibration needs to be done at least once with known weight.

There are three main pages:
- **Spooder Home Page**:
  - Single click: Cycles the display mode:
    - Total weight: measured (spool holder + filament) weight.
    - Filament weight: an estimated weight (total - spool holder weight)
    - Spool holder weight: an user input value. Fixed at 180g at this moment.
  - Long click: Perform a tare. 
- **Info Page**:
  - Displays additional information
- **Menu Page**: Rotate and click the menu items. Currently available menu items are:
  - Tare: Perform a tare.
  - Calibrate: Perform a calibration. Calibrated Value will be saved to EEPROM.
  - Display EEPROM: Dumps eeprom data to serial monitor for debugging.


