#include "OverheatState.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "Display.h"
#include "Lang.h"

OverheatState::OverheatState(Thermofan& context) : State(context) {}

OverheatState& OverheatState::instance(Thermofan& context) {
  static OverheatState state(context);
  return state;
}

void OverheatState::entry() {
  display.setStateLabel(S_OVERHEAT);
}

void OverheatState::loop() {
  // аварийное охлаждение: нагрев выключен, вентилятор на максимум
  context.setHeaterPower(0);
  context.setFanPercent(100);
  // остыли ниже безопасной температуры — возвращаемся к обычной работе
  if (context.currentTemp() < TEMP_OVERHEAT_RESUME) {
    context.setFanPercent(context.fanTarget());
    if (context.onStand()) {
      context.setState(CoolingState::instance(context));
    } else {
      context.setState(WarmingState::instance(context));
    }
  }
}
