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
                                       display{SSD1306_ADDRESS, SSD1306_SDA_PIN, SSD1306_SCL_PIN}

{
}

void WEIGHT_ESTIMATOR::begin(void)
{
}

void WEIGHT_ESTIMATOR::begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token)
{
    Serial.println("");

    //Setup for OLED
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    display.setLogBuffer(5, 30);

    outputLog("Program started.");

    //outputLog("OLED setup ok.");
#ifdef ENABLE_WIFI
    WiFi.hostname(hostname); //hostname is set here

//Setup for Blynk
#ifdef ENABLE_BLYNK
    Blynk.begin(blynk_auth_token, ssid, password);
    outputLog("Blynk setup ok.");
#else
    WiFi.begin(ssid, password);
#endif //ENABLE_BLYNK

    //WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    //outputLog("WiFi connected.");
    outputLog("IP Address = ");
    //char ipChar[30] = WiFi.localIP();
    outputLog(WiFi.localIP().toString());

    //Start mDNS after WiFi connected
    MDNS.begin(hostname);

    server.begin();
    //outputLog("Server ok.");

    //Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    //outputLog("mDNS service ok.");

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
    //outputLog("ArduinoOTA setup ok.");

#endif //ENABLE_WIFI

    //Setup for rotary switch
    estimatorPointer = this;
    button.setClickHandler(outsideButtonHandler);
    button.setLongClickHandler(outsideButtonHandler);
    button.setDoubleClickHandler(outsideButtonHandler);
    button.setTripleClickHandler(outsideButtonHandler);
    rotary.setLeftRotationHandler(outsideRotaryHandler);
    rotary.setRightRotationHandler(outsideRotaryHandler);
    //outputLog("Rotary Switch setup ok.");

    outputLog("Setup completed.");

    setPage(HOME_PAGE_1);
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
}

void WEIGHT_ESTIMATOR::buttonHandler(Button2 &btn)
{
    switch (btn.getClickType())
    {
    case SINGLE_CLICK:

        break;
    case DOUBLE_CLICK:

        break;
    case TRIPLE_CLICK:

        break;
    case LONG_CLICK:

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
        if (currentPage == MAIN_MENU_PAGE)
        {
            if (menuIndex > MENU_TARE)
            {
                menuIndex--;
                setPage(MAIN_MENU_PAGE);
            }
            else
            {
                setPage(HOME_PAGE_2);
            }
        }
        else if (currentPage == HOME_PAGE_2)
            setPage(HOME_PAGE_1);
        break;
    case RE_RIGHT:
        //rotateCW();
        if (currentPage == HOME_PAGE_1)
            setPage(HOME_PAGE_2);
        else if (currentPage == HOME_PAGE_2)
            setPage(MAIN_MENU_PAGE);
        else if (currentPage == MAIN_MENU_PAGE)
        {
            if (menuIndex < MENU_SETUP)
            {
                menuIndex++;
                setPage(MAIN_MENU_PAGE);
            }
        }
        break;
    }
    //Serial.print(rotary.directionToString(rotary.getDirection()));
    //Serial.print("  ");
    //Serial.println(rotary.getPosition());
}
void WEIGHT_ESTIMATOR::outputLog(String msg)
{
    Serial.println(msg);
    display.clear();
    display.println(msg);
    display.drawLogBuffer(0, 0);
    display.display();
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
    case HOME_PAGE_1:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "SPOODER HOME");

        display.drawRect(0, 12, 128, 1);
        drawRightIndicator(0);
        display.display();
        break;
    case HOME_PAGE_2:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "STATUS AND INFO");

        display.drawRect(0, 12, 128, 1);
        drawRightIndicator(1);
        display.display();
        break;
    case MAIN_MENU_PAGE:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "MENU");
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Tare");
        display.drawString(6, 22, "Calibrate");
        display.drawString(6, 32, "Set Weight");
        display.drawString(6, 42, "Setup");
        display.drawString(6, 52, "Test line");
        display.drawRect(0, 12, 128, 1);
        drawRightIndicator(2);

        drawLeftIndicator(menuIndex);

        display.display();
        break;
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
    const uint8_t y_step = 12;
    const uint8_t y = 24;
    uint8_t index_y = y + y_step * index;
    display.drawCircle(x, y, 2);
    display.drawCircle(x, y + y_step, 2);
    display.drawCircle(x, y + y_step * 2, 2);

    display.drawCircle(x, index_y, 1);

    display.drawCircle(x, index_y, 3);
    display.drawCircle(x, index_y, 4);
}
void WEIGHT_ESTIMATOR::drawLeftIndicator(uint8_t index)
{
    uint8_t x = 1;
    uint8_t y = 19 + index * 10;
    display.drawLine(x, y - 3, x, y + 3);
    display.drawLine(x + 1, y - 2, x + 1, y + 2);
    display.drawLine(x + 2, y - 1, x + 2, y + 1);
    display.drawLine(x + 3, y, x + 3, y);
}