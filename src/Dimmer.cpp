#include "Dimmer.h"

Dimmer dimmer;

static void dimmerZeroCrossIsr() {
  dimmer.onZeroCross();
}

void Dimmer::begin() {
  pinMode(OPTRON_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), dimmerZeroCrossIsr, FALLING);
  // Timer2: переполнение каждые 256 тактов (16 мкс при 16 МГц)
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = (1 << TOIE2);
  TCCR2B |= (1 << CS20);  // прескалер 1 (CS20; CS10 — бит таймера 1)
  sei();
}

// Сбрасываем счётчик тиков при переходе через ноль,
// синхронизируя таймер с сетью
void Dimmer::onZeroCross() {
  tickCount = 0;
}

ISR(TIMER2_OVF_vect) {
  dimmer.onTimerTick();
}

void Dimmer::onTimerTick() {
#if OPTRON_PIN != 9
#error "Прямая работа с портом рассчитана на OPTRON_PIN 9 (PB1)"
#endif
  // Прямой доступ к порту: ISR вызывается ~62500 раз/с
  if (tickCount == (WARMTICK - power) && power > 0) {
    PORTB |= _BV(PB1);
  } else {
    PORTB &= ~_BV(PB1);
  }
  // если нагрева нет, то и считать не нужно
  if (power != 0) {
    tickCount++;
  }
}

// power читается в ISR — пишем атомарно
void Dimmer::setPower(int ticks) {
  noInterrupts();
  power = ticks;
  interrupts();
}
