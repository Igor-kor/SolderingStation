#include <Arduino.h>
#include <U8g2lib.h>

//#define DEBAGSERIAL
//#define DEBAGPIDSERIAL
// Language and font
#include "u8x8_font_ikor.h"
#include "rus.h"
//#include "eng.h"

#include "Thermofan.h"

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
// Общие с прерываниями (INT0 энкодер, INT1 детектор нуля, Timer2 диммер) — обязательно volatile
volatile unsigned long lastTickEncoder;
volatile bool encDirection = 0;
volatile int encCounter = 0;
volatile bool echoEncoder = true;
volatile bool state0, lastState, turnFlag = false;
//uint32_t mil = 0; //текущие милисекунды для вывода на экран
volatile int countzerocross = 0;
volatile int warmcount = 10;
bool statewarm = false;
bool oldstatewarm = false;
// PID
double KP = 7, KI = 5, KD = 0.1;
Thermofan* thermofan1;

void setup() {
#if defined(DEBAGSERIAL) || defined(DEBAGPIDSERIAL)
  Serial.begin(9600);
#ifdef DEBAGSERIAL
  Serial.println("Setup SolderingStantion.ino");
#endif
#endif
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_ikor);
  thermofan1 =  new Thermofan();
  attachInterrupt(1, Thermofan::attachCrossZero, FALLING);
  attachInterrupt(0, Thermofan::attachEncoder, CHANGE);

  // инициализация Timer2: переполнение каждые 256 тактов (16 мкс при 16 МГц),
  // диммер считает тики от перехода через ноль (см. Thermofan::attachFun)
    cli(); // отключить глобальные прерывания
    TCCR2A = 0;
    TCCR2B = 0;

    // включить прерывание Timer2 overflow:
    TIMSK2 = (1 << TOIE2);
    // Прескалер 1 — бит CS20 таймера 2 (раньше стоял CS10 от таймера 1 и
    // работал лишь потому, что оба бита нулевые):
    TCCR2B |= (1 << CS20);
    sei(); // включить глобальные прерывания
#ifdef DEBAGSERIAL
  Serial.println("end Setup SolderingStantion.ino");
#endif
}

ISR(TIMER2_OVF_vect)
{
   Thermofan::attachFun();
}

void loop() {
  #ifdef DEBAGSERIAL
  Serial.println("Loop SolderingStantion.ino");
#endif 
  thermofan1->loopth();
    #ifdef DEBAGSERIAL
  Serial.println("End Loop SolderingStantion.ino");
#endif
}
