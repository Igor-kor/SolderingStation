#include <PID_v1.h>
#define CYCLE_1_TIME 10    // время цикла 1 ( * 2 = 10 мс)

class Thermofan {
    // Пин термопары(операционного усилителя)
    int thermocouplePin = A0;
    // Пин потенциометра температуры нагрева
    int potentiometerPin = A1;
    // Значение температуры нагрева
    int thermocoupleValue = 0;
    // Пин оптопары нагрева фена
    int optronPin = 9;
    // Задержка для показа выставляемой температуры
    int outValuePotentiometer = 0;
    // Предыдущее значение выставленной температуры
    int oldTemperature = 0;
    // Пин геркона
    int hermeticContactPin = 10;
    // Значение геркона
    bool hermeticContactState = false;
    // Таймер вывода значений температуры на дисплей
    byte timerEcho = 0;
    // Для пид регулировки
    double Input , Setpoint, Output;
    // Оптимальные значения 0.5 0 0.7
    // Как настраивать http://full-chip.net/arduino-proekty/110-pid-regulyator-dlya-nagrevaohlazhdeniya.html
    PID* fanpid;

    //функция оверсэмплинга
    void getOversampled() {
      unsigned long int result = 0;
      for (byte z = 0; z < 64; z++) { //делаем 64 замера
        result += analogRead(this->thermocouplePin); //складываем всё вместе
      }
      //делаем побитовый сдвиг для полученного значения (64 это 2 в 6-ой степени, поэтому »6)
      this->thermocoupleValue =  abs(int(206.36 * (result >> 6) * (5.0 / 1023.0) - 13.263));
      this->Input = this->thermocoupleValue;
    }

    int readValueTemperature(){
      return 50;
      return analogRead(this->potentiometerPin) / 2;
    }

    void readPotentiometr() {
       this->Setpoint = this->readValueTemperature();
      if (abs(this->Setpoint - this->oldTemperature) > 5) {
        this->outValuePotentiometer = 100;
      }
      this->oldTemperature = this->Setpoint;
    }

    void warming() {
      //если температура задана ниже 20 то не будем ничего нагревать
      if (this->Setpoint < 20 || this->hermeticContactState) {
        // Делаем расчет значения для пид
        this->fanpid->Compute();
        analogWrite(this->optronPin, LOW);
        digitalWrite(13, LOW);
        return;
      }
      //нагрев тена
      digitalWrite(13, HIGH);
      analogWrite(this->optronPin, Output);
    }

    void echo () {
      if ( this->timerEcho >= CYCLE_1_TIME ) {
        this->timerEcho = 0;
        if (this->outValuePotentiometer < 1) {
          this->echoDisplay(this->thermocoupleValue);
        } else {
          this->outValuePotentiometer--;
          this->echoDisplay(this->Setpoint);
        }
      }
      this->timerEcho++;
    }

    void echoDisplay(int) {
      return;
    }

  public:

    Thermofan() {
      this->fanpid = new PID(&this->Input, &this->Output, &this->Setpoint, 0.5, 0, 0.7, DIRECT);
      pinMode(optronPin, OUTPUT);
      pinMode(13, OUTPUT);
      pinMode(hermeticContactPin, INPUT);
      this->fanpid->SetMode(AUTOMATIC);
      this->fanpid->SetSampleTime(80);
    }

    void loop() {
      // Считыываем состояние геркона
      this->hermeticContactState = !digitalRead(this->hermeticContactPin);
      // Считываем с пина значение температуры
      this->getOversampled();
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
  thermofan1 =  new Thermofan();
  Serial.begin(9600);
}

void loop() {
  thermofan1->loop();
}
