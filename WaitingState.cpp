#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"

extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;
extern uint32_t lastTickEncoder;
extern bool encDirection;
extern bool encButtonChange;
extern  int encCounter, encCounterFan;
extern  bool echoEncoder;
extern  bool state0, lastState, turnFlag;
extern uint32_t mil;
extern int countzerocross;
extern int warmcount;
extern bool statewarm;
extern bool oldstatewarm;

WaitingState::WaitingState(Thermofan* context) : State(context) {
#ifdef DEBAGSERIAL
  Serial.println("WaitingState::WaitingState");
#endif
  context->echoDisplay("ОЖИДАНИЕ ", 2, 0);
}
void WaitingState::loop() {
#ifdef DEBAGSERIAL
  Serial.println("WaitingState::loop");
#endif
  // если температура больше 55 градусов то перейдем в состояние охлаждения
  if (context->Input > 55) {
    context->SetState(new CoolingState(context)); 
  }
  // если нагрев включили то переходим в состояние нагрева
  if (!context->hermeticContactState) {
    context->SetState(new WarmingState(context)); 
  }
  warmcount = 0;
  context->speedfan = 0;
}

WaitingState::~WaitingState(){ 
}
