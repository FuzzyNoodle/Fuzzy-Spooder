/*!
 *  @file Filament_Estimator.h
 *
 *
 *
 *
 */

#ifndef FILAMENT_ESTIMATOR_H
#define FILAMENT_ESTIMATOR_H

// Version for this firmware
#define CURRENT_VERSION "1.1.0"

#include "Arduino.h"
#include <stdio.h>
#include <ESP8266WebServer.h>

// Debug switches
#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints
#define BLYNK_NO_FANCY_LOGO

// Rotary library
#include "ESPRotary.h"
#include "Button2.h"
#define ROTARY_PIN_CLK D3
#define ROTARY_PIN_DT D4
#define DEFAULT_STEPS_PER_CLICK 4
#define BUTTON_PIN D0
#define LONG_CLICK_TIME 400

// Include the SSD1306 display library for esp8266
#include "SSD1306Wire.h"
#define SSD1306_SDA_PIN D2
#define SSD1306_SCL_PIN D1
#define SSD1306_ADDRESS 0x3c

// Include the HX711 library
#include "HX711_ADC.h"
#include <EEPROM.h>
//#include <ESP_EEPROM.h>   //use this library to extend life
#define HX711_DOUT_PIN D5 // mcu > HX711 dout pin
#define HX711_SCK_PIN D6  // mcu > HX711 sck pin

// Network time function
#include <TZ.h>
#define MYTZ TZ_Asia_Taipei
//#define MYTZ TZ_America_Los_Angeles

// File system
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include <WiFiClient.h>

// ArduinoOTA
#define NO_GLOBAL_ARDUINOOTA
#include <ArduinoOTA.h>

// Blynk
#define JSON_DOC_BUFFER_SIZE 1024
#define SPOOL_HOLDER_MAX_SLOT_SIZE 32

#ifndef DISABLE_SERIAL_DEBUG
#define ENABLE_SERIAL_DEBUG
#endif

// Detection
#include <math.h>
#define DETECTION_SAMPLE_SIZE 30

#define DEFAULT_SPOOL_HOLDER_WEIGHT 255
#define DEFAULT_CALIBRATION_WEIGHT 1284
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

#define PAGE_SET_SPOODER_ID 50
#define SET_SPOODER_ID_LETTER 0
#define SET_SPOODER_ID_NUMBER 1
#define SET_SPOODER_ID_OK 2
#define SET_SPOODER_ID_CANCEL 3
#define SPOODER_ID_USER_SET 0x65
#define SPOODER_ID_SYSTEM_SET 0x67

#define PAGE_LOW_FILAMENT_SETUP 60
#define LOW_FILAMENT_4_DIGIT 0
#define LOW_FILAMENT_3_DIGIT 1
#define LOW_FILAMENT_2_DIGIT 2
#define LOW_FILAMENT_1_DIGIT 3
#define LOW_FILAMENT_OK 4
#define LOW_FILAMENT_CANCEL 5
#define DEFAULT_LOW_FILAMENT_THRESHOLD_VALUE 80

#define PAGE_NOTIFICATION 70
#define NOTIFICATION_MENU_PRINT_STARTED 0
#define NOTIFICATION_MENU_PRINT_COMPLETED 1
#define NOTIFICATION_MENU_LOW_FILAMENTT 2
#define NOTIFICATION_MENU_FALL_OFF_RACK 3
#define NOTIFICATION_MENU_FALL_OFF_BEARING 4
#define NOTIFICATION_MENU_TANGLED 5
#define NOTIFICATION_MENU_RETURN 6

#define PAGE_FIRMWARE_UPDATE 80

#define PAGE_OPTIONS 90

#define PAGE_ENTER_HOTSPOT_MODE 100

#define DECLARED_EEPROM_SIZE 64
#define EEPROM_START_ADDRESS 0

#define DISPLAY_TYPE_TOTAL 0
#define DISPLAY_TYPE_FILAMENT 1
#define DISPLAY_TYPE_SPOOL_HOLDER 2

#define CONNECTION_STATUS_NONE 0
#define CONNECTION_STATUS_NO_WIFI 1
#define CONNECTION_STATUS_WIFI_AND_INTERNET 2
#define CONNECTION_STATUS_WIFI_NO_INTERNET 3
#define SYMBOL_NONE 0
#define SYMBOL_NO_WIFI 1
#define SYMBOL_WIFI_AND_INTERNET 2
#define SYMBOL_WIFI_NO_INTERNET_1 3
#define SYMBOL_WIFI_NO_INTERNET_2 4

