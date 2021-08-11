#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"

extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;
extern uint32_t lastTickEncoder;
extern bool encDirection;
extern  int encCounter;
extern  bool echoEncoder;
extern  bool state0, lastState, turnFlag;
extern uint32_t mil;
extern int countzerocross;
extern int warmcount;
extern bool statewarm;
extern bool oldstatewarm;

CoolingState::CoolingState(Thermofan* context) : State(context) {
#ifdef DEBAGSERIAL
  Serial.println("CoolingState::CoolingState");
#endif
  u8x8.clearLine(2);
  context->echoDisplay("\x9e\xa1\xa2\xab\x92\x90\x9d\x98\x95", 2, 0);
}

void CoolingState::loop() {
#ifdef DEBAGSERIAL
  Serial.println(" CoolingState::loop()");
#endif
  // если температура меньше 50 градусов то перейдем в состояние ожидания
  if (context->Input < 50) {
    context->speedfan = context->echospeedfan;
    echoEncoder = true;
    context->SetState(new WaitingState(context));
    return;
  }
  // если нагрев включили то переходим в состояние нагрева
  if (!context->hermeticContactState) {
    context->speedfan = context->echospeedfan;
    echoEncoder = true;
    context->SetState(new WarmingState(context));
    return;
  }
  warmcount = 0;
  if(context->speedfan != 100) {
     echoEncoder = true;
  }
  context->speedfan = 100;
}

CoolingState::~CoolingState() {
}
