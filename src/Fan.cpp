#include "Fan.h"

Fan fan;

void Fan::begin() {
  pinMode(SPEED_FAN_PIN, OUTPUT);
}

void Fan::setPercent(int percent) {
  analogWrite(SPEED_FAN_PIN, map(percent, 0, 100, 0, 255));
}
