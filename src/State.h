#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

class Thermofan;

// Базовое состояние машины состояний
class State {
  public:
    explicit State(Thermofan& context);
    virtual ~State() = default;
    virtual void entry() {}     // вход в состояние (вывод подписи и т.п.)
    virtual void loop() = 0;    // вызывается каждый цикл
    virtual void encoder();     // по умолчанию: регулировка уставок

  protected:
    Thermofan& context;
};

#endif
