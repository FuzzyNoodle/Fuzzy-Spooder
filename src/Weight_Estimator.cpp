#include "Weight_Estimator.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>

extern BlynkWifi Blynk;

WEIGHT_ESTIMATOR *estimatorPointer;            //Declare a pointer to WEIGHT_ESTIMATOR
static void outsideButtonHandler(Button2 &btn) // define global handler
{
    estimatorPointer->buttonHandler(btn); // calls class member handler
}
static void outsideRotaryHandler(ESPRotary &rty) // define global handler
{
    estimatorPointer->rotaryHandler(rty); // calls class member handler
}

WEIGHT_ESTIMATOR::WEIGHT_ESTIMATOR() : server{80},
                                       button{BUTTON_PIN},
                                       rotary{ROTARY_PIN_DT, ROTARY_PIN_CLK, CLICKS_PER_STEP},
                                       display{SSD1306_ADDRESS, SSD1306_SDA_PIN, SSD1306_SCL_PIN},
                                       loadcell{HX711_DOUT_PIN, HX711_SCK_PIN}

{
}
void WEIGHT_ESTIMATOR::begin(void)
{
    begin("", "", "", "");
}
void WEIGHT_ESTIMATOR::begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token)
{
    Serial.println("");

    //Setup for OLED
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    display.setLogBuffer(5, 30);

    Serial.println("Program started.");

    Serial.println("OLED setup ok.");
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

    //Setup for HX711 Strain Gauge
    EEPROM.begin(512);
    EEPROM.get(EEPROM_CALIBRATE_VALUE, calibrateValue);

    Serial.print("EEPROM Calibrate value = ");
    Serial.println(calibrateValue);
    loadcell.begin();
    // preciscion right after power-up can be improved by adding a few seconds of stabilizing time

    loadcell.start(stabilizingTime, false);
    loadcell.setCalFactor(calibrateValue);
    tare();
    Serial.println("Setup completed.");

    setPage(PAGE_HOME);
}
void WEIGHT_ESTIMATOR::update(void)
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
void WEIGHT_ESTIMATOR::buttonHandler(Button2 &btn)
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
            case MENU_DISPLAY_EEPROM:
                uint8_t value;
                for (uint16_t addr = 0; addr < 64; addr++)
                {
                    EEPROM.get(addr, value);
                    Serial.print(value);
                    Serial.print(" ");
                }
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
        {
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
        }
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
void WEIGHT_ESTIMATOR::rotaryHandler(ESPRotary &rty)
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
        }

        break;
    }
    //Serial.print(rotary.directionToString(rotary.getDirection()));
    //Serial.print("  ");
    //Serial.println(rotary.getPosition());
}
bool WEIGHT_ESTIMATOR::setPage(uint8_t page)
{

    previousPage = currentPage;
    currentPage = page;
    displayPage(currentPage);

    return true;
}
void WEIGHT_ESTIMATOR::displayPage(uint8_t page)
{
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
            if (abs(filamentWeight) < 9999)
            {
                display.drawString(90, 20, String(filamentWeight, 0));
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
            }

            break;
        }

        //Draw unit
        display.setFont(ArialMT_Plain_10);
        display.drawString(100, 32, "g");

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
    default:
        break;
    }
}
void WEIGHT_ESTIMATOR::drawBottomIndicator(uint8_t index)
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
void WEIGHT_ESTIMATOR::drawRightIndicator(uint8_t index)
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
void WEIGHT_ESTIMATOR::drawLeftIndicator(uint8_t index)
{
    uint8_t x = 0;
    uint8_t y = 18 + index * 10;
    drawTriangle(x, y);
}
void WEIGHT_ESTIMATOR::drawTriangle(uint8_t x, uint8_t y)
{
    display.drawLine(x, y - 3, x, y + 3);
    display.drawLine(x + 1, y - 3, x + 1, y + 3);
    display.drawLine(x + 2, y - 2, x + 2, y + 2);
    display.drawLine(x + 3, y - 1, x + 3, y + 1);
    display.drawLine(x + 4, y, x + 4, y);
}
void WEIGHT_ESTIMATOR::tare()
{
    loadcell.tareNoDelay();
    drawTareMessage = true;
    drawTareMessageTimer = millis();
    Serial.println("Tare");
}
void WEIGHT_ESTIMATOR::calibrate()
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
    uint16_t knownWeight = calibrate4Digit * 1000 + calibrate3Digit * 100 + calibrate2Digit * 10 + calibrate1Digit;
    //get the new calibration value
    newCalibrationValue = loadcell.getNewCalibration(knownWeight);
    Serial.print("New calibration value = ");
    Serial.println(newCalibrationValue);
    Serial.println("Calibrate completed.");
}
void WEIGHT_ESTIMATOR::updateHomepage()
{
    if (millis() - updateHomepageTimer < UPDATE_HOMEPAGE_PERIOD)
        return;
    updateHomepageTimer = millis();
    if (currentPage == PAGE_HOME)
    {
        displayPage(PAGE_HOME);
    }
}
void WEIGHT_ESTIMATOR::drawOverlay()
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
void WEIGHT_ESTIMATOR::checkTareTimer()
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
void WEIGHT_ESTIMATOR::checkCalibrateEditModeTimer()
{
    if (millis() - calibrateEditModerTimer > CALIBRATE_EDIT_MODE_PERIOD)
    {
        displayCalibrateDigit = !displayCalibrateDigit;
        calibrateEditModerTimer = millis();
        displayPage(PAGE_CALIBRATE);
    }
}
