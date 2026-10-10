#include "SettingState.h"
#include "Thermofan.h"
#include "WarmingState.h"
#include "Display.h"
#include "SettingsStore.h"
#include "Lang.h"

SettingState::SettingState(Thermofan& context) : State(context) {}

SettingState& SettingState::instance(Thermofan& context) {
  static SettingState state(context);
  return state;
}

void SettingState::entry() {
  context.applyPidTunings();   // на случай правок до перезагрузки
  display.setStateLabel(S_SETTING);
  showK();
}

void SettingState::showK() {
  Settings& s = context.settings();
  char buf[12];
  const char* key;
  switch (ksel) {
    case 0: key = "P"; dtostrf(s.kp, 0, 3, buf); break;
    case 1: key = "I"; dtostrf(s.ki, 0, 3, buf); break;
    default: key = "D"; dtostrf(s.kd, 0, 3, buf); break;
  }
  display.showPid(key, buf);
}

void SettingState::loop() {
  // фен сняли с подставки — выходим из настроек в нагрев
  if (!context.onStand()) {
    context.setFanPercent(context.fanTarget());
    context.setState(WarmingState::instance(context));
    return;
  }
  context.setHeaterPower(0);
  context.setFanPercent(100);
}

void SettingState::encoder() {
  int delta = context.encoderDelta();
  // кнопка энкодера перебирает P -> I -> D
  bool buttonChange = context.encoderTargetIsTemp();
  if (buttonChange != oldButtonChange) {
    oldButtonChange = buttonChange;
    ksel = (ksel + 1) % 3;
  }
  Settings& s = context.settings();
  switch (ksel) {
    case 0: s.kp += delta / 100.0; break;
    case 1: s.ki += delta / 100.0; break;
    case 2: s.kd += delta / 100.0; break;
  }
  showK();
  // применяем новые коэффициенты сразу, а не после перезагрузки
  context.applyPidTunings();
}
