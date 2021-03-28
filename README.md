## Important Notice
This library is work in progress, not ready for usage. It is registered in PlatformIO library manger for development purpose only.

# Fuzzy-Spooder
An add-on filament autochanger for existing 3D printers, in duel-spool configuration.

## Development Stages and Features
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

---
## Usage

## Library Installation
