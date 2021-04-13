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

WarmingState::WarmingState(Thermofan* context) : State(context) {
#ifdef DEBAGSERIAL
  Serial.println("WarmingState::WarmingState");
#endif
  this->StateName = new char[10] {'Н', 'А', 'Г', 'Р', 'Е', 'В', ' ', ' ', ' ', '\0'};
  context->echoDisplay(this->StateName, 2, 0);
}
void WarmingState::loop() {
#ifdef DEBAGSERIAL
  Serial.println("WarmingState::loop()");
#endif
  // если установлена температура меньше 20 или термофен лежит на подставке/выключен тумблер то
  // переходим в состояние остывания
  if (context->Setpoint < 20 || context->hermeticContactState) {
    context->SetState(new CoolingState(context)); 
  }
  // Делаем расчет значения для пид
  context->fanpid->Compute();
  //нагрев тена
  warmcount = context->Output;
  //скорость вращения фена
  context->speedfan = encCounterFan;
}

WarmingState::~WarmingState(){
  delete this->StateName;
}