// mDNS and communication between devices
#define MDNS_SERVICE_TXT_MAXLENGTH 100
#include <ESP8266mDNS.h>
#define MAX_NUM_OF_SPOODERS 64       // fix RAM is allocated
#define MAX_NUM_OF_SPOODER_SERVERS 3 // in a local network. more than 20 causes OOM using dynamic mDNS
#define DATASET_SPOODER_ID_SIZE 4
// Has to be global due to sort callback
struct SPOODERS_DATASET_STRUCT
{
  char spooderID[DATASET_SPOODER_ID_SIZE]; // A1, B12, ...
  int16_t fw;                              // filament weight in gram //2 bytes
};
//#include <WiFiUdp.h>
//#define UDP_PACKET_SIZE_LIMIT 32
#include <ArduinoWebsockets.h>
using namespace websockets;

// Github auto update
#define NO_GLOBAL_HTTPUPDATE // use local instance to free up RAM
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
typedef struct urlDetails_t
{
  String proto;
  String host;
  int port;
  String path;
};

// Hotspot mode for initial setup
#include <DNSServer.h>

class FILAMENT_ESTIMATOR
{
public:
  FILAMENT_ESTIMATOR(void);
  void begin(void);
  void begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token);
  void update(void);

  // Optional: enable wifi function
  void setWifi(bool value);

  void buttonHandler(Button2 &btn);
  void rotaryHandler(ESPRotary &rty);

  // made public due to using callback function in class, requires an outside-class function
  void handleStatus();     // Return the FS type, status and size info
  void handleFileList();   // Return the list of files in the directory specified by the "dir" query string parameter. Also demonstrates the use of chuncked responses.
  void handleGetEdit();    // This specific handler returns the index.htm from the /edit folder.
  void handleFileCreate(); // Handle the creation/rename of a new file
  void handleFileDelete(); // Handle a file deletion request
  void handleFileUpload(); // Handle a file upload request
  void handleNotFound();   // The "Not Found" handler
  void replyOK();
  void handleSetupSave(); // Handle the setup save button action

  void MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo *serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent);
  void handleWebsocketsMessage(WebsocketsClient &client, WebsocketsMessage message);
  void handleWebsocketsEvent(WebsocketsClient &client, WebsocketsEvent event, String data);

