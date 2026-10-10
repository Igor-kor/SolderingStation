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
extern double KP,KI,KD;

SettingState::SettingState(Thermofan* context) : State(context) {
#ifdef DEBAGSERIAL
  Serial.println("WaitingState::WaitingState");
#endif
  u8x8.clearLine(2);
  context->echoDisplay(S_SETTING, 2, 0);
  this->oldButtonChange = context->encButtonChange;
}
void SettingState::loop() {
#ifdef DEBAGSERIAL
  Serial.println("SettingState::loop");
#endif
  // если нагрев включили то переходим в состояние нагрева
  if (!context->hermeticContactState) {
    context->speedfan = context->echospeedfan;
    context->SetState(new WarmingState(context));
    echoEncoder = true;
    return;
  }
  noInterrupts();
  warmcount = 0;
  interrupts();
  if (context->speedfan > 0) {
    echoEncoder = true;
  }
  context->speedfan = 100;
}

void SettingState::encoder() {
  // 16-битный encCounter пишется в ISR энкодера — читаем атомарно
  noInterrupts();
  int delta = encCounter;
  interrupts();
  if (context->encButtonChange != this->oldButtonChange) {
    this->oldButtonChange = context->encButtonChange;
    this->Ksave++;
    if(this->Ksave>2)
      this->Ksave=0;
  }
   u8x8.clearLine(3);
  switch(this->Ksave){
    case 0:
        KP += delta/100.0;
        context->echoDisplay(String("P"), 3, 7);
        context->echoDisplay(String(KP,3), 3, 0);
      break;
    case 1:
        KI += delta/100.0;
        context->echoDisplay(String("I"), 3, 7);
        context->echoDisplay(String(KI,3), 3, 0);
      break;
    case 2:
        KD += delta/100.0;
        context->echoDisplay(String("D"), 3, 7);
        context->echoDisplay(String(KD,3), 3, 0);
      break;
  }
  // применяем новые коэффициенты сразу, а не после перезагрузки
  context->applyPidTunings();
}

SettingState::~SettingState() {
}
