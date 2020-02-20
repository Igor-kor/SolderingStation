#include <PID_v1.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include "Thermofan.h"

Thermofan* thermofan1;

void setup() {
  thermofan1 =  new Thermofan();
}

void loop() {
  thermofan1->loopth();
}
