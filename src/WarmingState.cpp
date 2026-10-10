#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"
#include "SettingState.h"

extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;
extern volatile unsigned long lastTickEncoder;
extern volatile bool encDirection;
extern volatile int encCounter;
extern volatile bool echoEncoder;
extern volatile bool state0, lastState, turnFlag;
extern volatile int countzerocross;
extern volatile int warmcount;
extern bool statewarm;
extern bool oldstatewarm;

WarmingState::WarmingState(Thermofan* context) : State(context) {
#ifdef DEBAGSERIAL
  Serial.println("WarmingState::WarmingState");
#endif
  u8x8.clearLine(2);
  context->echoDisplay(S_WARMING, 2, 0);
  noInterrupts();
  warmcount = context->Output;
  interrupts();
}
void WarmingState::loop() {
#ifdef DEBAGSERIAL
  Serial.println("WarmingState::loop()");
#endif
  context->speedfan = context->echospeedfan;
  // если установлена температура меньше 20 или термофен лежит на подставке/выключен тумблер то
  // переходим в состояние остывания
  if ( context->hermeticContactState) {
    context->SetState(new CoolingState(context));
    return;
  }
  //нагрев тена: warmcount читается в ISR Timer2 — пишем атомарно
  noInterrupts();
  warmcount = context->Output;
  interrupts();
}

WarmingState::~WarmingState() {
}
