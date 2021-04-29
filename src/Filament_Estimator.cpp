#include "Filament_Estimator.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>

extern BlynkWifi Blynk;

FILAMENT_ESTIMATOR *estimatorPointer;          //Declare a pointer to WEIGHT_ESTIMATOR
static void outsideButtonHandler(Button2 &btn) // define global handler
{
    estimatorPointer->buttonHandler(btn); // calls class member handler
}
static void outsideRotaryHandler(ESPRotary &rty) // define global handler
{
    estimatorPointer->rotaryHandler(rty); // calls class member handler
}

FILAMENT_ESTIMATOR::FILAMENT_ESTIMATOR() : server{80},
                                           button{BUTTON_PIN},
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
    Serial.println("");
    //Firmware version setup
    currentVersion.major = CURRENT_VERSION_MAJOR;
    currentVersion.minor = CURRENT_VERSION_MINOR;
    currentVersion.patch = CURRENT_VERSION_PATCH;

    Serial.println("Program started.");

    //Setup for OLED
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    display.setLogBuffer(5, 30);

    Serial.println("OLED setup ok.");

    //Mount file system
    if (!LittleFS.begin())
    {
        Serial.println(F("Failed to mount file system"));
    }
    else
    {
        Serial.println(F("LittleFS file system mounted."));
        listDir("");
    }

    displayMonoBitmap("/images/logo.bmp");

#ifdef ENABLE_WIFI
    WiFi.hostname(hostname); //hostname is set here

//Setup for Blynk
#ifdef ENABLE_BLYNK
    Blynk.begin(blynk_auth_token, ssid, password);
    Serial.println("Blynk setup ok.");
#else
    WiFi.begin(ssid, password);
#endif //ENABLE_BLYNK

    //WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    //Serial.println("WiFi connected.");
    Serial.println("IP Address = ");
    //char ipChar[30] = WiFi.localIP();
    Serial.println(WiFi.localIP().toString());

    //Start mDNS after WiFi connected
    MDNS.begin(hostname);

    server.begin();
    Serial.println("Server ok.");

    //Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS service ok.");

    //Setup for OTA
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
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
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("ArduinoOTA setup ok.");

