#include <Arduino.h>
#include <U8g2lib.h>
#include "u8x8_font_ikor.h"
#include "Thermofan.h"

Thermofan* thermofan1;

void setup() {
  thermofan1 =  new Thermofan();
  Serial.begin(9600);
}

void loop() {
  thermofan1->loopth();
}
