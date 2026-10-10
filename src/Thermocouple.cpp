#include "Thermocouple.h"

Thermocouple thermocouple;

// Оверсэмплинг: 64 замера, 64 = 2^6, поэтому сдвиг на 6
static int oversampleAdc(uint8_t pin) {
  unsigned long result = 0;
  for (uint8_t i = 0; i < 64; i++) {
    result += analogRead(pin);
  }
  return result >> 6;
}

static int correction(int x) {
  return x - (TC_CORR_K * x - TC_CORR_B);
}

int Thermocouple::readTemperature() {
  int adc = oversampleAdc(THERMOCOUPLE_PIN);
  return correction(abs(int(TC_SCALE * adc * (5.0 / 1023.0) - TC_OFFSET)));
}
