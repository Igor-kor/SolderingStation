#ifndef THERMOFAN_H
#define THERMOFAN_H

#include <PID_v1.h>
#include "ThermofanDef.h"
#include "rus.h"
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>

class State;

class Thermofan {
    // Состояние
    State* state;
    State* newstate;
  public:
    // Значение температуры нагрева
    int thermocoupleValue = 0;
    // Пин геркона
    const int hermeticContactPin = 10;
    // Значение геркона
    bool hermeticContactState = false;
    bool oldhermeticContactState = true;
    bool changeEncoderButton = true;
    bool encButtonChange = 0;
    //включен ли нагрев фена
    bool warmingFan = true;
    // Для пид регулировки
    double Input , Setpoint, Output;
    // Оптимальные значения 0.5 0 0.7
    PID* fanpid;
    int speedfan, echospeedfan;

  public:
    Thermofan();
    static void attachEncoder();
    static void attachCrossZero();
    static void attachFun();
    unsigned long int getOversampled(int pin);
    bool getOversampledDigital(int pin);
    bool getOversampledDigitalLow(int pin);
    void getTenTemperature();
    int correction(int x);
    void readhermeticContactState();
    void saveButton();
    void readEncoderButtonState();
    void echo();
    void echoDisplay(int i);
    void echoDisplay(int i, int str);
    void echoDisplay(int i, int str, int x);
    void echoDisplay(char* i, int str);
    void echoDisplay(String i, int str, int x);
    void ReadPins();
    void loopth();
    void EndLoop();
    void SetState(State* state);
    void ReadEEPROM();
    void ResetEEPROM();
};
#endif
