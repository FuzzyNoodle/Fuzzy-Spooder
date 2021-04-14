## Important Notice
This library is work in progress, not ready for usage. It is registered in PlatformIO library manger for development purpose only.

# Fuzzy-Spooder
An add-on filament autochanger for existing 3D printers, in duel-spool configuration.

---
## Usage

## Library Installation
### Using Arduino IDE
The following libraries need to be installed, preferrable using the Arduino [Library Manager](https://www.arduino.cc/en/guide/libraries)
- [ESP Rotary](https://github.com/LennartHennigs/ESPRotary) by Lennart Hennigs, version 1.4.2
- [Button2](https://github.com/LennartHennigs/Button2) by Lennart Hennigs, version 1.6.1
- [Blynk](https://github.com/blynkkk/blynk-library) by Volodymyr Shymanskyy, version 0.6.1
- [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://github.com/ThingPulse/esp8266-oled-ssd1306) by ThingPulse, Fabrice Weinberg, version 4.2.0
- [HX711_ADC](https://github.com/olkal/HX711_ADC) by Oolav Kallhovd, version 1.2.7

### Using VSCode + platformio
Specify the following dependencies in the **platformio.ini** configuration file. [PlatformIO Library Manager](https://docs.platformio.org/en/feature-test-docs/librarymanager/index.html) will automatically download and install the required libraries.
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
- May provide filament status/low warning to user throught web page hosting
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
