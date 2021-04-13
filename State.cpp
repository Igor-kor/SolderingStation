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
  this->StateName = new char[10] {'*', '*', '*', ' ', ' ', ' ', '*', '*', '*', '\0'};
  //context->echoDisplay(this->StateName, 2, 0);
}

void State::loop() {
#ifdef DEBAGSERIAL
  Serial.println("State::loop()");
#endif
  context->SetState(new WaitingState(context)); 
}

State::~State(){
  delete this->StateName;
}
