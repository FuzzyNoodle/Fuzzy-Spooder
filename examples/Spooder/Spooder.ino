/*

A simple example for Fuzzy Spooder
 
*/

#include "Filament_Estimator.h"

// Create the fuzzy spooder object
FILAMENT_ESTIMATOR spooder;

void setup()
{
  //Optional: Enable the Serial for debug outout.
  Serial.begin(115200);
  //Serial.println("Spooder started.");
  
  //Required: Initialize the spooder.
  spooder.begin();
}

void loop()
{
  //Required: loop process, needs to be called in the loop
  spooder.update();
}
