#ifndef DIMMER_H
#define DIMMER_H

#include <Arduino.h>
#include "Config.h"

// Фазовое управление нагревателем (симистор BT139 через MOC3021).
// Переход через ноль (INT1) запускает 16-битный Timer1 в режиме CTC:
// прерывание сравнения срабатывает дважды за полупериод — в момент
// открытия симистора и через ~16 мкс для закрытия. Итого ~200
// прерываний/с вместо ~62500 при постоянном счёте переполнений Timer2.
class Dimmer {
  public:
    void begin();
    void onZeroCross();           // вызывается из ISR INT1
    void onCompare();             // вызывается из ISR Timer1 COMPA
    void setPower(int ticks);     // 0..WARMTICK (0 — нагрев выключен)

  private:
    volatile int power = 0;       // заданная мощность в тиках по 16 мкс
    volatile bool gateOn = false; // идёт импульс открытия симистора
};

extern Dimmer dimmer;

#endif
