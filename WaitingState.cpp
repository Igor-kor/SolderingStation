#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"
#include "SettingState.h"

extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;
extern unsigned long lastTickEncoder;
extern bool encDirection;
extern int encCounter;
extern bool echoEncoder;
extern bool state0, lastState, turnFlag;
extern int countzerocross;
extern int warmcount;
extern bool statewarm;
extern bool oldstatewarm;

WaitingState::WaitingState(Thermofan* context) : State(context) {
#ifdef DEBAGSERIAL
  Serial.println("WaitingState::WaitingState");
#endif
  u8x8.clearLine(2);
  context->echoDisplay(S_WAITING, 2, 0);
}
void WaitingState::loop() {
#ifdef DEBAGSERIAL
  Serial.println("WaitingState::loop");
#endif
  // если температура больше 60 градусов то перейдем в состояние охлаждения
  if (context->Input > 60) {
    context->speedfan = context->echospeedfan;
    context->SetState(new CoolingState(context));
    echoEncoder = true;
    return;
  }
  // если нагрев включили то переходим в состояние нагрева
  if (!context->hermeticContactState) {
    context->speedfan = context->echospeedfan;
    context->SetState(new WarmingState(context));
    echoEncoder = true;
    return;
  }
  warmcount = 0;
  if (context->speedfan > 0) {
    echoEncoder = true;
  }
  context->speedfan = 0;
}

WaitingState::~WaitingState() {
}
