/*An Simple UI demo for the 
Fuzzy Spooder - Filament Estimator stage 1 project

Functions implemented:
- Onboard UI demonstration
- Basic weighting function
- Calibration
- Gauge calibration value saving and loading to/from EEPROM
- Gauge Tare

*/
#include "Weight_Estimator.h"

// Create the fuzzy spooder object
WEIGHT_ESTIMATOR spooder;

void setup()
{
  //Enable the Serial debug outout, not required
  Serial.begin(115200);

  //Initialize the spooder
  spooder.begin();
}

void loop()
{
  //loop process, needs to be called in the loop
  spooder.update();
}
