/*
Rotary Example Testing library dependency

*/

#include "ESPRotary.h"
#include "Button2.h"

#define ROTARY_PIN_CLK D3
#define ROTARY_PIN_DT D4
#define CLICKS_PER_STEP 4
#define BUTTON_PIN D0

ESPRotary rotary = ESPRotary(ROTARY_PIN_DT, ROTARY_PIN_CLK, CLICKS_PER_STEP);
Button2 button = Button2(BUTTON_PIN);

// on change
void rotate(ESPRotary &rotary)
{
  Serial.println(rotary.getPosition());
}

// on left or right rotattion
void showDirection(ESPRotary &rotary)
{
  Serial.println(rotary.directionToString(rotary.getDirection()));
}

void handler(Button2 &btn)
{
  switch (btn.getClickType())
  {
  case SINGLE_CLICK:
    break;
  case DOUBLE_CLICK:
    Serial.print("double ");
    break;
  case TRIPLE_CLICK:
    Serial.print("triple ");
    break;
  case LONG_CLICK:
    Serial.print("long");
    break;
  }
  Serial.print("click");
  Serial.print(" (");
  Serial.print(btn.getNumberOfClicks());
  Serial.println(")");
}
void setup()
{
  Serial.begin(115200);
  delay(50);
  Serial.println("\n\nSimple Counter");

  rotary.setChangedHandler(rotate);
  rotary.setLeftRotationHandler(showDirection);
  rotary.setRightRotationHandler(showDirection);

  button.setClickHandler(handler);
  button.setLongClickHandler(handler);
  button.setDoubleClickHandler(handler);
  button.setTripleClickHandler(handler);
}

void loop()
{
  rotary.loop();
  button.loop();
}
