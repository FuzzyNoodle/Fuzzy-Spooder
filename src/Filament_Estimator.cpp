#include "Filament_Estimator.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

extern BlynkWifi Blynk;

FILAMENT_ESTIMATOR *estimatorPointer;         // Declare a pointer to WEIGHT_ESTIMATOR
static void globalButtonHandler(Button2 &btn) // define global handler
{
    estimatorPointer->buttonHandler(btn); // calls class member handler
}
static void globalRotaryHandler(ESPRotary &rty) // define global handler
{
    estimatorPointer->rotaryHandler(rty); // calls class member handler
}
static void globalHandleStatus()
{
    estimatorPointer->handleStatus();
}
static void globalHandleFileList()
{
    estimatorPointer->handleFileList();
}
static void globalHandleGetEdit()
{
    estimatorPointer->handleGetEdit();
}
static void globalHandleFileCreate()
{
    estimatorPointer->handleFileCreate();
}
static void globalHandleFileDelete()
{
    estimatorPointer->handleFileDelete();
}
static void globalHandleFileUpload()
{
    estimatorPointer->handleFileUpload();
}
static void globalHandleNotFound()
{
    estimatorPointer->handleNotFound();
}
static void globalReplyOK()
{
    estimatorPointer->replyOK();
}
static void globalHandleWebsocketsMessage(WebsocketsClient &client, WebsocketsMessage message)
{
    estimatorPointer->handleWebsocketsMessage(client, message);
}
static void globalHandleWebsocketsEvent(WebsocketsClient &client, WebsocketsEvent event, String data)
{
    estimatorPointer->handleWebsocketsEvent(client, event, data);
}
static void globalHandleSetupSave()
{
    estimatorPointer->handleSetupSave();
}

