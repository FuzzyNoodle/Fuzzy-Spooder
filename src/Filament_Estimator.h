/*!
 *  @file Filament_Estimator.h
 *
 *  
 *
 *  
 */

#ifndef FILAMENT_ESTIMATOR_H
#define FILAMENT_ESTIMATOR_H

//#define ENABLE_BLYNK

//#version for this firmware
#define CURRENT_VERSION_MAJOR 0
#define CURRENT_VERSION_MINOR 3
#define CURRENT_VERSION_PATCH 0

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include "ESPRotary.h"
#include "Button2.h"

#define ROTARY_PIN_CLK D3
#define ROTARY_PIN_DT D4
#define DEFAULT_STEPS_PER_CLICK 4
#define BUTTON_PIN D0

//Include the SSD1306 display library for esp8266
#include "SSD1306Wire.h"
#define SSD1306_SDA_PIN D2
#define SSD1306_SCL_PIN D1
#define SSD1306_ADDRESS 0x3c

//Include the HX711 library
#include "HX711_ADC.h"
#include <EEPROM.h>
#define HX711_DOUT_PIN D5 //mcu > HX711 dout pin
#define HX711_SCK_PIN D6  //mcu > HX711 sck pin

//File system
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>

#define JSON_DOC_BUFFER_SIZE 1024
#define SPOOL_HOLDER_MAX_SLOT_SIZE 32

#ifndef DISABLE_SERIAL_DEBUG
#define ENABLE_SERIAL_DEBUG
#endif

#define DEFAULT_SPOOL_HOLDER_WEIGHT 180
#define DEFAULT_TOTAL_WEIGHT 1180
#define DEFAULT_EMPTY_THRESHOLD -30
#define DEFAULT_CALIBRATION_VALUE 250.0

#define WIFI_STATUS_DISABLED 0
#define WIFI_STATUS_BOOT 1
#define WIFI_STATUS_CONNECTING 2
#define WIFI_STATUS_CONNECTED 3
#define PAGE_NONE 0
#define PAGE_HOME 11
#define PAGE_INFO 12
#define PAGE_MENU 15

#define PAGE_TARE 20
#define TARE_OK 0
#define TARE_CANCEL 1

#define PAGE_CALIBRATE 30
#define PAGE_CALIBRATE_CONFIRM 31
#define PAGE_DEBUG 101

#define PAGE_SPOOL_HOLDER_WEIGHT 40
#define SPOOL_HOLDER_3_DIGIT 0
#define SPOOL_HOLDER_2_DIGIT 1
#define SPOOL_HOLDER_1_DIGIT 2
#define SPOOL_HOLDER_OK 3
#define SPOOL_HOLDER_CANCEL 4
#define SPOOL_HOLDER_SLOT_1 5
#define SPOOL_HOLDER_SLOT_2 6
#define SPOOL_HOLDER_SLOT_3 7
#define SPOOL_HOLDER_SLOT_4 8
#define SPOOL_HOLDER_SLOT_5 9

#define CALIBRATE_4_DIGIT 0
#define CALIBRATE_3_DIGIT 1
#define CALIBRATE_2_DIGIT 2
#define CALIBRATE_1_DIGIT 3
#define CALIBRATE_OK 4
#define CALIBRATE_CANCEL 5
#define CALIBRATE_SAVE_OK 0
#define CALIBRATE_SAVE_CANCEL 1

#define ADJUST_WEIGHT_PAGE 40

#define MENU_TARE 0
#define MENU_CALIBRATE 1
#define MENU_SPOOL_HOLDER_WEIGHT 2
#define MENU_SETUP 3
#define MENU_DEBUG 8
#define DEBUG_LOAD_TO_SETTING 0
#define DEBUG_SAVE_TO_EEPROM 1
#define DEBUG_DUMP_SETTING 2
#define DEBUG_DUMP_EEPROM 3
#define DEBUG_ERASE_EEPROM 4
#define DEBUG_LIST_DIRECTORY 5
#define DEBUG_LOAD_CONFIG 6
#define DEBUG_DUMP_CONFIG 7
#define DEBUG_REBOOT 8
#define DEBUG_RETURN 9

