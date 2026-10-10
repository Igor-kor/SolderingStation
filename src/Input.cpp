#include "Input.h"

Input input;

static void inputEncoderIsr() {
  input.onEncoderIsr();
}

void Input::begin() {
  pinMode(ENC_A_PIN, INPUT);
  pinMode(ENC_B_PIN, INPUT);
  pinMode(ENC_BUTTON_PIN, INPUT);
  pinMode(SAVE_BUTTON_PIN, INPUT);
  pinMode(REED_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), inputEncoderIsr, CHANGE);
}

void Input::onEncoderIsr() {
  state0 = digitalRead(ENC_A_PIN);
  int thisdirection = 0;
  unsigned long tickEncoder = millis();
  int iterCount = 1;
  // ускорение: быстрые повороты дают больший шаг
  if (tickEncoder - lastTick > ENC_ACCEL_MS) {
    iterCount = 1;
  }
  if (tickEncoder - lastTick < ENC_ACCEL_MS && !turnFlag) {
    iterCount = (ENC_ACCEL_BASE - (tickEncoder - lastTick)) / 2;
  }
  if (state0 != lastState) {
    turnFlag = !turnFlag;
    thisdirection = (digitalRead(ENC_B_PIN) != lastState) ? ENC_RIGHT : ENC_LEFT;
    // беззнаковая разность корректна и при переполнении millis()
    if (direction != thisdirection && (tickEncoder - lastTick) > MILLIS_CHANGE_DIRECTION) {
      direction = thisdirection;
    }
    if (turnFlag) {
      if (direction == ENC_RIGHT) {
        encCounter -= iterCount;
      } else {
        encCounter += iterCount;
      }
      turned = true;
    }
    lastTick = tickEncoder;
    lastState = state0;
  }
}

int Input::takeEncoderDelta() {
  noInterrupts();
  int delta = encCounter;
  encCounter = 0;
  interrupts();
  return delta;
}

bool Input::consumeTurned() {
  bool result = turned;
  turned = false;
  return result;
}

// 8 чтений: true, если хоть одно HIGH
bool Input::anyPinHigh(uint8_t pin) {
  uint8_t high = 0;
  for (uint8_t i = 0; i < 8; i++) {
    high += digitalRead(pin);
  }
  return high > 0;
}

// 8 чтений: true, если хоть одно LOW (кнопка нажата)
bool Input::anyPinLow(uint8_t pin) {
  uint8_t low = 0;
  for (uint8_t i = 0; i < 8; i++) {
    low += !digitalRead(pin);
  }
  return low > 0;
}

void Input::update() {
  // Кнопка энкодера: цель переключается при отпускании
  bool pressed = anyPinLow(ENC_BUTTON_PIN);
  if (!pressed) {
    if (changeButton) {
      encButtonChange = !encButtonChange;
    }
    changeButton = false;
  } else {
    changeButton = true;
  }
  // Кнопка сохранения: фиксируем фронт нажатия
  bool save = !anyPinHigh(SAVE_BUTTON_PIN);
  savePressed = save && !saveWasPressed;
  saveWasPressed = save;
  // Геркон: замкнут (все чтения LOW) — фен на подставке
  reed = !anyPinHigh(REED_PIN);
}
