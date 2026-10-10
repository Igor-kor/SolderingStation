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
  // Если зажата кнопка сохранения то перейдем в режим настройки
  if (!context->getOversampledDigital(7)) {
    context->SetState(new SettingState(context));
  }
  else{
    context->SetState(new WaitingState(context));
  }
}

void State::encoder() {
  // 16-битный encCounter пишется в ISR энкодера — читаем атомарно
  noInterrupts();
  int delta = encCounter;
  interrupts();
  if (context->encButtonChange) {
    context->Setpoint += delta;
  }
  else {
    context->echospeedfan += delta;
  }
  if (context->Setpoint < 10)context->Setpoint = 10;
  if (context->Setpoint > 500)context->Setpoint = 500;
  if (context->echospeedfan < 0)context->echospeedfan = 0;
  if (context->echospeedfan > 100)context->echospeedfan = 100;
}

State::~State() {
}