#define DECLARED_EEPROM_SIZE 1024
#define EEPROM_START_ADDRESS 0

#define DISPLAY_TYPE_TOTAL 0
#define DISPLAY_TYPE_FILAMENT 1
#define DISPLAY_TYPE_SPOOL_HOLDER 2

class FILAMENT_ESTIMATOR
{
public:
  FILAMENT_ESTIMATOR(void);
  void begin(void);
  void begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token);
  void update(void);

  //Optional: enable wifi function
  void setWifi(bool wifi);

  void buttonHandler(Button2 &btn);
  void rotaryHandler(ESPRotary &rty);

  //Set the default total weight in grams for calibration after power on
  //Valid values from 0 to 9999
  void setCalibrationWeight(uint16_t weight);

  //Set the default weight in grams for the spool holder. Can be changed in the UI.
  //Valid values from 0 to 999
  //Value is also saved to EEPROM
  void setCurrentSpoolHolderWeight(uint16_t weight);

  //Set the steps per click of the rotary encoder
  void setStepsPerClick(uint8_t steps);

  //Get the steps per click of the rotary encoder
  uint8_t getStepsPerClick();

  //Time settings for button
  void setDebounceTime(uint16_t ms);
  //Time settings for button
  void setLongClickTime(uint16_t ms);
  //Time settings for button
  void setDoubleClickTime(uint16_t ms);
  //Time settings for button
  uint16_t getDebounceTime();
  //Time settings for button
  uint16_t getLongClickTime();
  //Time settings for button
  uint16_t getDoubleClickTime();