private:
  // ESP8266WebServer server;
  Button2 button;
  ESPRotary rotary;
  SSD1306Wire display;

  struct VERSION_STRUCT
  {
    uint8_t major; // incompatible API changes
    uint8_t minor; // add functionality in a backwards compatible manner
    uint8_t patch; // make backwards compatible bug fixes
  };
  VERSION_STRUCT currentVersion;
  VERSION_STRUCT githubLatestReleaseVersion;

  // struct used to interact with eeprom, without the hasstle of address and lengths
  struct EEPROM_SETTING_STRUCT
  {
    VERSION_STRUCT version;
    float calValue;
    uint16_t spoolHolderWeight;
    byte spooderIDSetStatus;
    uint8_t spooderIDLetter; // 1=A, 2=B, ... 26=Z
    uint8_t spooderIDNumber; // 1 - 99
    uint16_t lowFilamentThreshold;
    uint8_t notifyOnPrintStarted;
    uint8_t notifyOnPrintCompleted;
    uint8_t notifyOnLowFilament;
    uint8_t notifyOnFallOffRack;
    uint8_t notifyOnFallOffBearing;
    uint8_t notifyOnTangled;
    uint8_t optionsWiFi;
    uint8_t optionsMDNS;
    uint8_t optionsBLYNK;
    uint8_t optionsWebServer;
    uint8_t optionsArduinoOTA;
    uint8_t autoGithubUpdate;
    long tareOffset; // using 32 bytes here
    uint16_t calibrationWeight;
    uint8_t calibrationWeightIsValid;
    uint8_t automaticUpdateJustPerformed;
    uint8_t optionsAutoHomepage;
    uint8_t optionsSpooderClient;
    uint8_t optionsSpooderServer;

  } setting;

  // Network related
  bool enableWifi = false;
  uint8_t wifiStatus = WIFI_STATUS_BOOT;
  bool enableBlynk = false;
  bool enableWebServer = false;
  bool enableSpooderClient = false;
  bool enableSpooderServer = false;
  bool enableArduinoOTA = false;
  bool enableNetworkTime = true;
  void updateWifi();
  void connectWifi();
  void beginServices();
  void setMDNS(bool value);
  void setBlynk(bool value);
  void setWebServer(bool value);
  void setSpooderClient(bool value);
  void setSpooderServer(bool value);
  void setArduinoOTA(bool value);
  void setAutoHomepage(bool value);
  ArduinoOTAClass *ArduinoOTA; // use pointer to creat/delete object in code
  ESP8266WebServer *webServer; // use pointer to creat/delete object in code
  void installDynamicServiceQuery();
  void removeDynamicServiceQuery();
  void listSpooderService();
  // Define how many clients we accpet simultaneously.
  const uint8_t maxClients = 20;
  const uint16_t websocketsPort = 8266;                  // Has to be different to webserver port
  WebsocketsServer webSocketsServer;                     // used in spooder server
  WebsocketsClient webSocketsClient;                     // used in spooder client
  std::vector<WebsocketsClient> webSocketsClientsVector; // Vector to store all the client connections in spooder server

  struct WEBSOCKETS_PACKET
  {
    uint8_t num;
    char str[10];
    float weight;
  };

  union WEBSOCKETS_HOLDER
  {
    WEBSOCKETS_PACKET p;
    char str[10];
  };

  struct WEBSOCKETS_PACKET *packetAddress;

  void updateWebsocketsServer();
  void updateWebsocketsClient();
  void spooderServerListenForWebsocketsClients();
  void spooderServerPollWebsocketsClients();
  int8_t getFreeClientIndex();
  bool isAnswerValidServer(uint8_t index);
  void connectSpooderServer(uint8_t index);
  enum SERVER_VALID_STATUS
  {
    INVALID,
    VALID,
  };
  SERVER_VALID_STATUS serverValidStatus[MAX_NUM_OF_SPOODER_SERVERS] = {};
  enum SERVER_CONNECTION_STATUS
  {
    NOT_CONNECTED,
    CONNECTED,
  };
  SERVER_CONNECTION_STATUS serverConnectionStatus[MAX_NUM_OF_SPOODER_SERVERS] = {};
  uint32_t updateSpooderClientTimer;
  const uint32_t UPDATE_SPOODER_CLIENT_PEROID = 3000;
  void updateSpooderClient();
  uint32_t updateSpooderServerStatusTimer;
  const uint32_t UPDATE_SPOODER_SERVER_STATUS_PEROID = 3000;
  void updateSpooderServer();

  bool netWorkTimeReceived = false;
  void updateNetworkTime();
  uint32_t updateNetworkTimeTimer;
  uint32_t UPDATE_NETWORK_TIME_PERIOD = 1000;

  // local communication between multiple devices
  SPOODERS_DATASET_STRUCT dataset[MAX_NUM_OF_SPOODERS]; // index 0 used as self
  uint8_t datasetLastPopulatedIndex = 0;
  void updateDataset();              // update own dataset, and announce out
  void copyDatasetFromServiceInfo(); // copy all info to dataset struct
  uint32_t updateDatsetTimer;
  const uint32_t UPDATE_DATASET_PERIOD = 5000; // 1000 * 60 = 1 minute
  void staticQueryMDNS();
  void removeStaticQuery();
  void listMDNSDynamicQueryAnswer();
  MDNSResponder::hMDNSService spooderService = 0; // The handle of the spooder service in the MDNS responder
  void updateServiceTxt();
  void printSpoodersDataset();
  void clearDataset(uint8_t index);
  void sortDataset(); // Sort data based on spooderID
  // bool complareTwoSpooders(SPOODERS_DATASET_STRUCT a, SPOODERS_DATASET_STRUCT b);
  uint8_t secondsNow(); // get current seconds, used for sequencing

  // display
  uint8_t currentPage = PAGE_NONE;
  uint8_t previousPage = PAGE_NONE;
  bool setPage(uint8_t page); // return true if page changed, false in page not changed
  void displayPage(uint8_t page);
  void drawBottomIndicator(uint8_t index);
  void drawRightIndicator(uint8_t index);
  uint8_t menuIndex = 0;
  void drawLeftIndicator(uint8_t index);
  uint8_t menuItemStartIndex = 0;
  const uint8_t menuItemPerPage = 5;
  const uint8_t numberOfMenuItems = 10;
  enum MENU
  {
    MENU_TARE,
    MENU_CALIBRATE,
    MENU_SPOOL_HOLDER_WEIGHT,
    MENU_SET_SPOODER_ID,
    MENU_LOW_FILAMENT_SETUP,
    MENU_NOTIFICATION,
    MENU_ENTER_HOTSPOT_MODE,
    MENU_FIRMWARE_UPDATE,
    MENU_OPTIONS,
    MENU_DEBUG
  };
  String menuTitle[10] = {"Tare",
                          "Calibrate",
                          "Spool Holder Weight",
                          "Set Spooder ID",
                          "Low Filament Setup",
                          "Notification",
                          "Hotspot Setup",
                          "Firmware Update",
                          "Options",
                          "Debug"};
  uint8_t tareSelection = TARE_OK;
  void drawTriangle(uint8_t x, uint8_t y);
  void tare();
  void calibrate();
  uint32_t returnToHomepageTimer;
  uint32_t RETURN_TO_HOMEPAGE_PERIOD = 30000; // 30 seconds
  void checkCurrentPage();

  // Set the default total weight in grams for calibration after power on
  // Valid values from 0 to 9999
  void setCalibrationWeight(uint16_t weight);

  // Set the default weight in grams for the spool holder. Can be changed in the UI.
  // Valid values from 0 to 999
  // Value is also saved to EEPROM
  void setCurrentSpoolHolderWeight(uint16_t weight);

  // Set the steps per click of the rotary encoder
  void setStepsPerClick(uint8_t steps);

  // Get the steps per click of the rotary encoder
  uint8_t getStepsPerClick();

  // Time settings for button
  void setDebounceTime(uint16_t ms);
  // Time settings for button
  void setLongClickTime(uint16_t ms);
  // Time settings for button
  void setDoubleClickTime(uint16_t ms);
  // Time settings for button
  uint16_t getDebounceTime();
  // Time settings for button
  uint16_t getLongClickTime();
  // Time settings for button
  uint16_t getDoubleClickTime();
  uint8_t stepsPerClick = DEFAULT_STEPS_PER_CLICK;

  HX711_ADC loadcell;
  // float calibrateValue; //a default value required for next calibration
  float newCalibrationValue;
  // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  uint32_t stabilizingTime = 2000;
  bool newDataReady = false;
  float totalWeight;
  float previousTotalWeight;
  uint32_t updateHomepageTimer;
  // The period to refresh homepage (weight)
  const uint32_t UPDATE_HOMEPAGE_PERIOD = 200;
  void updateHomepage();
  // Display overlay msg for [period ms]. If line2="", line1 is displayed at center.
  void drawOverlay(const char *msgLine1, const char *msgLine2, uint16_t period);
  void updateOverlay();
  const char *overlayMsgLine1;
  const char *overlayMsgLine2;
  uint16_t overlayDisplayPeriod = 1000;
  bool drawOverlayFlag = false;
  uint32_t drawOverlayTimer;
  void drawDisplay();

  uint8_t displayType = DISPLAY_TYPE_FILAMENT;
  float filamentWeight;

  uint8_t calibrateSelection = CALIBRATE_4_DIGIT;
  bool calibrateEditDigitMode = false;
  uint8_t calibrate4Digit = 0;
  uint8_t calibrate3Digit = 0;
  uint8_t calibrate2Digit = 0;
  uint8_t calibrate1Digit = 0;
  uint16_t getCalibrationWeight(); // get the weight from 4-digit UI menu
  // uint16_t calibrationWeight = 0;
  void checkCalibrateEditModeTimer();
  bool displayCalibrateDigit = true;
  uint32_t calibrateEditModerTimer;
  const uint32_t CALIBRATE_EDIT_MODE_PERIOD = 500;
  uint8_t calibrateSaveSelection = CALIBRATE_SAVE_OK;

  uint8_t lowFilamentSelection = LOW_FILAMENT_2_DIGIT;
  bool lowFilamentEditDigitMode = false;
  uint8_t lowFilament4Digit = 0;
  uint8_t lowFilament3Digit = 0;
  uint8_t lowFilament2Digit = 0;
  uint8_t lowFilament1Digit = 0;
  uint16_t getLowFilamentThreshold();
  void checkLowFilamentEditModeTimer();
  bool displayLowFilamentDigit = true;
  uint32_t lowFilamentEditModerTimer;
  const uint32_t LOW_FILAMENT_EDIT_MODE_PERIOD = 500;

  enum DEBUG_MENU
  {
    DEBUG_LOAD_TO_SETTING,
    DEBUG_SAVE_TO_EEPROM,
    DEBUG_DUMP_SETTING,
    DEBUG_DUMP_EEPROM,
    DEBUG_ERASE_EEPROM,
    DEBUG_LIST_DIRECTORY,
    DEBUG_LOAD_CONFIG,
    DEBUG_DUMP_CONFIG,
    DEBUG_REBOOT,
    DEBUG_BLYNK_NOTIFY,
    DEBUG_START_LOGGING,
    DEBUG_STOP_LOGGING,
    DEBUG_RUN_LOG_TXT,
    DEBUG_STOP_LOG_TXT,
    DEBUG_TOGGLE_DETECTION_OUTPUT,
    DEBUG_STATIC_QUERY_MDNS,
    DEBUG_REMOVE_STATIC_QUERY,
    DEBUG_LIST_DYNAMIC_QUERY_ANSWER,
    DEBUG_UPDATE_SERVICE_TXT,
    DEBUG_PRINT_SPOODERS_DATASET,
    DEBUG_CHECK_DATA_FOLDER,
    DEBUG_UPDATE_DATA_FOLDER,
    DEBUG_PRINT_MEMORY,
    DEBUG_MANAGE_LOCAL_FILES,
    DEBUG_TOOGLE_VERBOSE_DEBUG,
    DEBUG_CONNECT_SPOODER_SERVER,
    DEBUG_LIST_SPOODER_SERVICE,
    DEBUG_RETURN
  };
  uint8_t debugMenuSelection = DEBUG_LOAD_TO_SETTING;
  uint8_t debugMenuItemStartIndex = 0;
  uint8_t debugMenuItemPerPage = 5;
