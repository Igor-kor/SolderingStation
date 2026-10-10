#include "WarmingState.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "Display.h"
#include "Lang.h"

WarmingState::WarmingState(Thermofan& context) : State(context) {}

WarmingState& WarmingState::instance(Thermofan& context) {
  static WarmingState state(context);
  return state;
}

void WarmingState::entry() {
  display.showStateLabel(S_WARMING);
  context.setHeaterPower(context.heaterPower());
}

void WarmingState::loop() {
  context.setFanPercent(context.fanTarget());
  // фен положили на подставку/выключили тумблер — переходим в остывание
  if (context.onStand()) {
    context.setState(CoolingState::instance(context));
    return;
  }
  // нагрев тена: мощность из выхода ПИД
  context.setHeaterPower(context.heaterPower());
}
