/*


*/
#include "Weight_Estimator.h"

// Replace with your network credentials
const char *ssid = "gnet02";
const char *password = "K433bZz7";
const char *hostname = "SPL001";

WEIGHT_ESTIMATOR estimator;

void setup()
{
  Serial.begin(115200);
  estimator.begin(ssid, password, hostname);
}

void loop()
{
  estimator.update();
}