#define NUMBER_OF_DEBUG_ITEMS 28
  uint8_t numberOfDebugMenuItems = NUMBER_OF_DEBUG_ITEMS;
  String debugMenuTitle[NUMBER_OF_DEBUG_ITEMS] = {
      "Load to Setting",
      "Save to EEPROM",
      "Dump Setting",
      "Dump EEPROM",
      "Erase EEPROM",
      "List Directory",
      "Load Config",
      "Dump Config",
      "Reboot",
      "Send Blynk Test Message",
      "Start Logging",
      "Stop Logging",
      "Start Emulation",
      "Stop Emulation",
      "Toogle Detection Output",
      "Static Query mDNS",
      "Remove Static Query",
      "List Dynamic Query Ans",
      "Update Service Txt",
      "Print Spooders Dataset",
      "Check /data folder",
      "Update /data folder",
      "Print Memory",
      "Manage Local Files",
      "Toogle Verbose Debug",
      "Connect Spooder Server",
      "List Spooder Service",
      "<<-- Return to Menu "};
  enum OPTIONS_MENU
  {
    OPTIONS_MENU_WIFI,
    OPTIONS_MENU_MDNS,
    OPTIONS_MENU_BLYNK,
    OPTIONS_MENU_WEB_SERVER,
    OPTIONS_MENU_SPOODER_CLIENT,
    OPTIONS_MENU_SPOODER_SERVER,
    OPTIONS_MENU_ARDUINO_OTA,
    OPTIONS_MENU_AUTO_HOMEPAGE,
    OPTIONS_MENU_RETURN
  };
