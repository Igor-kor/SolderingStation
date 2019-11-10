#include <iarduino_OLED.h>

//Максимальная температура на потенциометре
#define TEMP_MAX 500
//Температура при достижении которой отключится нагрев(для аварийного охлаждения);
#define TEMP_MAX_OFF 600
//Температура после которой фен снова начнет свою работу после аварийного охлаждения
#define TEMP_MIN_ON 50
// Время вывода мс(вывод на дисплэй сильно замедляет ардуину)
#define TIME_ECHO 1000

int Testloop = 0;
iarduino_OLED myOLED(0x3C);                                // Объявляем объект myOLED, указывая адрес дисплея на шине I2C: 0x3C или 0x3D.
extern uint8_t MediumFont[];                               // Подключаем шрифт MediumFont.
uint32_t       mil = 0;

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

  public:
    //Конструктор
    Thermofan() {
      this->fanpid = new PID(&this->Input, &this->Output, &this->Setpoint, 0.5, 0, 0.7, DIRECT);
      pinMode(optronPin, OUTPUT);
      pinMode(this->warmingLed, OUTPUT);
      pinMode(4, OUTPUT);
      pinMode(this->hermeticContactPin, INPUT);
      this->fanpid->SetMode(AUTOMATIC);
      this->fanpid->SetSampleTime(40);
      //      this->fanpid->SetOutputLimits(0,250);
      myOLED.begin(); // Инициируем работу с дисплеем.
      myOLED.setFont(MediumFont);
    }

    //функция оверсэмплинга
    unsigned long int getOversampled(int pin) {
      unsigned long int result = 0;
      for (byte z = 0; z < 64; z++) { //делаем 64 замера
        result += analogRead(pin); //складываем всё вместе
      }
      //делаем побитовый сдвиг для полученного значения (64 это 2 в 6-ой степени, поэтому »6)
      return result >> 6;
    }

    //функция оверсэмплинга для геркона, (не сильно много тут нужно)
    bool getOversampledDigital(int pin) {
      unsigned long int result = 0;
      for (byte z = 0; z < 8; z++) { //делаем 8 замера
        result += digitalRead(pin); //складываем всё вместе
      }
      //делаем побитовый сдвиг для полученного значения (8 это 2 в 3-ой степени, поэтому »3)
      return (result >> 3) > 0;
    }

    // получение температуры с термопары фена
    void getTenTemperature() {
      this->thermocoupleValue =  abs(int(206.36 * this->getOversampled(this->thermocouplePin) * (5.0 / 1023.0) - 13.263));
      this->Input = this->thermocoupleValue;
      if (this->Input > 600)this->warmingFan = false;
      if (this->warmingFan == false && this->Input < TEMP_MIN_ON)this->warmingFan = true;
    }

    //чтение значения с потенциометра температуры
    void readPotentiometr() {
      this->Setpoint = map(this->getOversampled(this->potentiometerPin), 0, 1024, 0, TEMP_MAX) ;
      this->oldTemperature = this->Setpoint;
    }

    void readhermeticContactState() {
      this->hermeticContactState = !getOversampledDigital(this->hermeticContactPin);
    }

    //нагрев фена
    void warming() {
      //если температура задана ниже 20 то не будем ничего нагревать
      //или если фен лежит на подставке
      //или если нагрев выключен


      if (this->Setpoint < 20 || this->hermeticContactState  || !this->warmingFan) {
        digitalWrite(this->warmingLed, LOW);
        analogWrite(this->optronPin, LOW);
        //        this->echoDisplay("holding", 1);
      } else {
        // Делаем расчет значения для пид
        this->fanpid->Compute();
        //нагрев тена
        digitalWrite(this->warmingLed, HIGH);
        analogWrite(this->optronPin, this->Output);
        //        this->echoDisplay("warming", 1);
      }
    }

    //вывод температуры
    void echo () {
      this->echoDisplay(this->Input, 0);
      this->echoDisplay(this->Setpoint, 1);
      this->echoDisplay(this->hermeticContactState, 2);
      this->echoDisplay(this->Output, 0, 50);
          this->echoDisplay(Testloop++, 2, 40);
    }

    //сам вывод на дисплей или куда надо
    void echoDisplay(int i) {
      myOLED.setCursor(0, 17);
      myOLED.print(i);
      return;
    }

    //сам вывод на дисплей или куда надо
    void echoDisplay(int i, int str) {
      myOLED.setCursor(0, (str + 1) * 17);
      myOLED.print(i);
      myOLED.print(" ");
      return;
    }

    void echoDisplay(int i, int str, int x) {
      myOLED.setCursor(x, (str + 1) * 17);
      myOLED.print(i);
      myOLED.print(" ");
      return;
    }

    //сам вывод на дисплей или куда надо
    void echoDisplay(char* i, int str) {
      myOLED.setCursor(40, (str + 1) * 17);
      myOLED.print(i);
      return;
    }

    void loopth() {
      // Считыываем состояние геркона
      this->readhermeticContactState();
      // Считываем с пина значение температуры
      this->getTenTemperature();
      // Считыаем значение с потенциометра
      this->readPotentiometr();
      // Вывод значения
      //      this->echo();
      //todo: пока будет костыль
      if ( (millis() - mil) > TIME_ECHO) {
        this->echo();
        mil = millis();
      }
      // Нагрев фена
      this->warming();
      Testloop++;
    }

};
