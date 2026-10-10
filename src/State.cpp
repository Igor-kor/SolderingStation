#include "State.h"
#include "Thermofan.h"

State::State(Thermofan& context) : context(context) {}

// Регулировка уставок; цель (температура или вентилятор)
// переключается кнопкой энкодера
void State::encoder() {
  int delta = context.encoderDelta();
  if (context.encoderTargetIsTemp()) {
    context.adjustSetpoint(delta);
  } else {
    context.adjustFanTarget(delta);
  }
}
