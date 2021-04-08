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



CoolingState::CoolingState(Thermofan* context) : State(context) {
  this->StateName = new char[10] {'О', 'С', 'Т', 'Ы', 'В', 'А', 'Н', 'И', 'Е', '\0'};
  context->echoDisplay(this->StateName, 2, 0);
}
void CoolingState::loop() {
  // если температура меньше 50 градусов то перейдем в состояние ожидания
  if (context->Input < 50) {
    context->SetState(new WaitingState(context));
    delete this;
  }
  // если нагрев включили то переходим в состояние нагрева
  if (!context->hermeticContactState) {
    context->SetState(new WarmingState(context));
    delete this;
  }
  warmcount = 0;
  context->speedfan = 255;
}
