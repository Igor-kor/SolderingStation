#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"

extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;
extern unsigned long lastTickEncoder;
extern bool encDirection;
extern  int encCounter, encCounterFan;
extern  bool echoEncoder;
extern  bool state0, lastState, turnFlag;
extern int countzerocross;
extern int warmcount;
extern bool statewarm;
extern bool oldstatewarm;

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
  context->speedfan = context->echospeedfan;
  context->SetState(new WaitingState(context));
}

void State::encoder() {
  if (context->encButtonChange) {
    context->Setpoint += encCounter;
  }
  else {
    context->echospeedfan += encCounter;
  }
  if (context->Setpoint < 10)context->Setpoint = 10;
  if (context->Setpoint > 500)context->Setpoint = 500;
  if (context->echospeedfan < 0)context->echospeedfan = 0;
  if (context->echospeedfan > 100)context->echospeedfan = 100;
}

State::~State() {
}
