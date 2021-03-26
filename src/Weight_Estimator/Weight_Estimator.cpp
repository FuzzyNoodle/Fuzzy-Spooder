#include "Weight_Estimator.h"

WEIGHT_ESTIMATOR::WEIGHT_ESTIMATOR()
{
}

void WEIGHT_ESTIMATOR::begin(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void WEIGHT_ESTIMATOR::update(void)
{
    if (millis() - blink_timer > BLINK_PEROID)
    {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        blink_timer = millis();
    }
}