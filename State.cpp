#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"

State::State(Thermofan* context) {
#ifdef DEBAGSERIAL
  Serial.println("State::State(Thermofan* context)");
#endif
  this->context = context; 
}

void State::loop() {
#ifdef DEBAGSERIAL
  Serial.println("State::loop()");
#endif
  context->SetState(new WaitingState(context)); 
}

State::~State(){ 
}
