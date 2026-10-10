#ifndef FAN_H
#define FAN_H

#include <Arduino.h>
#include "Config.h"

// Вентилятор (турбина): ШИМ через оптопару на MOSFET
class Fan {
  public:
    void begin();
    void setPercent(int percent);   // 0..100
};

extern Fan fan;

#endif
