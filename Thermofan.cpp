#include "Thermofan.h"
#include "State.h"
#include "u8x8_font_ikor.h"

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

Thermofan::Thermofan() { 
  this->fanpid = new PID(&this->Input, &this->Output, &this->Setpoint, 0.5, 0.2, 0.01, DIRECT);
  this->fanpid->SetOutputLimits(0, WARMTICK);
  pinMode(OPTRON_PIN, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(this->hermeticContactPin, INPUT);
  this->fanpid->SetMode(AUTOMATIC);
  // оптимально 80
  this->fanpid->SetSampleTime(1);
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_ikor);
  EEPROM.get(0, encCounter);
  EEPROM.get(2, encCounterFan);
  attachInterrupt(1, this->attachFun, FALLING);
  attachInterrupt(0, this->attachEncoder, CHANGE);
  this->speedfan = 200;
  this->echoDisplay("C*", 0, 3);
  this->echoDisplay("C*", 1, 3);
  this->echoDisplay("%", 1, 10);
  this->loopth();
  this->state = new State(this);
}

static void  Thermofan::attachEncoder() {
  state0 = digitalRead(ENC_A);
  int thisdirection = 0;
  int tickEncoder = millis();
  int iterCount = 1;
  if (tickEncoder - lastTickEncoder > 12) {
    iterCount = 1;
  }
  if (tickEncoder - lastTickEncoder < 12 && !turnFlag) {
    iterCount = (14 - (tickEncoder - lastTickEncoder)) / 2;
  }
  if (state0 != lastState) {
    turnFlag = !turnFlag;
    thisdirection = (digitalRead(ENC_B) != lastState) ? ENC_RIGHT : ENC_LEFT;
    if (encDirection != thisdirection && (lastTickEncoder + MILLIS_CHANGE_DIRECTION) < tickEncoder) {
      encDirection = thisdirection;
    }
    if (turnFlag) {
      if (encDirection == ENC_RIGHT ) {
        if (encButtonChange)encCounter -= iterCount;
        else encCounterFan -= iterCount;
      } else {
        if (encButtonChange) encCounter += iterCount;
        else encCounterFan += iterCount;
      }
      echoEncoder = true;
    }
    lastTickEncoder = tickEncoder;
    lastState = state0;
    if (encCounter < 20)encCounter = 20;
    if (encCounter > 500)encCounter = 500;
    if (encCounterFan < 100)encCounterFan = 100;
    if (encCounterFan > 255)encCounterFan = 255;
  }
}

static void  Thermofan::attachFun() {
  if (countzerocross >= (WARMTICK - warmcount) && warmcount > 0) {
    statewarm = true;
    countzerocross = 0;
  } else {
    statewarm = false;
  }
  countzerocross++;
  if (oldstatewarm != statewarm) {
    digitalWrite(9, statewarm ? HIGH : LOW);
    oldstatewarm = statewarm;
  }
}

//функция оверсэмплинга
unsigned long int  Thermofan::getOversampled(int pin) {
  unsigned long int result = 0;
  for (byte z = 0; z < 64; z++) { //делаем 64 замера
    result += analogRead(pin); //складываем всё вместе
  }
  //делаем побитовый сдвиг для полученного значения (64 это 2 в 6-ой степени, поэтому »6)
  return result >> 6;
}

//функция оверсэмплинга для геркона, (не сильно много тут нужно)
bool  Thermofan::getOversampledDigital(int pin) {
  unsigned long int result = 0;
  for (byte z = 0; z < 8; z++) { //делаем 8 замера
    result += digitalRead(pin); //складываем всё вместе
  }
  //делаем побитовый сдвиг для полученного значения (8 это 2 в 3-ой степени, поэтому »3)
  return (result >> 3) > 0;
}

