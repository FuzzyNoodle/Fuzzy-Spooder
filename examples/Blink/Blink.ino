
/*
Blink Example to test platformio, github and library dependency

 PlatformIO 
Copy and paste this section of configuration code into platform.ini
[envnodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200
monitor_filters = send_on_enter
*/

#include "Weight_Estimator.h"

WEIGHT_ESTIMATOR b;

void setup()
{
  b.begin();
}

void loop()
{
  b.update();
}