#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"

State::State(Thermofan* context) {
  this->context = context;
  this->StateName = new char[10] {'*', '*', '*', ' ', ' ', ' ', '*', '*', '*', '\0'};
}
void State::loop() {
  context->SetState(new WaitingState(context));
  delete this;
}
