/*!
 *  @file Weight_Estimator.h
 *
 *  
 *
 *  
 */

#ifndef WEIGHT_ESTIMATOR_H
#define WEIGHT_ESTIMATOR_H

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include "ESPRotary.h"
#include "Button2.h"

#define ROTARY_PIN_CLK D3
#define ROTARY_PIN_DT D4
#define CLICKS_PER_STEP 4
#define BUTTON_PIN D0

class WEIGHT_ESTIMATOR
{
public:
  WEIGHT_ESTIMATOR(void);
  void begin(void);
  void begin(const char *ssid, const char *password, const char *hostname);
  void update(void);
  void buttonHandler(Button2 &btn);
  void rotaryHandler(ESPRotary &rty);

private:
  ESP8266WebServer server;
  Button2 button;
  ESPRotary rotary;
};

#endif //#ifndef WEIGHT_ESTIMATOR_H