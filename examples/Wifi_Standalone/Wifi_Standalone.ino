/*An Simple UI demo for the 
Fuzzy Spooder - Filament Estimator stage 1 project

Functions implemented:
- Onboard UI demonstration
- Basic weighting function
- Calibration
- Gauge calibration value saving and loading to/from EEPROM
- Gauge Tare
- File system
- Spool holder weight adjustment

*/
#include "Filament_Estimator.h"
// Create the fuzzy spooder object
FILAMENT_ESTIMATOR spooder;

//BearSSL::CertStore certStore;
//uint16_t numCerts = 0; //number or certs read from file system
//extern void outsideCheckGithubTag(bool connectWiFi);

void setup()
{

  //Serial.println("Check Gitgub before spooder begin:");
  //outsideCheckGithubTag(true);
  //Enable the Serial for debug outout, not required.
  Serial.begin(115200);

  Serial.println("Check Gitgub before spooder begin:");
  //outsideCheckGithubTag(true);

  //Initialize the spooder, required.
  spooder.begin();

  //Serial.println("Check Gitgub after begin:");
  //outsideCheckGithubTag();

  //Optional: set the default total weight(spool holder + filament) in grams for calibration after power on
  //Valid values from 0 to 9999
  spooder.setCalibrationWeight(1180);

  //Optional: set the steps per click of the rotary encoder
  spooder.setStepsPerClick(4);

  //Optional: set the time for long click (in ms)
  spooder.setLongClickTime(400);

  //Optional: set the current spool holder weight in grams
  //Valid values from 0 to 999
  //This function only works when there is no last used values in the EEPROM
  spooder.setCurrentSpoolHolderWeight(180);

  //Optional: enable wifi function
  //spooder.setWifi(true);
}

void loop()
{
  //loop process, needs to be called in the loop
  spooder.update();
}
