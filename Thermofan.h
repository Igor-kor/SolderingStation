#include "ThermofanDef.h"

int value = 0;
uint32_t lastTickEncoder;
bool encDirection = 0;
bool encButtonChange = 0;

volatile int encCounter = 0, encCounterFan = 100;
volatile boolean state0, lastState, turnFlag = false;

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
uint32_t mil = 0; //текущие милисекунды для вывода на экран
int countzerocross = 0;
int warmcount = 10;
bool statewarm = false;
bool oldstatewarm = false;
//также нужно сделать определения подключения фена в разьем
//при разогреве сверх ххх температуры отключить нагрев, сообщить о неисправности
class Thermofan {
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
    //пин кнопки включения нагрева фена todo: не используется сейчас
    //    const int warmingButton = 4;

    //пин индикатора нагрева фена todo: теперь не нужен, есть дисплэй
    const int warmingLed = 13;

    // Для пид регулировки
    double Input , Setpoint, Output;
    // Оптимальные значения 0.5 0 0.7
    PID* fanpid;

    int speedfan;

    // Строки для вывода, если не делать массив то между буквами кирилицы будут пробелы
    char ostiv[10] = {'О', 'С', 'Т', 'Ы', 'В', 'А', 'Н', 'И', 'Е', '\0'};
    char nagrev[10] = {'Н', 'А', 'Г', 'Р', 'Е', 'В', ' ', ' ', ' ', '\0'};


  public:
    //Конструктор
    Thermofan() {
      this->fanpid = new PID(&this->Input, &this->Output, &this->Setpoint, 1, 0.08, 0.3, DIRECT);
      this->fanpid->SetOutputLimits(0, WARMTICK);
      pinMode(OPTRON_PIN, OUTPUT);
      pinMode(this->warmingLed, OUTPUT);
      pinMode(5, OUTPUT);
      pinMode(this->hermeticContactPin, INPUT);
      this->fanpid->SetMode(AUTOMATIC);
      // оптимально 80
      this->fanpid->SetSampleTime(1); 
      u8x8.begin();
      u8x8.setPowerSave(0);
      u8x8.setFont(font_ikor);
      attachInterrupt(1, this->attachFun, FALLING);
      attachInterrupt(0, this->attachEncoder, CHANGE);
      this->speedfan = 200;
      this->loopth();
      EEPROM.get(0, encCounter);
      EEPROM.get(2, encCounterFan);

      this->echoDisplay("C*", 0, 3);
      this->echoDisplay("C*", 1, 3);
      this->echoDisplay("%", 1, 10);
    }

