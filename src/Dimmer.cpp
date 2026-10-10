#include "Dimmer.h"

Dimmer dimmer;

static void dimmerZeroCrossIsr() {
  dimmer.onZeroCross();
}

#if OPTRON_PIN != 9
#error "Прямая работа с портом рассчитана на OPTRON_PIN 9 (PB1)"
#endif

// Timer1 работает с прескалером 8 (тик 0.5 мкс): единица шкалы
// WARMTICK — 256 тактов ЦП (16 мкс), как в старом счётчике переполнений
static const uint16_t TIMER_TICKS_PER_UNIT = 32;
// длительность импульса открытия — 256 тактов ЦП (~16 мкс), как раньше
static const uint16_t GATE_PULSE_TICKS = 32;

void Dimmer::begin() {
  pinMode(OPTRON_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), dimmerZeroCrossIsr, FALLING);
  // Timer1 (16 бит) останавливается между полупериодами;
  // конфигурируется в ISR перехода через ноль
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TIMSK1 = 0;
  TCNT1 = 0;
  interrupts();
}

// Начало полупериода: рассчитать задержку открытия и запустить таймер.
// Мощность -> фаза: открытие происходило на (WARMTICK - power + 1)-м
// переполнении Timer2 (каждые 256 тактов ЦП) — фазовая шкала сохранена.
void Dimmer::onZeroCross() {
  PORTB &= ~_BV(PB1);   // закрыть симистор
  gateOn = false;
  if (power <= 0) {
    TCCR1B = 0;         // нагрев выключен — таймер не нужен
    TIMSK1 = 0;
    return;
  }
  uint16_t units = (uint16_t)(WARMTICK - power) + 1;   // блоков по 16 мкс
  TCNT1 = 0;
  OCR1A = (uint16_t)(units * TIMER_TICKS_PER_UNIT - 1);
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11);   // CTC по OCR1A, прескалер 8
  TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
  dimmer.onCompare();
}

void Dimmer::onCompare() {
  if (!gateOn) {
    // фаза наступила: короткий импульс открытия (MOC3021 защёлкивает
    // симистор, дальше он горит до перехода тока через ноль)
    PORTB |= _BV(PB1);
    gateOn = true;
    OCR1A = GATE_PULSE_TICKS - 1;
  } else {
    PORTB &= ~_BV(PB1);
    gateOn = false;
    TCCR1B = 0;    // стоп до следующего перехода через ноль
    TIMSK1 = 0;
  }
}

// power читается в ISR — пишем атомарно и в допустимых пределах
void Dimmer::setPower(int ticks) {
  if (ticks < 0) ticks = 0;
  if (ticks > WARMTICK) ticks = WARMTICK;
  noInterrupts();
  power = ticks;
  interrupts();
}
