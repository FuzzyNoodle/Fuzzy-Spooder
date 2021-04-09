/*!
 *  @file Weight_Estimator.h
 *
 *  
 *
 *  
 */

#ifndef WEIGHT_ESTIMATOR_H
#define WEIGHT_ESTIMATOR_H

//#define ENABLE_BLYNK
#define DISABLE_WIFI

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include "ESPRotary.h"
#include "Button2.h"

#define ROTARY_PIN_CLK D3
#define ROTARY_PIN_DT D4
#define CLICKS_PER_STEP 4
#define BUTTON_PIN D0

//Include the SSD1306 display library for esp8266
#include "SSD1306Wire.h"
#define SSD1306_SDA_PIN D2
#define SSD1306_SCL_PIN D1
#define SSD1306_ADDRESS 0x3c
//Include the UI library
#include "OLEDDisplayUi.h"

#ifndef DISABLE_SERIAL_DEBUG
#define ENABLE_SERIAL_DEBUG
#endif

#ifndef DISABLE_WIFI
#define ENABLE_WIFI
#endif

#define PAGE_NONE 0
#define HOME_PAGE_1 11
#define HOME_PAGE_2 12
#define MAIN_MENU_PAGE 15
#define TARE_PAGE 20
#define CALIBRATE_PAGE 30
#define ADJUST_WEIGHT_PAGE 40

#define MENU_TARE 0
#define MENU_CALIBRATE 1
#define MENU_SET_WEIGHT 2
#define MENU_SETUP 3

class WEIGHT_ESTIMATOR
{
public:
  WEIGHT_ESTIMATOR(void);
  void begin(void);
  void begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token);
  void update(void);
  void buttonHandler(Button2 &btn);
  void rotaryHandler(ESPRotary &rty);

private:
  ESP8266WebServer server;
  Button2 button;
  ESPRotary rotary;
  SSD1306Wire display;

  void outputLog(String msg);
  uint8_t currentPage = PAGE_NONE;
  uint8_t previousPage = PAGE_NONE;
  bool setPage(uint8_t page); //return true if page changed, false in page not changed
  void displayPage(uint8_t page);
  void drawBottomIndicator(uint8_t index);
  void drawRightIndicator(uint8_t index);
  uint8_t menuIndex = MENU_TARE;
  void drawLeftIndicator(uint8_t index);
};

#endif //#ifndef WEIGHT_ESTIMATOR_H