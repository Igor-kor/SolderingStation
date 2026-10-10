#include "WaitingState.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "Display.h"
#include "rus.h"

WaitingState::WaitingState(Thermofan& context) : State(context) {}

WaitingState& WaitingState::instance(Thermofan& context) {
  static WaitingState state(context);
  return state;
}

void WaitingState::entry() {
  display.showStateLabel(S_WAITING);
}

void WaitingState::loop() {
  // если температура больше 60 градусов то перейдем в состояние охлаждения
  if (context.currentTemp() > TEMP_WAITING_TO_COOLING) {
    context.setFanPercent(context.fanTarget());
    context.setState(CoolingState::instance(context));
    return;
  }
  // если фен сняли с подставки то переходим в состояние нагрева
  if (!context.onStand()) {
    context.setFanPercent(context.fanTarget());
    context.setState(WarmingState::instance(context));
    return;
  }
  context.setHeaterPower(0);
  context.setFanPercent(0);
}
