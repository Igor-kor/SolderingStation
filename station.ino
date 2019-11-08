#include <PID_v1.h>
//#include "timer-api.h"
#include <iarduino_OLED.h>

iarduino_OLED myOLED(0x3C);                                // Объявляем объект myOLED, указывая адрес дисплея на шине I2C: 0x3C или 0x3D.

extern uint8_t MediumFont[];                               // Подключаем шрифт MediumFont.

// время цикла 1 ( * 2 = 10 мс)
#define CYCLE_1_TIME 10
//Максимальная температура
#define TEMP_MAX 500


// Оптопара перехода нуля к аппаратному прерыванию
// подсчитывать количество полуволн и включать их


//нужно сделать 2 функции on & off одна будет включать нагрев фена другая выключать
//но при этом он должен продуваться и после остывания отключиться
//также нужно сделать определения подключения фена в разьем
//при разогреве сверх ххх температуры отключить нагрев, сообщить о неисправности
class Thermofan {
    // Пин термопары(операционного усилителя)
    int thermocouplePin = A0;
    // Значение температуры нагрева
    int thermocoupleValue = 0;

    // Пин потенциометра температуры нагрева
    int potentiometerPin = A1;
    // Пин оптопары нагрева фена
    int optronPin = 9;

    // Задержка для показа выставляемой температуры
    int outValuePotentiometer = 0;
    // Предыдущее значение выставленной температуры
    int oldTemperature = 0;
    // Таймер вывода значений температуры на дисплей
    byte timerEcho = 0;

    // Пин геркона
    int hermeticContactPin = 10;
    // Значение геркона
    bool hermeticContactState = false;

    //включен ли нагрев фена
    bool warmingFan = true;
    //пин кнопки включения нагрева фена
    int warmingButton = 4;

    //индикатор нагрева фена
    int warmingLed = 13;

    // Для пид регулировки
    double Input , Setpoint, Output;
    // Оптимальные значения 0.5 0 0.7
    // Как настраивать http://full-chip.net/arduino-proekty/110-pid-regulyator-dlya-nagrevaohlazhdeniya.html
    PID* fanpid;

    //функция оверсэмплинга
    unsigned long int getOversampled(int pin) {
      unsigned long int result = 0;
      for (byte z = 0; z < 64; z++) { //делаем 64 замера
        result += analogRead(pin); //складываем всё вместе
      }
      //делаем побитовый сдвиг для полученного значения (64 это 2 в 6-ой степени, поэтому »6)
      return result >> 6;
    }

    void getTenTemperature() {
      this->thermocoupleValue =  abs(int(206.36 * this->getOversampled(this->thermocouplePin) * (5.0 / 1023.0) - 13.263));
      this->Input = this->thermocoupleValue;
    }

    //чтение заданой температуры с потенциометра
    int readValueTemperature() {
      return map(analogRead(this->potentiometerPin), 0, 1024, 0, TEMP_MAX) ;
    }

    //чтение значения с потенциометра
    //todo: нужно переделать под разные потенциометры их может быть несколько
    void readPotentiometr() {
      this->Setpoint = this->readValueTemperature();
      //     if (abs(this->Setpoint - this->oldTemperature) > 5) {
      //      this->outValuePotentiometer = 100;
      //    }
      this->oldTemperature = this->Setpoint;
    }

    void readhermeticContactState() {
      this->hermeticContactState = !digitalRead(this->hermeticContactPin);
    }

    //нагрев фена
    void warming() {
      //если температура задана ниже 20 то не будем ничего нагревать
      //или если фен лежит на подставке
      //или если нагрев выключен

      //todo: нужно сделать пин для реле и нормальную логику
      this->echoDisplay(this->hermeticContactState, 2);
      if (this->Setpoint < 20 || this->thermocoupleValue > 500 || this->hermeticContactState) {
        digitalWrite(4, LOW);
      } else {
        digitalWrite(4, HIGH);
      }
      if (this->Setpoint < 20 || this->hermeticContactState  || !this->warmingFan) {
        digitalWrite(this->warmingLed, LOW);
        analogWrite(this->optronPin, LOW);
      } else {
        // Делаем расчет значения для пид
        this->fanpid->Compute();
        //нагрев тена
        digitalWrite(this->warmingLed, HIGH);
        analogWrite(this->optronPin, this->Output);
      }
    }

    //вывод температуры
    void echo () {
      if ( this->timerEcho >= CYCLE_1_TIME ) {
        this->timerEcho = 0;
        this->echoDisplay(this->thermocoupleValue, 0);
        this->echoDisplay(this->Setpoint, 1);
      }
      this->timerEcho++;
    }

    //сам вывод на дисплей или куда надо
    void echoDisplay(int i) {
      //      Serial.write( 0xff );
      //      Serial.write( (i >> 8) & 0xff );
      //      Serial.write( i & 0xff );
      //      Serial.println(i, DEC);
      myOLED.setCursor(0, 17);
      myOLED.print(i);
      return;
    }

    //сам вывод на дисплей или куда надо
    void echoDisplay(int i, int str) {
      //      Serial.write( 0xff );
      //      Serial.write( (i >> 8) & 0xff );
      //      Serial.write( i & 0xff );
      //      Serial.println(i, DEC);
      myOLED.setCursor(0, (str + 1) * 17);
      myOLED.print(i);
      myOLED.print(" ");
      return;
    }

  public:
    //устанавливаем все значения для термофена
    Thermofan() {
      this->fanpid = new PID(&this->Input, &this->Output, &this->Setpoint, 0.5, 0, 0.7, DIRECT);
      pinMode(optronPin, OUTPUT);
      pinMode(this->warmingLed, OUTPUT);
      pinMode(4, OUTPUT);
      pinMode(this->hermeticContactPin, INPUT);
      this->fanpid->SetMode(AUTOMATIC);
      this->fanpid->SetSampleTime(80);
    }

    void loopth() {
      // Считыываем состояние геркона
      this->readhermeticContactState();
      // Считываем с пина значение температуры
      this->getTenTemperature();
      // Считыаем значение с потенциометра
      this->readPotentiometr();
      // Вывод значения
      this->echo();
      // Нагрев фена
      this->warming();
    }
};

Thermofan* thermofan1;

void setup() {
  myOLED.begin();                                        // Инициируем работу с дисплеем.
  myOLED.setFont(MediumFont);
  thermofan1 =  new Thermofan();
  Serial.begin(9600);
}

void loop() {
  thermofan1->loopth();
}
