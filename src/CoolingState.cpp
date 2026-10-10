#include "CoolingState.h"
#include "Thermofan.h"
#include "WaitingState.h"
#include "WarmingState.h"
#include "Display.h"
#include "rus.h"

CoolingState::CoolingState(Thermofan& context) : State(context) {}

CoolingState& CoolingState::instance(Thermofan& context) {
  static CoolingState state(context);
  return state;
}

void CoolingState::entry() {
  display.showStateLabel(S_COOLING);
}

void CoolingState::loop() {
  // если температура меньше 50 градусов то перейдем в состояние ожидания
  if (context.currentTemp() < TEMP_COOLING_TO_WAITING) {
    context.setFanPercent(context.fanTarget());
    context.setState(WaitingState::instance(context));
    return;
  }
  // если фен сняли с подставки то переходим в состояние нагрева
  if (!context.onStand()) {
    context.setFanPercent(context.fanTarget());
    context.setState(WarmingState::instance(context));
    return;
  }
  context.setHeaterPower(0);
  context.setFanPercent(100);
}