MDNSResponder::hMDNSServiceQuery hMDNSServiceQuery = 0; // The global handle of the 'http.tcp' service query in the MDNS responder
void globalMDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent)
{
    // pass information the the inside-class method
    estimatorPointer->MDNSServiceQueryCallback(&serviceInfo, answerType, p_bSetContent);
}
bool globalComplareTwoSpooders(SPOODERS_DATASET_STRUCT a, SPOODERS_DATASET_STRUCT b)
{
    // padding
    if (strlen(a.spooderID) == 2)
    {
        // need to pad A1 to A01, so that A2 (A02) will be smaller than A11
        SPOODERS_DATASET_STRUCT temp;
        strncpy(temp.spooderID, a.spooderID, 1);
        strcpy(temp.spooderID + 1, "0");
        strcpy(temp.spooderID + 2, a.spooderID + 1);
        strcpy(a.spooderID, temp.spooderID);
    }
    if (strlen(b.spooderID) == 2)
    {
        SPOODERS_DATASET_STRUCT temp;
        strncpy(temp.spooderID, b.spooderID, 1);
        strcpy(temp.spooderID + 1, "0");
        strcpy(temp.spooderID + 2, b.spooderID + 1);
        strcpy(b.spooderID, temp.spooderID);
    }

    // This function starts comparing the first character of each string.
    // If they are equal to each other, it continues with the following
    // pairs until the characters differ or until a terminating
    // null-character is reached.
    if (strcmp(a.spooderID, b.spooderID) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void globalPrintMemory(String s)
{
    Serial.print(s);
    Serial.print(F(" Heap: "));
    Serial.print(ESP.getFreeHeap());
    Serial.print(F("    Block: "));
    Serial.println(ESP.getMaxFreeBlockSize());
}

// Hotspot mode for initial setup
// Using PROGMEM to save RAM
// tools used:
// HTML online editor https://www.w3schools.com/html/tryit.asp?filename=tryhtml_basic
// HTML formatter https://webformatter.com/html
// HTML compresser https://www.textfixer.com/html/compress-html-compression.php

// static const char captivePortalResponseHTML[] PROGMEM = "<!DOCTYPE html><html lang='en'> <head> <meta charset='utf-8' /> <meta name='viewport' content='width=device-width, initial-scale=1' /> <title>Spooder Setup</title> <style> *, ::after, ::before { box-sizing: border-box; } body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 1rem; font-weight: 400; line-height: 1.5; color: #212529; background-color: #f5f5f5; } .form-control { display: block; width: 100%; height: calc(1.5em + 0.75rem + 2px); border: 1px solid #ced4da; } button { cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; padding: 0.5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: 0.3rem; width: 100%; } .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; } h1 { text-align: center; } </style> </head> <body> <main class='form-signin'> <form action='/' method='post'> <h1 class=''>Spooder Setup</h1> <br /> <div class='form-floating'> <label>WiFi SSID</label> <input type='text' class='form-control' name='ssid' value='gnet02' /> </div> <div class='form-floating'> <br /> <label>WiFi Password</label> <input type='password' class='form-control' name='password' /> </div> <div class='form-floating'> <br /> <label>Blynk Auth Token</label> <input type='text' class='form-control' name='blynk_auth_token' /> </div> <br /> <br /> <button type='submit'>Save</button> <p style='text-align: right;'></p> </form> </main> </body></html> ";

static const char captivePortalResponseHTML1[] PROGMEM = "<!DOCTYPE html><html lang='en'> <head> <meta charset='utf-8' /> <meta name='viewport' content='width=device-width, initial-scale=1' /> <title>Spooder Setup</title> <style> *, ::after, ::before { box-sizing: border-box; } body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 1rem; font-weight: 400; line-height: 1.5; color: #212529; background-color: #f5f5f5; } .form-control { display: block; width: 100%; height: calc(1.5em + 0.75rem + 2px); border: 1px solid #ced4da; } button { cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; padding: 0.5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: 0.3rem; width: 100%; } .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; } h1 { text-align: center; } </style> </head> <body> <main class='form-signin'> <form action='/' method='post'> <h1 class=''>Spooder Setup</h1> <br /> <div class='form-floating'> <label>WiFi SSID</label> ";
// ssid reply message here
static const char captivePortalResponseHTML2[] PROGMEM = "<input type='text' class='form-control' name='ssid' value='";
// ssid here
static const char captivePortalResponseHTML3[] PROGMEM = "'/> </div> <div class='form-floating'> <br /> <label>WiFi Password</label> ";
// password reply message here
static const char captivePortalResponseHTML4[] PROGMEM = "<input type='text' class='form-control' name='password' value ='";
// password here
static const char captivePortalResponseHTML5[] PROGMEM = "'/> </div> <div class='form-floating'> <br /> <label>Blynk Auth Token</label>";
// blynk auth token reply message here
static const char captivePortalResponseHTML6[] PROGMEM = "<input type='text' class='form-control' name='blynk_auth_token' value ='";
// blynk auth token here
static const char captivePortalResponseHTML7[] PROGMEM = "'/> </div> <br /> <br /> <button type='submit'>Save</button> <p style='text-align: right;'></p> </form> </main> </body></html>";
// Debug print memory: Heap: 24160    Block: 20384
static const char setupReplySaved[] PROGMEM = "<label style='color: green'>- Saved</label>";
static const char setupReplyCleared[] PROGMEM = "<label style='color: green'>- Cleared</label>";
static const char setupReplyTooLong[] PROGMEM = "<label style='color: red'>- Too long</label>";
static const char setupReplyIncorrectLength[] PROGMEM = "<label style='color: red'>- Incorrect length (need 32 bytes)</label>";
static const char setupReplyNoChange[] PROGMEM = "<label style='color: black'>- No change</label>";

FILAMENT_ESTIMATOR::FILAMENT_ESTIMATOR() : button{BUTTON_PIN},
                                           rotary{ROTARY_PIN_DT, ROTARY_PIN_CLK, stepsPerClick},
                                           display{SSD1306_ADDRESS, SSD1306_SDA_PIN, SSD1306_SCL_PIN},
                                           loadcell{HX711_DOUT_PIN, HX711_SCK_PIN}

{
}
void FILAMENT_ESTIMATOR::begin(void)
{
    begin("", "", "", "");
}
void FILAMENT_ESTIMATOR::begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token)
{
    Serial.println();
    // Firmware version setup
    convertTagToVersion(CURRENT_VERSION, &currentVersion);

    Serial.println("Program started.");
    Serial.print(F("Firmware version "));

    Serial.print(currentVersion.major);
    Serial.print(F("."));
    Serial.print(currentVersion.minor);
    Serial.print(F("."));
    Serial.print(currentVersion.patch);
    Serial.println();

    // Setup for OLED
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    display.setLogBuffer(5, 30);

    Serial.println("OLED setup ok.");

    // Mount file system
    fileSystemConfig.setAutoFormat(false);
    fileSystem->setConfig(fileSystemConfig);
    fsOK = fileSystem->begin();
    if (!fsOK)
    {
        Serial.println(F("Failed to mount file system"));
    }
    else
    {
        Serial.println(F("LittleFS file system mounted."));
        // listDir("");
    }
    /*
    numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.print(F("Number of CA certs read: "));
    Serial.println(numCerts);
    */

    displayMonoBitmap("/images/logo.bmp");

    // Setup for settings and eeprom
    EEPROM.begin(DECLARED_EEPROM_SIZE);
    // setting.calValue = 3.14159;
    Serial.print(F("Size of setting struct: "));
    Serial.println((uint32_t)sizeof(setting));

    // Load eeprom data to setting
    loadToSetting();
    // validate data and reset to defaults
    bool isDirty = false;

    if (isnan(setting.calValue) || setting.calValue < 10)
    {
        Serial.println(F("EEPROM calValue invalid, using default value."));
        setting.calValue = DEFAULT_CALIBRATION_VALUE;
        isDirty = true;
    }

    Serial.print(F("Low filament threshold = "));
    Serial.println(setting.lowFilamentThreshold);
    if (setting.lowFilamentThreshold > 9999)
    {
        Serial.println(F("Low filament threshold invalid, using default value."));
        setting.lowFilamentThreshold = DEFAULT_LOW_FILAMENT_THRESHOLD_VALUE;
        isDirty = true;
    }
    lowFilament4Digit = (setting.lowFilamentThreshold / 1000U) % 10;
    lowFilament3Digit = (setting.lowFilamentThreshold / 100U) % 10;
    lowFilament2Digit = (setting.lowFilamentThreshold / 10U) % 10;
    lowFilament1Digit = (setting.lowFilamentThreshold / 1U) % 10;

    setCalibrationWeight(setting.calibrationWeight); // set the four digits
    // dumpSetting();

    // Initialize notification settings for the first time
    if (setting.notifyOnPrintStarted != 0 && setting.notifyOnPrintStarted != 1)
    {
        setting.notifyOnPrintStarted = true;
        isDirty = true;
    }
    if (setting.notifyOnPrintCompleted != 0 && setting.notifyOnPrintCompleted != 1)
    {
        setting.notifyOnPrintCompleted = true;
        isDirty = true;
    }
    if (setting.notifyOnLowFilament != 0 && setting.notifyOnLowFilament != 1)
    {
        setting.notifyOnLowFilament = true;
        isDirty = true;
    }
    if (setting.notifyOnFallOffRack != 0 && setting.notifyOnFallOffRack != 1)
    {
        setting.notifyOnFallOffRack = true;
        isDirty = true;
    }
    if (setting.notifyOnFallOffBearing != 0 && setting.notifyOnFallOffBearing != 1)
    {
        setting.notifyOnFallOffBearing = true;
        isDirty = true;
    }
    if (setting.notifyOnTangled != 0 && setting.notifyOnTangled != 1)
    {
        setting.notifyOnTangled = true;
        isDirty = true;
    }

    // Initialize services settings for the first time
    if (setting.optionsWiFi != 0 && setting.optionsWiFi != 1)
    {
        setting.optionsWiFi = true;
        isDirty = true;
    }
    if (setting.optionsMDNS != 0 && setting.optionsMDNS != 1)
    {
        setting.optionsMDNS = true;
        isDirty = true;
    }
    if (setting.optionsBLYNK != 0 && setting.optionsBLYNK != 1)
    {
        setting.optionsBLYNK = true;
        isDirty = true;
    }
    if (setting.optionsWebServer != 0 && setting.optionsWebServer != 1)
    {
        setting.optionsWebServer = true;
        isDirty = true;
    }
    if (setting.optionsArduinoOTA != 0 && setting.optionsArduinoOTA != 1)
    {
        setting.optionsArduinoOTA = true;
        isDirty = true;
    }
    if (setting.optionsAutoHomepage != 0 && setting.optionsAutoHomepage != 1)
    {
        setting.optionsAutoHomepage = true;
        isDirty = true;
    }
    if (setting.optionsSpooderClient != 0 && setting.optionsSpooderClient != 1)
    {
        setting.optionsSpooderClient = false;
        isDirty = true;
    }
    if (setting.optionsSpooderServer != 0 && setting.optionsSpooderServer != 1)
    {
        setting.optionsSpooderServer = false;
        isDirty = true;
    }
    // Initialize auto github update settings for the first time
    if (setting.autoGithubUpdate != 0 && setting.autoGithubUpdate != 1)
    {
        setting.autoGithubUpdate = false; // auto update default value is off
        isDirty = true;
    }
    if (setting.automaticUpdateJustPerformed != 0 && setting.automaticUpdateJustPerformed != 1)
    {
        setting.automaticUpdateJustPerformed = false;
        isDirty = true;
    }
    if (isDirty == true)
    {
        Serial.println(F("Initializing EEPROM"));
        saveToEEPROM();
    }

    // Compare firmware version

    if (versionToNumber(currentVersion) > versionToNumber(setting.version))
    {
        Serial.println(F("Newer version number of firmware uploaded."));
        setting.version.major = currentVersion.major;
        setting.version.minor = currentVersion.minor;
        setting.version.patch = currentVersion.patch;
        saveToEEPROM();
    }
    else if (versionToNumber(currentVersion) < versionToNumber(setting.version))
    {
        Serial.println(F("Older version number of firmware uploaded."));
        setting.version.major = currentVersion.major;
        setting.version.minor = currentVersion.minor;
        setting.version.patch = currentVersion.patch;
        saveToEEPROM();
    }
    else
    {
        // Same version
    }
    if (setting.spoolHolderWeight < 0 or setting.spoolHolderWeight > 999)
    {
        Serial.print(F("EEPROM spool holder weight invalid, using default value: "));
        Serial.println(DEFAULT_SPOOL_HOLDER_WEIGHT);
        setting.spoolHolderWeight = DEFAULT_SPOOL_HOLDER_WEIGHT;
        spoolHolder3Digit = (setting.spoolHolderWeight / 100U) % 10;
        spoolHolder2Digit = (setting.spoolHolderWeight / 10U) % 10;
        spoolHolder1Digit = (setting.spoolHolderWeight / 1U) % 10;
        // Set flag, if spool holder weight is set by user in sketch, it will be stored to EEPROM.
        noSpoolHolderWeightInEEPROM = true;
    }

    // load config file
    loadConfig();
    // dumpConfig();

    // Setup for rotary switch
    estimatorPointer = this;
    button.setClickHandler(globalButtonHandler);
    button.setLongClickHandler(globalButtonHandler);
    button.setDoubleClickHandler(globalButtonHandler);
    button.setTripleClickHandler(globalButtonHandler);
    rotary.setLeftRotationHandler(globalRotaryHandler);
    rotary.setRightRotationHandler(globalRotaryHandler);
    setLongClickTime(LONG_CLICK_TIME);
    setStepsPerClick(DEFAULT_STEPS_PER_CLICK); // This could be different for different hardware

    Serial.println(F("Rotary Switch setup ok."));

    Serial.print("Initializing Loadcell using ");
    Serial.println(setting.calValue);

    loadcell.begin();
    loadcell.start(stabilizingTime, true);
    loadcell.setCalFactor(setting.calValue);
    // tare();
    loadcell.setTareOffset(setting.tareOffset);

    // WiFi related
    // Disable esp8266 wifi auto connection. Connection behavior is controlled by code and user.
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);

    // update related
    githubVersion.reserve(githubVersionSize); // reserve a fixed lenght for version tag, such as "11.22.33"
    _updateURL.reserve(_updateURLSize);
    setNextCheckCountdown();

    // local communication related
    updateSpooderServerStatusTimer = millis() + 5000; // startup delay
    uint16_t s = sizeof(dataset[0]) * MAX_NUM_OF_SPOODERS;
    Serial.print(F("Size of dataset table: "));
    Serial.println(s);

    printingStatus = STATUS_BOOT;
    printingStatusString = "STATUS_BOOT";
    setPage(PAGE_HOME);

    uint32_t num = spooderIDLetter * 26 + spooderIDNumber + (setting.calValue * 100.0);
    randomSeed(num);

    Serial.println(F("Setup completed."));

    // Enable WiFi according to user setting
    setWifi(setting.optionsWiFi);
}
void FILAMENT_ESTIMATOR::update(void)
{
    if (enableWifi == true)
    {
        updateWifi();
    }

    button.loop();
    rotary.loop();

    if (loadcell.update() == true)
    {
        newDataReady = true;
    }
    if (newDataReady == true)
    {

        totalWeight = loadcell.getData();

        // used for development purpose
        if (emulationStarted == true)
        {
            totalWeight = emulatedWeight;
        }

        filamentWeight = totalWeight - setting.spoolHolderWeight;
    }
    updateHomepage();

    // Refresh the screen after tare message period
    if (drawOverlayFlag == true)
    {
        if (millis() - drawOverlayTimer > overlayDisplayPeriod)
        {
            drawOverlayFlag = false;
            displayPage(currentPage);
        }
    }

    if (calibrateEditDigitMode == true)
    {
        checkCalibrateEditModeTimer();
    }
    if (spoolHolderEditDigitMode == true)
    {
        checkSpoolHolderEditModeTimer();
    }
    if (setSpooderIDEditMode == true)
    {
        checkSetSpooderIDEditModeTimer();
    }
    if (lowFilamentEditDigitMode == true)
    {
        checkLowFilamentEditModeTimer();
    }

    checkConnectionStatus();
    checkConnectionDisplaySymbol();

    if (isLogging == true)
    {
        // updateLogging();
        // function moved inside updateDetection()
    }

    if (emulationStarted == true)
    {
        updateEmulation();
    }
    updateDetection();

    if (setting.autoGithubUpdate == true)
    {
        updateAutoGithubCheck();
    }

    if (setting.optionsAutoHomepage == true)
    {
        checkCurrentPage();
    }

    updateDataset();

    if (isHotspotMode == true)
    {
        webServer->handleClient();
        dnsServer.processNextRequest();
    }
}
void FILAMENT_ESTIMATOR::setWifi(bool value)
{
    if (value == true)
    {
        if (enableWifi == false)
        {
            enableWifi = true;
            Serial.println(F("Wifi enabled."));
            WiFi.setAutoReconnect(true);
            wifiStatus = WIFI_STATUS_CONNECTING;
            connectWifi();
        }
    }
    else
    {
        if (enableWifi == true)
        {
            // turn off all dependent services

            setBlynk(false);
            setWebServer(false);
            setSpooderClient(false);
            setSpooderServer(false);
            setArduinoOTA(false);
            setMDNS(false);

            Serial.println(F("Wifi disabled."));
            WiFi.disconnect();
            WiFi.setAutoReconnect(false);
            wifiStatus = WIFI_STATUS_DISABLED;
            enableWifi = false;

            displayPage(currentPage); // Refresh page for WIFI symbol change
        }
    }
    // globalPrintMemory("setWifi: ");
}
void FILAMENT_ESTIMATOR::updateWifi()
{
    switch (wifiStatus)
    {
    case WIFI_STATUS_CONNECTING:
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            wifiStatus = WIFI_STATUS_CONNECTED;
            Serial.println(F("Wifi connected."));
            Serial.print("IP Address = ");
            Serial.println(WiFi.localIP().toString());
            Serial.print("Hostname = ");
            Serial.println(WiFi.hostname());
            WiFi.setSleepMode(WIFI_NONE_SLEEP); // Test if this fix the 7-day dropoff problem

            displayPage(currentPage); // Refresh page for WIFI symbol change
            beginServices();
        }
        break;
    }
    case WIFI_STATUS_CONNECTED:
    {
        if (MDNS.isRunning())
        {
            MDNS.update();
        }
        if (enableBlynk == true)
        {
            Blynk.run();
        }
        if (enableWebServer == true)
        {
            webServer->handleClient();
        }
        if (enableSpooderClient == true) // Acting as client
        {
            updateSpooderClient();
        }
        if (enableSpooderServer == true) // Acting as server
        {
            updateSpooderServer(); // Perform server related functions
        }
        if (enableArduinoOTA == true)
        {
            ArduinoOTA->handle();
        }
        if (enableNetworkTime == true)
        {
            updateNetworkTime();
        }

    } // case WIFI_STATUS_CONNECTED:

    default:
        break;
    } // switch (wifiStatus)
}
bool FILAMENT_ESTIMATOR::isAnswerValidServer(uint8_t index)
{
    if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)

        Serial.println("Validating answer:");
    /*
    valid hostname example: spooderA1.local
    Checking validity
    - The answer must have a hostname.
    - Then lengh of the hostname must be 15 or 16.
    - The hostname must contains  "spooder"
    - The hostname must contains  ".local"
    - IP adress must exist.
    - Port must exist.
    - Port number must equal to websocketsPort. (8266)
    */

    bool isValid = true;
    if (MDNS.hasAnswerHostDomain(hMDNSServiceQuery, index) == true)
    {

        uint8_t len = strlen(MDNS.answerHostDomain(hMDNSServiceQuery, index));
        if (len != 15 && len != 16)
        {
            if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)

                Serial.println(F("  Hostname length is invalid."));
            isValid = false;
        }
        else // has correct length 15 or 16
        {
            if (strstr(MDNS.answerHostDomain(hMDNSServiceQuery, index), "spooder") != NULL)
            {
                if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
                    Serial.println(F("  Hostname contains 'spooder"));
            }
            else
            {
                isValid = false;
            }

            if (strstr(MDNS.answerHostDomain(hMDNSServiceQuery, index), ".local") != NULL)
            {
                if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
                    Serial.println(F("  Hostname contains '.local"));
            }
            else
            {
                isValid = false;
            }
        }
    }
    else // if (MDNS.hasAnswerHostDomain(hMDNSServiceQuery, index) == true)
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("  Hostname does not exist."));
        isValid = false;
    }

    if (MDNS.hasAnswerIP4Address(hMDNSServiceQuery, index) == true)
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("  IP adress exists."));
    }
    else
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("  IP adress does not exist."));
        isValid = false;
    }

    if (MDNS.hasAnswerPort(hMDNSServiceQuery, index) == true)
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("  Port exists."));
        if (MDNS.answerPort(hMDNSServiceQuery, index) == websocketsPort)
        {
            if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
                Serial.println(F("  Port number is correct."));
        }
        else

        {
            if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
                Serial.println(F("  Port number is not correct."));
            isValid = false;
        }
    }
    else
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("  Port does not exist."));
        isValid = false;
    }
    if (isValid == true)
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("The answer is a valid server."));
    }
    else
    {
        if (VERBOSE_DEBUG_PRINT_VALIDATE_ANSWER == true)
            Serial.println(F("The answer is not a valid server."));
    }

    return isValid;
}
void FILAMENT_ESTIMATOR::connectSpooderServer(uint8_t serverIndex)
{
    Serial.print(MDNS.answerHostDomain(hMDNSServiceQuery, serverIndex));
    Serial.print(" (");
    Serial.print(MDNS.answerIP4Address(hMDNSServiceQuery, serverIndex, 0)); // only list the first(index==0) ip
    Serial.print(":");
    Serial.print(MDNS.answerPort(hMDNSServiceQuery, serverIndex));
    Serial.print(")");
    IPAddress serverIP = MDNS.answerIP4Address(hMDNSServiceQuery, serverIndex, 0);
    uint16_t serverPort = MDNS.answerPort(hMDNSServiceQuery, serverIndex);

    // try to connect to Websockets server
    bool connected = webSocketsClient.connect(
        serverIP.toString(),
        serverPort, "/");
    if (connected)
    {
        Serial.println("Connecetd!");

        // run callback when messages are received
        webSocketsClient.onMessage(globalHandleWebsocketsMessage);
        webSocketsClient.onEvent(globalHandleWebsocketsEvent);
    }
    else
    {
        Serial.println("Not Connected!");
    }
    serverConnectionStatus[0] = connected ? CONNECTED : NOT_CONNECTED;
}
void FILAMENT_ESTIMATOR::handleWebsocketsMessage(WebsocketsClient &client, WebsocketsMessage message)
{

    if (message.isText())
    {
        Serial.print("Got text message: ");
        auto data = message.data();
        Serial.println(data);

        if (data.indexOf("server!") > 0)
        {
            // got message from server
            Serial.println("Send: Hello from client!");
            client.send("Hello from client!");
        }
        else if (data.indexOf("client!") > 0)
        {
            // got reply from client
            WEBSOCKETS_HOLDER holder;
            holder.p.num = 20;
            strcpy(holder.p.str, "hello");
            holder.p.weight = 113.22;
            packetAddress = &holder.p;

            Serial.print("Send test packet:");
            for (uint8_t i = 0; i < sizeof(holder.p); ++i)
                Serial.printf("%02x", holder.str[i]);
            Serial.println();

            Serial.print("Size of packet =");
            Serial.println(sizeof(holder.p));

            client.sendBinary((char *)packetAddress, sizeof(holder.p));
        }
    }
    else if (message.isBinary())
    {
        // got a test packet
        Serial.print("Got binary len = ");
        Serial.println(message.length());

        WEBSOCKETS_HOLDER holder;
        Serial.print("Size of WEBSOCKETS_HOLDER = ");
        Serial.println(sizeof(holder));

        memcpy(holder.str, message.c_str(), message.length());

        Serial.print("Got test packet:");
        for (uint8_t i = 0; i < sizeof(holder.p); ++i)
            Serial.printf("%02x", holder.str[i]);
        Serial.println();

        Serial.println(holder.p.num);
        Serial.println(holder.p.str);
        Serial.println(holder.p.weight);
    }
}
void FILAMENT_ESTIMATOR::handleWebsocketsEvent(WebsocketsClient &client, WebsocketsEvent event, String data)
{
    switch (event)
    {
    case WebsocketsEvent::ConnectionOpened:
        Serial.println("Connection opened");

        break;
    case WebsocketsEvent::ConnectionClosed:
        Serial.print("Connection closed:");
        Serial.println(client.getCloseReason());
        /*
        CloseReason_None                =       -1,
        CloseReason_NormalClosure       =       1000,
        CloseReason_GoingAway           =       1001,
        CloseReason_ProtocolError       =       1002,
        CloseReason_UnsupportedData     =       1003,
        CloseReason_NoStatusRcvd        =       1005,
        CloseReason_AbnormalClosure     =       1006,
        CloseReason_InvalidPayloadData  =       1007,
        CloseReason_PolicyViolation     =       1008,
        CloseReason_MessageTooBig       =       1009,
        CloseReason_InternalServerError =       1011,
        */
        break;
    case WebsocketsEvent::GotPing:
        Serial.println("Got ping.");
        break;
    case WebsocketsEvent::GotPong:
        Serial.println("Got pong");
        break;
    default:
        break;
    }
}
void FILAMENT_ESTIMATOR::updateSpooderServer()
{
    // Perform looped tasks

    updateWebsocketsServer();

    // Perform periodic tasks
    if (millis() - updateSpooderServerStatusTimer < UPDATE_SPOODER_SERVER_STATUS_PEROID)
        return;
    // if (VERBOSE_DEBUG_PRINT == true)
    // Serial.println(F("Updating Spooder Server periodic tasks:"));

    updateSpooderServerStatusTimer = millis();
}
void FILAMENT_ESTIMATOR::updateSpooderClient()
{
    // Perform looped tasks

    updateWebsocketsClient();

    // Perform periodic tasks
    if (millis() - updateSpooderClientTimer < UPDATE_SPOODER_CLIENT_PEROID)
    {
    }
    return;
    // if (VERBOSE_DEBUG_PRINT == true)
    // Serial.println(F("Updating Spooder Server periodic tasks:"));

    updateSpooderClientTimer = millis();
}
void FILAMENT_ESTIMATOR::updateNetworkTime()
{

    if (millis() - updateNetworkTimeTimer < UPDATE_NETWORK_TIME_PERIOD)
    {
        return;
    }

    // Check if the network time has been received, using year>2000
    time_t now = time(nullptr);
    tm *tmp = localtime(&now);

    if (netWorkTimeReceived == false)
    {
        if (tmp->tm_year > 100)
        {
            netWorkTimeReceived = true;
            Serial.print(F("Network time received: "));
            Serial.print(ctime(&now)); // ctime includes "\n"
        }
    }
    else // netWorkTimeReceived == false
    {
    }

    updateNetworkTimeTimer = millis();
}
void FILAMENT_ESTIMATOR::connectWifi()
{

    WiFi.mode(WIFI_STA);
    Serial.print("Set WiFi Hostname to: ");
    Serial.println(hostname);
    WiFi.hostname(hostname); // Need this here for windows mDns to work
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.print(F("Connecting to "));
    Serial.println(wifi_ssid);
}
void FILAMENT_ESTIMATOR::beginServices()
{
    // Serial.println("Check Gitgub at beginServices():");
    // globalCheckGithubTag(false);
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("WiFi not connected."));
        return;
    }
    if (setting.optionsMDNS == true)
    {
        setMDNS(true);
    }
    if (setting.optionsBLYNK == true)
    {
        setBlynk(true);
    }
    if (setting.optionsWebServer == true)
    {
        setWebServer(true);
    }
    if (setting.optionsArduinoOTA == true)
    {
        setArduinoOTA(true);
    }
    if (setting.optionsSpooderClient == true)
    {
        setSpooderClient(true);
    }
    if (setting.optionsSpooderServer == true)
    {
        setSpooderServer(true);
    }
    if (enableNetworkTime == true)
    {
        configTime(TZ_Etc_GMT, "pool.ntp.org");
    }

    // Serial.println(F("NTP started."));

    Serial.println(F("Services started."));
}
void FILAMENT_ESTIMATOR::setMDNS(bool value)
{
    if (value == true) // turn on mDNS
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(F("MDNS Failed. WiFi not connected."));
        }
        else // Wifi is connected
        {
            if (MDNS.isRunning())
            {
                Serial.println(F("mDNS already running."));
                if (MDNS.setHostname(hostname) == true)
                {
                    Serial.print(F("Hostname changed to: "));
                    Serial.println(hostname);
                }
            }
            else // MDNS is not running
            {
                if (validSpooderID == true)
                {
                    Serial.print(F("Starting mDNS as: "));
                    Serial.print(hostname);
                    if (MDNS.begin(hostname) == true)
                    {
                        Serial.println(F(" succeeded."));
                        // add service announcements
                        MDNS.addService(0, "http", "tcp", 80); // for web access

                        // put own ID into dataset index 0
                        // get the position of '.'
                        strncpy(dataset[0].spooderID, hostname.c_str() + 7, strlen(hostname.c_str()) - 7);
                        // Serial.print(F("Copy ID into own dataset."));
                        // Serial.println(dataset[0].spooderID);
                    }
                    else
                    {
                        Serial.println(" failed.");
                    }
                }
                else // no valid spooder ID
                {
                    Serial.println("Invalid spooder ID, mDNS not started.");
                }
            } // MDNS is not running
        }

    }    // turn on mDNS
    else // turn off mDNS
    {
        if (MDNS.isRunning())
        {
            hMDNSServiceQuery = 0;
            MDNS.close();
            Serial.println(F("mDNS stopped."));
            clearDataset(0); // clear own data

            // close server, if opened

            // stop spooder client, if opended
        }
        else // MDNS is not running
        {
            Serial.println(F("mDNS not started."));
        }
    } // turn off mDNS

    return;
}
void FILAMENT_ESTIMATOR::installDynamicServiceQuery()
{
    if (!hMDNSServiceQuery)
    {
        hMDNSServiceQuery = MDNS.installServiceQuery("spooder", "tcp", globalMDNSServiceQueryCallback);
        if (hMDNSServiceQuery)
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' install succeeded."));
        }
        else
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' install failed."));
        }
    }
}
void FILAMENT_ESTIMATOR::removeDynamicServiceQuery()
{
    if (hMDNSServiceQuery)
    {
        if (MDNS.removeServiceQuery(hMDNSServiceQuery))
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' remove succeeded."));
            hMDNSServiceQuery = 0;
        }
        else
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' remove failed."));
        }
    }
}
void FILAMENT_ESTIMATOR::listSpooderService()
{
}
void FILAMENT_ESTIMATOR::updateWebsocketsServer()
{
    spooderServerListenForWebsocketsClients();
    spooderServerPollWebsocketsClients();
}
void FILAMENT_ESTIMATOR::updateWebsocketsClient()
{
    // websockets client side check for incoming messages
    if (webSocketsClient.available())
    {
        webSocketsClient.poll();
    }
}
void FILAMENT_ESTIMATOR::spooderServerListenForWebsocketsClients()
{
    if (webSocketsServer.poll())
    {
        int8_t freeIndex = getFreeClientIndex();
        if (freeIndex >= 0)
        {
            WebsocketsClient newClient = webSocketsServer.accept();
            Serial.printf("Accepted new websockets client at index %d\n", freeIndex);
            newClient.onMessage(globalHandleWebsocketsMessage);
            newClient.onEvent(globalHandleWebsocketsEvent);
            Serial.println(F("Send: Hello from server!"));
            newClient.send("Hello from server!");

            webSocketsClientsVector[freeIndex] = newClient;
        }
        else
        {
            Serial.println(F("Max clients reached. Not accepted."));
        }
    }
}
void FILAMENT_ESTIMATOR::spooderServerPollWebsocketsClients()
{
    for (byte i = 0; i < maxClients; i++)
    {
        webSocketsClientsVector[i].poll();
    }
}
int8_t FILAMENT_ESTIMATOR::getFreeClientIndex()
{
    // If a client in our list is not available, it's connection is closed and we
    // can use it for a new client.
    for (byte i = 0; i < maxClients; i++)
    {
        if (!webSocketsClientsVector[i].available())
            return i;
    }
    return -1;
}
void FILAMENT_ESTIMATOR::MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo *serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent)
{
    listMDNSDynamicQueryAnswer();
    return;
    // Not printing out answers
    /*
    if (answerType == MDNSResponder::AnswerType::IP4Address && p_bSetContent == true)
    {
        Serial.print("===== ");
        Serial.print(serviceInfo->hostDomain());
        Serial.print(":");
        Serial.print(serviceInfo->IP4Adresses()[0]);
        Serial.println(" Modified.");
        listMDNSDynamicQueryAnswer();
    }
    if (answerType == MDNSResponder::AnswerType::ServiceDomain && p_bSetContent == false)
    {
        Serial.print("===== ");
        Serial.print(serviceInfo->hostDomain());
        Serial.print(":");
        Serial.print(serviceInfo->IP4Adresses()[0]);
        Serial.println(" Deleted.");
        listMDNSDynamicQueryAnswer();
    }

    String answerInfo;
    answerInfo = String(serviceInfo->hostDomain()) + ": ";
    switch (answerType)
    {
    case MDNSResponder::AnswerType::ServiceDomain:
        answerInfo += "ServiceDomain " + String(serviceInfo->serviceDomain());
        break;
    case MDNSResponder::AnswerType::HostDomainAndPort:
        answerInfo += "HostDomainAndPort " + String(serviceInfo->hostDomain()) + ":" + String(serviceInfo->hostPort());
        break;
    case MDNSResponder::AnswerType::IP4Address:
        answerInfo += "IP4Address ";
        for (IPAddress ip : serviceInfo->IP4Adresses())
        {
            answerInfo += "- " + ip.toString();
        };
        break;
    case MDNSResponder::AnswerType::Txt:
        answerInfo += "TXT " + String(serviceInfo->strKeyValue());
        for (auto kv : serviceInfo->keyValues())
        {
            answerInfo += "\nkv : " + String(kv.first) + " : " + String(kv.second);
        }
        break;
    default:
        answerInfo = "Unknown Answertype";
    }
    Serial.printf("Answer %s %s\n", answerInfo.c_str(), p_bSetContent ? "Modified" : "Deleted");
    */
}
void FILAMENT_ESTIMATOR::updateDataset()
{
    if (millis() - updateDatsetTimer < UPDATE_DATASET_PERIOD)
    {
        return;
    }
    updateDatsetTimer = millis();
    if (!MDNS.isRunning())
    {
        return;
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    // update own dataset
    dataset[0].fw = (int16_t)filamentWeight;

    // announce to others
    updateServiceTxt();
}
void FILAMENT_ESTIMATOR::copyDatasetFromServiceInfo()
{
    // Serial.println(F("Copy all data from ServiceInfo into dataset:"));

    uint16_t index = 0; // used as dataset[index]
    // Serial.print(F("answerInfo size = "));
    // Serial.println(MDNS.answerInfo(hMDNSServiceQuery).size());
    if (MDNS.answerInfo(hMDNSServiceQuery).size() == 0)
        return;
    // Serial.println(MDNS.answerInfo(hMDNSServiceQuery).size());

    for (auto info : MDNS.answerInfo(hMDNSServiceQuery))
    {
        index++;
        // copy ID into dataset
        if (info.hostDomainAvailable())
        { // info.hostDomain() example: spooderA2.local
            if (strncmp(info.hostDomain(), "spooder", 7) == 0)
            {
                // get the position of '.'
                uint8_t pos;
                char *dotPtr = strchr(info.hostDomain(), '.');
                if (dotPtr != NULL)
                {
                    pos = dotPtr - info.hostDomain() + 1;
                    // Serial.print(F("'.' found at "));
                    // Serial.println(pos); //'.' found at 10
                    // copy 'A2' or 'A13' into spooderID
                    strncpy(dataset[index].spooderID, info.hostDomain() + 7, pos - 8);
                }
            }
        } // if (info.hostDomainAvailable())
        // copy txt info into dataset
        if (info.txtAvailable())
        {
            // FW: filament weight in gram
            if (info.value("FW") != nullptr)
            {
                dataset[index].fw = atoi(info.value("FW"));
                // Serial.println(dataset[index].fw);
            }
        } // if (info.txtAvailable())
        datasetLastPopulatedIndex = index;
    } // for (auto info : MDNS.answerInfo(hMDNSServiceQuery))
}
void FILAMENT_ESTIMATOR::staticQueryMDNS()
{
    if (MDNS.isRunning() == true)
    {
        Serial.println(F("Query 'spooder' services in the local network:"));
        int n = MDNS.queryService("spooder", "tcp"); // Send out query for esp tcp services
        Serial.println(F("mDNS query done."));
        if (n == 0)
        {
            Serial.println("No services found.");
        }
        else // services found
        {
            Serial.print(n);
            Serial.println(" service(s) found");
            for (int i = 0; i < n; ++i)
            {
                // Print details for each service found
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(MDNS.answerHostname(i));
                Serial.print(" (");
                Serial.print(MDNS.answerIP(i));
                Serial.print(":");
                Serial.print(MDNS.answerPort(i));
                Serial.print(") TXT:");
                Serial.print(MDNS.answerTxts(hMDNSServiceQuery, i));

                Serial.println();
            }
        }
    }
    else // MDNS is not running on this device
    {
        Serial.println(F("Failed. mDNS is not running."));
    }
    return;
}
void FILAMENT_ESTIMATOR::removeStaticQuery()
{
    if (MDNS.isRunning() == true)
    {
        if (MDNS.removeQuery())
        {
            Serial.println(F("Static query removed."));
        }
        else
        {
            Serial.println(F("Static query not removed."));
        }
    }
    else // MDNS is not running on this device
    {
        Serial.println(F("Failed. mDNS is not running."));
    }
}
void FILAMENT_ESTIMATOR::listMDNSDynamicQueryAnswer()
{
    uint32_t numAns = MDNS.answerCount(hMDNSServiceQuery);
    if (VERBOSE_DEBUG_PRINT == true)
    {
        // Serial.print(numAns);
        // Serial.println(" service(s) found");
        if (numAns > 0)
        {
            for (int i = 0; i < numAns; i++)
            {
                if (isAnswerValidServer(i) == true)
                {
                    // Print details for each service found
                    Serial.print(F("Valid server at index "));
                    Serial.print(i);
                    Serial.print(": ");
                    Serial.print(MDNS.answerHostDomain(hMDNSServiceQuery, i));
                    Serial.print(" (");
                    Serial.print(MDNS.answerIP4Address(hMDNSServiceQuery, i, 0)); // only list the first(index==0) ip
                    Serial.print(":");
                    Serial.print(MDNS.answerPort(hMDNSServiceQuery, i));
                    Serial.print(")");

                    Serial.println();
                }
                else
                {
                    // Serial.print(F("Invalid :"));
                }
            }
        }
    }
}
void FILAMENT_ESTIMATOR::updateServiceTxt()
{
    if (MDNS.isRunning() == false)
    {
        // Serial.println(F("Failed. mDNS is not running."));
        return;
    }

    // Serial.print(F("Update mDNS service txt: filament,"));
    // Serial.print((int16_t)filamentWeight);
    // Serial.println();

    // MDNS.addDynamicServiceTxt(spooderService, "FW", (int16_t)filamentWeight);
    //  'announce' can be called every time, the configuration of some service
    //  changes. Mainly, this would be changed content of TXT items.
    // MDNS.announce();
    return;
}
void FILAMENT_ESTIMATOR::printSpoodersDataset()
{
    if (MDNS.isRunning() == false)
    {
        Serial.println(F("Failed. mDNS is not running."));
        return;
    }

    copyDatasetFromServiceInfo();

    Serial.print(F("Total number of spooders:"));
    Serial.println(MDNS.answerInfo(hMDNSServiceQuery).size() + 1);

    Serial.println(F("Index\tID\tFW"));
    for (uint8_t i = 0; i <= datasetLastPopulatedIndex; i++)
    {
        if (i < 10)
        {
            Serial.print(F(" "));
        }
        Serial.print(i);
        Serial.print(F(":\t"));

        Serial.print(dataset[i].spooderID);
        Serial.print(F("\t"));
        Serial.print(dataset[i].fw);
        Serial.println();
    }

    return;
}
void FILAMENT_ESTIMATOR::clearDataset(uint8_t index)
{
    memset(dataset[0].spooderID, 0, DATASET_SPOODER_ID_SIZE);
}
void FILAMENT_ESTIMATOR::sortDataset()
{
    // Sort structure array using user defined
    // function complareTwoSpooders()
    std::sort(dataset, dataset + datasetLastPopulatedIndex + 1, globalComplareTwoSpooders);
}
void FILAMENT_ESTIMATOR::setBlynk(bool value)
{

    if (value == true) // turn on blynk
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(F("Blynk failed. WiFi not connected."));
            return;
        }
        enableBlynk = true;
        if (Blynk.connected() == true)
        {
            Serial.println(F("Blynk already connected."));
        }
        else // Blynk not connected yet
        {
            Serial.print(F("Connecting Blynk using:"));
            Serial.println(blynk_auth);

            // Not using Blynk.begin(), because it
            // 1.Connection to the network (WiFi, Ethernet, …)
            // 2.Call of Blynk.config(...) to set Auth Token, Server Address, etc.
            // 3.Attempts to connect to the server once (can block for more than 30s)
            // WiFi is controlled by code/user

            Blynk.config(blynk_auth);
            Blynk.connect(3000);

            if (Blynk.connected() == true)
            {
                Serial.println(F("Blynk connected."));
            }
            else
            {
                Serial.println(F("Blynk not connected."));
                if (Blynk.isTokenInvalid() == true)
                {
                    Serial.println(F("Blynk token is invalid."));
                }
            }
        }
    }
    else // turn off blynk
    {
        enableBlynk = false;
        if (Blynk.CONNECTED == true)
        {
            Blynk.disconnect();
            Serial.println(F("Blynk disonnected."));
        }
        else // Blynk not connected yet
        {
            Serial.print(F("Blynk not connected."));
        }
    }
    displayPage(currentPage); // Refresh page for blynk symbol change
    // globalPrintMemory("setBlynk: ");
}
void FILAMENT_ESTIMATOR::setWebServer(bool value)
{

    if (value == true) // turn on web server
    {
        if (WiFi.status() != WL_CONNECTED && isHotspotMode == false)
        {
            Serial.println(F("Web server failed. WiFi not connected."));
            return;
        }
        if (enableWebServer == true) // already enabled
        {
            Serial.println(F("Web Server already enabled."));
        }
        else // enable simple web server for file management
        {
            webServer = new ESP8266WebServer(80);

            // web server init for file system manger
            // Filesystem status
            webServer->on("/status", HTTP_GET, globalHandleStatus);

            // List directory
            webServer->on("/list", HTTP_GET, globalHandleFileList);

            // Load editor
            webServer->on("/edit", HTTP_GET, globalHandleGetEdit);

            // Create file
            webServer->on("/edit", HTTP_PUT, globalHandleFileCreate);

            // Delete file
            webServer->on("/edit", HTTP_DELETE, globalHandleFileDelete);

            // Upload file
            // - first callback is called after the request has ended with all parsed arguments
            // - second callback handles file upload at that location
            webServer->on("/edit", HTTP_POST, globalReplyOK, globalHandleFileUpload);

            // Spooder Setup Save
            webServer->on("/", HTTP_POST, globalHandleSetupSave);

            // Default handler for all URIs not defined above
            // Use it to read files from filesystem
            webServer->onNotFound(globalHandleNotFound);

            // Start server
            webServer->begin();
            enableWebServer = true;
            Serial.print(F("Web Server enabled at:  "));
            Serial.print(WiFi.localIP().toString());
            if (MDNS.isRunning() == true)
            {
                Serial.print(F("  or  "));
                Serial.print(hostname);
                Serial.print(F(".local"));
            }
            Serial.println();
        }
    }    // turn on web server
    else // trun off web server
    {
        if (enableWebServer == true) // disable web server
        {
            webServer->close();
            delete webServer;
            enableWebServer = false;
            Serial.println(F("Web Server disabled."));
        }
        else // not enabled
        {
            Serial.println(F("Web Server already disabled."));
        }
    } // trun off web server
    // globalPrintMemory("setServer: ");
    return;
}
void FILAMENT_ESTIMATOR::setSpooderClient(bool value)
{

    if (value == true) // turn on Spooder Client
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(F("Spooder Client failed. WiFi not connected."));
            return;
        }
        if (enableSpooderClient == true) // already enabled
        {
            Serial.println(F("Spooder Client already enabled."));
        }
        else // enable SpooderClient
        {
            Serial.println(F("Spooder Client enabled"));
            installDynamicServiceQuery();
            enableSpooderClient = true;
        }
    }    // turn on Spooder Client
    else // trun off Spooder Client
    {
        if (enableSpooderClient == true) // disable Spooder Client
        {
            Serial.println(F("Spooder Client disabled."));
            removeDynamicServiceQuery();
            enableSpooderClient = false;
        }
        else // SpooderClient not enabled
        {
            Serial.println(F("Spooder Client already disabled."));
        }
    } // trun off Spooder Client
    return;
}
void FILAMENT_ESTIMATOR::setSpooderServer(bool value)
{
    if (value == true) // turn on Spooder Server
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(F("SpooderServer failed. WiFi not connected."));
            return;
        }
        if (enableSpooderServer == true) // already enabled
        {
            Serial.println(F("Spooder Server already enabled."));
        }
        else // enable Spooder Server
        {
            if (!spooderService)
            {
                spooderService = MDNS.addService(0, "spooder", "tcp", websocketsPort); // for inter-device data exchange, using tcp packet
                if (spooderService)
                {
                    Serial.print(F("Spooder Service: 'spooder.tcp' port:"));
                    Serial.print(websocketsPort);
                    Serial.println(F(" install succeeded."));

                    // Configure websockets server and clients
                    webSocketsServer.listen(websocketsPort);

                    Serial.print(F("Websocket server is "));
                    Serial.print(webSocketsServer.available() ? "" : "not ");
                    Serial.println("started.");
                    Serial.print(F("Max number of clients: "));
                    Serial.println(maxClients);

                    // globalPrintMemory("Before client instantiation");

                    // create clients dynamically so that memory can be released
                    for (uint8_t i = 0; i < maxClients; i++)
                    {
                        WebsocketsClient client;
                        webSocketsClientsVector.push_back(client);
                    }
                    // globalPrintMemory("After client instantiation");
                    /*
                    Websocket server is started.
                    Max number of clients: 20
                    Before client instantiation Heap: 31616    Block: 31336
                    After client instantiation Heap: 25496    Block: 21464
                    */

                    // Enable websockets server handling
                    enableSpooderServer = true;
                }
                else
                {
                    Serial.print(F("Spooder Service: 'spooder.tcp' port:"));
                    Serial.print(websocketsPort);
                    Serial.println(F(" install failed."));
                    enableSpooderServer = false;
                    spooderService = 0;
                }
            }
            else
            {
                Serial.println(F("Spooder Service already exists."));
                enableSpooderServer = true;
            }
        }
    }    // turn on Spooder Server
    else // trun off Spooder Server
    {
        if (enableSpooderServer == true) // disable Spooder Server
        {
            if (spooderService)
            {
                MDNS.removeService(spooderService);
                MDNS.announce();
                Serial.println(F("Spooder Service: 'spooder.tcp' removed."));
                // globalPrintMemory("Before client removal"); // Heap: 25872    Block: 22096
                std::vector<WebsocketsClient>().swap(webSocketsClientsVector); // This frees up memory
                // globalPrintMemory("After client removal"); //Heap: 31992    Block: 30184
                spooderService = 0;
                enableSpooderServer = false;
            }
            else
            {
                Serial.println(F("Spooder Service not running."));
                enableSpooderServer = false;
            }
        }
        else // Spooder Server not enabled
        {
            Serial.println(F("Spooder Server already disabled."));
        }
    } // trun off Spooder Server
    return;
}
void FILAMENT_ESTIMATOR::setArduinoOTA(bool value)
{

    if (value == true) // enable arduino OTA
    {

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(F("ArduinoOTA failed. WiFi not connected."));
            if (MDNS.isRunning() == false)
            {
                Serial.println(F("mDNS not running."));
            }
            return;                   // conditions not met
        }                             //(WiFi.status() != WL_CONNECTED)
        if (enableArduinoOTA == true) // already enabled
        {
            Serial.println(F("Arduino OTA already enabled."));
        }
        else // enable arduino OTA
        {
            ArduinoOTA = new ArduinoOTAClass;
            // Setup for OTA
            ArduinoOTA->setHostname(hostname.c_str()); // set ArduinoOTA hostname before onStart()
            ArduinoOTA->onStart([]()
                                { Serial.println("Start"); });
            ArduinoOTA->onEnd([]()
                              { Serial.println("\nEnd"); });
            ArduinoOTA->onProgress([](unsigned int progress, unsigned int total)
                                   { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
            ArduinoOTA->onError([](ota_error_t error)
                                {
                                        Serial.printf("Error[%u]: ", error);
                                        if (error == OTA_AUTH_ERROR)
                                            Serial.println("Auth Failed");
                                        else if (error == OTA_BEGIN_ERROR)
                                            Serial.println("Begin Failed");
                                        else if (error == OTA_CONNECT_ERROR)
                                            Serial.println("Connect Failed");
                                        else if (error == OTA_RECEIVE_ERROR)
                                            Serial.println("Receive Failed");
                                        else if (error == OTA_END_ERROR)
                                            Serial.println("End Failed"); });
            ArduinoOTA->begin();
            enableArduinoOTA = true;

            Serial.print(F("ArduinoOTA hostname: "));
            Serial.println(ArduinoOTA->getHostname());
            Serial.println(F("Arduino OTA enabled."));
        }
    }    //(value == true) //enable arduino OTA
    else //(value == false) disable arduino OTA
    {
        if (enableArduinoOTA == true) // disable arduino OTA
        {
            delete ArduinoOTA;
            enableArduinoOTA = false;
            Serial.println(F("Arduino OTA disabled."));
        }
        else // not enabled
        {
            Serial.println(F("Arduino OTA already disabled."));
        }
    } //(value == false) disable arduino OTA

    // globalPrintMemory("setArduinoOTA: ");
}
void FILAMENT_ESTIMATOR::setAutoHomepage(bool value)
{
    if (value == true) // turn on auto homepage
    {
        setting.optionsAutoHomepage = true;
        returnToHomepageTimer = millis();
        Serial.println(F("Auto homepage enabled."));
    }
    else // turn off auto homepage
    {
        setting.optionsAutoHomepage = false;
        Serial.println(F("Auto homepage disabled."));
    }
    return;
}
void FILAMENT_ESTIMATOR::checkConnectionStatus()
{
    if (millis() - checkConnectionTimer < CHECK_CONNECTION_PERIOD)
        return;
    if (enableWifi == false)
    {
        connectionStatus = CONNECTION_STATUS_NONE;
        return;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        connectionStatus = CONNECTION_STATUS_WIFI_AND_INTERNET;
        // Todo: check internet connection
        // checkGithubTag();
        return;
    }
    else
    {
        connectionStatus = CONNECTION_STATUS_NO_WIFI;
    }

    checkConnectionTimer = millis();
}
void FILAMENT_ESTIMATOR::checkConnectionDisplaySymbol()
{
    switch (connectionStatus)
    {
    case CONNECTION_STATUS_NONE:
        symbolType = SYMBOL_NONE;
        break;
    case CONNECTION_STATUS_NO_WIFI:
        symbolType = SYMBOL_NO_WIFI;
        break;
    case CONNECTION_STATUS_WIFI_AND_INTERNET:
        symbolType = SYMBOL_WIFI_AND_INTERNET;
        break;
    case CONNECTION_STATUS_WIFI_NO_INTERNET:
        if (millis() - checkConnectionDisplaySymbolTimer > CHANGE_CONNECTION_SYMBOL_PERIOD)
        {
            if (symbolType == SYMBOL_WIFI_NO_INTERNET_1)
            {
                symbolType = SYMBOL_WIFI_NO_INTERNET_2;
            }
            else
            {
                symbolType = SYMBOL_WIFI_NO_INTERNET_1;
            }
            checkConnectionDisplaySymbolTimer = millis();
        }

        break;
    }
}
void FILAMENT_ESTIMATOR::buttonHandler(Button2 &btn)
{
    switch (btn.getClickType())
    {
    case SINGLE_CLICK:
        switch (currentPage)
        {
        case PAGE_MENU:
            switch (menuIndex)
            {
            case MENU_TARE:
                setPage(PAGE_TARE);
                break;
            case MENU_CALIBRATE:
                setPage(PAGE_CALIBRATE);
                break;
            case MENU_SPOOL_HOLDER_WEIGHT:
                setPage(PAGE_SPOOL_HOLDER_WEIGHT);
                break;
            case MENU_SET_SPOODER_ID:
                setPage(PAGE_SET_SPOODER_ID);
                break;
            case MENU_LOW_FILAMENT_SETUP:
                setPage(PAGE_LOW_FILAMENT_SETUP);
                break;
            case MENU_NOTIFICATION:
                setPage(PAGE_NOTIFICATION);
                break;
            case MENU_ENTER_HOTSPOT_MODE:
                enterHotspotMode();
                setPage(PAGE_ENTER_HOTSPOT_MODE);
                break;
            case MENU_FIRMWARE_UPDATE:
                setPage(PAGE_FIRMWARE_UPDATE);
                break;
            case MENU_OPTIONS:
                setPage(PAGE_OPTIONS);
                break;
            case MENU_DEBUG:
                setPage(PAGE_DEBUG);
                break;
            }

            break;
        case PAGE_TARE:
            if (tareSelection == TARE_OK)
            {
                tare();
                setPage(PAGE_MENU);
            }
            else if (tareSelection == TARE_CANCEL)
            {
                // Reset the selection to ok, for next entry
                tareSelection = TARE_OK;
                setPage(PAGE_MENU);
            }
            break;
        case PAGE_HOME:
            switch (displayType)
            {
            case DISPLAY_TYPE_TOTAL:
                displayType = DISPLAY_TYPE_FILAMENT;
                break;
            case DISPLAY_TYPE_FILAMENT:
                displayType = DISPLAY_TYPE_SPOOL_HOLDER;
                break;
            case DISPLAY_TYPE_SPOOL_HOLDER:
                displayType = DISPLAY_TYPE_TOTAL;
                break;
            }
            break;
        case PAGE_CALIBRATE:
            switch (calibrateSelection)
            {
            case CALIBRATE_4_DIGIT:
            case CALIBRATE_3_DIGIT:
            case CALIBRATE_2_DIGIT:
            case CALIBRATE_1_DIGIT:
                if (calibrateEditDigitMode == false)
                {
                    calibrateEditDigitMode = true;
                    displayCalibrateDigit = false;
                    displayPage(PAGE_CALIBRATE);
                    calibrateEditModerTimer = millis();
                }
                else
                {
                    calibrateEditDigitMode = false;
                    displayCalibrateDigit = true;
                    displayPage(PAGE_CALIBRATE);
                }
                break;
            case CALIBRATE_OK:
                calibrateEditDigitMode = false;
                // Reset the selection to the fourth digit, for next entry
                calibrateSelection = CALIBRATE_4_DIGIT;
                if (getCalibrationWeight() != setting.calibrationWeight)
                {
                    setting.calibrationWeight = getCalibrationWeight();
                    saveToEEPROM(); // store the newly selected calibration weight
                }
                calibrate();
                setPage(PAGE_CALIBRATE_CONFIRM);
                break;
            case CALIBRATE_CANCEL:
                calibrateEditDigitMode = false;
                // Reset the selection to the fourth digit, for next entry
                calibrateSelection = CALIBRATE_4_DIGIT;
                if (getCalibrationWeight() != setting.calibrationWeight)
                {
                    setting.calibrationWeight = getCalibrationWeight();
                    saveToEEPROM(); // store the newly selected calibration weight
                }
                setPage(PAGE_MENU);
                break;
            }
            break;
        case PAGE_SPOOL_HOLDER_WEIGHT:
            switch (spoolHolderSelection)
            {

            case SPOOL_HOLDER_3_DIGIT:
            case SPOOL_HOLDER_2_DIGIT:
            case SPOOL_HOLDER_1_DIGIT:
                if (spoolHolderEditDigitMode == false)
                {
                    spoolHolderEditDigitMode = true;
                    displaySpoolHolderDigit = false;
                    displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    spoolHolderEditModerTimer = millis();
                }
                else
                {
                    spoolHolderEditDigitMode = false;
                    displaySpoolHolderDigit = true;
                    displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                }
                break;
            case SPOOL_HOLDER_OK:
                spoolHolderEditDigitMode = false;
                // Reset the selection to the third digit, for next entry
                spoolHolderSelection = SPOOL_HOLDER_3_DIGIT;
                setting.spoolHolderWeight = getSpoolHolderWeight();
                saveToEEPROM();
                setPage(PAGE_MENU);
                break;
            case SPOOL_HOLDER_CANCEL:
                spoolHolderEditDigitMode = false;
                // Reset the selection to the third digit, for next entry
                spoolHolderSelection = SPOOL_HOLDER_3_DIGIT;
                setPage(PAGE_MENU);
                break;
            default: // other than above, which means loading preset slot value into current weight
            {
                uint8_t slotIndex = spoolHolderSelection - SPOOL_HOLDER_SLOT_1;
                spoolHolder3Digit = (spoolHolderSlotWeight[slotIndex] / 100U) % 10;
                spoolHolder2Digit = (spoolHolderSlotWeight[slotIndex] / 10U) % 10;
                spoolHolder1Digit = (spoolHolderSlotWeight[slotIndex] / 1U) % 10;
                spoolHolderSelection = SPOOL_HOLDER_OK;
                displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                break;
            }
            }
            break;
        case PAGE_SET_SPOODER_ID:
            switch (setSpooderIDSelection)
            {

            case SET_SPOODER_ID_LETTER:
            case SET_SPOODER_ID_NUMBER:
                if (setSpooderIDEditMode == false)
                {
                    setSpooderIDEditMode = true;
                    displaySpooderIDDigit = false;
                    displayPage(PAGE_SET_SPOODER_ID);
                    setSpooderIDTimer = millis();
                }
                else
                {
                    setSpooderIDEditMode = false;
                    displaySpooderIDDigit = true;
                    displayPage(PAGE_SET_SPOODER_ID);
                }
                break;
            case SET_SPOODER_ID_OK:
                setSpooderIDEditMode = false;
                // Reset the selection to the letter digit, for next entry
                setSpooderIDSelection = SET_SPOODER_ID_LETTER;
                if (spooderIDLetter >= 1 && spooderIDLetter <= 26)
                {
                    if (spooderIDNumber >= 0 && spooderIDNumber <= 99)
                    {
                        // valid new spooder ID
                        setting.spooderIDLetter = spooderIDLetter;
                        setting.spooderIDNumber = spooderIDNumber;
                        setting.spooderIDSetStatus = SPOODER_ID_USER_SET;
                        saveToEEPROM();
                        validSpooderID = true;
                        hostname = "spooder" + String((char)(spooderIDLetter + 64)) + String(spooderIDNumber);
                        Serial.print("Reset hostname to: ");
                        Serial.println(hostname);
                        setMDNS(setting.optionsMDNS); // change hostname if mDNS is enabled
                    }
                }

                // saveToEEPROM();

                setPage(PAGE_MENU);
                break;
            case SET_SPOODER_ID_CANCEL:
                spoolHolderEditDigitMode = false;
                // Reset the selection to the letter digit, for next entry
                setSpooderIDSelection = SET_SPOODER_ID_LETTER;
                if (validSpooderID == false)
                {
                    // reset to invalid value for next entry
                    spooderIDLetter = 0;
                    spooderIDNumber = 0;
                }
                setPage(PAGE_MENU);
                break;
            default: // other than above, which means loading preset slot value into current weight
            {
                uint8_t slotIndex = spoolHolderSelection - SPOOL_HOLDER_SLOT_1;
                spoolHolder3Digit = (spoolHolderSlotWeight[slotIndex] / 100U) % 10;
                spoolHolder2Digit = (spoolHolderSlotWeight[slotIndex] / 10U) % 10;
                spoolHolder1Digit = (spoolHolderSlotWeight[slotIndex] / 1U) % 10;
                spoolHolderSelection = SPOOL_HOLDER_OK;
                displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                break;
            }
            }
            break;
        case PAGE_CALIBRATE_CONFIRM:

            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_OK:

                setting.calValue = newCalibrationValue;
                saveToEEPROM();
                break;
            case CALIBRATE_SAVE_CANCEL:
                // Reset the selection to ok
                calibrateSaveSelection = CALIBRATE_SAVE_OK;
                break;
            }
            setPage(PAGE_MENU);
            break;
        case PAGE_LOW_FILAMENT_SETUP:
            switch (lowFilamentSelection)
            {
            case LOW_FILAMENT_4_DIGIT:
            case LOW_FILAMENT_3_DIGIT:
            case LOW_FILAMENT_2_DIGIT:
            case LOW_FILAMENT_1_DIGIT:
                if (lowFilamentEditDigitMode == false)
                {
                    lowFilamentEditDigitMode = true;
                    displayLowFilamentDigit = false;
                    displayPage(PAGE_LOW_FILAMENT_SETUP);
                    lowFilamentEditModerTimer = millis();
                }
                else
                {
                    lowFilamentEditDigitMode = false;
                    displayLowFilamentDigit = true;
                    displayPage(PAGE_LOW_FILAMENT_SETUP);
                }
                break;
            case CALIBRATE_OK:
                lowFilamentEditDigitMode = false;
                // Reset the selection to the second digit, for next entry
                lowFilamentSelection = LOW_FILAMENT_2_DIGIT;
                setting.lowFilamentThreshold = getLowFilamentThreshold();
                saveToEEPROM();
                setPage(PAGE_MENU);
                break;
            case CALIBRATE_CANCEL:
                lowFilamentEditDigitMode = false;
                // Reset the selection to the second digit, for next entry
                lowFilamentSelection = LOW_FILAMENT_2_DIGIT;
                // Revert to original value
                lowFilament4Digit = (setting.lowFilamentThreshold / 1000U) % 10;
                lowFilament3Digit = (setting.lowFilamentThreshold / 100U) % 10;
                lowFilament2Digit = (setting.lowFilamentThreshold / 10U) % 10;
                lowFilament1Digit = (setting.lowFilamentThreshold / 1U) % 10;
                setPage(PAGE_MENU);
                break;
            }
            break;
        case PAGE_NOTIFICATION:
        {
            switch (notificationMenuSelection)
            {
            case NOTIFICATION_MENU_PRINT_STARTED:
            case NOTIFICATION_MENU_PRINT_COMPLETED:
            case NOTIFICATION_MENU_LOW_FILAMENTT:
            case NOTIFICATION_MENU_FALL_OFF_RACK:
            case NOTIFICATION_MENU_FALL_OFF_BEARING:
            case NOTIFICATION_MENU_TANGLED:
                // Toogle the selected settings
                setNotificationSetting(notificationMenuSelection, !getNotificationSetting(notificationMenuSelection));
                displayPage(PAGE_NOTIFICATION);
                break;
            case NOTIFICATION_MENU_RETURN:
                notificationMenuSelection = NOTIFICATION_MENU_PRINT_STARTED;
                notificationMenuItemStartIndex = NOTIFICATION_MENU_PRINT_STARTED;
                setPage(PAGE_MENU);
                break;
            }
            break; // case PAGE_NOTIFICATION:
        }
        case PAGE_ENTER_HOTSPOT_MODE:
        {
            exitHotspotMode();
            setPage(PAGE_MENU);

            break; // case PAGE_ENTER_HOTSPOT_MODE:
        }
        case PAGE_FIRMWARE_UPDATE:
        {
            switch (updateMenuSelection)
            {
            case FIRMWARE_UPDATE_MENU_CHECK_NOW:
            {
                checkGithubLatestRelease(true, false, false, false);
                displayPage(currentPage);
                break;
            }
            case FIRMWARE_UPDATE_MENU_UPDATE_NOW:
            {
                checkGithubLatestRelease(false, true, false, false);
                displayPage(currentPage);
                break;
            }
            case FIRMWARE_UPDATE_MENU_AUTO_UPDATE:
            {
                setting.autoGithubUpdate = !setting.autoGithubUpdate;
                Serial.print(F("Auto Github Update set to: "));
                Serial.println((setting.autoGithubUpdate == true) ? "On" : "Off");
                saveToEEPROM();
                if (setting.autoGithubUpdate == true)
                {
                    // reset nextCheckCountdown whenever autoGithubUpdate set to on
                    setNextCheckCountdown();
                }
                displayPage(currentPage);
                break;
            }
            case FIRMWARE_UPDATE_MENU_RETURN_TO_MENU:
            {
                updateMenuSelection = FIRMWARE_UPDATE_MENU_CHECK_NOW;
                updateMenuItemStartIndex = FIRMWARE_UPDATE_MENU_CHECK_NOW;
                setPage(PAGE_MENU);
                break;
            }

            default:
                break;
            }      // switch (notificationMenuSelection)
            break; // case PAGE_FIRMWARE_UPDATE:
        }
        case PAGE_OPTIONS:
        {
            switch (optionsMenuSelection)
            {
            case OPTIONS_MENU_WIFI:
            {
                // Toogle the selected settings
                setOptionsSetting(OPTIONS_MENU_WIFI, !getOptionsSetting(OPTIONS_MENU_WIFI));
                displayPage(PAGE_OPTIONS);
                setWifi(getOptionsSetting(OPTIONS_MENU_WIFI)); // turn WiFi on or off
                break;
            }
            case OPTIONS_MENU_MDNS:
            {
                // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                displayPage(PAGE_OPTIONS);
                setMDNS(getOptionsSetting(OPTIONS_MENU_MDNS)); // turn mDNS on or off
                break;
            }
            case OPTIONS_MENU_BLYNK:
            {
                // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                displayPage(PAGE_OPTIONS);
                setBlynk(getOptionsSetting(OPTIONS_MENU_BLYNK)); // turn blynk on or off
                break;
            }
            case OPTIONS_MENU_WEB_SERVER:
            {
                // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                displayPage(PAGE_OPTIONS);
                setWebServer(getOptionsSetting(OPTIONS_MENU_WEB_SERVER)); // turn web server on or off
                break;
            }
            case OPTIONS_MENU_SPOODER_CLIENT:
            {
                // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                displayPage(PAGE_OPTIONS);
                setSpooderClient(getOptionsSetting(OPTIONS_MENU_SPOODER_CLIENT)); // turn Spooder Client on or off
                break;
            }
            case OPTIONS_MENU_SPOODER_SERVER:
            {
                // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                displayPage(PAGE_OPTIONS);
                setSpooderServer(getOptionsSetting(OPTIONS_MENU_SPOODER_SERVER)); // turn Spooder Server on or off
                break;
            }
            case OPTIONS_MENU_ARDUINO_OTA:
            { // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                setArduinoOTA(getOptionsSetting(OPTIONS_MENU_ARDUINO_OTA)); // turn arduino OTA on or off
                displayPage(PAGE_OPTIONS);
                break;
            }
            case OPTIONS_MENU_AUTO_HOMEPAGE:
            { // Toogle the selected settings
                setOptionsSetting(optionsMenuSelection, !getOptionsSetting(optionsMenuSelection));
                setAutoHomepage(getOptionsSetting(OPTIONS_MENU_AUTO_HOMEPAGE)); // turn arduino OTA on or off
                displayPage(PAGE_OPTIONS);
                break;
            }
            case OPTIONS_MENU_RETURN:
                optionsMenuSelection = OPTIONS_MENU_WIFI;
                optionsMenuItemStartIndex = OPTIONS_MENU_WIFI;
                setPage(PAGE_MENU);
                break;
            }

            break;
        }
        case PAGE_DEBUG:
            switch (debugMenuSelection)
            {
            case DEBUG_LOAD_TO_SETTING:
                loadToSetting();
                break;
            case DEBUG_SAVE_TO_EEPROM:
                saveToEEPROM();
                break;
            case DEBUG_DUMP_SETTING:
                dumpSetting();
                break;
            case DEBUG_DUMP_EEPROM:
                dumpEEPROM();
                break;
            case DEBUG_ERASE_EEPROM:
                eraseEEPROM();
                break;
            case DEBUG_LIST_DIRECTORY:
                listDir("");
                break;
            case DEBUG_LOAD_CONFIG:
                loadConfig();
                break;
            case DEBUG_DUMP_CONFIG:
                dumpConfig();
                break;
            case DEBUG_REBOOT:
                Serial.println(F("Reboot spooder."));
                ESP.restart();
                break;
            case DEBUG_BLYNK_NOTIFY:
                notify(NOTIFICATION_TEST_MESSAGE);
                break;
            case DEBUG_START_LOGGING:
                startLogging();
                drawOverlay("Start", "Logging", 1000);
                break;
            case DEBUG_STOP_LOGGING:
                stopLogging();
                drawOverlay("Stop", "Logging", 1000);
                break;
            case DEBUG_RUN_LOG_TXT:
                startEmulation();
                break;
            case DEBUG_STOP_LOG_TXT:
                stopEmulation();
                break;
            case DEBUG_TOGGLE_DETECTION_OUTPUT:
                detectionDebugOutput = !detectionDebugOutput;
                Serial.print(F("Detection output "));
                if (detectionDebugOutput == true)
                {
                    Serial.println(F("on."));
                }
                else
                {
                    Serial.println(F("off."));
                }
                break;
            case DEBUG_STATIC_QUERY_MDNS:
                staticQueryMDNS();
                break;
            case DEBUG_REMOVE_STATIC_QUERY:
                removeStaticQuery();
                break;
            case DEBUG_LIST_DYNAMIC_QUERY_ANSWER:
                listMDNSDynamicQueryAnswer();
                break;
            case DEBUG_UPDATE_SERVICE_TXT:
                // drawOverlay("Update", "Srvc Txt", 1000);
                updateServiceTxt();
                break;
            case DEBUG_PRINT_SPOODERS_DATASET:
                // drawOverlay("Print", "Spooders", 1000);
                printSpoodersDataset();
                break;
            case DEBUG_CHECK_DATA_FOLDER:
                checkGithubLatestRelease(false, false, true, false);
                break;
            case DEBUG_UPDATE_DATA_FOLDER:
                checkGithubLatestRelease(false, false, true, true);
                break;
            case DEBUG_PRINT_MEMORY:
                globalPrintMemory("Debug print memory:");
                break;
            case DEBUG_MANAGE_LOCAL_FILES:
                manageLocalFileInfo();
                break;
            case DEBUG_TOOGLE_VERBOSE_DEBUG:
                toogleVerboseDebug();
                break;
            case DEBUG_CONNECT_SPOODER_SERVER:
                connectSpooderServer(0);
                break;
            case DEBUG_LIST_SPOODER_SERVICE:
                // listSpooderService();
                break;
            case DEBUG_RETURN:
                debugMenuSelection = DEBUG_LOAD_TO_SETTING;
                debugMenuItemStartIndex = DEBUG_LOAD_TO_SETTING;
                setPage(PAGE_MENU);
                break;
            }
            break;
        }
        break; // case SINGLE_CLICK:
    case DOUBLE_CLICK:
    {

        break;
    }

    case TRIPLE_CLICK:
    {

        break;
    }
    case LONG_CLICK:
    {
        break;
    }
    } // switch (btn.getClickType())
    returnToHomepageTimer = millis();
}
void FILAMENT_ESTIMATOR::rotaryHandler(ESPRotary &rty)
{
    uint8_t direction = rotary.getDirection();
    // Serial.println(direction);
    switch (direction)
    {
    case RE_LEFT:
    {
        switch (currentPage)
        {
        case PAGE_MENU:
        {
            if (menuIndex > 0)
            {
                menuIndex--;
                if (menuIndex < menuItemStartIndex)
                {
                    menuItemStartIndex--;
                }
                setPage(PAGE_MENU);
            }
            else
            {
                setPage(PAGE_INFO);
            }
            break;
        }
        case PAGE_INFO:
        {
            setPage(PAGE_HOME);
            break;
        }
        case PAGE_CALIBRATE:
        {
            switch (calibrateEditDigitMode)
            {
            case true:
                switch (calibrateSelection)
                {
                case CALIBRATE_4_DIGIT:
                    if (calibrate4Digit > 0)
                    {
                        calibrate4Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }

                    break;
                case CALIBRATE_3_DIGIT:
                    if (calibrate3Digit > 0)
                    {
                        calibrate3Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_2_DIGIT:
                    if (calibrate2Digit > 0)
                    {
                        calibrate2Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_1_DIGIT:
                    if (calibrate1Digit > 0)
                    {
                        calibrate1Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                }
                break;
            case false:
                if (calibrateSelection > CALIBRATE_4_DIGIT)
                {
                    calibrateSelection = calibrateSelection - 1;
                    setPage(PAGE_CALIBRATE);
                }
                break;
            }
            break;
        }
        case PAGE_TARE:
        {
            if (tareSelection == TARE_CANCEL)
            {
                tareSelection = TARE_OK;
                setPage(PAGE_TARE);
            }

            break;
        }
        case PAGE_CALIBRATE_CONFIRM:
        {
            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_CANCEL:
                calibrateSaveSelection = CALIBRATE_SAVE_OK;
                displayPage(PAGE_CALIBRATE_CONFIRM);
                break;
            }
            break;
        }
        case PAGE_SPOOL_HOLDER_WEIGHT:
        {
            switch (spoolHolderEditDigitMode)
            {
            case true:
                switch (spoolHolderSelection)
                {
                case SPOOL_HOLDER_3_DIGIT:
                    if (spoolHolder3Digit > 0)
                    {
                        spoolHolder3Digit--;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_2_DIGIT:
                    if (spoolHolder2Digit > 0)
                    {
                        spoolHolder2Digit--;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_1_DIGIT:
                    if (spoolHolder1Digit > 0)
                    {
                        spoolHolder1Digit--;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                }
                break;
            case false:
                if (spoolHolderSelection > SPOOL_HOLDER_3_DIGIT)
                {
                    spoolHolderSelection = spoolHolderSelection - 1;
                    if (spoolHolderSelection < spoolHolderSlotStartIndex)
                    {
                        spoolHolderSlotStartIndex--;
                        if (spoolHolderSlotStartIndex < SPOOL_HOLDER_SLOT_1)
                        {
                            spoolHolderSlotStartIndex = SPOOL_HOLDER_SLOT_1;
                        }
                    }
                    setPage(PAGE_SPOOL_HOLDER_WEIGHT);
                }
                break;
            }
            break;
        }
        case PAGE_SET_SPOODER_ID:
        {
            switch (setSpooderIDEditMode)
            {
            case true:
                switch (setSpooderIDSelection)
                {
                case SET_SPOODER_ID_LETTER:
                    if (spooderIDLetter > 1)
                    {
                        spooderIDLetter--;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                case SET_SPOODER_ID_NUMBER:
                    if (spooderIDNumber > 1)
                    {
                        spooderIDNumber--;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                }
                break;
            case false:
                if (setSpooderIDSelection > SET_SPOODER_ID_LETTER)
                {
                    setSpooderIDSelection = setSpooderIDSelection - 1;
                    setPage(PAGE_SET_SPOODER_ID);
                }
                break;
            }
            break;
        }
        case PAGE_LOW_FILAMENT_SETUP:
        {
            switch (lowFilamentEditDigitMode)
            {
            case true:
                switch (lowFilamentSelection)
                {
                case LOW_FILAMENT_4_DIGIT:
                    if (lowFilament4Digit > 0)
                    {
                        lowFilament4Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }

                    break;
                case LOW_FILAMENT_3_DIGIT:
                    if (lowFilament3Digit > 0)
                    {
                        lowFilament3Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_2_DIGIT:
                    if (lowFilament2Digit > 0)
                    {
                        lowFilament2Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_1_DIGIT:
                    if (lowFilament1Digit > 0)
                    {
                        lowFilament1Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                }
                break;
            case false:
                if (lowFilamentSelection > LOW_FILAMENT_4_DIGIT)
                {
                    lowFilamentSelection = lowFilamentSelection - 1;
                    setPage(PAGE_LOW_FILAMENT_SETUP);
                }
                break;
            }
            break;
        }
        case PAGE_NOTIFICATION:
        {
            if (notificationMenuSelection > NOTIFICATION_MENU_PRINT_STARTED)
            {
                notificationMenuSelection--;
                if (notificationMenuSelection < notificationMenuItemStartIndex)
                {
                    notificationMenuItemStartIndex--;
                }
                setPage(PAGE_NOTIFICATION);
            }

            break;
        }
        case PAGE_FIRMWARE_UPDATE:
        {
            if (updateMenuSelection > FIRMWARE_UPDATE_MENU_CHECK_NOW)
            {
                updateMenuSelection--;
                if (updateMenuSelection < updateMenuItemStartIndex)
                {
                    updateMenuItemStartIndex--;
                }
                setPage(PAGE_FIRMWARE_UPDATE);
            }
            break;
        }
        case PAGE_OPTIONS:
        {
            if (optionsMenuSelection > OPTIONS_MENU_WIFI)
            {
                optionsMenuSelection--;
                if (optionsMenuSelection < optionsMenuItemStartIndex)
                {
                    optionsMenuItemStartIndex--;
                }
                displayPage(PAGE_OPTIONS);
            }
            break;
        }
        case PAGE_DEBUG:
        {
            if (debugMenuSelection > DEBUG_LOAD_TO_SETTING)
            {
                debugMenuSelection--;
                if (debugMenuSelection < debugMenuItemStartIndex)
                {
                    debugMenuItemStartIndex--;
                }
                setPage(PAGE_DEBUG);
            }

            break;
        }

        } // switch (currentPage)
        break;
    } // case RE_LEFT:
    case RE_RIGHT:
    {
        switch (currentPage)
        {
        case PAGE_HOME:
        {
            setPage(PAGE_INFO);
            break;
        }
        case PAGE_INFO:
        {
            setPage(PAGE_MENU);
            break;
        }
        case PAGE_MENU:
        {
            if (menuIndex < (numberOfMenuItems - 1))
            {
                menuIndex++;
                if (menuIndex >= menuItemStartIndex + menuItemPerPage)
                {
                    menuItemStartIndex++;
                }
                setPage(PAGE_MENU);
            }
            break;
        }
        case PAGE_TARE:
        {
            if (tareSelection == TARE_OK)
            {
                tareSelection = TARE_CANCEL;
                setPage(PAGE_TARE);
            }
            break;
        }
        case PAGE_CALIBRATE:
        {
            switch (calibrateEditDigitMode)
            {
            case true:
                switch (calibrateSelection)
                {
                case CALIBRATE_4_DIGIT:
                    if (calibrate4Digit < 9)
                    {
                        calibrate4Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }

                    break;
                case CALIBRATE_3_DIGIT:
                    if (calibrate3Digit < 9)
                    {
                        calibrate3Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_2_DIGIT:
                    if (calibrate2Digit < 9)
                    {
                        calibrate2Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_1_DIGIT:
                    if (calibrate1Digit < 9)
                    {
                        calibrate1Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                }
                break;
            case false:
                if (calibrateSelection < CALIBRATE_CANCEL)
                {
                    calibrateSelection = calibrateSelection + 1;
                    setPage(PAGE_CALIBRATE);
                }
                break;
            }
            break;
        }
        case PAGE_CALIBRATE_CONFIRM:
        {
            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_OK:
                calibrateSaveSelection = CALIBRATE_SAVE_CANCEL;
                displayPage(PAGE_CALIBRATE_CONFIRM);
                break;
            }
            break;
        }
        case PAGE_SPOOL_HOLDER_WEIGHT:
        {
            switch (spoolHolderEditDigitMode)
            {
            case true:
                switch (spoolHolderSelection)
                {
                case SPOOL_HOLDER_3_DIGIT:
                    if (spoolHolder3Digit < 9)
                    {
                        spoolHolder3Digit++;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_2_DIGIT:
                    if (spoolHolder2Digit < 9)
                    {
                        spoolHolder2Digit++;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_1_DIGIT:
                    if (spoolHolder1Digit < 9)
                    {
                        spoolHolder1Digit++;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                }
                break;
            case false:
                if (spoolHolderSelection < SPOOL_HOLDER_CANCEL + spoolHolderSlotSize)
                {
                    if (spoolHolderSelection != SPOOL_HOLDER_CANCEL)
                    {
                        spoolHolderSelection = spoolHolderSelection + 1;
                        if (spoolHolderSelection - spoolHolderSlotStartIndex == 3)
                        {
                            spoolHolderSlotStartIndex++;
                        }
                    }
                    else
                    {
                        if (spoolHolderSlotStartIndex == SPOOL_HOLDER_SLOT_1)
                        {
                            spoolHolderSelection = spoolHolderSelection + 1;
                            if (spoolHolderSelection - spoolHolderSlotStartIndex == 3)
                            {
                                spoolHolderSlotStartIndex++;
                            }
                        }
                        else // menu was rotated before, return to ok after pressing,then rotate right
                        {
                            spoolHolderSelection = spoolHolderSlotStartIndex;
                        }
                    }
                    setPage(PAGE_SPOOL_HOLDER_WEIGHT);
                }
                break;
            }
            break;
        }
        case PAGE_SET_SPOODER_ID:
        {
            switch (setSpooderIDEditMode)
            {
            case true:
                switch (setSpooderIDSelection)
                {
                case SET_SPOODER_ID_LETTER:
                    if (spooderIDLetter < 26)
                    {
                        spooderIDLetter++;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                case SET_SPOODER_ID_NUMBER:
                    if (spooderIDNumber < 99)
                    {
                        spooderIDNumber++;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                }
                break;
            case false:
                if (setSpooderIDSelection < SET_SPOODER_ID_CANCEL)
                {
                    setSpooderIDSelection = setSpooderIDSelection + 1;
                    setPage(PAGE_SET_SPOODER_ID);
                }
                break;
            }
            break;
        }
        case PAGE_LOW_FILAMENT_SETUP:
        {
            switch (lowFilamentEditDigitMode)
            {
            case true:
                switch (lowFilamentSelection)
                {
                case LOW_FILAMENT_4_DIGIT:
                    if (lowFilament4Digit < 9)
                    {
                        lowFilament4Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }

                    break;
                case LOW_FILAMENT_3_DIGIT:
                    if (lowFilament3Digit < 9)
                    {
                        lowFilament3Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_2_DIGIT:
                    if (lowFilament2Digit < 9)
                    {
                        lowFilament2Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_1_DIGIT:
                    if (lowFilament1Digit < 9)
                    {
                        lowFilament1Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                }
                break;
            case false:
                if (lowFilamentSelection < LOW_FILAMENT_CANCEL)
                {
                    lowFilamentSelection = lowFilamentSelection + 1;
                    setPage(PAGE_LOW_FILAMENT_SETUP);
                }
                break;
            }
            break;
        }
        case PAGE_NOTIFICATION:
        {
            if (notificationMenuSelection < (numberOfNotificationMenuItems - 1))
            {
                notificationMenuSelection++;
                if (notificationMenuSelection >= notificationMenuItemStartIndex + notificationMenuItemPerPage)
                {
                    notificationMenuItemStartIndex++;
                }
                setPage(PAGE_NOTIFICATION);
            }
            break;
        }
        case PAGE_FIRMWARE_UPDATE:
        {
            if (updateMenuSelection < (numberOfUpdateMenuItems - 1))
            {
                updateMenuSelection++;
                if (updateMenuSelection >= updateMenuItemStartIndex + updateMenuItemPerPage)
                {
                    updateMenuItemStartIndex++;
                }
                setPage(PAGE_FIRMWARE_UPDATE);
            }
            break;
        }
        case PAGE_OPTIONS:
        {
            if (optionsMenuSelection < (numberOfOptionsMenuItems - 1))
            {
                optionsMenuSelection++;
                if (optionsMenuSelection >= optionsMenuItemStartIndex + optionsMenuItemPerPage)
                {
                    optionsMenuItemStartIndex++;
                }
                displayPage(PAGE_OPTIONS);
            }
            break;
        }
        case PAGE_DEBUG:
        {
            if (debugMenuSelection < (numberOfDebugMenuItems - 1))
            {
                debugMenuSelection++;
                if (debugMenuSelection >= debugMenuItemStartIndex + debugMenuItemPerPage)
                {
                    debugMenuItemStartIndex++;
                }
                setPage(PAGE_DEBUG);
            }
            break;
        }

        } // switch (currentPage)

        break;
    } // case RE_RIGHT:

    } // switch (direction)
    returnToHomepageTimer = millis();
}
bool FILAMENT_ESTIMATOR::setPage(uint8_t page)
{
    // Serial.print(F("Set page:"));
    // Serial.println(page);
    previousPage = currentPage;
    currentPage = page;
    displayPage(currentPage);

    return true;
}
void FILAMENT_ESTIMATOR::displayPage(uint8_t page)
{

    // if (page != PAGE_HOME)
    //{
    // Serial.print(F("Display page: "));
    // Serial.println(page);
    // }

    switch (page)
    {
    case PAGE_HOME:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        String h = hostname;
        h.toUpperCase();
        display.drawString(display.getWidth() / 2, 0, h);
        display.drawRect(0, 12, 128, 1);

        drawRightIndicator(0);
        // Draw display type symbol and weight

        switch (displayType)
        {
        case DISPLAY_TYPE_TOTAL:
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setColor(WHITE);
            display.fillRect(6, 22, 4, 26);
            display.fillRect(27, 22, 4, 26);
            // display.fillRect(11, 28, 15, 13);
            display.fillRect(12, 26, 3, 17);
            display.fillRect(17, 26, 3, 17);
            display.fillRect(22, 26, 3, 32);
            display.setPixel(13, 25);
            display.setPixel(18, 25);
            display.setPixel(23, 25);
            display.setPixel(13, 43);
            display.setPixel(18, 43);
            display.setPixel(23, 58);

            if (abs(totalWeight) < 9999)
            {
                int16_t displayWeight = totalWeight;
                display.drawString(90, 20, String(displayWeight));
                // Draw unit
                display.setFont(ArialMT_Plain_10);
                display.drawString(100, 32, "g");
            }

            break;
        case DISPLAY_TYPE_FILAMENT:
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setColor(WHITE);
            display.fillRect(12, 26, 3, 17);
            display.fillRect(17, 26, 3, 17);
            display.fillRect(22, 26, 3, 32);
            display.setPixel(13, 25);
            display.setPixel(18, 25);
            display.setPixel(23, 25);
            display.setPixel(13, 43);
            display.setPixel(18, 43);
            display.setPixel(23, 58);
            switch (printingStatus)
            {
            case STATUS_BOOT:
                display.drawString(112, 20, "Booting");
                break;
            case STATUS_EMPTY:
                display.drawString(104, 20, "Empty");
                break;
            case STATUS_IDLE:
                if (abs(filamentWeight) < 9999)
                {
                    display.drawString(90, 20, String(filamentWeight, 0));
                    // Draw unit
                    display.setFont(ArialMT_Plain_10);
                    display.drawString(100, 32, "g");
                }
                break;
            case STATUS_PRINTING:
                if (abs(filamentWeight) < 9999)
                {
                    display.drawString(90, 20, String(filamentWeight, 0));
                    // Draw unit
                    display.setFont(ArialMT_Plain_10);
                    display.drawString(100, 32, "g");
                }
                break;
            default:
                break;
            } // switch (printingStatus)

            break;
        case DISPLAY_TYPE_SPOOL_HOLDER:
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setColor(WHITE);
            display.fillRect(6, 22, 4, 26);
            display.fillRect(27, 22, 4, 26);
            display.fillRect(11, 28, 15, 13);
            if (abs(setting.spoolHolderWeight) < 1000)
            {
                display.drawString(90, 20, String(setting.spoolHolderWeight));
                // Draw unit
                display.setFont(ArialMT_Plain_10);
                display.drawString(100, 32, "g");
            }

            break;
        }

        // drawSymbols();

        drawDisplay();
    }
    break;
    case PAGE_INFO:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "INFO");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Name: " + String(hostname));
        String f = String(setting.version.major) + "." + String(setting.version.minor) + "." + String(setting.version.patch);
        display.drawString(6, 22, "Firmware: " + f);
        display.drawString(6, 32, "CAL Value: " + String(setting.calValue));
        if (enableWifi == true)
        {
            display.drawString(6, 42, "Wifi: " + String(wifi_ssid));
            display.drawString(6, 52, "IP:" + WiFi.localIP().toString());
        }

        // drawSymbols();
        drawRightIndicator(1);
        drawDisplay();
    }
    break;
    case PAGE_MENU:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "MENU");
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, menuTitle[menuItemStartIndex]);
        display.drawString(6, 22, menuTitle[menuItemStartIndex + 1]);
        display.drawString(6, 32, menuTitle[menuItemStartIndex + 2]);
        display.drawString(6, 42, menuTitle[menuItemStartIndex + 3]);
        display.drawString(6, 52, menuTitle[menuItemStartIndex + 4]);
        display.drawRect(0, 12, 128, 1);

        drawRightIndicator(2);

        drawLeftIndicator(menuIndex - menuItemStartIndex);

        // drawSymbols();

        drawDisplay();

        break;
    }
    case PAGE_TARE:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "TARE");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Remove spool and press ");
        display.drawString(6, 22, "button to tare and save:");
        display.drawString(40, 36, "Ok");
        display.drawString(40, 46, "Cancel");

        if (tareSelection == TARE_OK)
        {
            drawTriangle(34, 42);
        }
        else
        {
            drawTriangle(34, 52);
        }

        drawDisplay();
        break;
    }
    case PAGE_CALIBRATE:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Calibrate");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Put known weight and");
        display.drawString(6, 22, "enter its value:");

        display.setFont(ArialMT_Plain_16);
        switch (calibrateSelection)
        {
        case CALIBRATE_4_DIGIT:
            if (displayCalibrateDigit == true)
            {
                display.drawString(12, 36, String(calibrate4Digit));
            }
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(6, 45);
            break;
        case CALIBRATE_3_DIGIT:
            display.drawString(12, 36, String(calibrate4Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(32, 36, String(calibrate3Digit));
            }
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(26, 45);
            break;
        case CALIBRATE_2_DIGIT:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(52, 36, String(calibrate2Digit));
            }
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(46, 45);
            break;
        case CALIBRATE_1_DIGIT:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(72, 36, String(calibrate1Digit));
            }
            drawTriangle(66, 45);
            break;
        case CALIBRATE_OK:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(90, 42);
            break;
        case CALIBRATE_CANCEL:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(90, 52);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        drawDisplay();
        break;
    }
    case PAGE_CALIBRATE_CONFIRM:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Calibrate - Confirm");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Old value: " + String(setting.calValue));
        display.drawString(6, 22, "New value: " + String(newCalibrationValue));
        display.drawString(6, 32, "Save data?");

        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        switch (calibrateSaveSelection)
        {
        case CALIBRATE_SAVE_OK:
            drawTriangle(90, 42);
            break;
        case CALIBRATE_SAVE_CANCEL:
            drawTriangle(90, 52);
            break;
        }

        drawDisplay();
        break;
    }
    case PAGE_SPOOL_HOLDER_WEIGHT:
    { // need this because variable is defined in case
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Spool Holder Weight");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);

        uint8_t x = 22;
        uint8_t y = 16;

        switch (spoolHolderSelection)
        {
        case SPOOL_HOLDER_3_DIGIT:

            if (displaySpoolHolderDigit == true)
            {
                display.drawString(x, y, String(spoolHolder3Digit));
            }
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(x - 6, y + 9);
            break;
        case SPOOL_HOLDER_2_DIGIT:

            display.drawString(x, y, String(spoolHolder3Digit));
            if (displaySpoolHolderDigit == true)
            {
                display.drawString(x + 20, y, String(spoolHolder2Digit));
            }
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(x - 6 + 20, y + 9);
            break;
        case SPOOL_HOLDER_1_DIGIT:

            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            if (displaySpoolHolderDigit == true)
            {
                display.drawString(x + 40, y, String(spoolHolder1Digit));
            }
            drawTriangle(x - 6 + 40, y + 9);
            break;
        case SPOOL_HOLDER_OK:
            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(90, 20);
            break;
        case SPOOL_HOLDER_CANCEL:
            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(90, 30);
            break;
        default:
            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 14, "Ok");
        display.drawString(96, 24, "Cancel");

        // display slot menu
        if (spoolHolderSlotSize == 0)
        {
            return;
        }
        uint8_t nameX = 20;
        uint8_t nameY = 34;
        uint8_t weightX = 96;
        for (uint8_t i = 0; i < spoolHolderSlotSize; i++)
        {
            display.drawString(nameX, nameY + i * 10, String(spoolHolderSlotName[spoolHolderSlotStartIndex - SPOOL_HOLDER_SLOT_1 + i]).substring(0, 12));
            display.drawString(weightX, nameY + i * 10, String(spoolHolderSlotWeight[spoolHolderSlotStartIndex - SPOOL_HOLDER_SLOT_1 + i]));
            if (i == 2)
            {
                break;
            }
        }

        if (spoolHolderSelection > SPOOL_HOLDER_CANCEL)
        {
            uint8_t triangleIndex = spoolHolderSelection - spoolHolderSlotStartIndex;
            // Serial.print("triangleIndex = ");
            // Serial.println(triangleIndex);
            drawTriangle(nameX - 6, nameY + 6 + (triangleIndex * 10));
        }

        drawDisplay();

        break;
    }
    case PAGE_SET_SPOODER_ID:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Set Spooder ID");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.setFont(ArialMT_Plain_24);
        uint8_t x = 24;
        uint8_t y = 16;
        uint8_t ok = 62;

        uint8_t displayLetter;

        if (spooderIDLetter == 0)
        {
            displayLetter = 45; //"-"
        }
        else
        {
            displayLetter = spooderIDLetter + 64; // Starting with A
        }

        switch (setSpooderIDSelection)
        {
        case SET_SPOODER_ID_LETTER:
            if (displaySpooderIDDigit == true)
            {
                display.drawString(x, y, String((char)(displayLetter)));
            }
            if (spooderIDNumber == 0)
            {
                display.drawString(x + 24, y, "-");
            }
            else
            {
                display.drawString(x + 24, y, String(spooderIDNumber));
            }

            drawTriangle(x - 6, y + 13);

            break;
        case SET_SPOODER_ID_NUMBER:
            display.drawString(x, y, String((char)(displayLetter)));
            if (displaySpooderIDDigit == true)
            {
                if (spooderIDNumber == 0)
                {
                    display.drawString(x + 24, y, "-");
                }
                else
                {
                    display.drawString(x + 24, y, String(spooderIDNumber));
                }
            }
            drawTriangle(x - 6 + 24, y + 13);
            break;
        case SET_SPOODER_ID_OK:
            display.drawString(x, y, String((char)(displayLetter)));
            if (spooderIDNumber == 0)
            {
                display.drawString(x + 24, y, "-");
            }
            else
            {
                display.drawString(x + 24, y, String(spooderIDNumber));
            }

            drawTriangle(x - 6 + ok, y + 8);
            break;
        case SET_SPOODER_ID_CANCEL:
            display.drawString(x, y, String((char)(displayLetter)));
            if (spooderIDNumber == 0)
            {
                display.drawString(x + 24, y, "-");
            }
            else
            {
                display.drawString(x + 24, y, String(spooderIDNumber));
            }
            drawTriangle(x - 6 + ok, y + 8 + 10);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(x + ok, y + 2, "Ok");
        display.drawString(x + ok, y + 12, "Cancel");

        if (spooderIDLetter != 0 && spooderIDNumber != 0)
        {
            display.setFont(ArialMT_Plain_16);
            String host = "spooder";
            host += String((char)(displayLetter));
            host += String(spooderIDNumber);
            host += ".local";
            display.drawString(2, 44, host);
        }
        drawDisplay();
    }
    break;
    case PAGE_LOW_FILAMENT_SETUP:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Low Filament Setup");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Set filament threshold weight");
        display.drawString(6, 22, "for notification:");

        display.setFont(ArialMT_Plain_16);
        switch (lowFilamentSelection)
        {
        case LOW_FILAMENT_4_DIGIT:
            if (displayLowFilamentDigit == true)
            {
                display.drawString(12, 36, String(lowFilament4Digit));
            }
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(6, 45);
            break;
        case LOW_FILAMENT_3_DIGIT:
            display.drawString(12, 36, String(lowFilament4Digit));
            if (displayLowFilamentDigit == true)
            {
                display.drawString(32, 36, String(lowFilament3Digit));
            }
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(26, 45);
            break;
        case LOW_FILAMENT_2_DIGIT:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            if (displayLowFilamentDigit == true)
            {
                display.drawString(52, 36, String(lowFilament2Digit));
            }
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(46, 45);
            break;
        case LOW_FILAMENT_1_DIGIT:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            if (displayLowFilamentDigit == true)
            {
                display.drawString(72, 36, String(lowFilament1Digit));
            }
            drawTriangle(66, 45);
            break;
        case LOW_FILAMENT_OK:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(90, 42);
            break;
        case LOW_FILAMENT_CANCEL:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(90, 52);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        drawDisplay();
        break;
    }
    case PAGE_NOTIFICATION:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Notification");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, notificationMenuTitle[notificationMenuItemStartIndex]);
        display.drawString(100, 12, (getNotificationSetting(notificationMenuItemStartIndex) == true) ? "On" : "    Off");
        display.drawString(6, 22, notificationMenuTitle[notificationMenuItemStartIndex + 1]);
        display.drawString(100, 22, (getNotificationSetting(notificationMenuItemStartIndex + 1) == true) ? "On" : "    Off");
        display.drawString(6, 32, notificationMenuTitle[notificationMenuItemStartIndex + 2]);
        display.drawString(100, 32, (getNotificationSetting(notificationMenuItemStartIndex + 2) == true) ? "On" : "    Off");
        display.drawString(6, 42, notificationMenuTitle[notificationMenuItemStartIndex + 3]);
        display.drawString(100, 42, (getNotificationSetting(notificationMenuItemStartIndex + 3) == true) ? "On" : "    Off");
        display.drawString(6, 52, notificationMenuTitle[notificationMenuItemStartIndex + 4]);
        if (notificationMenuItemStartIndex + 4 != NOTIFICATION_MENU_RETURN)
        {
            display.drawString(100, 52, (getNotificationSetting(notificationMenuItemStartIndex + 4) == true) ? "On" : "    Off");
        }

        drawLeftIndicator(notificationMenuSelection - notificationMenuItemStartIndex);
        drawDisplay();
        break;
    }
    case PAGE_FIRMWARE_UPDATE:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Firmware Update");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, "Current Version:");
        display.drawString(86, 12, CURRENT_VERSION);
        display.drawString(6, 22, "Github Version:");
        display.drawString(86, 22, githubVersion);

        display.drawString(6, 32, updateMenuTitle[updateMenuItemStartIndex]);
        display.drawString(6, 42, updateMenuTitle[updateMenuItemStartIndex + 1]);
        display.drawString(6, 52, updateMenuTitle[updateMenuItemStartIndex + 2]);

        // display auto update setting
        if (updateMenuItemStartIndex >= 0 && updateMenuItemStartIndex <= 2)
        {
            uint8_t autoUpdateRow = 2 - updateMenuItemStartIndex;
            display.drawString(100, 32 + autoUpdateRow * 10, setting.autoGithubUpdate == true ? "On" : "    Off");
        }

        // display next check countdown
        if (updateMenuItemStartIndex >= 1 && updateMenuItemStartIndex <= 3)
        {
            uint8_t autoUpdateRow = 3 - updateMenuItemStartIndex;
            uint16_t ctMin;
            if (nextCheckTimeMillis > millis())
            {
                ctMin = (nextCheckTimeMillis - millis()) / 60000;
            }
            else
            {
                ctMin = 0;
            }
            if (setting.autoGithubUpdate == true)
            {
                display.drawString(80, 32 + autoUpdateRow * 10, String(ctMin));
                display.drawString(108, 32 + autoUpdateRow * 10, "Min.");
            }
        }

        drawLeftIndicator(updateMenuSelection - updateMenuItemStartIndex + 2);
        drawDisplay();
        break;
    }
    case PAGE_OPTIONS:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Options");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, optionsMenuTitle[optionsMenuItemStartIndex]);
        display.drawString(100, 12, (getOptionsSetting(optionsMenuItemStartIndex) == true) ? "On" : "    Off");
        display.drawString(6, 22, optionsMenuTitle[optionsMenuItemStartIndex + 1]);
        display.drawString(100, 22, (getOptionsSetting(optionsMenuItemStartIndex + 1) == true) ? "On" : "    Off");
        display.drawString(6, 32, optionsMenuTitle[optionsMenuItemStartIndex + 2]);
        display.drawString(100, 32, (getOptionsSetting(optionsMenuItemStartIndex + 2) == true) ? "On" : "    Off");
        display.drawString(6, 42, optionsMenuTitle[optionsMenuItemStartIndex + 3]);
        display.drawString(100, 42, (getOptionsSetting(optionsMenuItemStartIndex + 3) == true) ? "On" : "    Off");
        display.drawString(6, 52, optionsMenuTitle[optionsMenuItemStartIndex + 4]);
        if (optionsMenuItemStartIndex + 4 != OPTIONS_MENU_RETURN)
        {
            display.drawString(100, 52, (getOptionsSetting(optionsMenuItemStartIndex + 4) == true) ? "On" : "    Off");
        }

        drawLeftIndicator(optionsMenuSelection - optionsMenuItemStartIndex);
        drawDisplay();

        break;
    }
    case PAGE_ENTER_HOTSPOT_MODE:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, F("Hotspot Setup"));
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 12, F("Connect to:"));
        display.setFont(ArialMT_Plain_16);
        display.drawString(8, 22, F("Spooder_Setup"));
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 38, "Browse:");
        display.setFont(ArialMT_Plain_16);
        display.drawString(8, 48, "192.168.4.1 ");

        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(108, 52, F("Exit"));
        drawTriangle(102, 58);
        drawDisplay();
        break;
    }
    case PAGE_DEBUG:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Debug");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, debugMenuTitle[debugMenuItemStartIndex]);
        display.drawString(6, 22, debugMenuTitle[debugMenuItemStartIndex + 1]);
        display.drawString(6, 32, debugMenuTitle[debugMenuItemStartIndex + 2]);
        display.drawString(6, 42, debugMenuTitle[debugMenuItemStartIndex + 3]);
        display.drawString(6, 52, debugMenuTitle[debugMenuItemStartIndex + 4]);

        drawLeftIndicator(debugMenuSelection - debugMenuItemStartIndex);
        drawDisplay();
        break;
    }

    default:
        break;
    }
}
void FILAMENT_ESTIMATOR::drawBottomIndicator(uint8_t index)
{
    const uint8_t x = 48;
    const uint8_t x_step = 12;
    const uint8_t y = 59;
    uint8_t index_x = x + x_step * index;
    display.drawCircle(x, y, 2);
    display.drawCircle(x + x_step, y, 2);
    display.drawCircle(x + x_step * 2, y, 2);

    display.drawCircle(index_x, y, 1);

    display.drawCircle(index_x, y, 3);
    display.drawCircle(index_x, y, 4);
}
void FILAMENT_ESTIMATOR::drawRightIndicator(uint8_t index)
{
    const uint8_t x = 122;
    const uint8_t y_step = 10;
    const uint8_t y_start = 20;
    uint8_t index_y = y_start + y_step * index;
    display.fillCircle(x, y_start, 2);
    display.fillCircle(x, y_start + y_step, 2);

    for (uint8_t dy = 0; dy < (numberOfMenuItems * 2); dy++)
    {
        uint8_t y = y_start + y_step * 2 + dy;
        display.fillCircle(x, y, 2);
    }

    if (index < 2)
    {
        display.fillCircle(x, index_y, 5);
    }
    else
    {
        uint8_t y = y_start + y_step * 2 + menuIndex * 2;
        display.fillCircle(x, y, 5);
    }
}
void FILAMENT_ESTIMATOR::drawLeftIndicator(uint8_t index)
{
    uint8_t x = 0;
    uint8_t y = 18 + index * 10;
    drawTriangle(x, y);
}
void FILAMENT_ESTIMATOR::drawTriangle(uint8_t x, uint8_t y)
{
    display.drawLine(x, y - 3, x, y + 3);
    display.drawLine(x + 1, y - 3, x + 1, y + 3);
    display.drawLine(x + 2, y - 2, x + 2, y + 2);
    display.drawLine(x + 3, y - 1, x + 3, y + 1);
    display.drawLine(x + 4, y, x + 4, y);
}
void FILAMENT_ESTIMATOR::drawSymbols()
{
    // Draw WiFi and Blynk symbols
    uint8_t x = 117;
    uint8_t y = 0;
    switch (symbolType)
    {
    case SYMBOL_NONE: // wifi off
        break;
    case SYMBOL_NO_WIFI: // wifi on but not connected
        break;
    case SYMBOL_WIFI_AND_INTERNET: // wifi and internet connected
        display.drawHorizontalLine(x + 2, y + 1, 6);
        display.drawHorizontalLine(x + 1, y + 2, 8);
        display.drawHorizontalLine(x, y + 3, 3);
        display.drawHorizontalLine(x + 7, y + 3, 3);
        display.drawHorizontalLine(x, y + 4, 2);
        display.drawHorizontalLine(x + 8, y + 4, 2);
        display.setPixel(x, y + 5);
        display.setPixel(x + 9, y + 5);
        display.drawHorizontalLine(x + 3, y + 5, 4);
        display.drawHorizontalLine(x + 2, y + 6, 6);
        display.setPixel(x + 2, y + 7);
        display.setPixel(x + 7, y + 7);
        display.drawHorizontalLine(x + 4, y + 8, 2);
        display.drawHorizontalLine(x + 4, y + 9, 2);
        break;
    case SYMBOL_WIFI_NO_INTERNET_1: // wifi but no internet flashing icon 1
        display.drawVerticalLine(x, y + 3, 3);
        display.drawVerticalLine(x + 9, y + 3, 3);
        display.drawVerticalLine(x + 1, y + 2, 3);
        display.drawVerticalLine(x + 8, y + 2, 3);
        display.drawVerticalLine(x + 2, y + 6, 2);
        display.drawVerticalLine(x + 7, y + 6, 2);
        display.fillRect(x + 3, y, 4, 5);
        display.fillRect(x + 4, y + 5, 2, 2);

        display.drawHorizontalLine(x + 4, y + 8, 2);
        display.drawHorizontalLine(x + 4, y + 9, 2);

        break;
    case SYMBOL_WIFI_NO_INTERNET_2: // wifi but no internet flashing icon 2
        display.drawVerticalLine(x, y + 3, 3);
        display.drawVerticalLine(x + 9, y + 3, 3);
        display.drawVerticalLine(x + 1, y + 2, 3);
        display.drawVerticalLine(x + 8, y + 2, 3);
        display.drawVerticalLine(x + 2, y + 6, 2);
        display.drawVerticalLine(x + 7, y + 6, 2);

        break;
    default:
        break;
    }
    // Blynk status
    if (Blynk.connected() == true)
    {
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.fillRect(104 + 1, 1 + 1, 11 - 2, 10 - 1);
        display.drawHorizontalLine(104 + 2, 1, 7);
        display.drawVerticalLine(104, 3, 7);
        display.drawVerticalLine(104 + 10, 3, 7);
        display.setColor(BLACK);
        display.drawString(106, 0, "B");
        display.setColor(WHITE);
    }
}
void FILAMENT_ESTIMATOR::tare()
{
    Serial.println("Tare");
    drawOverlay("Tare", "", 1000);
    loadcell.tare();
    drawOverlay("Offset", "Saved", 1000);
    // testing purpose
    setting.tareOffset = loadcell.getTareOffset();
    Serial.print(F("Tare offset: "));
    Serial.print(setting.tareOffset);
    Serial.println(F(" saved to EEPROM."));
    saveToEEPROM();
    printingStatus = STATUS_IDLE;
    printingStatusString = "STATUS_IDLE";
    Serial.println(F("Status changed to STATUS_IDLE after tare."));
    purgeCounter = 3; // purge array after few samples
}
void FILAMENT_ESTIMATOR::calibrate()
{
    // Draw the overlay
    display.setColor(BLACK);
    display.fillRect(16 - 8, 12 - 4, 96 + 18, 40 + 8);
    display.setColor(WHITE);
    display.drawRect(16, 12, 96, 40);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(64, 32, "Calibrating...");
    drawDisplay();
    // refresh the dataset to be sure that the known mass is measured correct
    loadcell.refreshDataSet();
    setting.calibrationWeight = getCalibrationWeight();
    Serial.print("Calibration weight = ");
    Serial.println(setting.calibrationWeight);
    // get the new calibration value
    newCalibrationValue = loadcell.getNewCalibration(setting.calibrationWeight);
    Serial.print("New calibration value = ");
    Serial.println(newCalibrationValue);
    Serial.println("Calibrate completed.");
}
void FILAMENT_ESTIMATOR::updateHomepage()
{
    if (millis() - updateHomepageTimer < UPDATE_HOMEPAGE_PERIOD)
        return;
    updateHomepageTimer = millis();
    if (currentPage == PAGE_HOME)
    {
        displayPage(PAGE_HOME);
    }
}
void FILAMENT_ESTIMATOR::checkCurrentPage()
{
    if (millis() - returnToHomepageTimer < RETURN_TO_HOMEPAGE_PERIOD)
        return;
    if (currentPage == PAGE_HOME)
        return;
    if (calibrateEditDigitMode == true)
        return;
    if (lowFilamentEditDigitMode == true)
        return;
    if (spoolHolderEditDigitMode == true)
        return;
    if (setSpooderIDEditMode == true)
        return;
    setPage(PAGE_HOME);
}
void FILAMENT_ESTIMATOR::drawOverlay(const char *msgLine1, const char *msgLine2, uint16_t period)
{
    overlayMsgLine1 = msgLine1;
    overlayMsgLine2 = msgLine2;
    overlayDisplayPeriod = period;
    drawOverlayFlag = true;
    drawOverlayTimer = millis();
    drawDisplay();
}
void FILAMENT_ESTIMATOR::updateOverlay()
{
    if (drawOverlayFlag == true)
    {

        display.setColor(BLACK);
        display.fillRect(6, 6, 127 - 6, 64 - 6);
        display.setColor(WHITE);
        display.drawRect(8, 8, 127 - 8, 64 - 8);
        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        if (String(overlayMsgLine2).length() == 0)
        {
            display.drawString(64, 35, overlayMsgLine1);
        }
        else
        {
            display.drawString(64, 24, overlayMsgLine1);
            display.drawString(64, 48, overlayMsgLine2);
        }

        // display.drawString(64, 52, msg);
    }
}
void FILAMENT_ESTIMATOR::drawDisplay()
{
    updateOverlay();
    drawSymbols();
    display.display();
}
void FILAMENT_ESTIMATOR::checkCalibrateEditModeTimer()
{
    if (millis() - calibrateEditModerTimer > CALIBRATE_EDIT_MODE_PERIOD)
    {
        displayCalibrateDigit = !displayCalibrateDigit;
        calibrateEditModerTimer = millis();
        displayPage(PAGE_CALIBRATE);
    }
}
void FILAMENT_ESTIMATOR::checkSpoolHolderEditModeTimer()
{
    if (millis() - spoolHolderEditModerTimer > SPOOL_HOLDER_EDIT_MODE_PERIOD)
    {
        displaySpoolHolderDigit = !displaySpoolHolderDigit;
        spoolHolderEditModerTimer = millis();
        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
    }
}
void FILAMENT_ESTIMATOR::checkSetSpooderIDEditModeTimer()
{
    if (millis() - setSpooderIDTimer > SET_SPOODER_ID_EDIT_MODE_PERIOD)
    {
        displaySpooderIDDigit = !displaySpooderIDDigit;
        setSpooderIDTimer = millis();
        displayPage(PAGE_SET_SPOODER_ID);
    }
}
void FILAMENT_ESTIMATOR::checkLowFilamentEditModeTimer()
{
    if (millis() - lowFilamentEditModerTimer > LOW_FILAMENT_EDIT_MODE_PERIOD)
    {
        displayLowFilamentDigit = !displayLowFilamentDigit;
        lowFilamentEditModerTimer = millis();
        displayPage(PAGE_LOW_FILAMENT_SETUP);
    }
}
void FILAMENT_ESTIMATOR::setCalibrationWeight(uint16_t weight)
{
    if (weight > 9999)
        weight = 9999;
    calibrate4Digit = (weight / 1000U) % 10;
    calibrate3Digit = (weight / 100U) % 10;
    calibrate2Digit = (weight / 10U) % 10;
    calibrate1Digit = (weight / 1U) % 10;
}
uint16_t FILAMENT_ESTIMATOR::getCalibrationWeight()
{
    uint16_t weight = calibrate4Digit * 1000 + calibrate3Digit * 100 + calibrate2Digit * 10 + calibrate1Digit;
    return weight;
}
void FILAMENT_ESTIMATOR::setCurrentSpoolHolderWeight(uint16_t weight)
{
    if (weight > 999)
    {
        weight = 999;
    }
    if (noSpoolHolderWeightInEEPROM == true)
    {
        Serial.print(F("Current spool holder weight set to "));
        Serial.println(weight);
        setting.spoolHolderWeight = weight;
        Serial.println(F("Saving spool holder weight to EEPROM:"));
        saveToEEPROM();
        spoolHolder3Digit = (weight / 100U) % 10;
        spoolHolder2Digit = (weight / 10U) % 10;
        spoolHolder1Digit = (weight / 1U) % 10;
    }
    else
    {
        Serial.println(F("Spool holder weight exists in EEPROM. Not modified by this function."));
    }
}
uint16_t FILAMENT_ESTIMATOR::getSpoolHolderWeight()
{
    uint16_t weight = spoolHolder3Digit * 100 + spoolHolder2Digit * 10 + spoolHolder1Digit;
    return weight;
}
uint16_t FILAMENT_ESTIMATOR::getLowFilamentThreshold()
{
    uint16_t weight = lowFilament4Digit * 1000 + lowFilament3Digit * 100 + lowFilament2Digit * 10 + lowFilament1Digit;
    return weight;
}
void FILAMENT_ESTIMATOR::setStepsPerClick(uint8_t steps)
{
    stepsPerClick = steps;
    rotary.setStepsPerClick(steps);
}
uint8_t FILAMENT_ESTIMATOR::getStepsPerClick()
{

    return rotary.getStepsPerClick();
}
void FILAMENT_ESTIMATOR::setDebounceTime(uint16_t ms)
{
    button.setDebounceTime(ms);
}
void FILAMENT_ESTIMATOR::setLongClickTime(uint16_t ms)
{
    button.setLongClickTime(ms);
}
void FILAMENT_ESTIMATOR::setDoubleClickTime(uint16_t ms)
{
    button.setDoubleClickTime(ms);
}
uint16_t FILAMENT_ESTIMATOR::getDebounceTime()
{
    return button.getDebounceTime();
}
uint16_t FILAMENT_ESTIMATOR::getLongClickTime()
{
    return button.getLongClickTime();
}
uint16_t FILAMENT_ESTIMATOR::getDoubleClickTime()
{
    return button.getDoubleClickTime();
}
void FILAMENT_ESTIMATOR::loadToSetting()
{
    Serial.println(F("Load EEPROM data to setting:"));
    EEPROM.get(EEPROM_START_ADDRESS, setting);
    spoolHolder3Digit = (setting.spoolHolderWeight / 100U) % 10;
    spoolHolder2Digit = (setting.spoolHolderWeight / 10U) % 10;
    spoolHolder1Digit = (setting.spoolHolderWeight / 1U) % 10;

    switch (setting.spooderIDSetStatus)
    {
    case SPOODER_ID_USER_SET:
    case SPOODER_ID_SYSTEM_SET:
        spooderIDLetter = setting.spooderIDLetter;
        spooderIDNumber = setting.spooderIDNumber;
        validSpooderID = true;
        hostname = "spooder" + String((char)(spooderIDLetter + 64)) + String(spooderIDNumber);
        break;
    default:
        Serial.println(F("Invalid spooder ID in EEPROM."));
        break;
    }

    // handles newly added calibrationWeight to settings
    if (setting.calibrationWeightIsValid != 253) // arbitury value
    {
        setting.calibrationWeightIsValid = 253;
        setting.calibrationWeight = DEFAULT_CALIBRATION_WEIGHT;
    }
}
void FILAMENT_ESTIMATOR::saveToEEPROM()
{
    Serial.println(F("Save setting data to EEPROM:"));
    EEPROM.put(EEPROM_START_ADDRESS, setting);
    EEPROM.commit();
}
void FILAMENT_ESTIMATOR::dumpSetting()
{
    Serial.println(F("Dump setting:"));
    // Starting from v0.3.0
    Serial.print(F("  - Firmware version: "));
    Serial.print(setting.version.major);
    Serial.print(F("."));
    Serial.print(setting.version.minor);
    Serial.print(F("."));
    Serial.println(setting.version.patch);

    Serial.print(F("  - calValue: "));
    Serial.println(setting.calValue);

    Serial.print(F("  - spoolHolderWeight: "));
    Serial.println(setting.spoolHolderWeight);

    Serial.print(F("  - spooderIDSetStatus: 0x"));
    Serial.println(setting.spooderIDSetStatus, HEX);

    Serial.print(F("  - spooderIDLetter: "));
    Serial.println(setting.spooderIDLetter);

    Serial.print(F("  - spooderIDNumber: "));
    Serial.println(setting.spooderIDNumber);

    Serial.print(F("  - lowFilamentThreshold: "));
    Serial.println(setting.lowFilamentThreshold);

    Serial.print(F("  - notifyOnPrintStarted: "));

    Serial.println(setting.notifyOnPrintStarted);

    Serial.print(F("  - notifyOnPrintCompleted: "));
    Serial.println(setting.notifyOnPrintCompleted);

    Serial.print(F("  - notifyOnLowFilament: "));
    Serial.println(setting.notifyOnLowFilament);

    Serial.print(F("  - notifyOnFallOffRack: "));
    Serial.println(setting.notifyOnFallOffRack);

    Serial.print(F("  - notifyOnFallOffBearing: "));
    Serial.println(setting.notifyOnFallOffBearing);

    Serial.print(F("  - notifyOnTangled: "));
    Serial.println(setting.notifyOnTangled);
    Serial.print(F("  - optionsWiFi: "));
    Serial.println(setting.optionsWiFi);
    Serial.print(F("  - optionsMDNS: "));
    Serial.println(setting.optionsMDNS);
    Serial.print(F("  - optionsBLYNK: "));
    Serial.println(setting.optionsBLYNK);
    Serial.print(F("  - optionsWebServer: "));
    Serial.println(setting.optionsWebServer);
    Serial.print(F("  - optionsSpooderClient: "));
    Serial.println(setting.optionsSpooderClient);
    Serial.print(F("  - optionsSpooderServer: "));
    Serial.println(setting.optionsSpooderServer);
    Serial.print(F("  - optionsArduinoOTA: "));
    Serial.println(setting.optionsArduinoOTA);
    Serial.print(F("  - optionsAutoHomepage: "));
    Serial.println(setting.optionsAutoHomepage);

    Serial.print(F("  - autoGithubUpdate: "));
    Serial.println(setting.autoGithubUpdate);
    Serial.print(F("  - tareOffset: "));
    Serial.println(setting.tareOffset);
    Serial.print(F("  - automaticUpdateJustPerformed: "));
    Serial.println(setting.automaticUpdateJustPerformed);

    Serial.println(F("Setting dump completed."));
}
void FILAMENT_ESTIMATOR::dumpEEPROM()
{
    Serial.println(F("Dump EEPROM data:"));
    for (uint16_t addr = 0; addr < DECLARED_EEPROM_SIZE; addr++)
    {
        if (addr % 32 == 0)
        {
            Serial.println("");
            Serial.print("[");

            if (addr == 0)
                Serial.print("00");
            if (addr == 64)
                Serial.print("0");

            Serial.print(addr);
            Serial.print("]:");
        }

        if (EEPROM.read(addr) < 10)
            Serial.print("0");
        Serial.print(EEPROM.read(addr), HEX);
        Serial.print(" ");
    }
    Serial.println();
}
void FILAMENT_ESTIMATOR::eraseEEPROM()
{
    Serial.println(F("EEPROM data erased."));
    for (uint16_t addr = 0; addr < DECLARED_EEPROM_SIZE; addr++)
    {
        EEPROM.write(addr, 0xff);
    }
    EEPROM.commit();
}
void FILAMENT_ESTIMATOR::toogleVerboseDebug()
{
    VERBOSE_DEBUG_PRINT = !VERBOSE_DEBUG_PRINT;
    Serial.print(F("Verbose debug print is now "));
    Serial.println(VERBOSE_DEBUG_PRINT ? "on." : "off.");
}
uint32_t FILAMENT_ESTIMATOR::versionToNumber(VERSION_STRUCT v)
{
    // a simple conversion
    // v1.2.3 will convert to something like 10203
    // So that the max number for each item that works will be 99
    uint32_t number = 0;
    number += v.major * 10000;
    number += v.minor * 100;
    number += v.patch * 1;
    return number;
}
void FILAMENT_ESTIMATOR::listDir(const char *dirname)
{
    Serial.printf("Listing directory: %s\r\n", dirname);
    Serial.println("<root>");
    _listDir(dirname, 0);
    FSInfo fs_info;

    LittleFS.info(fs_info);
    Serial.print(F("File system used: "));
    Serial.print(fs_info.usedBytes);
    Serial.println(F(" Bytes"));
    Serial.print(F("File system total: "));
    Serial.print(fs_info.totalBytes);
    Serial.println(F(" Bytes"));
}
void FILAMENT_ESTIMATOR::_listDir(const char *dirname, uint8_t level)
{
    Dir dir = LittleFS.openDir(dirname);
    level++;
    while (dir.next())
    {
        Serial.print(F("|"));
        for (uint8_t i = 0; i < level; i++)
        {
            if (i > 0)
            {
                Serial.print(F("   "));
                if (i == (level - 1))
                {
                    Serial.print(F("|"));
                }
            }
            else // i=0
            {
                // Serial.print("--");
            }
            // Serial.print("--");
        }
        Serial.print("--");
        if (dir.isFile())
        {

            Serial.print(dir.fileName());
            Serial.print(F("  "));
            Serial.println(dir.fileSize());
        }
        else // isDirectory()
        {
            Serial.print("<");
            Serial.print(dir.fileName());
            Serial.println(">");
            String subDir = (String(dirname) + String("/") + dir.fileName());
            _listDir(subDir.c_str(), level);
        }
    }

    level++;
}
void FILAMENT_ESTIMATOR::displayMonoBitmap(const char *filename)
{
    File f = LittleFS.open(filename, "r");
    Serial.print(filename);
    if (!f)
    {
        Serial.println(F(" - File open failed."));
        f.close();
        return;
    }

    Serial.println(F(" - File open succeeded."));
    Serial.print(F("File size: "));
    Serial.print(f.size());
    Serial.println(F(" Bytes."));

    if (f.size() < 1024)
    {
        Serial.print(F("File size too small(<1024). "));
        return;
    }

    struct
    {
        char signature[2];
        uint32_t filesize = 0;
        uint32_t offset = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t colors = 0;
        uint32_t bitmapInfoHeaderSize = 0;

    } bmp;
    bmp.signature[0] = f.read();
    bmp.signature[1] = f.read();
    bool mismatch = false;
    if (!(bmp.signature[0] == 0x42 && bmp.signature[1] == 0x4d))
    {
        Serial.print(F("Not a bitmap file."));
        mismatch = true;
    }
    bmp.filesize = read32(f, 0x03);
    bmp.offset = read32(f, 0x0a);
    bmp.width = read32(f, 0x12);
    bmp.height = read32(f, 0x16);
    bmp.colors = read32(f, 0x2E);
    bmp.bitmapInfoHeaderSize = read32(f, 0x0E);

    if (bmp.width != 128 && bmp.height != 64)
    {
        Serial.println(F("BMP file width and height mismatch."));
        Serial.print(F("Width = "));
        Serial.println(bmp.width);
        Serial.print(F("Height = "));
        Serial.println(bmp.height);
        mismatch = true;
    }
    // Serial.print(F("Header Offset = "));
    // Serial.println(bmp.offset);
    if (bmp.colors != 2)
    {
        Serial.println(F("BMP file color mismatch."));
        Serial.print(F("Colors = "));
        Serial.println(bmp.colors);
        mismatch = true;
    }
    if (mismatch == true)
        return;
    // Serial.print(F("Header size = "));
    // Serial.println(bmp.bitmapInfoHeaderSize);
    uint8_t colorOffset = 14 + bmp.bitmapInfoHeaderSize;
    f.seek(colorOffset);
    bool normalDir;
    if (f.read() == 0 && f.read() == 0 && f.read() == 0)
    {
        // if the color 0 is (0,0,0) black,
        // bits of 0 is black
        normalDir = true;
    }
    else
    {
        normalDir = false;
    }
    // dump the file for debugging
    // f.seek(0);
    //  while (f.available())
    //  {
    //      char c = f.read();
    //      if (c < 10)
    //          Serial.print("0");
    //      Serial.print(c, HEX);
    //      Serial.print("  ");
    //  }
    uint8_t buffer[1024];
    f.seek(bmp.offset);
    for (uint16_t i = 0; i < 1024; i++)
    {
        if (normalDir == true)
        {
            // bits of 0 means black in the bmp
            buffer[1023 - i] = f.read();
        }
        else
        {
            // bits of 0 means white in the bmp, reading is inverted
            buffer[1023 - i] = ~f.read();
        }
    }
    display.drawXbm(0, 0, 128, 64, buffer);
    drawDisplay();
    f.close();
}
uint32_t FILAMENT_ESTIMATOR::read32(File f, uint32_t offset)
{
    f.seek(offset);
    uint32_t r = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        r += f.read() << (i * 8);
    }
    return r;
}
void FILAMENT_ESTIMATOR::loadConfig()
{
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Failed to open config file."));
        return;
    }
    configSize = configFile.size();
    if (configSize > JSON_DOC_BUFFER_SIZE)
    {
        Serial.println(F("Config file size is too large"));
        return;
    }

    // Use local json buffer to free up memory after loading
    DynamicJsonDocument jsonDoc(JSON_DOC_BUFFER_SIZE);

    deserializeJson(jsonDoc, configFile);

    configFile.close();

    strcpy(config_version, jsonDoc["config_version"]);
    strcpy(wifi_ssid, jsonDoc["wifi_ssid"]);
    strcpy(wifi_password, jsonDoc["wifi_password"]);
    strcpy(blynk_auth, jsonDoc["blynk_auth"]);

    uint8_t index = 0;
    for (JsonObject elem : jsonDoc["spool_holder"].as<JsonArray>())
    {
        strcpy(spoolHolderSlotName[index], elem["name"]);
        // spoolHolderSlotName[index] = elem["name"];     // "esun black", "your_holder_name_here", "your_other_holder", ...
        spoolHolderSlotWeight[index] = elem["weight"]; // 180, 150, 250, 250, 250
        index++;
    }
    spoolHolderSlotSize = index;

    Serial.print(F("Config file version "));
    Serial.print(config_version);
    Serial.println(F(" loaded."));

    // handles config file version update from 0.3.0 to 0.7.0
    /*
    if (jsonDoc["config_version"] == "0.3.0")
    {
        Serial.println(F("Config file is old version."));
        //handles config file version update to 0.7.0

        //if (!jsonDoc.containsKey("calibration_weight"))
        //{
        //    jsonDoc["calibration_weight"] = DEFAULT_CALIBRATION_WEIGHT;
        //}

        //if (!jsonDoc.containsKey("steps_per_click"))
        //{
        //jsonDoc["steps_per_click"] = DEFAULT_STEPS_PER_CLICK;
        //}

        File configFile = LittleFS.open("/config.json", "w"); //re-open the config file in write mode
        serializeJson(jsonDoc, configFile);
        Serial.println(F("Config file updated."));
    }
    */
}
void FILAMENT_ESTIMATOR::dumpConfig()
{
    Serial.println(F("Dump config file:"));
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Failed to open config file."));
        return;
    }
    configSize = configFile.size();
    if (configSize > JSON_DOC_BUFFER_SIZE)
    {
        Serial.println(F("Config file size is too large"));
        return;
    }

    // Use local json buffer to free up memory after loading
    DynamicJsonDocument jsonDoc(JSON_DOC_BUFFER_SIZE);

    deserializeJson(jsonDoc, configFile);
    serializeJsonPretty(jsonDoc, Serial); // output to Serial
    Serial.println();
}
void FILAMENT_ESTIMATOR::saveConfig()
{
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Failed to open config file."));
        return;
    }
    configSize = configFile.size();
    if (configSize > JSON_DOC_BUFFER_SIZE)
    {
        Serial.println(F("Config file size is too large"));
        return;
    }

    // Use local json buffer to free up memory after loading
    DynamicJsonDocument jsonDoc(JSON_DOC_BUFFER_SIZE);

    // Deserialize the JSON document
    deserializeJson(jsonDoc, configFile);
    configFile.close();

    // Set the values in the document
    jsonDoc["wifi_ssid"] = wifi_ssid;
    jsonDoc["wifi_password"] = wifi_password;
    jsonDoc["blynk_auth"] = blynk_auth;

    LittleFS.remove("/config.json"); // delete the file

    // Serialize JSON to file
    configFile = LittleFS.open("/config.json", "w");
    if (serializeJsonPretty(jsonDoc, configFile) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }
    else
    {
        Serial.println(F("Config file updated."));
    }
    configFile.close();
}
void FILAMENT_ESTIMATOR::replyOK()
{
    webServer->send(200, FPSTR(TEXT_PLAIN), "");
}
void FILAMENT_ESTIMATOR::replyOKWithMsg(String msg)
{
    webServer->send(200, FPSTR(TEXT_PLAIN), msg);
}
void FILAMENT_ESTIMATOR::replyNotFound(String msg)
{
    webServer->send(404, FPSTR(TEXT_PLAIN), msg);
}
void FILAMENT_ESTIMATOR::replyBadRequest(String msg)
{
    Serial.println(msg);
    webServer->send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}
