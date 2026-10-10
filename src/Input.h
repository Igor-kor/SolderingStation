#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "Config.h"

// Энкодер (с ускорением), две кнопки и геркон подставки
class Input {
  public:
    void begin();
    void update();                  // чтение кнопок и геркона, раз в цикл
    void onEncoderIsr();            // вызывается из ISR INT0

    int takeEncoderDelta();         // атомарно забрать накопленное смещение
    bool consumeTurned();           // true один раз после поворота энкодера
    bool encoderToggle() const { return encButtonChange; } // чем управляем
    bool savePressedEdge() const { return savePressed; }   // фронт нажатия
    bool saveHeld() const { return saveWasPressed; }       // зажата сейчас
    bool reedClosed() const { return reed; }               // фен на подставке

  private:
    // оверсемплинг цифровых пинов: 8 чтений
    static bool anyPinHigh(uint8_t pin);
    static bool anyPinLow(uint8_t pin);

    // общее с ISR энкодера — volatile
    volatile int encCounter = 0;
    volatile bool turned = false;
    volatile unsigned long lastTick = 0;
    volatile bool direction = 0;
    volatile bool state0 = 0, lastState = 0, turnFlag = 0;

    // только главный цикл
    bool changeButton = true;       // кнопка энкодера была зажата
    bool encButtonChange = false;   // текущая цель регулировки
    bool saveWasPressed = false;
    bool savePressed = false;
    bool reed = false;
};

extern Input input;

#endif