#define NUM_OPTIONS_MENU_TITLE 9
  String optionsMenuTitle[NUM_OPTIONS_MENU_TITLE] =
      {
          "WiFi",
          "mDNS",
          "Blynk",
          "Web server",
          "Spooder Client",
          "Spooder Server",
          "Arduino OTA",
          "Auto Homepage",
          "<<-- Return to Menu"};
  uint8_t optionsMenuSelection = OPTIONS_MENU_WIFI;
  uint8_t optionsMenuItemStartIndex = 0;
  uint8_t optionsMenuItemPerPage = 5;
  uint8_t numberOfOptionsMenuItems = NUM_OPTIONS_MENU_TITLE;
  bool getOptionsSetting(uint8_t selection);
  void setOptionsSetting(uint8_t selection, bool value);
  void loadToSetting();
  void saveToEEPROM();
  void dumpSetting();
  void dumpEEPROM();
  void eraseEEPROM();
  uint32_t versionToNumber(VERSION_STRUCT v);
  void toogleVerboseDebug();
  bool VERBOSE_DEBUG_PRINT = true;
  bool VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER = false;
  void listDir(const char *dirname); // List the FSn directory in a user-friendly text format
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
  uint16_t getSpoolHolderWeight(); // get the weight from 3-digit UI menu
  bool noSpoolHolderWeightInEEPROM = false;

  // Allocate a buffer to store contents of the file.
  // char configBuffer[CONFIG_FILE_BUFFER_SIZE];
  uint16_t configSize;
  void loadConfig(); // load the config file into buffer
  void dumpConfig(); // Pring all the config.json content for debugging purpose
  void saveConfig(); // update ssid, password and blynk auth token
  // StaticJsonDocument<JSON_DOC_BUFFER_SIZE> jsonDocS;

  char config_version[16]; // "0.3.0"
  char wifi_ssid[32];      // "your_ssid", max 32 bytes
  char wifi_password[63];  // "your_password", max 63 bytes
  char blynk_auth[33];     // 32-bytes blynk authorization code, +1 byte for null terminator

  // const char *spoolHolderSlotName[SPOOL_HOLDER_MAX_SLOT_SIZE];
  char spoolHolderSlotName[SPOOL_HOLDER_MAX_SLOT_SIZE][12];

  uint16_t spoolHolderSlotWeight[SPOOL_HOLDER_MAX_SLOT_SIZE];
  uint8_t spoolHolderSlotSize = 0;
  uint8_t spoolHolderSlotStartIndex = SPOOL_HOLDER_SLOT_1;

  uint8_t setSpooderIDSelection = SET_SPOODER_ID_LETTER;
  uint8_t spooderIDLetter = 0;
  uint8_t spooderIDNumber = 0;
  bool setSpooderIDEditMode = false;
  bool displaySpooderIDDigit = true;
  bool validSpooderID = false;
  uint32_t setSpooderIDTimer;
  void checkSetSpooderIDEditModeTimer();
  const uint32_t SET_SPOODER_ID_EDIT_MODE_PERIOD = 500;
  String hostname = "";

  void drawSymbols(); // Draw WiFi and Blynk symbols
  uint8_t connectionStatus = CONNECTION_STATUS_NONE;
  uint8_t symbolType = SYMBOL_NONE;
  uint32_t displayConnectionStatusTimer;

  const uint32_t CHECK_CONNECTION_PERIOD = 2000;
  uint32_t checkConnectionTimer;
  void checkConnectionStatus();
  const uint32_t CHANGE_CONNECTION_SYMBOL_PERIOD = 1000;
  uint32_t checkConnectionDisplaySymbolTimer;
  void checkConnectionDisplaySymbol();

  // file system manager, imported from https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/FSBrowser
  const char *fsName = "LittleFS";
  FS *fileSystem = &LittleFS;
  LittleFSConfig fileSystemConfig = LittleFSConfig();
  bool fsOK;
  String unsupportedFiles = String();
  File uploadFile;
  const char *TEXT_PLAIN = "text/plain";
  const char *FS_INIT_ERROR = "FS INIT ERROR";
  const char *FILE_NOT_FOUND = "FileNotFound";
  // Utils to return HTTP codes, and determine content-type
  void replyOKWithMsg(String msg);
  void replyNotFound(String msg);
  void replyBadRequest(String msg);
  void replyServerError(String msg);
  // Request handlers
  bool handleFileRead(String path);       // Read the given file from the filesystem and stream it back to the client
  String lastExistingParent(String path); // As some FS (e.g. LittleFS) delete the parent folder when the last child has been removed, return the path of the closest parent still existing
  void deleteRecursive(String path);      // Delete the file or folder designed by the given path. If it's a file, delete it. If it's a folder, delete all nested contents first then the folder itself

  // Data logging
  // 10 min = 18 kB
  // 1 hr = 108 kB
  // 7 hr = 756 k
  uint32_t loggerTimer;
  uint32_t loggerInterval = 1000;
  uint16_t loggerCounter = 0;
  bool isLogging = false;
  File logFile;
  void startLogging();
  void stopLogging();
  void updateLogging();
  time_t now;
  time_t previous;

  // Logged data emulation
  void startEmulation();  // emulate the logged data, using log/log.txt
  void stopEmulation();   // stop the emulation
  void updateEmulation(); // update emulated weight
  bool emulationStarted = false;
  uint32_t emulationTimer;
  uint32_t EMULATION_PERIOD = 1000;
  float emulatedWeight;
  File emulatedLogFile;

  // Status detection using average and std dev
  // function imported from https://github.com/MajenkoLibraries/Average
  uint32_t DETECTION_PERIOD = 1000;
  uint32_t detectionTimer;
  void updateDetection();
  uint16_t weightCount = 0;
  uint16_t stddevCount = 0;
  uint16_t detectionPosition = 0;
  uint16_t stddevPosition = 0;
  float weightArray[DETECTION_SAMPLE_SIZE]; // array to store weight
  float stddevArray[DETECTION_SAMPLE_SIZE]; // array to store stddev3
  void pushWeight(float entry);
  void pushStddev(float entry);
  void purgeWeight(float value);
  void purgeStddev(float value);
  uint8_t purgeCounter = 0;
  enum PRINTING_STATUS
  {
    STATUS_BOOT,
    STATUS_EMPTY,
    STATUS_IDLE,
    STATUS_PRINTING,
    STATUS_
  } printingStatus;
  String printingStatusString;
  bool detectionDebugOutput = false;
  float getSum(uint16_t samples);
  float getMean(uint16_t samples);         // retern mean of the latest number of samples
  float getStddev(uint16_t samples);       // retern stddev of the latest number of samples
  uint8_t getStddevCount(float threshold); // return number of samples that are greater than threshold

  // Notification functions
  enum NOTIFICATION_MESSAGE
  {
    NOTIFICATION_TEST_MESSAGE,
    NOTIFICATION_PRINT_STARTED,
    NOTIFICATION_PRINT_COMPLETED,
    NOTIFICATIONI_LOW_FILAMENT,
    NOTIFICATIONI_FALL_OFF_RACK,
    NOTIFICATIONI_FALL_OFF_BEARING,
    NOTIFICATIONI_TANGLED
  } notificationMessage;
  void notify(NOTIFICATION_MESSAGE message);
  bool lowFilamentNotificationSent = false;
  bool fallOffRackNotificationSent = false;
  uint8_t notificationMenuSelection = NOTIFICATION_MENU_PRINT_STARTED;
  uint8_t notificationMenuItemStartIndex = 0;
  uint8_t notificationMenuItemPerPage = 5;
  uint8_t numberOfNotificationMenuItems = 7;
  String notificationMenuTitle[7] = {
      "Print Started",
      "Print Completed",
      "Low Filament",
      "Fall off Rack",
      "Fall off Bearing",
      "Tangled",
      "<<-- Return to Menu "};
  bool getNotificationSetting(uint8_t selection);
  void setNotificationSetting(uint8_t selection, bool value);

  // Github auto-update function related:
  // Imported from https://github.com/yknivag/ESP_OTA_GitHub
  void checkGithubLatestRelease(bool forceCheck, bool doUpdate, bool getDataFolder, bool updateDataFolder);
  bool checkGithubConnectionPrerequisite(); // return true if connection is ready
  BearSSL::CertStore *_certStore;

  const char *user = "FuzzyNoodle";
  const char *repo = "Fuzzy-Spooder";
  const char *githubHost = "api.github.com";
  int githubPort = 443;
  const char *_currentTag = CURRENT_VERSION;
  // Need to reserve global String to prevent unwanted memory usage
  const int githubVersionSize = 12;
  String githubVersion = "- - - - -";
  const int _updateURLSize = 768;
  String _updateURL = "URL"; // Holds the update URL (changes when getFinalURL() is run).
  const char *_binFile = "firmware.bin";
