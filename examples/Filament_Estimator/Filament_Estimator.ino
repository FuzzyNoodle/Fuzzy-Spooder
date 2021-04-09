/*


*/
#include "Weight_Estimator.h"
#include <ESP8266WiFi.h>

// Replace with your network credentials:
const char *ssid = "gnet02-guest";
const char *password = "password";
//const char *ssid = "gnetmobile";
//const char *password = "georgy256";

// This hostname is used as mDNS name. ex: device can be located as "SPDR01.local"
const char *hostname = "SPDR01";

// A unique Auth Token should be obtained from the Blynk App and be replaced here:
const char *blynk_auth_token = "fW44-mHrLC6cBy8w7u0sDTTRjsr2FBLb";

// Create the fuzzy spooder object
WEIGHT_ESTIMATOR spooder;

void setup()
{
  Serial.begin(115200);
  spooder.begin(ssid, password, hostname, blynk_auth_token);
}

void loop()
{
  spooder.update();
}
