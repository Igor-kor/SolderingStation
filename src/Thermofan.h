#ifndef THERMOFAN_H
#define THERMOFAN_H

#include <PID_v1.h>
#include "Config.h"
#include "SettingsStore.h"

class State;

// Координатор: ПИД, модули (диммер, вентилятор, термопара, ввод, дисплей,
// настройки) и машина состояний
class Thermofan {
  public:
    void begin();   // загрузка настроек, выбор начального состояния
    void tick();    // один проход главного цикла

    // --- интерфейс для состояний ---
    void setState(State& state);
    int currentTemp() const { return (int)Input; }
    int heaterPower() const { return (int)Output; }   // выход ПИД, 0..WARMTICK
    void setHeaterPower(int ticks);
    void setFanPercent(int percent);
    int fanTarget() const;
    void adjustSetpoint(int delta);
    void adjustFanTarget(int delta);
    int encoderDelta();
    bool encoderTargetIsTemp() const;
    bool onStand() const;
    Settings& settings() { return store.settings; }
    void applyPidTunings();

  private:
    State* state = nullptr;
    State* pendingState = nullptr;   // отложенная смена состояния
    PID* pid = nullptr;
    double Input = 0, Output = 0, Setpoint = 0;
    int speedfan = 0;                // текущая скорость вентилятора, %
    bool row1Dirty = true;           // перерисовать строку 1 дисплея
};

#endif
