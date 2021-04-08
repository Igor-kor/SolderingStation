#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"


extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;

extern uint32_t lastTickEncoder;
extern bool encDirection;
extern bool encButtonChange;

extern volatile int encCounter, encCounterFan;
extern volatile bool echoEncoder;
extern volatile bool state0, lastState, turnFlag;

extern uint32_t mil; 
extern int countzerocross;
extern int warmcount;
extern bool statewarm;
extern bool oldstatewarm;



WaitingState::WaitingState(Thermofan* context) : State(context) {
  this->StateName = new char[10] {'О', 'Ж', 'И', 'Д', 'А', 'Н', 'И', 'Е', ' ', '\0'};
  context->echoDisplay(this->StateName, 2, 0);
}
void WaitingState::loop() {
  // если температура больше 55 градусов то перейдем в состояние охлаждения
  if (context->Input > 55) {
    context->SetState(new CoolingState(context));
    delete this;
  }
  // если нагрев включили то переходим в состояние нагрева
  if (!context->hermeticContactState) {
    context->SetState(new WarmingState(context));
    delete this;
  }
  warmcount = 0;
  context->speedfan = 0;
}