#define GHOTA_CONTENT_TYPE "application/octet-stream"
  bool _preRelease = true;
  int numCerts = 0; // number or certs read from file system
  bool githubVersionObtained = false;
  bool githubUpdateAvailable = false;
  bool versionTagIsValid(VERSION_STRUCT v);
  bool convertTagToVersion(String t, VERSION_STRUCT *v); // convert String tag into VERSION_STRUCT, return false if invalid tag
  void printVersion(VERSION_STRUCT v);
  void saveGitgubFileToFs();
  void listGitgubContent(BearSSL::WiFiClientSecure &client, String &path, bool updateDataFolder);
  void manageLocalFileInfo(); // Create, manage the json file info of the local files. Used for comparison, if the local file needs to be updated.
  const char *localFileInfoPath = "/fileinfo.json";
  struct LOCAL_FILE_INFO_STRUCT
  {
    char name[32 + 1];
    char path[32 + 1];
    char sha[40 + 1];
  };
  void iterateLocalFiles(String dirname, DynamicJsonDocument &localFileDoc);

  // urlDetails_t _urlDetails(String url); // Separates a URL into protocol, host and path into a custom struct
  enum FIRMWARRE_UPDATE_MENU
  {
    FIRMWARE_UPDATE_MENU_CHECK_NOW,
    FIRMWARE_UPDATE_MENU_UPDATE_NOW,
    FIRMWARE_UPDATE_MENU_AUTO_UPDATE,
    FIRMWARE_UPDATE_NEXT_CHECK_TIME,
    FIRMWARE_UPDATE_MENU_RETURN_TO_MENU

  };
  String updateMenuTitle[5] =
      {
          "Check Now",
          "Update Now",
          "Auto Update:",
          "Next check in",
          "<<-- Return to Menu"};
  uint8_t updateMenuSelection = FIRMWARE_UPDATE_MENU_CHECK_NOW;
  uint8_t updateMenuItemStartIndex = 0;
  uint8_t updateMenuItemPerPage = 3;
  uint8_t numberOfUpdateMenuItems = 5;
  uint32_t nextCheckTimeMillis = 0; // random countdown between 60~120 minutes
  void setNextCheckCountdown();
  void updateAutoGithubCheck();
  uint32_t updateAutoGithubCheckTimer;
  const uint32_t UPDATE_AUTO_GITHUB_CHECK_PEROID = 1000;

  void pause(); // for debugging, enter any key to continue

  // Hotspot mode for initial setup
  void enterHotspotMode(); // to setup spooder using ap mode and browser
  void exitHotspotMode();
  bool isHotspotMode = false;
  const byte DNS_PORT = 53;
  DNSServer dnsServer;
  // String captivePortalResponseHTML = "<!DOCTYPE html><html lang='en'> <head> <meta charset='utf-8' /> <meta name='viewport' content='width=device-width, initial-scale=1' /> <title>Spooder Setup</title> <style> *, ::after, ::before { box-sizing: border-box; } body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 1rem; font-weight: 400; line-height: 1.5; color: #212529; background-color: #f5f5f5; } .form-control { display: block; width: 100%; height: calc(1.5em + 0.75rem + 2px); border: 1px solid #ced4da; } button { cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; padding: 0.5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: 0.3rem; width: 100%; } .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; } h1 { text-align: center; } </style> </head> <body> <main class='form-signin'> <form action='/' method='post'> <h1 class=''>Spooder Setup</h1> <br /> <div class='form-floating'> <label>WiFi SSID</label> <input type='text' class='form-control' name='ssid' /> </div> <div class='form-floating'> <br /> <label>WiFi Password</label> <input type='password' class='form-control' name='password' /> </div> <div class='form-floating'> <br /> <label>Blynk Auth Token</label> <input type='text' class='form-control' name='blynk_auth_token' /> </div> <br /> <br /> <button type='submit'>Save</button> <p style='text-align: right;'></p> </form> </main> </body></html>";
  // Debug print memory: Heap: 21128    Block: 17384
  // String captivePortalResponseHTML = "";
  // Debug print memory: Heap: 24272    Block: 20256
  enum SETUP_SAVE_REPLY_MESSAGE
  {
    SETUP_SAVED,
    SETUP_CLEARED,
    SETUP_TOO_LONG,
    SETUP_INCORRECT_LENGTH,
    SETUP_NO_CHANGE,
  };
  bool resetWiFi = false;
  bool resetBlynk = false;
};

#endif //#ifndef FILAMENT_ESTIMATOR_H