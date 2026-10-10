#ifndef DIMMER_H
#define DIMMER_H

#include <Arduino.h>
#include "Config.h"

// Фазовое управление нагревателем (симистор BT139 через MOC3021).
// Timer2 тикает каждые 16 мкс, детектор нуля (INT1) сбрасывает счётчик
// тиков в начале каждого полупериода сети
class Dimmer {
  public:
    void begin();
    void onZeroCross();             // вызывается из ISR INT1
    void onTimerTick();             // вызывается из ISR Timer2
    void setPower(int ticks);       // 0..WARMTICK (0 — нагрев выключен)

  private:
    volatile int tickCount = 0;     // тиков с последнего перехода через ноль
    volatile int power = 0;         // заданная мощность в тиках
};

extern Dimmer dimmer;

#endif
