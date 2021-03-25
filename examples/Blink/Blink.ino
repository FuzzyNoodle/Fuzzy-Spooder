/*
  Blink Example to test platformio, github and library dependency
*/
#include <Arduino.h>
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
