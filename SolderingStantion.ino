#include <Arduino.h>
#include <U8g2lib.h>
#include "u8x8_font_ikor.h"
#include "Thermofan.h"

// todo: вечная перезагрузка ардуино!
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

uint32_t lastTickEncoder;
bool encDirection = 0;
bool encButtonChange = 0;

volatile int encCounter = 0, encCounterFan = 100;
volatile bool echoEncoder = true;
volatile bool state0, lastState, turnFlag = false;

uint32_t mil = 0; //текущие милисекунды для вывода на экран
int countzerocross = 0;
int warmcount = 10;
bool statewarm = false;
bool oldstatewarm = false;
Thermofan* thermofan1;

void setup() {
  attachInterrupt(1, Thermofan::attachFun, FALLING);
  attachInterrupt(0, Thermofan::attachEncoder, CHANGE);
  thermofan1 =  new Thermofan();
  Serial.begin(9600);
    thermofan1->loopth();
}

void loop() {

}
