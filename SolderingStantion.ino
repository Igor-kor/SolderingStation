#include <PID_v1.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>


#include "u8x8_font_ikor.c"
#define font_ikor   u8x8_font_ikor

#include "Thermofan.h"

Thermofan* thermofan1;

void setup() {
  thermofan1 =  new Thermofan();
  Serial.begin(9600);
}

void loop() {
  thermofan1->loopth();
}