void FILAMENT_ESTIMATOR::replyServerError(String msg)
{
    Serial.println(msg);
    webServer->send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}
void FILAMENT_ESTIMATOR::handleStatus()
{
    Serial.println(F("handleStatus"));
    FSInfo fs_info;
    String json;
    json.reserve(128);

    json = "{\"type\":\"";
    json += fsName;
    json += "\", \"isOk\":";
    if (fsOK)
    {
        fileSystem->info(fs_info);
        json += F("\"true\", \"totalBytes\":\"");
        json += fs_info.totalBytes;
        json += F("\", \"usedBytes\":\"");
        json += fs_info.usedBytes;
        json += "\"";
    }
    else
    {
        json += "\"false\"";
    }
    json += F(",\"unsupportedFiles\":\"");
    json += unsupportedFiles;
    json += "\"}";

    webServer->send(200, "application/json", json);
}
void FILAMENT_ESTIMATOR::handleFileList()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    if (!webServer->hasArg("dir"))
    {
        return replyBadRequest(F("DIR ARG MISSING"));
    }

    String path = webServer->arg("dir");
    if (path != "/" && !fileSystem->exists(path))
    {
        return replyBadRequest(F("BAD PATH"));
    }

    Serial.println(String("handleFileList: ") + path);
    Dir dir = fileSystem->openDir(path);
    path.clear();

    // use HTTP/1.1 Chunked response to avoid building a huge temporary string
    if (!webServer->chunkedResponseModeStart(200, "text/json"))
    {
        webServer->send(505, F("text/html"), F("HTTP1.1 required"));
        return;
    }

    // use the same string for every line
    String output;
    output.reserve(64);
    while (dir.next())
    {

        if (output.length())
        {
            // send string from previous iteration
            // as an HTTP chunk
            webServer->sendContent(output);
            output = ',';
        }
        else
        {
            output = '[';
        }

        output += "{\"type\":\"";
        if (dir.isDirectory())
        {
            output += "dir";
        }
        else
        {
            output += F("file\",\"size\":\"");
            output += dir.fileSize();
        }

        output += F("\",\"name\":\"");
        // Always return names without leading "/"
        if (dir.fileName()[0] == '/')
        {
            output += &(dir.fileName()[1]);
        }
        else
        {
            output += dir.fileName();
        }

        output += "\"}";
    }

    // send last string
    output += "]";
    webServer->sendContent(output);
    webServer->chunkedResponseFinalize();
}
bool FILAMENT_ESTIMATOR::handleFileRead(String path)
{
    Serial.println(String("handleFileRead: ") + path);
    if (!fsOK)
    {
        replyServerError(FPSTR(FS_INIT_ERROR));
        return true;
    }

    if (path.endsWith("/"))
    {
        path += "index.htm";
    }

    String contentType;
    if (webServer->hasArg("download"))
    {
        contentType = F("application/octet-stream");
    }
    else
    {
        contentType = mime::getContentType(path);
    }

    if (!fileSystem->exists(path))
    {
        // File not found, try gzip version
        path = path + ".gz";
    }
    if (fileSystem->exists(path))
    {
        File file = fileSystem->open(path, "r");
        if (webServer->streamFile(file, contentType) != file.size())
        {
            Serial.println(F("Sent less data than expected!"));
        }
        file.close();
        return true;
    }

    return false;
}
String FILAMENT_ESTIMATOR::lastExistingParent(String path)
{
    while (!path.isEmpty() && !fileSystem->exists(path))
    {
        if (path.lastIndexOf('/') > 0)
        {
            path = path.substring(0, path.lastIndexOf('/'));
        }
        else
        {
            path = String(); // No slash => the top folder does not exist
        }
    }
    Serial.println(String("Last existing parent: ") + path);
    return path;
}
void FILAMENT_ESTIMATOR::handleFileCreate()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String path = webServer->arg("path");
    if (path.isEmpty())
    {
        return replyBadRequest(F("PATH ARG MISSING"));
    }

    if (path == "/")
    {
        return replyBadRequest(F("BAD PATH"));
    }
    if (fileSystem->exists(path))
    {
        return replyBadRequest(F("PATH FILE EXISTS"));
    }

    String src = webServer->arg("src");
    if (src.isEmpty())
    {
        // No source specified: creation
        Serial.println(String("handleFileCreate: ") + path);
        if (path.endsWith("/"))
        {
            // Create a folder
            path.remove(path.length() - 1);
            if (!fileSystem->mkdir(path))
            {
                return replyServerError(F("MKDIR FAILED"));
            }
        }
        else
        {
            // Create a file
            File file = fileSystem->open(path, "w");
            if (file)
            {
                file.write((const char *)0);
                file.close();
            }
            else
            {
                return replyServerError(F("CREATE FAILED"));
            }
        }
        if (path.lastIndexOf('/') > -1)
        {
            path = path.substring(0, path.lastIndexOf('/'));
        }
        replyOKWithMsg(path);
    }
    else
    {
        // Source specified: rename
        if (src == "/")
        {
            return replyBadRequest("BAD SRC");
        }
        if (!fileSystem->exists(src))
        {
            return replyBadRequest(F("SRC FILE NOT FOUND"));
        }

        Serial.println(String("handleFileCreate: ") + path + " from " + src);

        if (path.endsWith("/"))
        {
            path.remove(path.length() - 1);
        }
        if (src.endsWith("/"))
        {
            src.remove(src.length() - 1);
        }
        if (!fileSystem->rename(src, path))
        {
            return replyServerError(F("RENAME FAILED"));
        }
        replyOKWithMsg(lastExistingParent(src));
    }
}
void FILAMENT_ESTIMATOR::deleteRecursive(String path)
{
    File file = fileSystem->open(path, "r");
    bool isDir = file.isDirectory();
    file.close();

    // If it's a plain file, delete it
    if (!isDir)
    {
        fileSystem->remove(path);
        return;
    }

    // Otherwise delete its contents first
    Dir dir = fileSystem->openDir(path);

    while (dir.next())
    {
        deleteRecursive(path + '/' + dir.fileName());
    }

    // Then delete the folder itself
    fileSystem->rmdir(path);
}
void FILAMENT_ESTIMATOR::handleFileDelete()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String path = webServer->arg(0);
    if (path.isEmpty() || path == "/")
    {
        return replyBadRequest("BAD PATH");
    }

    Serial.println(String("handleFileDelete: ") + path);
    if (!fileSystem->exists(path))
    {
        return replyNotFound(FPSTR(FILE_NOT_FOUND));
    }
    deleteRecursive(path);

    replyOKWithMsg(lastExistingParent(path));
}
void FILAMENT_ESTIMATOR::handleFileUpload()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }
    if (webServer->uri() != "/edit")
    {
        return;
    }
    HTTPUpload &upload = webServer->upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        // Make sure paths always start with "/"
        if (!filename.startsWith("/"))
        {
            filename = "/" + filename;
        }
        Serial.println(String("handleFileUpload Name: ") + filename);
        uploadFile = fileSystem->open(filename, "w");
        if (!uploadFile)
        {
            return replyServerError(F("CREATE FAILED"));
        }
        Serial.println(String("Upload: START, filename: ") + filename);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadFile)
        {
            size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
            if (bytesWritten != upload.currentSize)
            {
                return replyServerError(F("WRITE FAILED"));
            }
        }
        Serial.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (uploadFile)
        {
            uploadFile.close();
        }
        Serial.println(String("Upload: END, Size: ") + upload.totalSize);
    }
}
void FILAMENT_ESTIMATOR::handleNotFound()
{
    if (isHotspotMode == true)
    {
        Serial.println(F("Client connected, sending setup page."));

        // Send captive portal web page in chunks
        webServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
        webServer->send(200, "text/html", FPSTR(captivePortalResponseHTML1));
        webServer->sendContent(FPSTR(captivePortalResponseHTML2));
        if (wifi_ssid[0] != '\0') // prevent null value ending the transmission
            webServer->sendContent(wifi_ssid);
        webServer->sendContent(FPSTR(captivePortalResponseHTML3));
        webServer->sendContent(FPSTR(captivePortalResponseHTML4));
        if (wifi_password[0] != '\0') // prevent null value ending the transmission
            webServer->sendContent(wifi_password);
        webServer->sendContent(FPSTR(captivePortalResponseHTML5));
        webServer->sendContent(FPSTR(captivePortalResponseHTML6));
        if (blynk_auth[0] != '\0') // prevent null value ending the transmission
            webServer->sendContent(blynk_auth);
        webServer->sendContent(FPSTR(captivePortalResponseHTML7));
        webServer->sendContent("");

        return;
    }
    else
    {

        if (!fsOK)
        {
            return replyServerError(FPSTR(FS_INIT_ERROR));
        }

        String uri = ESP8266WebServer::urlDecode(webServer->uri()); // required to read paths with blanks

        if (handleFileRead(uri))
        {
            return;
        }

        // Dump debug data
        String message;
        message.reserve(100);
        message = F("Error: File not found\n\nURI: ");
        message += uri;
        message += F("\nMethod: ");
        message += (webServer->method() == HTTP_GET) ? "GET" : "POST";
        message += F("\nArguments: ");
        message += webServer->args();
        message += '\n';
        for (uint8_t i = 0; i < webServer->args(); i++)
        {
            message += F(" NAME:");
            message += webServer->argName(i);
            message += F("\n VALUE:");
            message += webServer->arg(i);
            message += '\n';
        }
        message += "path=";
        message += webServer->arg("path");
        message += '\n';
        Serial.print(message);

        return replyNotFound(message);
    }
}
void FILAMENT_ESTIMATOR::handleGetEdit()
{
    if (handleFileRead(F("/edit/index.htm")))
    {
        return;
    }
    replyNotFound(FPSTR(FILE_NOT_FOUND));
}
void FILAMENT_ESTIMATOR::startLogging()
{
    Serial.println(F("Start logging."));
    String filename = "/log/log";
    uint8_t suf = 1;
    filename += suf;
    filename += ".txt";

    while (LittleFS.exists(filename))
    {
        filename = "/log/log";
        suf += 1;
        filename += suf;
        filename += ".txt";
        yield();
    }
    Serial.print(F("Logging into file: "));
    Serial.println(filename);
    logFile = LittleFS.open(filename, "a");
    isLogging = true;
}
void FILAMENT_ESTIMATOR::stopLogging()
{
    Serial.println(F("Stop logging."));
    logFile.close();
    isLogging = false;
}
void FILAMENT_ESTIMATOR::updateLogging()
{
    // logs every second
    now = time(nullptr);
    if (now != previous)
    {
        String record = (String)now;
        record += ",";
        record += (String)totalWeight;
        if (logFile.println(record))
        {
            Serial.print("Logging: ");
            Serial.println(record);
        }
        previous = now;
    }
}
void FILAMENT_ESTIMATOR::startEmulation()
{
    emulatedLogFile = LittleFS.open("log/log.txt", "r");
    Serial.print("log/log.txt");
    if (!emulatedLogFile)
    {
        Serial.println(F(" - File open failed."));
        emulatedLogFile.close();
        return;
    }
    drawOverlay("Start", "Emulation", 1000);
    Serial.println(F(" - File open succeeded."));
    Serial.print(F("File size: "));
    Serial.print(emulatedLogFile.size());
    Serial.println(F(" Bytes."));

    emulationStarted = true;
    Serial.println(F("Emulation started."));
    EMULATION_PERIOD = 100;
    DETECTION_PERIOD = 100;
    loggerCounter = 0;
    detectionTimer = millis(); // reset timer, to fill the weight first, debug purpose
}
void FILAMENT_ESTIMATOR::stopEmulation()
{
    if (emulatedLogFile == true)
    {
        drawOverlay("Stop", "Emulation", 1000);
        Serial.println(F("Emulation stopped."));
        emulatedLogFile.close();
        emulationStarted = false;
        EMULATION_PERIOD = 1000;
        DETECTION_PERIOD = 1000;
    }
}
void FILAMENT_ESTIMATOR::updateEmulation()
{
    if (millis() - emulationTimer < EMULATION_PERIOD)
        return;
    if (emulatedLogFile.available() > 5) // normally a valid data set has more than 4 bytes
    {
        // Reading a set of data
        emulatedLogFile.parseInt();
        emulatedLogFile.read();
        emulatedWeight = emulatedLogFile.parseFloat();
    }
    else
    {
        stopEmulation();
    }

    emulationTimer = millis();
}
void FILAMENT_ESTIMATOR::updateDetection()
{
    if (millis() - detectionTimer < DETECTION_PERIOD)
        return;

    float change = totalWeight - previousTotalWeight;
    float mean = getMean(weightCount);
    float mean3 = getMean(3);
    float stddev = getStddev(weightCount);
    float stddev3 = getStddev(3);

    pushWeight(totalWeight);
    pushStddev(stddev3);
    // calculate how many samples has the stddev3>1 condition, in the last 30 seconds
    uint8_t stddev3Count = getStddevCount(1.0);

    time_t now = time(nullptr);
    tm *tmp = localtime(&now);
    String timeStamp;
    if (tmp->tm_hour < 10)
        timeStamp += "0";
    timeStamp += tmp->tm_hour;
    timeStamp += ":";
    if (tmp->tm_min < 10)
        timeStamp += "0";
    timeStamp += tmp->tm_min;
    timeStamp += ":";
    if (tmp->tm_sec < 10)
        timeStamp += "0";
    timeStamp += tmp->tm_sec;

    if (detectionDebugOutput == true)
    {
        Serial.print(timeStamp);
        Serial.print(" Count = ");
        Serial.print(weightCount);
        Serial.print(" Total = ");
        if (totalWeight >= 0)
            Serial.print(F(" "));
        Serial.print(totalWeight);
        Serial.print("  Change = ");
        if (change >= 0)
            Serial.print(F(" "));
        Serial.print(change);
        Serial.print("  Mean = ");
        if (mean >= 0)
            Serial.print(F(" "));
        Serial.print(mean);
        Serial.print("  Mean(3) = ");
        if (mean3 >= 0)
            Serial.print(F(" "));
        Serial.print(mean3);
        Serial.print("  Stddev = ");
        if (stddev >= 0)
            Serial.print(F(" "));
        Serial.print(stddev);
        Serial.print("  Stddev(3) = ");
        if (stddev3 >= 0)
            Serial.print(F(" "));
        Serial.print(stddev3);

        Serial.print("  stddev3Count = ");
        if (stddev3Count < 10)
            Serial.print(F(" "));
        Serial.print(stddev3Count);

        Serial.print("  printingStatus = ");
        Serial.print(printingStatusString);
        Serial.println();
    }

    if (isLogging == true)
    {
        // logs every second

        String record;
        record += (String)timeStamp;
        record += ",";
        record += (String)loggerCounter;
        record += ",";
        record += (String)totalWeight;
        record += ",";
        record += (String)change;
        record += ",";
        record += (String)stddev;
        record += ",";
        record += (String)stddev3;
        record += ",";
        record += (String)stddev3Count;
        record += ",";
        record += (String)printingStatusString;
        if (logFile.println(record))
        {
            // Serial.print("Logging: ");
            // Serial.println(record);
        }
        loggerCounter++;
    }

    switch (printingStatus)
    {
    case STATUS_BOOT:
        if (weightCount > 2 && getMean(3) < 30 && getStddev(3) < 0.5)
        {
            printingStatus = STATUS_EMPTY;
            printingStatusString = "STATUS_EMPTY";
            Serial.println(F("Status changed from STATUS_BOOT to STATUS_EMPTY."));
            purgeCounter = 3; // purge array
            if (isLogging == true)
            {
                String record;
                record += (String)loggerCounter;
                record += ",";
                record += "Status changed from STATUS_BOOT to STATUS_EMPTY.";
                logFile.println(record);
            }
        }
        else if (getMean(3) >= (setting.spoolHolderWeight - 50) && getStddev(3) < 0.5)
        {
            if (purgeCounter == 0)
            {
                // Do not change state during purge counting
                printingStatus = STATUS_IDLE;
                printingStatusString = "STATUS_IDLE";
                Serial.println(F("Status changed from STATUS_BOOT to STATUS_IDLE."));
                purgeCounter = 3; // purge array after few samples
                if (isLogging == true)
                {
                    String record;
                    record += (String)loggerCounter;
                    record += ",";
                    record += "Status changed from STATUS_BOOT to STATUS_IDLE.";
                    logFile.println(record);
                }
            }
        }
        break;
    case STATUS_EMPTY:
        if (getMean(3) >= (setting.spoolHolderWeight - 50) && getStddev(3) < 0.5)
        {
            if (purgeCounter == 0)
            {
                // Do not change state during purge counting
                printingStatus = STATUS_IDLE;
                printingStatusString = "STATUS_IDLE";
                Serial.println(F("Status changed from STATUS_EMPTY to STATUS_IDLE."));
                purgeCounter = 3; // purge array after few samples
                if (isLogging == true)
                {
                    String record;
                    record += (String)loggerCounter;
                    record += ",";
                    record += "Status changed from STATUS_EMPTY to STATUS_IDLE.";
                    logFile.println(record);
                }
            }
        }
        break;
    case STATUS_IDLE:
        if (getMean(3) < (setting.spoolHolderWeight - 50) && getStddev(3) < 0.5)
        {
            if (purgeCounter == 0)
            {
                // Do not change state during purge counting
                printingStatus = STATUS_EMPTY;
                printingStatusString = "STATUS_EMPTY";
                Serial.println(F("Status changed from STATUS_IDLE to STATUS_EMPTY."));
                purgeCounter = 3; // purge array after few samples
                if (isLogging == true)
                {
                    String record;
                    record += (String)loggerCounter;
                    record += ",";
                    record += "Status changed from STATUS_IDLE to STATUS_EMPTY.";
                    logFile.println(record);
                }
            }
        }
        else if (getStddev(30) >= 1 && getStddev(30) < 30 && stddev3Count >= 10)
        {
            // Print started
            // From STATUS_IDLE to STATUS_PRINTING
            //(1) 30 sec stddev between 1 and 30
            //(2) 10 of the last 30 stddev(3) samples is more than 1.0

            // Reset all notification flags
            lowFilamentNotificationSent = false;
            fallOffRackNotificationSent = false;

            printingStatus = STATUS_PRINTING;
            printingStatusString = "STATUS_PRINTING";
            Serial.println(F("Status changed from STATUS_IDLE to STATUS_PRINTING."));
            if (setting.notifyOnPrintStarted)
            {
                notify(NOTIFICATION_PRINT_STARTED);
            }

            if (isLogging == true)
            {
                String record;
                record += (String)loggerCounter;
                record += ",";
                record += "Status changed from STATUS_IDLE to STATUS_PRINTING.";
                record += ",";
                record += "NOTIFICATION_PRINT_STARTED sent";
                logFile.println(record);
            }
        }

        break;
    case STATUS_PRINTING:
        if (getStddev(30) < 0.5 && stddev3Count < 3)
        {
            // print job completed
            printingStatus = STATUS_IDLE;
            printingStatusString = "STATUS_IDLE";
            Serial.println(F("Status changed from STATUS_PRINTING to STATUS_IDLE."));
            if (setting.notifyOnPrintCompleted)
            {
                notify(NOTIFICATION_PRINT_COMPLETED);
            }

            if (isLogging == true)
            {
                String record;
                record += (String)loggerCounter;
                record += ",";
                record += "Status changed from STATUS_PRINTING to STATUS_IDLE.";
                record += ",";
                record += "NOTIFICATION_PRINT_COMPLETED sent";
                logFile.println(record);
            }
        }

        if (fallOffRackNotificationSent == false)
        {
            if (getStddev(10) > 100 && getMean(3) < (setting.spoolHolderWeight - 50))
            {
                // fall of rack, once per print, reset flag upon new print
                notify(NOTIFICATIONI_FALL_OFF_RACK);
                fallOffRackNotificationSent = true;
                lowFilamentNotificationSent = true; // inhibit low filament notification
            }
        }

        if (lowFilamentNotificationSent == false)
        {
            if (filamentWeight < setting.lowFilamentThreshold && getStddev(30) < 30)
            {
                // notification sent once per print, reset flag upon new print
                if (setting.notifyOnLowFilament)
                {
                    notify(NOTIFICATIONI_LOW_FILAMENT);
                }
                lowFilamentNotificationSent = true;
            }
        }
        break;
    default:
        break;
    } // switch (printingStatus)

    if (purgeCounter > 0)
    {
        purgeCounter--;
        if (purgeCounter == 0)
        {
            purgeWeight(totalWeight);
            purgeStddev(0);
        }
    }

    previousTotalWeight = totalWeight;

    // auto github update related
    if (setting.autoGithubUpdate == true)
    {
        if (nextCheckTimeMillis - millis() < 10 * 60000)
        {
            if (stddev3Count >= 10)
            {
                // increase countdown time to 10 minutes
                // if there are some movements detected
                nextCheckTimeMillis = millis() + 10 * 60000 + 5000;
                // Serial.println(F("Auto github update countdown extended."));
            }
        }
    }

    // globalPrintMemory(); //debug oom

    // update page every second
    displayPage(currentPage);

    detectionTimer = millis();
}
void FILAMENT_ESTIMATOR::pushWeight(float entry)
{
    if (weightCount < DETECTION_SAMPLE_SIZE)
    {
        weightCount++;
    }
    weightArray[detectionPosition] = entry;
    detectionPosition += 1;
    if (detectionPosition == DETECTION_SAMPLE_SIZE)
    {
        detectionPosition = 0;
    }
}
void FILAMENT_ESTIMATOR::pushStddev(float entry)
{
    if (stddevCount < DETECTION_SAMPLE_SIZE)
    {
        stddevCount++;
    }
    stddevArray[stddevPosition] = entry;
    stddevPosition += 1;
    if (stddevPosition == DETECTION_SAMPLE_SIZE)
    {
        stddevPosition = 0;
    }
}
void FILAMENT_ESTIMATOR::purgeWeight(float value)
{
    // purge the whole detectionArray with single value
    // used when there is a condition change
    for (uint16_t i = 0; i < DETECTION_SAMPLE_SIZE; i++)
    {
        pushWeight(value);
    }
    // Serial.println(F("Weight Array purged."));
}
void FILAMENT_ESTIMATOR::purgeStddev(float value)
{
    // purge the whole detectionArray with single value
    // used when there is a condition change
    for (uint16_t i = 0; i < DETECTION_SAMPLE_SIZE; i++)
    {
        pushStddev(value);
    }
    // Serial.println(F("Stddev Array purged."));
}
float FILAMENT_ESTIMATOR::getSum(uint16_t samples)
{
    if (weightCount == 0 || samples == 0)
    {
        return 0;
    }
    else
    {
        if (samples > weightCount)
        {
            samples = weightCount;
        }
        if (samples > DETECTION_SAMPLE_SIZE)
        {
            samples = DETECTION_SAMPLE_SIZE;
        }
    }
    float sum = 0;
    int16_t getArrayPosition;
    for (uint8_t i = 0; i < samples; i++)
    {
        getArrayPosition = detectionPosition - 1 - i;
        if (getArrayPosition < 0)
        {
            getArrayPosition += DETECTION_SAMPLE_SIZE;
        }
        sum += weightArray[getArrayPosition];
    }
    return sum;
}
float FILAMENT_ESTIMATOR::getMean(uint16_t samples)
{
    if (weightCount == 0 || samples == 0)
    {
        return 0;
    }
    float sum = getSum(samples);
    return sum / samples;
}
float FILAMENT_ESTIMATOR::getStddev(uint16_t samples)
{
    if (weightCount == 0 || samples == 0)
    {
        return 0;
    }
    else
    {
        if (samples > weightCount)
        {
            samples = weightCount;
        }
        if (samples > DETECTION_SAMPLE_SIZE)
        {
            samples = DETECTION_SAMPLE_SIZE;
        }
    }
    float sum = 0;
    float square;
    float mu = getMean(samples);
    float theta;
    int16_t getArrayPosition;
    for (uint16_t i = 0; i < samples; i++)
    {
        getArrayPosition = detectionPosition - 1 - i;
        if (getArrayPosition < 0)
        {
            getArrayPosition += DETECTION_SAMPLE_SIZE;
        }
        theta = mu - weightArray[getArrayPosition];
        square = theta * theta;
        sum += square;
    }
    return sqrtf(sum / samples);
}
uint8_t FILAMENT_ESTIMATOR::getStddevCount(float threshold)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < DETECTION_SAMPLE_SIZE; i++)
    {
        if (stddevArray[i] > threshold)
        {
            count++;
        }
    }
    return count;
}
void FILAMENT_ESTIMATOR::notify(NOTIFICATION_MESSAGE message)
{
    if (setting.optionsBLYNK == false)
    {
        return;
    }
    if (!Blynk.connected())
    {
        Serial.println(F("Notification failed. Blynk not connected."));
        return;
    }
    switch (message)
    {
    case NOTIFICATION_TEST_MESSAGE:
        Blynk.notify(hostname + " test notification message");
        drawOverlay("Test msg", "Sent", 1000);
        Serial.println(F("Blynk notification message sent."));
        break;
    case NOTIFICATION_PRINT_STARTED:
        Serial.println(F("NOTIFICATION_PRINT_STARTED"));
        if (setting.notifyOnPrintStarted == true)
        {
            Blynk.notify(hostname + " print job started.");
            Serial.println(F("Blynk NOTIFICATION_PRINT_STARTED sent."));
        }
        drawOverlay("Print job", "Started", 1000);

        break;
    case NOTIFICATION_PRINT_COMPLETED:
        Serial.println(F("NOTIFICATION_PRINT_COMPLETED"));
        if (setting.notifyOnPrintCompleted == true)
        {
            Blynk.notify(hostname + " print job completed.");
            Serial.println(F("Blynk NOTIFICATION_PRINT_COMPLETED sent."));
        }
        drawOverlay("Print job", "Completed", 1000);
        break;
    case NOTIFICATIONI_LOW_FILAMENT:
        Serial.println(F("NOTIFICATIONI_LOW_FILAMENT"));
        if (setting.notifyOnLowFilament == true)
        {
            Blynk.notify(hostname + " low filament(" + setting.lowFilamentThreshold + "g).");
            Serial.println(F("Blynk NOTIFICATIONI_LOW_FILAMENT sent."));
        }
        drawOverlay("Low", "Filament", 1000);
        break;
    case NOTIFICATIONI_FALL_OFF_RACK:
        Serial.println(F("NOTIFICATIONI_FALL_OFF_RACK"));
        if (setting.notifyOnFallOffRack == true)
        {
            Blynk.notify(hostname + " fall off rack.");
            Serial.println(F("Blynk NOTIFICATIONI_FALL_OFF_RACK sent."));
        }
        drawOverlay("Fall Off", "Rack", 1000);
        break;
    case NOTIFICATIONI_FALL_OFF_BEARING:
        Serial.println(F("NOTIFICATIONI_FALL_OFF_BEARING"));
        if (setting.notifyOnFallOffBearing == true)
        {
            Blynk.notify(hostname + " fall off bearing.");
            Serial.println(F("Blynk NOTIFICATIONI_FALL_OFF_BEARING sent."));
        }
        drawOverlay("Fall Off", "Bearing", 1000);
        break;
    case NOTIFICATIONI_TANGLED:
        Serial.println(F("NOTIFICATIONI_TANGLED"));
        if (setting.notifyOnTangled == true)
        {
            Blynk.notify(hostname + " tangled.");
            Serial.println(F("Blynk NOTIFICATIONI_TANGLED sent."));
        }
        drawOverlay("Filament", "Tangled", 1000);
        break;
    default:
        break;
    }
}
bool FILAMENT_ESTIMATOR::getNotificationSetting(uint8_t selection)
{
    bool result = false;
    switch (selection)
    {
    case NOTIFICATION_MENU_PRINT_STARTED:
        result = setting.notifyOnPrintStarted;
        break;
    case NOTIFICATION_MENU_PRINT_COMPLETED:
        result = setting.notifyOnPrintCompleted;
        break;
    case NOTIFICATION_MENU_LOW_FILAMENTT:
        result = setting.notifyOnLowFilament;
        break;
    case NOTIFICATION_MENU_FALL_OFF_RACK:
        result = setting.notifyOnFallOffRack;
        break;
    case NOTIFICATION_MENU_FALL_OFF_BEARING:
        result = setting.notifyOnFallOffBearing;
        break;
    case NOTIFICATION_MENU_TANGLED:
        result = setting.notifyOnTangled;
        break;
    default:
        break;
    }
    return result;
}
void FILAMENT_ESTIMATOR::setNotificationSetting(uint8_t selection, bool value)
{
    switch (selection)
    {
    case NOTIFICATION_MENU_PRINT_STARTED:
        setting.notifyOnPrintStarted = value;
        break;
    case NOTIFICATION_MENU_PRINT_COMPLETED:
        setting.notifyOnPrintCompleted = value;
        break;
    case NOTIFICATION_MENU_LOW_FILAMENTT:
        setting.notifyOnLowFilament = value;
        break;
    case NOTIFICATION_MENU_FALL_OFF_RACK:
        setting.notifyOnFallOffRack = value;
        break;
    case NOTIFICATION_MENU_FALL_OFF_BEARING:
        setting.notifyOnFallOffBearing = value;
        break;
    case NOTIFICATION_MENU_TANGLED:
        setting.notifyOnTangled = value;
        break;
    default:
        break;
    }
    saveToEEPROM();
}
void FILAMENT_ESTIMATOR::checkGithubLatestRelease(bool forceCheck, bool doUpdate, bool checkDataFolder, bool updateDataFolder)
{

    drawOverlay("Checking", "Github", 2000);

    // globalPrintMemory("Before certStore");

    // A single, global CertStore which can be used by all
    // connections.  Needs to stay live the entire time any of
    // the WiFiClientBearSSLs are present.
    BearSSL::CertStore certStore; // takes only 16 bytes
    _certStore = &certStore;

    // globalPrintMemory("After certStore");

    if (checkGithubConnectionPrerequisite() == false) // certs read takes 50 bytes
    {
        return;
    }
    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory("After certs read");

    BearSSL::WiFiClientSecure client; // 7k memory required
    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory(F("After client creation.")); // about 26k left
    client.setCertStore(_certStore);
    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory("After client setCertStore."); // same, 26k left

    // Try to reduce connection memory usage
    // Checking MFLN support
    bool MFLN_Support = client.probeMaxFragmentLength(githubHost, githubPort, 512);
    if (MFLN_Support == true)
    {
        // Serial.println(F("MFLN 512 fragment length probing ok."));
        client.setBufferSizes(512, 512);
    }

    if (!client.connect(githubHost, githubPort)) // 5k memory required (22k without MFLN)
    {
        Serial.println(F("Connection to gitgub failed."));
        drawOverlay("Failed", "", 2000);
        return;
    }
    else // connected ok
    {
        if (MFLN_Support == true)
        {
            if (client.getMFLNStatus())
            {
                // connection RAM usage reduced to 5.5k using 512 bytes MFLN
                // Serial.println(F("MFLN 512 negotiation succeeded."));
            }
            else
            {
                Serial.println(F("MFLN 512 negotiation failed."));
                drawOverlay("Failed", "", 2000);
            }
        }
    }
    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory("After client connection"); // about 20k left

    // check firmware
    if (forceCheck == true || (githubUpdateAvailable == false && doUpdate == true))
    {
        // Start to check github

        Serial.println(F("Checking github latest release version:"));

        String url = "/repos/FuzzyNoodle/Fuzzy-Spooder/releases/latest"; // has to use readStringUntil('\n'), otherwise buffer overflow result in nothing can be printed

        // rate limit to 60 calls per hour
        // fetch the X-RateLimit-Remaining value
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + githubHost + "\r\n" +
                     "User-Agent: FuzzySpooder\r\n" +
                     "Connection: close\r\n\r\n");
        // fetch the http header
        uint8_t releaseLimitRemaining = 0;
        uint16_t releaseContentLength = 0;
        while (client.connected())
        {
            String response = client.readStringUntil('\n');
            if (VERBOSE_DEBUG_PRINT == true)
            {
                Serial.println(response);
            }
            if (response == "\r")
            {
                break;
            }
            if (response.indexOf("X-RateLimit-Remaining:") == 0)
            {
                // response looks like this:
                // X-RateLimit-Remaining: 56
                String sub = response.substring(response.indexOf(" ") + 1);
                releaseLimitRemaining = sub.toInt();
                // Serial.print(F("X-RateLimit-Remaining: "));
                // Serial.println(limitRemaining);
            }
            if (response.indexOf("Content-Length:") == 0)
            {
                // response looks like this:
                // Content-Length: 3799
                String sub = response.substring(response.indexOf(" ") + 1);
                releaseContentLength = sub.toInt();
                // Serial.print(F("Content-Length: "));
                // Serial.println(contentLength);
            }
        }
        // Check RAM
        if (ESP.getFreeHeap() > (releaseContentLength + 2048))
        {
            // ok, with 2k reserve
        }
        else
        {
            Serial.print(F("Not enough memory to parse json doc."));
            drawOverlay("Failed", "", 2000);
            globalPrintMemory("");
            return;
        }
        uint16_t capacity = releaseContentLength + 2048; // https://arduinojson.org/v6/assistant/
        DynamicJsonDocument doc(capacity);

        // fetch the github release json doc
        while (client.connected())
        {
            if (client.available())
            {
                DeserializationError error;
                error = deserializeJson(doc, client.readStringUntil('\n')); // no RAM/speed difference using Stream or String
                if (error != error.Ok)
                {
                    Serial.print(F("Deserialization error: "));
                    drawOverlay("Failed", "", 2000);
                    Serial.println(error.c_str());
                    return;
                }
            }
        }
        globalPrintMemory("After deserializeJson"); // Heap: 14888    Block: 12040
        // client.stop(); //increases 5k RAM
        // globalPrintMemory("After client.stop()");   //23k left,

        // print the content
        if (VERBOSE_DEBUG_PRINT == true)
        {
            serializeJsonPretty(doc, Serial);
            Serial.println();
        }

        // globalPrintMemory("Before doc.containsKey('tag_name')"); //Heap: 18312    Block: 18128
        // RAM release ok at this point
        // Fetch the content
        if (doc.containsKey("tag_name"))
        {
            String releaseTag = doc["tag_name"];
            githubVersion = releaseTag; // Storing the String tag into global reserved variable
            githubVersionObtained = true;

            convertTagToVersion(releaseTag, &githubLatestReleaseVersion);
            // Serial.print(F("Github version(string tag): "));
            // Serial.println(releaseTag);

            Serial.print(F("Github latest version: "));
            printVersion(githubLatestReleaseVersion);

            Serial.print(F("Current firmware version: "));
            printVersion(currentVersion);
            // bool isPrerelease = doc["prerelease"];
            // Serial.print(F("Is prerelease: "));
            // Serial.println(isPrerelease ? "Yes" : "No");
            if (versionToNumber(githubLatestReleaseVersion) > versionToNumber(currentVersion))
            {

                JsonArray assets = doc["assets"];
                for (auto asset : assets)
                {
                    const char *asset_type = asset["content_type"];
                    const char *asset_name = asset["name"];
                    const char *asset_url = asset["browser_download_url"];

                    if (strcmp(asset_type, GHOTA_CONTENT_TYPE) == 0 && strcmp(asset_name, _binFile) == 0)
                    {
                        // firmware file found
                        Serial.print(F("New version at: "));
                        Serial.println(asset_url);
                        _updateURL = asset_url;
                        githubUpdateAvailable = true;
                    }
                    else
                    {

                        Serial.println(F("Firmware file not found."));
                        githubUpdateAvailable = false;
                        return;
                    }
                }
            }
            else // if (versionToNumber(githubLatestReleaseVersion) > versionToNumber(currentVersion))
            {
                // Serial.println(F("No new version available on github."));
                githubUpdateAvailable = false;
                // RAM release ok at this point.
                return;
            }
        }
        else // if (!doc.containsKey("tag_name"))
        {
            Serial.println(F("No tag_name found."));
            githubVersionObtained = false;
            githubUpdateAvailable = false;
            drawOverlay("Failed", "", 2000);
            return;
        }

        // resolve redirects
        // splitURL = _urlDetails(_updateURL); //Could be sourece of RAM fragmentation
        //_updateURL requires some 513 bytes to hold the URL
        // If used locally, spooder cannot perform seperated check/update operation.
        // If used globally, need to reserved large enough RAM to prevent RAM fragmentation
        // So _updateURL String size 768 is used here, globally.

        // Serial.print(F("_updateURL = "));
        // Serial.println(_updateURL);
        /* _updateURL example: 515 bytes
        _updateURL = https://github-releases.githubusercontent.com/347605015/
        c6847f91-dac8-43b9-a55e-7bdae94af08f?X-Amz-Algorithm=AWS4-HMAC-SHA256&X
        -Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20210829%2Fus-east-1%2Fs3%2Faws4
        _request&X-Amz-Date=20210829T150643Z&X-Amz-Expires=300&X-Amz-Signature=
        8286df8a22c44df7b53c6ad9e4747923bc8120459d11ad96c5f7085a51b30082&X-Amz-
        SignedHeaders=host&actor_id=0&key_id=0&repo_id=347605015&response-content
        -disposition=attachment%3B%20filename%3Dfirmware.bin&response-content-
        type=application%2Foctet-stream
        */
        String proto = "";
        int port = 80;
        if (_updateURL.startsWith("http://"))
        {
            proto = "http://";
            _updateURL.replace("http://", "");
        }
        else
        {
            proto = "https://";
            port = 443;
            _updateURL.replace("https://", "");
        }
        int firstSlash = _updateURL.indexOf('/');
        String host = _updateURL.substring(0, firstSlash);
        String path = _updateURL.substring(firstSlash);

        bool isFinalURL = false;

        while (!isFinalURL)
        {
            if (!client.connect(host, port))
            {
                Serial.println(F("Client connection Failed."));
                return;
            }

            client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                         "Host: " + host + "\r\n" +
                         "User-Agent: FuzzySpooder\r\n" +
                         "Connection: close\r\n\r\n");

            while (client.connected())
            {
                String response = client.readStringUntil('\n');
                if (response.startsWith("location: ") || response.startsWith("Location: "))
                {
                    isFinalURL = false;
                    String location = response;
                    if (response.startsWith("location: "))
                    {
                        location.replace("location: ", "");
                    }
                    else
                    {
                        location.replace("Location: ", "");
                    }
                    location.remove(location.length() - 1);

                    if (location.startsWith("http://") || location.startsWith("https://"))
                    {
                        // absolute URL - separate host from path
                        // urlDetails_t url = _urlDetails(location);

                        int port = 80;
                        if (location.startsWith("http://"))
                        {
                            proto = "http://";
                            location.replace("http://", "");
                        }
                        else
                        {
                            proto = "https://";
                            port = 443;
                            location.replace("https://", "");
                        }
                        int firstSlash = location.indexOf('/');
                        host = location.substring(0, firstSlash);
                        path = location.substring(firstSlash);
                    }
                    else
                    {
                        // relative URL - host is the same as before, location represents the new path.
                        path = location;
                    }
                    // leave the while loop so we don't set isFinalURL on the next line of the header.
                    break;
                }
                else
                {
                    // location header not present - this must not be a redirect. Treat this as the final address.
                    isFinalURL = true;
                }
                if (response == "\r")
                {
                    break;
                }
            }
        }
        if (isFinalURL)
        {
            String finalURL = proto + host + path;
            _updateURL = finalURL; // Not the source of RAM fragment problem
        }
        else
        {
            Serial.println(F("URL unresolved."));
            drawOverlay("Failed", "", 2000);
            return;
        }

    } // if (forceCheck == true || githubUpdateAvailable == false)

    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory("After tag check"); // Heap: 20432    Block: 19944

    // RAM release ok only tag check: Heap: 32112    Block: 30192

    // Check Data Folder
    if (checkDataFolder == true)
    {
        // root/sub folders to sync
        String pathData = "/data";
        listGitgubContent(client, pathData, updateDataFolder);
        // String pathImages = "/data/images";
        // listGitgubContent(client, pathImages, updateDataFolder)

    } // if (getDataFolder == true)

    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory("After data fetching: ");

    // globalPrintMemory("Before update:"); //Heap: 21040    Block: 19640 (without update)
    // return; //Heap: 34800    Block: 34472 if return here

    // Update firmware
    if (githubUpdateAvailable == true && doUpdate == true)
    {
        ESP8266HTTPUpdate ESPhttpUpdate; // use local instance to free up RAM
        Serial.println(F("Updating now..."));
        drawOverlay("Updating", "Firmware", 5000);

        // set flag for time extension
        setting.automaticUpdateJustPerformed = true;
        saveToEEPROM();

        t_httpUpdate_return ret = ESPhttpUpdate.update(client, _updateURL);
        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            Serial.println(ESPhttpUpdate.getLastErrorString());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
            break;

        case HTTP_UPDATE_OK:
            Serial.println(F("HTTP_UPDATE_OK"));
            break;
        }

        // if the program gets here, automatic update is not successful
        // clear flag
        setting.automaticUpdateJustPerformed = false;
        saveToEEPROM();
    }
    drawOverlay("Done", "", 2000);
    // globalPrintMemory("After update:"); //Heap: 21040    Block: 19640
    return;
}
void FILAMENT_ESTIMATOR::listGitgubContent(BearSSL::WiFiClientSecure &client, String &path, bool updateDataFolder)
{
    // Getting contents of \data folder

    Serial.print(F("Getting contents of "));
    Serial.println(path);
    if (VERBOSE_DEBUG_PRINT == true)
        globalPrintMemory("data");
    if (!client.connect(githubHost, githubPort))
    {
        Serial.println(F("Client connection Failed."));
        return;
    }
    String url = "/repos/FuzzyNoodle/Fuzzy-Spooder/contents" + path;
    // String url = "/repos/FuzzyNoodle/Fuzzy-Spooder/contents/data/images?ref=main"; //subfolder requires ref
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + githubHost + "\r\n" +
                 "User-Agent: FuzzySpooder\r\n" +
                 "Connection: close\r\n\r\n");

    // fetch the http header
    uint8_t dataLimitRemaining = 0;
    uint16_t dataContentLength = 0;
    while (client.connected())
    {
        String response = client.readStringUntil('\n');
        if (VERBOSE_DEBUG_PRINT == true)
        {
            Serial.println(response);
        }
        if (response == "\r")
        {
            break;
        }
        if (response.indexOf("X-RateLimit-Remaining:") == 0)
        {
            // response looks like this:
            // X-RateLimit-Remaining: 56
            String sub = response.substring(response.indexOf(" ") + 1);
            dataLimitRemaining = sub.toInt();
            // Serial.print(F("X-RateLimit-Remaining: "));
            // Serial.println(limitRemaining);
        }
        if (response.indexOf("Content-Length:") == 0)
        {
            // response looks like this:
            // Content-Length: 3799
            String sub = response.substring(response.indexOf(" ") + 1);
            dataContentLength = sub.toInt();
            // Serial.print(F("Content-Length: "));
            // Serial.println(contentLength);
        }
    }
    // Check RAM
    if (ESP.getFreeHeap() > (dataContentLength + 2048))
    {
        // ok, with 2k reserve
    }
    else
    {
        Serial.print(F("Not enough memory to parse json doc."));
        drawOverlay("Failed", "", 2000);
        globalPrintMemory("");
        return;
    }
    uint16_t capacity = dataContentLength + 2048; // https://arduinojson.org/v6/assistant/
    DynamicJsonDocument doc(capacity);

    // fetch the github release json doc
    while (client.connected())
    {
        if (client.available())
        {
            DeserializationError error;
            error = deserializeJson(doc, client.readStringUntil('\n')); // no RAM/speed difference using Stream or String
            if (error != error.Ok)
            {
                Serial.print(F("Deserialization error: "));
                drawOverlay("Failed", "", 2000);
                Serial.println(error.c_str());
                return;
            }
        }
    }

    // print the content
    if (VERBOSE_DEBUG_PRINT == true)
    {
        serializeJsonPretty(doc, Serial);
        Serial.println();
    }

    // JsonObject object = doc.as<JsonObject>();
    /*
        for (JsonObject item : doc.as<JsonArray>())
        {

            const char *name = item["name"]; // "certs.ar", "config.json", "edit", "favicon.ico", "images", ...
            const char *path = item["path"]; // "data/certs.ar", "data/config.json", "data/edit", ...
            const char *sha = item["sha"];   // "1ee8be5142f1f05ac8a9453e509beee04ad1fa4a", ...
            long size = item["size"];        // 173138, 568, 0, 15406, 0, 64
            const char *url = item["url"];
            const char *html_url = item["html_url"];
            const char *git_url = item["git_url"];
            const char *download_url = item["download_url"];
            const char *type = item["type"]; // "file", "file", "dir", "file", "dir", "file"

            JsonObject links = item["_links"];
            const char *links_self = links["self"];
            const char *links_git = links["git"];
            const char *links_html = links["html"];

            Serial.print(F("name = "));
            Serial.print(name);
            Serial.print("\t");

            Serial.print(F("type = "));
            Serial.print(type);
            Serial.print("\t");

            Serial.print(F("path = "));
            Serial.print(path);
            Serial.print("\t");

            Serial.println();
        }
        */

    JsonArray remoteArr = doc.as<JsonArray>();
    /*
    uint8_t numberOfFiles = 0;
    uint8_t numberOfDir = 0;
    for (uint8_t i = 0; i < remoteArr.size(); i++)
    {
        if (strncmp((const char *)remoteArr.getElement(i).getMember("type"), "file", 4) == 0)
        {
            numberOfFiles++;
        }
        if (strncmp((const char *)remoteArr.getElement(i).getMember("type"), "dir", 3) == 0)
        {
            numberOfDir++;
        }
        //Serial.println((const char *)remoteArr.getElement(i).getMember("path"));
    }
    Serial.print(F("Total "));
    Serial.print(numberOfFiles);
    Serial.print(F(" file(s), and "));
    Serial.print(numberOfDir);
    Serial.println(F(" dir(s)."));
    */

    manageLocalFileInfo();

    File localFileInfo = LittleFS.open(localFileInfoPath, "r");
    DynamicJsonDocument localFileDoc(2048);
    deserializeJson(localFileDoc, localFileInfo);
    JsonArray localArr = localFileDoc.as<JsonArray>();

    for (JsonObject remoteFileObj : remoteArr)
    {
        if (remoteFileObj.getMember("type") == "file")
        {
            String remoteName = remoteFileObj.getMember("name");
            String remotePath = remoteFileObj.getMember("path");
            String sha = remoteFileObj.getMember("sha");
            Serial.print(F("checking "));
            Serial.print(remotePath);
            Serial.print(F(" "));
            Serial.println(sha);
            bool excluded = false;
            if (remoteName == "config.json")
            {
                excluded = true;
            }
            if (excluded == true)
            {
                Serial.println(F("Excluded."));
            }
            else // excluded == false
            {
                for (JsonObject localFileObj : localArr)
                {
                    if (localFileObj.getMember("path") == remotePath)
                    {
                        Serial.println(F("File info found."));
                        if (localFileObj.getMember("sha") == "")
                        {
                            // first time checking github, putting github sha into local file info
                            localFileObj["sha"] = remoteFileObj.getMember("sha");
                            Serial.print(F("Update \"sha\" to"));
                            Serial.println((const char *)localFileObj.getMember("sha"));
                        }
                    }
                    else
                    {
                        // Serial.println(F("Not found in local file info."));
                    }
                } // for (JsonObject localFileObj : localArr)
            }

        } // if (remoteFileObj.getMember("type") == "file")

    } // for (JsonObject remoteFileObj : remoteArr)

    serializeJsonPretty(localFileDoc, Serial); // output to Serial
    Serial.println();
    // todo steps
    // 1. list github  directory

    // 2. if listed item is a file, push the item into a json doc

    // 3. iterate json doc items
    // 4 omit these files
    //   - config.json
    // 5. compare each item with items in the localfiles.json
    // 6. if the tag is different, download the file from github to esp8266 file system
    // 7. update the new tag in localfiles.json

    // downliod a test file
    // imported from https://forum.arduino.cc/t/nodemcu-esp8266-downloading-an-image-file-to-spiffs/543405/2

    // https://api.github.com/repos/FuzzyNoodle/Fuzzy-Spooder/contents/data/testdoc.txt?ref=main
    // https://github.com/FuzzyNoodle/Fuzzy-Spooder/blob/main/data/testdoc.txt
    // https://raw.githubusercontent.com/FuzzyNoodle/Fuzzy-Spooder/main/data/testdoc.txt
    if (updateDataFolder == true)
    {
        HTTPClient http;
        // url = "https://raw.githubusercontent.com/FuzzyNoodle/Fuzzy-Spooder/main/data/testdoc.txt";
        String url = "https://raw.githubusercontent.com/FuzzyNoodle/Fuzzy-Spooder/main/data/images/logo.bmp";

        String fileName = "/logo.bmp";

        Serial.println("[HTTP] begin...\n");
        Serial.println(fileName);
        Serial.println(url);
        http.begin(client, url);

        Serial.printf("[HTTP] GET...\n", url.c_str());
        // start connection and send HTTP header
        int httpCode = http.GET();
        if (httpCode > 0)
        {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            Serial.printf("[FILE] open file for writing %d\n", fileName.c_str());

            File file = LittleFS.open(fileName, "w");

            // file found at server
            if (httpCode == HTTP_CODE_OK)
            {

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = http.getSize();

                // create buffer for read
                uint8_t buff[128] = {0};

                // get tcp stream
                WiFiClient *stream = http.getStreamPtr();

                // read all data from server
                while (http.connected() && (len > 0 || len == -1))
                {
                    // get available data size
                    size_t size = stream->available();
                    if (size)
                    {
                        // read up to 128 byte
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        // write it to Serial
                        // Serial.write(buff, c);
                        file.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println();
                Serial.println("[HTTP] connection closed or file end.");
                Serial.println("[FILE] closing file");
                file.close();
            }
        }
        http.end();
    }
}
void FILAMENT_ESTIMATOR::manageLocalFileInfo()
{
    if (!LittleFS.begin())
    {
        Serial.println(F("LittleFS failed."));
        drawOverlay("Failed", "", 2000);
        return;
    }
    Serial.println(F("Managing local files:"));
    // 1.open localfile.json as JSONDOC
    // 2.iterate through FS dir, if exist in json, add to json
    // 3.iterate through localfile.json, if not exist in FS, remove json
    // 4.save localfile.json

    // listDir("");

    // 1.open localfile.json
    // Serial.println(F("Listing localfiles.json:"));
    File localFileInfo = LittleFS.open(localFileInfoPath, "r");
    // todo dertimine size
    DynamicJsonDocument localFileDoc(2048);
    deserializeJson(localFileDoc, localFileInfo);
    // serializeJsonPretty(localFileDoc, Serial); //output to Serial
    // Serial.println();
    LOCAL_FILE_INFO_STRUCT fInfo;
    // 2.iterate through FS dir
    Serial.println(F("Iterate through local files:"));

    iterateLocalFiles("/", localFileDoc);

    // Serial.println(F("Listing new localfiles.json:"));
    // serializeJsonPretty(localFileDoc, Serial); //output to Serial
    // Serial.println();

    // 3.iterate through localfile.json, if not exist in FS, remove json
    Serial.println(F("Iterate through fileinfo.json:"));
    JsonArray arr = localFileDoc.as<JsonArray>();
    uint8_t indexToRemove = 0;
    for (JsonObject objInFile : arr)
    {
        String path = objInFile.getMember("path");
        if (path)
        {
            // path contains "data/" for all files, however, FS does not. Need to remove this.
            String fsPath = path.substring(5);
            if (!LittleFS.open(fsPath, "r"))
            {
                Serial.print(path);
                Serial.println(F(" does not exist, info removed."));
                arr.remove(indexToRemove);
            }
        }
        indexToRemove++;
    }

    // 4.save localfile.json
    localFileInfo = LittleFS.open(localFileInfoPath, "w");
    serializeJsonPretty(localFileDoc, localFileInfo);
}
void FILAMENT_ESTIMATOR::iterateLocalFiles(String dirname, DynamicJsonDocument &localFileDoc)
{
    Dir dir = LittleFS.openDir(dirname);
    while (dir.next())
    {
        if (dir.isFile())
        {
            String name = dir.fileName();
            String path = "data"; // github path includes filename
            if (dirname == "/")
            {
                path += "/";
            }
            else
            {
                path += dirname + "/";
            }
            path += name;

            // check if the obj already exist
            JsonArray arr = localFileDoc.as<JsonArray>();
            bool isExist = false;
            for (JsonObject objInFile : arr)
            {
                if (objInFile.getMember("path") == path)
                {
                    isExist = true;
                }
            }
            if (isExist == true)
            {
                // Serial.print(path);
                // Serial.println(isExist ? " exists." : " does not exist.");
            }
            else // does not exist, add a new object
            {
                JsonObject obj = localFileDoc.createNestedObject();
                obj["name"] = name;
                obj["path"] = path;
                obj["sha"] = "";
                Serial.print(path);
                Serial.println(F(" added to localfile.json."));
            }
        }
        else // isDirectory()
        {
            String subDir = (String(dirname) + dir.fileName());
            iterateLocalFiles(subDir, localFileDoc);
        }
    }
}
bool FILAMENT_ESTIMATOR::checkGithubConnectionPrerequisite()
{

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("WiFi not connected."));
        drawOverlay("Failed", "", 2000);
        return false;
    }
    if (!LittleFS.begin())
    {
        Serial.println(F("LittleFS failed."));
        drawOverlay("Failed", "", 2000);
        return false;
    }
    if (netWorkTimeReceived == false)
    {
        Serial.println(F("Network time not configured."));
        drawOverlay("Failed", "", 2000);
        return false;
    }
    numCerts = _certStore->initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    // Serial.print(F("Number of CA certs read: "));
    // Serial.println(numCerts);
    if (numCerts == 0)
    {
        Serial.println(F("No CA certs found. Unable to establish https connection."));
        drawOverlay("Failed", "", 2000);
        return false;
    }
    return true;
}
bool FILAMENT_ESTIMATOR::getOptionsSetting(uint8_t selection)
{
    bool result = false;
    switch (selection)
    {
    case OPTIONS_MENU_WIFI:
        result = setting.optionsWiFi;
        break;
    case OPTIONS_MENU_MDNS:
        result = setting.optionsMDNS;
        break;
    case OPTIONS_MENU_BLYNK:
        result = setting.optionsBLYNK;
        break;
    case OPTIONS_MENU_WEB_SERVER:
        result = setting.optionsWebServer;
        break;
    case OPTIONS_MENU_SPOODER_CLIENT:
        result = setting.optionsSpooderClient;
        break;
    case OPTIONS_MENU_SPOODER_SERVER:
        result = setting.optionsSpooderServer;
        break;
    case OPTIONS_MENU_ARDUINO_OTA:
        result = setting.optionsArduinoOTA;
        break;
    case OPTIONS_MENU_AUTO_HOMEPAGE:
        result = setting.optionsAutoHomepage;
        break;
    default:
        break;
    }
    return result;
}
void FILAMENT_ESTIMATOR::setOptionsSetting(uint8_t selection, bool value)
{
    switch (selection)
    {
    case OPTIONS_MENU_WIFI:
        setting.optionsWiFi = value;
        break;
    case OPTIONS_MENU_MDNS:
        setting.optionsMDNS = value;
        break;
    case OPTIONS_MENU_BLYNK:
        setting.optionsBLYNK = value;
        break;
    case OPTIONS_MENU_WEB_SERVER:
        setting.optionsWebServer = value;
        break;
    case OPTIONS_MENU_SPOODER_CLIENT:
        setting.optionsSpooderClient = value;
        break;
    case OPTIONS_MENU_SPOODER_SERVER:
        setting.optionsSpooderServer = value;
        break;
    case OPTIONS_MENU_ARDUINO_OTA:
        setting.optionsArduinoOTA = value;
        break;
    case OPTIONS_MENU_AUTO_HOMEPAGE:
        setting.optionsAutoHomepage = value;
        break;
    default:
        break;
    }
    Serial.print(optionsMenuTitle[selection]);
    Serial.print(F(" set to: "));
    Serial.println((value == true) ? "On" : "Off");
    saveToEEPROM();
}
bool FILAMENT_ESTIMATOR::versionTagIsValid(VERSION_STRUCT v)
{
    return true;
}
bool FILAMENT_ESTIMATOR::convertTagToVersion(String t, VERSION_STRUCT *v)
{
    int8_t firstDotIndex = t.indexOf(".");
    int8_t secondDotIndex = t.lastIndexOf(".");
    // simple check if the tag is valid, should contain two '.' at different places
    if (firstDotIndex == -1 || secondDotIndex == -1)
    {
        return false;
    }
    if (firstDotIndex == 0 || secondDotIndex == t.length() - 1 || secondDotIndex - firstDotIndex <= 1)
    {
        return false;
    }
    v->major = t.substring(0, firstDotIndex).toInt();
    v->minor = t.substring(firstDotIndex + 1, secondDotIndex).toInt();
    v->patch = t.substring(secondDotIndex + 1).toInt();
    return true; // tag passes simple check
}
void FILAMENT_ESTIMATOR::printVersion(VERSION_STRUCT v)
{
    Serial.print(v.major);
    Serial.print(".");
    Serial.print(v.minor);
    Serial.print(".");
    Serial.print(v.patch);
    Serial.println();
}
void FILAMENT_ESTIMATOR::saveGitgubFileToFs()
{
    Serial.println(F("Save github file to FS:"));
    return;
}
void FILAMENT_ESTIMATOR::setNextCheckCountdown()
{
    nextCheckTimeMillis = millis() + random(60 * 60000, 120 * 60000); // 60~120 min in ms
    // nextCheckTimeMillis = millis() + 2 * 60000;
    if (setting.automaticUpdateJustPerformed == true)
    {
        // just performed an automatic update, increase next check count down
        setting.automaticUpdateJustPerformed = false;
        saveToEEPROM();
        nextCheckTimeMillis += random(120 * 60000, 240 * 60000);
    }
}
void FILAMENT_ESTIMATOR::updateAutoGithubCheck()
{
    if (millis() - updateAutoGithubCheckTimer < UPDATE_AUTO_GITHUB_CHECK_PEROID)
    {
        return;
    }

    // less than one minute
    if (nextCheckTimeMillis - millis() < 60000)
    {
        // check version on GitHub and update firmware if new version is available
        checkGithubLatestRelease(true, true, false, false);
        setNextCheckCountdown(); // in case of update firmware failure
    }

    updateAutoGithubCheckTimer = millis();
}
void FILAMENT_ESTIMATOR::pause()
{
    while (true)
    {
        if (Serial.available())
        {
            String s = Serial.readString();
            break;
        }
    }
}
void FILAMENT_ESTIMATOR::enterHotspotMode()
{
    Serial.println(F("Enter hotspot mode."));
    isHotspotMode = true;

    if (enableWifi == true)
    {
        setWifi(false);
    }
    delay(100);

    IPAddress accessPointIP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);

    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(WiFi.softAPConfig(accessPointIP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting soft-AP ... ");
    Serial.println(WiFi.softAP("Spooder_Setup") ? "Ready" : "Failed!");

    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());

    setWebServer(true);

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", accessPointIP);
}
void FILAMENT_ESTIMATOR::exitHotspotMode()
{
    Serial.println(F("Exit hotspot mode."));
    dnsServer.stop();
    bool exitSoftAP = WiFi.softAPdisconnect(true);
    Serial.print(F("Disconnecting soft-AP... "));
    Serial.println(exitSoftAP ? F("ok") : F("failed"));
    delay(500); // prevent wdt reset for unknown reason

    WiFi.mode(WIFI_STA);
    isHotspotMode = false;
    // turn wifi on again if desired
    if (setting.optionsWiFi == true)
    {
        setWifi(true);
    }
    /*
    if (resetWiFi == true)
    {
        resetWiFi = false;
    }
    // reconnect blynk if blynk auth token changed
    if (resetBlynk == true)
    {
        if (enableBlynk == true)
        {
            setBlynk(false);
            setBlynk(true);
        }
        resetBlynk = false;
    }
    */
}
void FILAMENT_ESTIMATOR::handleSetupSave()
{
    Serial.println(F("Setup save button pressed, values:"));
    SETUP_SAVE_REPLY_MESSAGE ssidReply;
    SETUP_SAVE_REPLY_MESSAGE passwordReply;
    SETUP_SAVE_REPLY_MESSAGE blynkAuthReply;

    String ssid = webServer->arg("ssid");
    String password = webServer->arg("password");
    String blynkAuth = webServer->arg("blynk_auth_token");

    Serial.print(ssid);
    Serial.print(F(":"));
    Serial.println(ssid.length());

    Serial.print(password);
    Serial.print(F(":"));
    Serial.println(password.length());

    Serial.print(blynkAuth);
    Serial.print(F(":"));
    Serial.println(blynkAuth.length());

    // some error checking
    if (ssid.equals(String(wifi_ssid)))
    {
        ssidReply = SETUP_NO_CHANGE;
    }
    else
    {
        if (ssid.length() > 32)
        {
            ssidReply = SETUP_TOO_LONG;
        }
        else if (ssid.length() == 0)
        {
            ssidReply = SETUP_CLEARED;
            strcpy(wifi_ssid, ssid.c_str());
        }
        else
        {
            ssidReply = SETUP_SAVED;
            strcpy(wifi_ssid, ssid.c_str());
        }
    }

    if (password.equals(String(wifi_password)))
    {
        passwordReply = SETUP_NO_CHANGE;
    }
    else
    {
        if (password.length() > 63)
        {
            passwordReply = SETUP_TOO_LONG;
        }
        else if (ssid.length() == 0)
        {
            passwordReply = SETUP_CLEARED;
            strcpy(wifi_password, password.c_str());
        }
        else
        {
            passwordReply = SETUP_SAVED;
            strcpy(wifi_password, password.c_str());
        }
    }

    if (blynkAuth.equals(String(blynk_auth)))
    {
        blynkAuthReply = SETUP_NO_CHANGE;
    }
    else
    {
        if (blynkAuth.length() != 32)
        {
            blynkAuthReply = SETUP_INCORRECT_LENGTH;
        }
        else
        {
            blynkAuthReply = SETUP_SAVED;
            strcpy(blynk_auth, blynkAuth.c_str());
        }
    }

    // Send captive portal web page in chunks
    webServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
    webServer->send(200, "text/html", FPSTR(captivePortalResponseHTML1));

    switch (ssidReply)
    {
    case SETUP_TOO_LONG:
        webServer->sendContent(FPSTR(setupReplyTooLong));
        break;
    case SETUP_CLEARED:
        webServer->sendContent(FPSTR(setupReplyCleared));
        resetWiFi = true;
        break;
    case SETUP_SAVED:
        webServer->sendContent(FPSTR(setupReplySaved));
        resetWiFi = true;
        break;
    case SETUP_NO_CHANGE:
        webServer->sendContent(FPSTR(setupReplyNoChange));
        break;
    default:
        break;
    }

    webServer->sendContent(FPSTR(captivePortalResponseHTML2));
    if (wifi_ssid[0] != '\0') // prevent null value ending the transmission
        webServer->sendContent(wifi_ssid);
    webServer->sendContent(FPSTR(captivePortalResponseHTML3));

    switch (passwordReply)
    {
    case SETUP_TOO_LONG:
        webServer->sendContent(FPSTR(setupReplyTooLong));
        break;
    case SETUP_CLEARED:
        webServer->sendContent(FPSTR(setupReplyCleared));
        resetWiFi = true;
        break;
    case SETUP_SAVED:
        webServer->sendContent(FPSTR(setupReplySaved));
        resetWiFi = true;
        break;
    case SETUP_NO_CHANGE:
        webServer->sendContent(FPSTR(setupReplyNoChange));
        break;
    default:
        break;
    }

    webServer->sendContent(FPSTR(captivePortalResponseHTML4));
    if (wifi_password[0] != '\0')
        webServer->sendContent(wifi_password);
    webServer->sendContent(FPSTR(captivePortalResponseHTML5));

    switch (blynkAuthReply)
    {
    case SETUP_INCORRECT_LENGTH:
        webServer->sendContent(FPSTR(setupReplyIncorrectLength));
        break;
    case SETUP_CLEARED:
        webServer->sendContent(FPSTR(setupReplyCleared));
        resetBlynk = true;
        break;
    case SETUP_SAVED:
        webServer->sendContent(FPSTR(setupReplySaved));
        resetBlynk = true;
        break;
    case SETUP_NO_CHANGE:
        webServer->sendContent(FPSTR(setupReplyNoChange));
        break;
    default:
        break;
    }

    webServer->sendContent(FPSTR(captivePortalResponseHTML6));
    if (blynk_auth[0] != '\0')
        webServer->sendContent(blynk_auth);
    webServer->sendContent(FPSTR(captivePortalResponseHTML7));
    webServer->sendContent(""); // an empty line finishes the transmission

    // update the config file
    saveConfig();
}