    static void attachEncoder() {
      state0 = digitalRead(ENC_A);
      int thisdirection = 0;
      int tickEncoder = millis();
      int iterCount = 1;
      if(tickEncoder - lastTickEncoder >10) {
        iterCount = 1;
      }
      if(tickEncoder - lastTickEncoder <10 && !turnFlag) {
        iterCount = (12- (tickEncoder - lastTickEncoder))/2;
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
        }
        lastTickEncoder = tickEncoder;
        lastState = state0;
        if (encCounter < 0)encCounter = 0;
        if (encCounter > 500)encCounter = 500;
        if (encCounterFan < 100)encCounterFan = 100;
        if (encCounterFan > 255)encCounterFan = 255;
      }
    }

    static void attachFun() {
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

    //функция оверсэмплинга
    bool getOversampledDigitalLow(int pin) {
      unsigned long int result = 0;
      for (byte z = 0; z < 8; z++) { //делаем 8 замера
        result += !digitalRead(pin); //складываем всё вместе
      }
      //делаем побитовый сдвиг для полученного значения (8 это 2 в 3-ой степени, поэтому »3)
      return (result >> 3) > 0;
    }

    // получение температуры с термопары фена
    void getTenTemperature() {

      this->thermocoupleValue =  correction(abs(int(206.36 * this->getOversampled(THERMOCOUPLE_PIN) * (5.0 / 1023.0) - 13.263)));
      this->Input = this->thermocoupleValue;
    }

    // корректировка
    int correction(int x) {
      // return  x-int(0.4266*x-11.4621);
      return x - (0.2087 * x - 23.2);
    }

    //чтение значения установленной температуры
    void readPotentiometr() {
      this->Setpoint = encCounter;
    }

    void readhermeticContactState() {
      this->hermeticContactState = !getOversampledDigital(this->hermeticContactPin);
    }

    void saveButton() {
      if (!getOversampledDigital(7)) {
        // будет сохранятся установленные значения
        EEPROM.put(0, encCounter);
        EEPROM.put(2, encCounterFan);
      }
    }
    void readEncoderButtonState() {
      if (!getOversampledDigitalLow(8)) {
        if (this->changeEncoderButton) {
          encButtonChange = !encButtonChange;
        }
        this->changeEncoderButton = false;
      } else {
        this->changeEncoderButton = true;
      }
    }

    //нагрев фена
    void warming() {
      //если температура задана ниже 20 то не будем ничего нагревать
      //или если фен лежит на подставке
      //или если нагрев выключен
      //или если достигнут максимум нагрева
      if (this->Input > TEMP_MAX_OFF) {
        this->warmingFan = false;
        this->error = 1;
        this->speedfan = 255;
      }
      if (this->error == 1) {
        //в случае перегрева если не работает оптопара вызовим сами
        this->attachFun();
        if (this->warmingFan == false && this->Input < TEMP_MIN_ON) {
          this->warmingFan = true;
          this->error = 0;
        }
      }
      if (this->Setpoint < 20 || this->hermeticContactState  || !this->warmingFan ) {
        warmcount = 0;
      } else {
        // Делаем расчет значения для пид
        this->fanpid->Compute();
        //нагрев тена
        warmcount = this->Output;
      }
    }

    //вывод
    void echo () {
      this->echoDisplay(this->Input, 0);
      this->echoDisplay(this->Setpoint, 1);
      this->echoDisplay(map(encCounterFan, 100, 255, 0, 100), 1, 7); 
      if(this->hermeticContactState != this->oldhermeticContactState){
        this->echoDisplay(this->hermeticContactState ? ostiv : nagrev, 2, 0);
        this->oldhermeticContactState = this->hermeticContactState;
      }
      this->echoDisplay(warmcount, 0, 7);
      this->echoDisplay(this->Output, 0, 9);
      //Serial.println(this->getOversampled(THERMOCOUPLE_PIN));
    }

    //сам вывод на дисплей или куда надо
    void echoDisplay(int i) {
      u8x8.drawString(0, 0,  String(i).c_str());
      return;
    }

    void echoDisplay(int i, int str) {
      u8x8.drawString(0, str,  String(i).c_str());
      return;
    }

    void echoDisplay(int i, int str, int x) {
      u8x8.drawString(x, str, String(i).c_str());
      return;
    }

    void echoDisplay(char* i, int str) {
      u8x8.drawString(0, str, i);
      return;
    }

    void echoDisplay(char* i, int str, int x) {
      u8x8.drawString(x, str, i);
      return;
    }

    void loopth() {
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
      // Вывод значения todo: пока будет костыль
      if ( (millis() - mil) > TIME_ECHO) {
        //Так как нагрев может быть выключен то нужно проверять прежде чем изменять значение
        if (this->warmingFan) {
          //отключаем нагрев во время вывода на дисплэй
          this->warmingFan = false;
          //чтобы он отключился нужно выполнить функцию нагрева, она уберет питание с пина
          this->warming();
          this->echo(); 
          //после вывода снова включаем нагрев
          this->warmingFan = true;
        } else {
          this->echo(); 
        }
        mil = millis();
      }
      // Нагрев фена
      this->warming();
      //скорость вращения фена
      if (this->error == 0) {
        if ( this->Input < 15 && this->Setpoint < 10) {
          this->speedfan = 0;
        } else {
          this->speedfan = encCounterFan;
        }
      }
      analogWrite(SPEED_FAN_PIN, this->speedfan);
    }

};
