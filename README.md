## Important Notice
This library is work in progress, not ready for usage. It is registered in PlatformIO library manger for development purpose only.

# Fuzzy-Spooder
An add-on filament autochanger for existing 3D printers, in duel-spool configuration.

---
## Getting Started

### Upload the NoWifi_Demo Example

#### Using Arduino IDE

1. [Install](https://arduino-esp8266.readthedocs.io/en/latest/installing.html) the ESP8266 Arduino Core.
1. Arduino IDE->Tools->Board->ESP8266 Boards, select NodeMCU 1.0 (ESP-12 Module). You may change the **Upload Speed** to higher baud rate. 
1. Download and [install](https://www.arduino.cc/en/Guide/Libraries) the [Fuzzy-Spooder](https://github.com/FuzzyNoodle/Fuzzy-Spooder) library.
1. Individually install the library dependencies using the the Arduino [Library Manager]:
  1. [ESP Rotary](https://github.com/LennartHennigs/ESPRotary) by Lennart Hennigs, version 1.4.2
  1. [Button2](https://github.com/LennartHennigs/Button2) by Lennart Hennigs, version 1.6.1
  1. [Blynk](https://github.com/blynkkk/blynk-library) by Volodymyr Shymanskyy, version 0.6.1
  1. [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://github.com/ThingPulse/esp8266-oled-ssd1306) by ThingPulse, Fabrice Weinberg, version 4.2.0
  1. [HX711_ADC](https://github.com/olkal/HX711_ADC) by Oolav Kallhovd, version 1.2.7
5. Open the File->Examples->Fuzzy Spooder->NoWifi_Demo sketch and upload to your board via USB connection.
6. Calibrate the sensors for the first time.

#### Using VSCode IDE + platformio extension

### Using the demo sketch

### Menu and Functions

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
