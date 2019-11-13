#include <PID_v1.h>
#include <iarduino_OLED.h>

#include "Thermofan.h"

Thermofan* thermofan1;

void setup() {
  thermofan1 =  new Thermofan();
}

void loop() {
  thermofan1->loopth();
}