//функция оверсэмплинга
bool  Thermofan::getOversampledDigitalLow(int pin) {
  unsigned long int result = 0;
  for (byte z = 0; z < 8; z++) { //делаем 8 замера
    result += !digitalRead(pin); //складываем всё вместе
  }
  //делаем побитовый сдвиг для полученного значения (8 это 2 в 3-ой степени, поэтому »3)
  return (result >> 3) > 0;
}

// получение температуры с термопары фена
void  Thermofan::getTenTemperature() {

  this->thermocoupleValue =  correction(abs(int(206.36 * this->getOversampled(THERMOCOUPLE_PIN) * (5.0 / 1023.0) - 13.263)));
  this->Input = this->thermocoupleValue;
}

// корректировка
int  Thermofan::correction(int x) {
  // return  x-int(0.4266*x-11.4621);
  return x - (0.2087 * x - 23.2);
}

//чтение значения установленной температуры
void  Thermofan::readPotentiometr() {
  this->Setpoint = encCounter;
}

void  Thermofan::readhermeticContactState() {
  this->hermeticContactState = !getOversampledDigital(this->hermeticContactPin);
}

void  Thermofan::saveButton() {
  if (!getOversampledDigital(7)) {
    // будет сохранятся установленные значения
    EEPROM.put(0, encCounter);
    EEPROM.put(2, encCounterFan);
  }
}
void  Thermofan::readEncoderButtonState() {
  if (!getOversampledDigitalLow(8)) {
    if (this->changeEncoderButton) {
      encButtonChange = !encButtonChange;
    }
    this->changeEncoderButton = false;
  } else {
    this->changeEncoderButton = true;
  }
}

//вывод
void  Thermofan::echo () {
  this->echoDisplay(this->Input, 0);
  if (this->Input < 100) {
    this->echoDisplay(" ", 0, 2);
  }
  if (this->Setpoint < 100) {
    this->echoDisplay(" ", 1, 2);
  }
  if (echoEncoder) {
    this->echoDisplay(this->Setpoint, 1);
    this->echoDisplay(map(encCounterFan, 100, 255, 0, 100), 1, 7);
    echoEncoder = false;
  }
  this->echoDisplay(warmcount, 0, 7);
  this->echoDisplay(this->Output, 0, 9);
  Serial.println((String("*temperatura*:") + String((int)this->Input)).c_str());
  Serial.println((String("*output*:") + String((int)this->Output)).c_str());
  Serial.println((String("*set*:") + String((int)this->Setpoint)).c_str());
}

//сам вывод на дисплей или куда надо
void  Thermofan::echoDisplay(int i) {
  u8x8.drawString(0, 0,  String(i).c_str());
  return;
}

void  Thermofan::echoDisplay(int i, int str) {
  u8x8.drawString(0, str,  String(i).c_str());
  return;
}

void  Thermofan::echoDisplay(int i, int str, int x) {
  u8x8.drawString(x, str, String(i).c_str());
  return;
}

void  Thermofan::echoDisplay(char* i, int str) {
  u8x8.drawString(0, str, i);
  return;
}

void  Thermofan::echoDisplay(char* i, int str, int x) {
  u8x8.drawString(x, str, i);
  return;
}

void  Thermofan::ReadPins() {
  // Считыываем состояние геркона
  this->readhermeticContactState();
  // Считываем значение кнопки энкодера
  this->readEncoderButtonState();
  // Считываем значение кнопки сохранения
  this->saveButton();
  // Считываем с пина значение температуры
  this->getTenTemperature();
  // Считыаем значение с потенциометра
  this->readPotentiometr();
}

void  Thermofan::loopth() {
  // Вывод значения todo: пока будет костыль
  if ( (millis() - mil) > TIME_ECHO) {
    this->echo();
  }
  mil = millis();
  this->ReadPins();
  this->state->loop();
  this->EndLoop();
}
void  Thermofan::EndLoop() {
  analogWrite(SPEED_FAN_PIN, this->speedfan);
}

void  Thermofan::SetState(State* state){
  this->state = state;
}
