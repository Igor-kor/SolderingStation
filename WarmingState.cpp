
#include "State.h"
#include "Thermofan.h"
#include "CoolingState.h"
#include "WarmingState.h"
#include "WaitingState.h"
WarmingState::WarmingState(Thermofan* context) : State(context) {
  this->StateName = new char[10] {'Н', 'А', 'Г', 'Р', 'Е', 'В', ' ', ' ', ' ', '\0'};
  context->echoDisplay(this->StateName, 2, 0);
}
void WarmingState::loop() {
  // если установлена температура меньше 20 или термофен лежит на подставке/выключен тумблер то
  // переходим в состояние остывания
  if (context->Setpoint < 20 || context->hermeticContactState) {
    context->SetState(new CoolingState(context));
    delete this;
  }
  // Делаем расчет значения для пид
  context->fanpid->Compute();
  //нагрев тена
  warmcount = context->Output;
  //скорость вращения фена
  context->speedfan = encCounterFan;
}
