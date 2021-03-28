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
                                       rotary{ROTARY_PIN_DT, ROTARY_PIN_CLK, CLICKS_PER_STEP}
{
}

void WEIGHT_ESTIMATOR::begin(void)
{
}

void WEIGHT_ESTIMATOR::begin(const char *ssid, const char *password, const char *hostname)
{
    Serial.println("");
    Serial.println("WEIGHT_ESTIMATOR started.");
    WiFi.hostname(hostname); //hostname is set here

    //Setup for Blynk
    Blynk.begin("fW44-mHrLC6cBy8w7u0sDTTRjsr2FBLb", ssid, password);
    Serial.println("Blynk setup completed.");

    //WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP Address = ");
    Serial.println(WiFi.localIP());

    //Start mDNS after WiFi connected
    MDNS.begin(hostname);

    server.begin();
    Serial.println("Server started.");

    //Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS service started.");

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
    Serial.println("ArduinoOTA setup completed.");

    //Setup for rotary switch
    estimatorPointer = this;
    button.setClickHandler(outsideButtonHandler);
    button.setLongClickHandler(outsideButtonHandler);
    button.setDoubleClickHandler(outsideButtonHandler);
    button.setTripleClickHandler(outsideButtonHandler);
    rotary.setLeftRotationHandler(outsideRotaryHandler);
    rotary.setRightRotationHandler(outsideRotaryHandler);
    Serial.println("Rotary Switch setup completed.");

    Serial.println("WEIGHT_ESTIMATOR setup completed.");
}

void WEIGHT_ESTIMATOR::update(void)
{
    ArduinoOTA.handle();
    server.handleClient();
    MDNS.update();
    button.loop();
    rotary.loop();
    Blynk.run();
}

void WEIGHT_ESTIMATOR::buttonHandler(Button2 &btn)
{
    switch (btn.getClickType())
    {
    case SINGLE_CLICK:
        break;
    case DOUBLE_CLICK:
        Serial.print("double ");
        break;
    case TRIPLE_CLICK:
        Serial.print("triple ");
        break;
    case LONG_CLICK:
        Serial.print("long");
        break;
    }
    Serial.print("click");
    Serial.print(" (");
    Serial.print(btn.getNumberOfClicks());
    Serial.println(")");
    //Blynk.notify("Hello from ESP8266! Button Pressed!");
}
void WEIGHT_ESTIMATOR::rotaryHandler(ESPRotary &rty)
{
    Serial.print(rotary.directionToString(rotary.getDirection()));
    Serial.print("  ");
    Serial.println(rotary.getPosition());
}