private:
  ESP8266WebServer server;
  Button2 button;
  ESPRotary rotary;
  SSD1306Wire display;

  struct VERSION_STRUCT
  {
    uint8_t major; //incompatible API changes
    uint8_t minor; //add functionality in a backwards compatible manner
    uint8_t patch; //make backwards compatible bug fixes
  };
  VERSION_STRUCT currentVersion;

  //struct used to interact with eeprom, without the hasstle of address and lengths
  struct EEPROM_SETTING_STRUCT
  {
    VERSION_STRUCT version;
    float calValue;
    uint16_t spoolHolderWeight;

  } setting;

  //wifi related
  bool enableWifi = false;
  uint8_t wifiStatus = WIFI_STATUS_BOOT;
  void updateWifi();
  void connectWifi();
  void beginServices();

  uint8_t currentPage = PAGE_NONE;
  uint8_t previousPage = PAGE_NONE;
  bool setPage(uint8_t page); //return true if page changed, false in page not changed
  void displayPage(uint8_t page);
  void drawBottomIndicator(uint8_t index);
  void drawRightIndicator(uint8_t index);
  uint8_t menuIndex = 0;
  void drawLeftIndicator(uint8_t index);
  uint8_t menuItemStartIndex = 0;
  const uint8_t menuItemPerPage = 5;
  const uint8_t numberOfMenuItems = 9;
  String menuTitle[9] = {"Tare",
                         "Calibrate",
                         "Spool Holder Weight",
                         "Set Spooder ID(X)",
                         "WIFI Setup(X)",
                         "Notification(X)",
                         "Spood Holder(X)",
                         "Instruction(X)",
                         "Debug"};
  uint8_t tareSelection = TARE_OK;
  void drawTriangle(uint8_t x, uint8_t y);
  void tare();
  void calibrate();

  uint8_t stepsPerClick = DEFAULT_STEPS_PER_CLICK;

  HX711_ADC loadcell;
  //float calibrateValue; //a default value required for next calibration
  float newCalibrationValue;
  // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  uint32_t stabilizingTime = 2000;
  bool newDataReady = false;
  float totalWeight;
  uint32_t updateHomepageTimer;
  //The period to refresh homepage (weight)
  const uint32_t UPDATE_HOMEPAGE_PERIOD = 200;
  void updateHomepage();
  void drawOverlay();
  bool drawTareMessage = false;
  uint32_t drawTareMessageTimer;
  const uint32_t DRAW_TARE_MESSAGE_PERIOD = 1000;
  void checkTareTimer();

  uint8_t displayType = DISPLAY_TYPE_FILAMENT;
  float filamentWeight;
  float emptyThreshold = DEFAULT_EMPTY_THRESHOLD;

  uint8_t calibrateSelection = CALIBRATE_4_DIGIT;
  bool calibrateEditDigitMode = false;
  uint8_t calibrate4Digit = 0;
  uint8_t calibrate3Digit = 0;
  uint8_t calibrate2Digit = 0;
  uint8_t calibrate1Digit = 0;
  uint16_t getCalibrationWeight(); //get the weight from 4-digit UI menu
  uint16_t calibrationWeight = 0;
  void checkCalibrateEditModeTimer();
  bool displayCalibrateDigit = true;
  uint32_t calibrateEditModerTimer;
  const uint32_t CALIBRATE_EDIT_MODE_PERIOD = 500;
  uint8_t calibrateSaveSelection = CALIBRATE_SAVE_OK;

  uint8_t debugMenuSelection = DEBUG_LOAD_TO_SETTING;
  uint8_t debugMenuItemStartIndex = 0;
  uint8_t debugMenuItemPerPage = 5;
  uint8_t numberOfDebugMenuItems = 10;
  String debugMenuTitle[10] = {
      "Load To Setting",
      "Save To EEPROM",
      "Dump Setting",
      "Dump EEPROM",
      "Erase EEPROM",
      "List Directory",
      "Load Config",
      "Dump Config",
      "Reboot",
      "Return to Menu"};
  void loadToSetting();
  void saveToEEPROM();
  void dumpSetting();
  void dumpEEPROM();
  void eraseEEPROM();
  uint32_t versionToNumber(VERSION_STRUCT v);

  void listDir(const char *dirname); //List the FSn directory in a user-friendly text format
  void _listDir(const char *dirname, uint8_t level);

  void displayMonoBitmap(const char *filename);
  uint32_t read32(File f, uint32_t offset);

  float spoolHolderWeight = DEFAULT_SPOOL_HOLDER_WEIGHT;
  uint8_t spoolHolderSelection = SPOOL_HOLDER_3_DIGIT;
  bool spoolHolderEditDigitMode = false;
  bool displaySpoolHolderDigit = true;
  uint32_t spoolHolderEditModerTimer;
  uint8_t spoolHolder3Digit = 1;
  uint8_t spoolHolder2Digit = 8;
  uint8_t spoolHolder1Digit = 0;
  const uint32_t SPOOL_HOLDER_EDIT_MODE_PERIOD = 500;
  void checkSpoolHolderEditModeTimer();
  uint16_t getSpoolHolderWeight(); //get the weight from 3-digit UI menu
  bool noSpoolHolderWeightInEEPROM = false;

  // Allocate a buffer to store contents of the file.
  //char configBuffer[CONFIG_FILE_BUFFER_SIZE];
  uint16_t configSize;
  void loadConfig(); //load the config file into buffer
  void dumpConfig(); //Pring all the config.json content for debugging purpose
  StaticJsonDocument<JSON_DOC_BUFFER_SIZE> jsonDoc;
  const char *config_version; // "0.3.0"
  const char *wifi_ssid;      // "your_ssid"
  const char *wifi_password;  // "your_password"
  const char *blynk_auth;
  const char *spoolHolderSlotName[SPOOL_HOLDER_MAX_SLOT_SIZE];
  uint16_t spoolHolderSlotWeight[SPOOL_HOLDER_MAX_SLOT_SIZE];
  uint8_t spoolHolderSlotSize = 0;
  uint8_t spoolHolderSlotStartIndex = SPOOL_HOLDER_SLOT_1;
};

#endif //#ifndef FILAMENT_ESTIMATOR_H