#endif //ENABLE_WIFI

    //Setup for rotary switch
    estimatorPointer = this;
    button.setClickHandler(outsideButtonHandler);
    button.setLongClickHandler(outsideButtonHandler);
    button.setDoubleClickHandler(outsideButtonHandler);
    button.setTripleClickHandler(outsideButtonHandler);
    rotary.setLeftRotationHandler(outsideRotaryHandler);
    rotary.setRightRotationHandler(outsideRotaryHandler);
    Serial.println(F("Rotary Switch setup ok."));

    //Setup for settings and eeprom
    EEPROM.begin(DECLARED_EEPROM_SIZE);
    //setting.calValue = 3.14159;
    Serial.print(F("Size of setting struct: "));
    Serial.println((uint32_t)sizeof(setting));

    //Load eeprom data to setting
    loadToSetting();
    //validate data and reset to defaults
    bool isDirty = false;
    if (setting.version.major > 99)
    {
        setting.version.major = 0;
        isDirty = true;
    }

    if (setting.version.minor > 99)
    {
        setting.version.minor = 0;
        isDirty = true;
    }
    if (setting.version.patch > 99)
    {
        setting.version.patch = 0;
        isDirty = true;
    }
    if (isnan(setting.calValue))
    {
        setting.calValue = DEFAULT_CALIBRATION_VALUE;
        isDirty = true;
    }
    if (isDirty == true)
    {
        Serial.println(F("Saving default values to EEPROM."));
        saveToEEPROM();
    }
    //Compare firmware version

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
    }
    else
    {
        //Same version
    }

    dumpSetting();
    //Setup for version and setting
    //setting.version.major = curretVersionMajor;
    //setting.version.minor = curretVersionMinor;
    //setting.version.patch = curretVersionPatch;

    // EEPROM.get(EEPROM_CALIBRATE_VALUE, calibrateValue);

    // if (isnan(calibrateValue) == true)
    // {
    //     //not a number
    //     Serial.println("EEPROM Calibrate value is nan");
    //     Serial.println("Reset to 1.0 and save to EEROPM.");
    //     calibrateValue = 1.0;
    //     EEPROM.put(EEPROM_CALIBRATE_VALUE, calibrateValue);
    //     EEPROM.commit();
    //     Serial.println("Perform a calibration after start up.");
    // }

    // Serial.print("EEPROM Calibrate value = ");
    // Serial.println(calibrateValue);

    loadcell.begin();
    loadcell.start(stabilizingTime, false);
    loadcell.setCalFactor(calibrateValue);
    tare();
    Serial.println("Setup completed.");

    setPage(PAGE_HOME);
}
void FILAMENT_ESTIMATOR::update(void)
{
#ifdef ENABLE_WIFI
    ArduinoOTA.handle();
    server.handleClient();
    MDNS.update();
#endif
    button.loop();
    rotary.loop();
#ifdef ENABLE_BLYNK
    Blynk.run();
#endif //ENABLE_BLYNK
    //keyStroke();
    if (loadcell.update() == true)
    {
        newDataReady = true;
    }
    if (newDataReady == true)
    {
        totalWeight = loadcell.getData();
        filamentWeight = totalWeight - spoolHolderWeight;
    }
    updateHomepage();
    if (drawTareMessage == true)
    {
        //Refresh the screen after tare message period
        checkTareTimer();
    }
    if (calibrateEditDigitMode == true)
    {
        checkCalibrateEditModeTimer();
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
                //Reset the selection to ok, for next entry
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
                //Reset the selection to the fourth digit, for next entry
                calibrateSelection = CALIBRATE_4_DIGIT;
                calibrate();
                setPage(PAGE_CALIBRATE_CONFIRM);
                break;
            case CALIBRATE_CANCEL:
                calibrateEditDigitMode = false;
                //Reset the selection to the fourth digit, for next entry
                calibrateSelection = CALIBRATE_4_DIGIT;
                setPage(PAGE_MENU);
                break;
            }
            break;
        case PAGE_CALIBRATE_CONFIRM:

            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_OK:
                calibrateValue = newCalibrationValue;
                EEPROM.put(EEPROM_CALIBRATE_VALUE, calibrateValue);
                EEPROM.commit();
                break;
            case CALIBRATE_SAVE_CANCEL:
                //Reset the selection to ok
                calibrateSaveSelection = CALIBRATE_SAVE_OK;
                break;
            }
            setPage(PAGE_MENU);
            break;
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
            case DEBUG_RETURN:
                debugMenuSelection = DEBUG_LOAD_TO_SETTING;
                setPage(PAGE_MENU);
                break;
            }
            break;
        }
        break; //switch (currentPage)
    case DOUBLE_CLICK:

        break;
    case TRIPLE_CLICK:

        break;
    case LONG_CLICK:
        if (currentPage == PAGE_HOME)
        {
            tare();
        }
        break;
    }

    //Serial.print(btn.getNumberOfClicks());
    //Blynk.notify("Hello from ESP8266! Button Pressed!");
}
void FILAMENT_ESTIMATOR::rotaryHandler(ESPRotary &rty)
{
    uint8_t direction = rotary.getDirection();
    //Serial.println(direction);
    switch (direction)
    {
    case RE_LEFT:
        //rotateCCW();
        switch (currentPage)
        {
        case PAGE_MENU:
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
        case PAGE_INFO:
            setPage(PAGE_HOME);
            break;
        case PAGE_CALIBRATE:
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
        case PAGE_TARE:
        {
            if (tareSelection == TARE_CANCEL)
            {
                tareSelection = TARE_OK;
                setPage(PAGE_TARE);
            }
        }
        break;
        case PAGE_CALIBRATE_CONFIRM:
            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_CANCEL:
                calibrateSaveSelection = CALIBRATE_SAVE_OK;
                displayPage(PAGE_CALIBRATE_CONFIRM);
                break;
            }
            break;
        case PAGE_DEBUG:
            if (debugMenuSelection > DEBUG_LOAD_TO_SETTING)
            {
                debugMenuSelection--;
                if (debugMenuSelection < debugMenuItemStartIndex)
                {
                    debugMenuItemStartIndex--;
                }
                setPage(PAGE_DEBUG);
            }
        }
        break;
    case RE_RIGHT:
        //rotateCW();
        switch (currentPage)
        {
        case PAGE_HOME:
            setPage(PAGE_INFO);
            break;
        case PAGE_INFO:
            setPage(PAGE_MENU);
            break;
        case PAGE_MENU:
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
        case PAGE_TARE:
            if (tareSelection == TARE_OK)
            {
                tareSelection = TARE_CANCEL;
                setPage(PAGE_TARE);
            }
            break;
        case PAGE_CALIBRATE:
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
        case PAGE_CALIBRATE_CONFIRM:
            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_OK:
                calibrateSaveSelection = CALIBRATE_SAVE_CANCEL;
                displayPage(PAGE_CALIBRATE_CONFIRM);
                break;
            }
            break;
        case PAGE_DEBUG:
            if (debugMenuSelection < (numberOfDebugMenuItems - 1))
            {
                debugMenuSelection++;
                if (debugMenuSelection >= debugMenuItemStartIndex + debugMenuItemPerPage)
                {
                    debugMenuItemStartIndex++;
                }
                setPage(PAGE_DEBUG);
            }
        }

        break;
    }
    //Serial.print(rotary.directionToString(rotary.getDirection()));
    //Serial.print("  ");
    //Serial.println(rotary.getPosition());
}
bool FILAMENT_ESTIMATOR::setPage(uint8_t page)
{
    Serial.print(F("Set page:"));
    Serial.println(page);
    previousPage = currentPage;
    currentPage = page;
    displayPage(currentPage);

    return true;
}
void FILAMENT_ESTIMATOR::displayPage(uint8_t page)
{

    if (page != PAGE_HOME)
    {
        Serial.print(F("Display page: "));
        Serial.println(page);
    }

    switch (page)
    {
    case PAGE_HOME:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "SPOODER HOME");
        display.drawRect(0, 12, 128, 1);

        drawRightIndicator(0);
        //Draw display type symbol and weight
        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        switch (displayType)
        {
        case DISPLAY_TYPE_TOTAL:
            display.setFont(ArialMT_Plain_24);
            display.setColor(WHITE);
            display.fillRect(6, 22, 4, 26);
            display.fillRect(27, 22, 4, 26);
            //display.fillRect(11, 28, 15, 13);
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
                display.drawString(90, 20, String(totalWeight, 0));
                //Draw unit
                display.setFont(ArialMT_Plain_10);
                display.drawString(100, 32, "g");
            }

            break;
        case DISPLAY_TYPE_FILAMENT:
            display.setFont(ArialMT_Plain_24);

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
            if (filamentWeight > emptyThreshold)
            {
                if (abs(filamentWeight) < 9999)
                {
                    display.drawString(90, 20, String(filamentWeight, 0));
                    //Draw unit
                    display.setFont(ArialMT_Plain_10);
                    display.drawString(100, 32, "g");
                }
            }
            else //shows empty on the homescreen
            {
                display.drawString(104, 20, "Empty");
            }

            break;
        case DISPLAY_TYPE_SPOOL_HOLDER:
            display.setFont(ArialMT_Plain_24);

            display.setColor(WHITE);
            display.fillRect(6, 22, 4, 26);
            display.fillRect(27, 22, 4, 26);
            display.fillRect(11, 28, 15, 13);
            if (abs(spoolHolderWeight) < 9999)
            {
                display.drawString(90, 20, String(spoolHolderWeight, 0));
                //Draw unit
                display.setFont(ArialMT_Plain_10);
                display.drawString(100, 32, "g");
            }

            break;
        }

        drawOverlay();

        display.display();
        break;
    case PAGE_INFO:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "INFO");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "CAL Value: " + String(calibrateValue));
        display.drawString(6, 22, "Spool: " + String(uint16_t(spoolHolderWeight)));

        drawRightIndicator(1);
        display.display();
        break;
    case PAGE_MENU:
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
        drawOverlay();
        display.display();
        break;
    case PAGE_TARE:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "TARE");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Remove spool and press ");
        display.drawString(6, 22, "button to tare:");
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

        display.display();
        break;
    case PAGE_CALIBRATE:
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
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(72, 36, String(calibrate1Digit));
            }
            drawTriangle(66, 45);
            break;
        case CALIBRATE_OK:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(90, 42);
            break;
        case CALIBRATE_CANCEL:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(90, 52);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        display.display();
        break;
    case PAGE_CALIBRATE_CONFIRM:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Calibrate - Confirm");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Old value: " + String(calibrateValue));
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

        display.display();
        break;
    case PAGE_DEBUG:
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

        display.display();
        break;
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
void FILAMENT_ESTIMATOR::tare()
{
    loadcell.tareNoDelay();
    drawTareMessage = true;
    drawTareMessageTimer = millis();
    Serial.println("Tare");
}
void FILAMENT_ESTIMATOR::calibrate()
{
    //Draw the overlay
    display.setColor(BLACK);
    display.fillRect(16 - 8, 12 - 4, 96 + 18, 40 + 8);
    display.setColor(WHITE);
    display.drawRect(16, 12, 96, 40);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(64, 32, "Calibrating...");
    display.display();
    //refresh the dataset to be sure that the known mass is measured correct
    loadcell.refreshDataSet();
    calibrationWeight = getCalibrationWeight();
    //get the new calibration value
    newCalibrationValue = loadcell.getNewCalibration(calibrationWeight);
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
void FILAMENT_ESTIMATOR::drawOverlay()
{
    if (drawTareMessage == true)
    {
        if (millis() - drawTareMessageTimer < DRAW_TARE_MESSAGE_PERIOD)
        {
            display.setColor(BLACK);
            display.fillRect(16 - 8, 12 - 4, 96 + 18, 40 + 8);
            display.setColor(WHITE);
            display.drawRect(16, 12, 96, 40);
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
            display.drawString(64, 32, "TARE");
        }
        else
        {
            drawTareMessage = false;
            displayPage(currentPage);
        }
    }
}
void FILAMENT_ESTIMATOR::checkTareTimer()
{
    if (drawTareMessage == true)
    {
        if (millis() - drawTareMessageTimer > DRAW_TARE_MESSAGE_PERIOD)
        {
            drawTareMessage = false;
            displayPage(currentPage);
        }
    }
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
void FILAMENT_ESTIMATOR::setCalibrationWeight(uint16_t weight)
{
    calibrate4Digit = (weight / 1000U) % 10;
    calibrate3Digit = (weight / 100U) % 10;
    calibrate2Digit = (weight / 10U) % 10;
    calibrate1Digit = (weight / 1U) % 10;
}
uint16_t FILAMENT_ESTIMATOR::getCalibrationWeight()
{
    calibrationWeight = calibrate4Digit * 1000 + calibrate3Digit * 100 + calibrate2Digit * 10 + calibrate1Digit;
    return calibrationWeight;
}
void FILAMENT_ESTIMATOR::setCurrentSpoolHolderWeight(uint16_t weight)
{
    spoolHolderWeight = weight;
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
    //Starting from v0.3.0
    Serial.print(F("  - Firmware version: "));
    Serial.print(setting.version.major);
    Serial.print(F("."));
    Serial.print(setting.version.minor);
    Serial.print(F("."));
    Serial.println(setting.version.patch);
    Serial.print(F("  - calValue: "));
    Serial.println(setting.calValue);
    Serial.println(F("Setting dump completed."));
}
void FILAMENT_ESTIMATOR::dumpEEPROM()
{
    Serial.println(F("Dump EEPROM data:"));
    for (uint16_t addr = 0; addr < DECLARED_EEPROM_SIZE; addr++)
    {
        if (addr % 64 == 0)
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
    Serial.println(F("Erase EEPROM data:"));
    for (uint16_t addr = 0; addr < DECLARED_EEPROM_SIZE; addr++)
    {
        EEPROM.write(addr, 0xff);
    }

    EEPROM.commit();
}
uint32_t FILAMENT_ESTIMATOR::versionToNumber(VERSION_STRUCT v)
{
    //a simple conversion
    //v1.2.3 will convert to something like 10203
    //So that the max number for each item that works will be 99
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
            else //i=0
            {
                //Serial.print("--");
            }
            //Serial.print("--");
        }
        Serial.print("--");
        if (dir.isFile())
        {

            Serial.print(dir.fileName());
            Serial.print(F("  "));
            Serial.println(dir.fileSize());
        }
        else //isDirectory()
        {
            Serial.print("<");
            Serial.print(dir.fileName());
            Serial.println(">");
            _listDir((dirname + String("/") + dir.fileName()).c_str(), level);
        }
    }

    level++;
}
void FILAMENT_ESTIMATOR::dumpConfig()
{
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
        //if the color 0 is (0,0,0) black,
        //bits of 0 is black
        normalDir = true;
    }
    else
    {
        normalDir = false;
    }
    //dump the file for debugging
    //f.seek(0);
    // while (f.available())
    // {
    //     char c = f.read();
    //     if (c < 10)
    //         Serial.print("0");
    //     Serial.print(c, HEX);
    //     Serial.print("  ");
    // }
    uint8_t buffer[1024];
    f.seek(bmp.offset);
    for (uint16_t i = 0; i < 1024; i++)
    {
        if (normalDir == true)
        {
            //bits of 0 means black in the bmp
            buffer[1023 - i] = f.read();
        }
        else
        {
            //bits of 0 means white in the bmp, reading is inverted
            buffer[1023 - i] = ~f.read();
        }
    }
    display.drawXbm(0, 0, 128, 64, buffer);
    display.display();
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