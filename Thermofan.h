#ifndef THERMOFAN_H
#define THERMOFAN_H

#include <PID_v1.h>
#include "ThermofanDef.h" 
#include <U8g2lib.h>
#include <PID_v1.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>

class State; 

extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8;

extern uint32_t lastTickEncoder;
extern bool encDirection;
extern bool encButtonChange;

extern volatile int encCounter, encCounterFan;
extern volatile bool echoEncoder;
extern volatile bool state0, lastState, turnFlag;

extern uint32_t mil; 
extern int countzerocross;
extern int warmcount;
extern bool statewarm;
extern bool oldstatewarm;




//также нужно сделать определения подключения фена в разьем
//при разогреве сверх ххх температуры отключить нагрев, сообщить о неисправности
class Thermofan {
    // Состояние
    State* state;
    public:
    // Значение температуры нагрева
    int thermocoupleValue = 0;
    // ОШИБКИ
    // 1 - Перегрев фена выше TEMP_MAX_OFF
    int error = 0;
    // Пин геркона
    const int hermeticContactPin = 10;
    // Значение геркона
    bool hermeticContactState = false;
    bool oldhermeticContactState = true;
    bool changeEncoderButton = true;
    //включен ли нагрев фена
    bool warmingFan = true;
    // Для пид регулировки
    double Input , Setpoint, Output;
    // Оптимальные значения 0.5 0 0.7
    PID* fanpid;
    int speedfan;

  public:
    Thermofan();
    static void attachEncoder();
    static void attachFun();
    unsigned long int getOversampled(int pin);
    bool getOversampledDigital(int pin);
    bool getOversampledDigitalLow(int pin);
    void getTenTemperature();
    int correction(int x);
    void readPotentiometr();
    void readhermeticContactState();
    void saveButton();
    void readEncoderButtonState();
    void echo();
    void echoDisplay(int i);
    void echoDisplay(int i, int str);
    void echoDisplay(int i, int str, int x);
    void echoDisplay(char* i, int str);
    void echoDisplay(char* i, int str, int x);
    void ReadPins();
    void loopth();
    void EndLoop();
    void SetState(State* state);
};
#endif
