#include <Arduino.h>
#include <U8g2lib.h>
//#define DEBAGSERIAL
#include "u8x8_font_ikor.h"
#include "Thermofan.h"

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
uint32_t lastTickEncoder;
bool encDirection = 0;
bool encButtonChange = 0;
int encCounter = 0, encCounterFan = 100;
bool echoEncoder = true;
bool state0, lastState, turnFlag = false;
uint32_t mil = 0; //текущие милисекунды для вывода на экран
int countzerocross = 0;
int warmcount = 10;
bool statewarm = false;
bool oldstatewarm = false;
Thermofan* thermofan1;

void setup() {
  Serial.begin(9600);
#ifdef DEBAGSERIAL
  Serial.println("Setup SolderingStantion.ino");
#endif
  u8x8.begin();
  u8x8.setPowerSave(0);
  //u8x8.setFont(u8g2_font_logisoso_tn);
  u8x8.setFont(u8x8_font_ikor);
  thermofan1 =  new Thermofan();
  attachInterrupt(1, Thermofan::attachFun, FALLING);
  attachInterrupt(0, Thermofan::attachEncoder, CHANGE);
#ifdef DEBAGSERIAL
  Serial.println("end Setup SolderingStantion.ino");
#endif
}

void loop() {
  thermofan1->loopth();
}
