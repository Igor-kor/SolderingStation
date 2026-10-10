#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include <Arduino.h>
#include "Config.h"

// Термопара фена через ОУ LM358 (усиление ~6.1)
class Thermocouple {
  public:
    int readTemperature();   // температура, °C
};

extern Thermocouple thermocouple;

#endif
