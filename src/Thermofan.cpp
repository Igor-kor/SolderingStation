#include "Thermofan.h"
#include "State.h"
#include "WaitingState.h"
#include "WarmingState.h"
#include "CoolingState.h"
#include "SettingState.h"
#include "OverheatState.h"
#include "Dimmer.h"
#include "Fan.h"
#include "Thermocouple.h"
#include "Input.h"
#include "Display.h"
#include "SettingsStore.h"

void Thermofan::begin() {
  store.load();
  Setpoint = store.settings.setpoint;
  // стартовая скорость вентилятора — сохранённая уставка
  speedfan = store.settings.fanSpeed;
  pid = new PID(&Input, &Output, &Setpoint, store.settings.kp, store.settings.ki, store.settings.kd, DIRECT);
  pid->SetOutputLimits(0, WARMTICK);
  pid->SetMode(AUTOMATIC);
  pid->SetSampleTime(10);
  input.update();
  // зажатая при включении кнопка сохранения — вход в режим настройки ПИД
  if (input.saveHeld()) {
    state = &SettingState::instance(*this);
  } else {
    state = &WaitingState::instance(*this);
  }
  state->entry();
}

void Thermofan::tick() {
  // отложенная смена состояния
  if (pendingState != nullptr) {
    state = pendingState;
    pendingState = nullptr;
    state->entry();
  }
  input.update();
  Input = thermocouple.readTemperature();
  // сохранение настроек — по фронту нажатия кнопки
  if (input.savePressedEdge()) {
    store.save();
  }
  // расчёт ПИД по свежему значению температуры
  pid->Compute();
  state->encoder();
  state->loop();
  // Аварийная защита от перегрева: приоритетнее любого состояния
  if (Input >= TEMP_OVERHEAT_OFF && state != &OverheatState::instance(*this)) {
    setHeaterPower(0);
    setState(OverheatState::instance(*this));
  }
  fan.setPercent(speedfan);
  display.update((int)Input, (int)Setpoint, fanTarget(), (int)Output);
#ifdef DEBUG_PID_SERIAL
  Serial.print(F("t=")); Serial.print((int)Input);
  Serial.print(F(" out=")); Serial.print((int)Output);
  Serial.print(F(" set=")); Serial.println((int)Setpoint);
#endif
}

void Thermofan::setState(State& newState) {
  pendingState = &newState;
}

void Thermofan::setHeaterPower(int ticks) {
  dimmer.setPower(ticks);
}

void Thermofan::setFanPercent(int percent) {
  speedfan = percent;
}

int Thermofan::fanTarget() const {
  return store.settings.fanSpeed;
}

void Thermofan::adjustSetpoint(int delta) {
  Setpoint += delta;
  if (Setpoint < SETPOINT_MIN) Setpoint = SETPOINT_MIN;
  if (Setpoint > SETPOINT_MAX) Setpoint = SETPOINT_MAX;
  // держим сохраняемую уставку в соответствии с текущей
  store.settings.setpoint = (int)Setpoint;
}

void Thermofan::adjustFanTarget(int delta) {
  store.settings.fanSpeed += delta;
  if (store.settings.fanSpeed < 0) store.settings.fanSpeed = 0;
  if (store.settings.fanSpeed > 100) store.settings.fanSpeed = 100;
}

int Thermofan::encoderDelta() {
  return input.takeEncoderDelta();
}

bool Thermofan::encoderTargetIsTemp() const {
  return input.encoderToggle();
}

bool Thermofan::onStand() const {
  return input.reedClosed();
}

// Применение коэффициентов ПИД на лету: в конструктор PID они
// передаются по значению
void Thermofan::applyPidTunings() {
  pid->SetTunings(store.settings.kp, store.settings.ki, store.settings.kd);
}
