#include <Arduino.h>
#include "Config.h"
#include "Thermofan.h"
#include "Dimmer.h"
#include "Fan.h"
#include "Input.h"
#include "Display.h"

Thermofan thermofan;

void setup() {
#if defined(DEBUG_SERIAL) || defined(DEBUG_PID_SERIAL)
  Serial.begin(9600);
#endif
  display.begin();
  input.begin();
  fan.begin();
  dimmer.begin();     // Timer2 + детектор нуля
  thermofan.begin();
}

void loop() {
  thermofan.tick();
}
