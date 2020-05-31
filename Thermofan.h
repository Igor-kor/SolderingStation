//Максимальная температура на потенциометре
#define TEMP_MAX 600
//Температура при достижении которой отключится нагрев(для аварийного охлаждения);
#define TEMP_MAX_OFF 600
//Температура после которой фен снова начнет свою работу после аварийного охлаждения
#define TEMP_MIN_ON 80
// Время вывода мс(вывод на дисплэй сильно замедляет ардуину)
#define TIME_ECHO 50

// для энкодера
#define ENC_LEFT 1
#define ENC_RIGHT 0
#define MILLIS_CHANGE_DIRECTION 100
int value = 0;
uint32_t lastTickEncoder;
bool encDirection = 0;
bool encButtonChange = 0;
#define ENC_A 2       // пин энкодера
#define ENC_B 4       // пин энкодера
#define ENC_TYPE 1    // тип энкодера, 0 или 1
volatile int encCounter = 0, encCounterFan = 100;
volatile boolean state0, lastState, turnFlag = false;

int Testloop = 0;
//iarduino_OLED myOLED(0x3C);   // Объявляем объект myOLED, указывая адрес дисплея на шине I2C: 0x3C или 0x3D.
//extern uint8_t MediumFont[];  // Подключаем шрифт MediumFont.
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
uint32_t       mil = 0; //текущие милисекунды для вывода на экран
int countzerocross = 0;
int warmcount = 10;

//также нужно сделать определения подключения фена в разьем
//при разогреве сверх ххх температуры отключить нагрев, сообщить о неисправности
class Thermofan {
    // Пин термопары(операционного усилителя)
    const int thermocouplePin = A0;
    // Значение температуры нагрева
    int thermocoupleValue = 0;
    // ОШИБКИ
    // 1 - Перегрев фена выше TEMP_MAX_OFF
    int error = 0;

    const int speedfanPin = 5;

    // Пин потенциометра температуры нагрева
    const int potentiometerPin = A1;
    // Пин оптопары нагрева фена
    const int optronPin = 9;

    // Пин геркона
    const int hermeticContactPin = 10;
    // Значение геркона
    bool hermeticContactState = false;

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
    char ostiv[10] = {'О','С','Т','Ы','В','А','Н','И','Е','\0'};
    char nagrev[10] = {'Н','А','Г','Р','Е','В',' ',' ',' ','\0'};


  public:
    //Конструктор
    Thermofan() {
      this->fanpid = new PID(&this->Input, &this->Output, &this->Setpoint, 0.4, 0.1, 0, DIRECT);
      this->fanpid->SetOutputLimits(0, 20);
      pinMode(optronPin, OUTPUT);
      pinMode(this->warmingLed, OUTPUT);
      pinMode(5, OUTPUT);
      pinMode(this->hermeticContactPin, INPUT);
      this->fanpid->SetMode(AUTOMATIC);
      // оптимально 80
      this->fanpid->SetSampleTime(80);
      //      this->fanpid->SetOutputLimits(0,250);
      u8x8.begin();
      u8x8.setPowerSave(0);
      u8x8.setFont(font_ikor);
      //u8x8.setFont(u8x8_font_chroma48medium8_r);
      attachInterrupt(1, this->attachFun, FALLING);
      attachInterrupt(0, this->attachEncoder, CHANGE);
      this->speedfan = 200;
      this->loopth();
      EEPROM.get(0, encCounter);
      EEPROM.get(2, encCounterFan);
    }

    static void attachEncoder() {
      state0 = digitalRead(ENC_A);
      int thisdirection = 0;
      if (state0 != lastState) {
        turnFlag = !turnFlag;
        thisdirection = (digitalRead(ENC_B) != lastState) ? ENC_RIGHT : ENC_LEFT;

        if (encDirection != thisdirection && (lastTickEncoder + MILLIS_CHANGE_DIRECTION) < millis()) {
          encDirection = thisdirection;
        }
        if (turnFlag) {
          if (encDirection == ENC_RIGHT ) {
            if (encButtonChange)encCounter -= 1;
            else encCounterFan -= 1;
          } else {
            if (encButtonChange) encCounter += 1;
            else encCounterFan += 1;
          }
        }
        lastTickEncoder = millis();
        lastState = state0;
        if (encCounter < 0)encCounter = 0;
        if (encCounter > 500)encCounter = 500;
        if (encCounterFan < 100)encCounterFan = 100;
        if (encCounterFan > 255)encCounterFan = 255;
      }
    }

    static void attachFun() {
      countzerocross++;
      if (countzerocross >= (20 - warmcount) && warmcount > 0) {
        digitalWrite(9, HIGH);
        digitalWrite(13, HIGH);
        countzerocross = 0;
      } else {
        digitalWrite(9, LOW);
        digitalWrite(13, LOW);
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
      this->thermocoupleValue =  abs(int(206.36 * this->getOversampled(this->thermocouplePin) * (5.0 / 1023.0) - 13.263));
      this->Input = this->thermocoupleValue;
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
      this->echoDisplay("C*", 0, 3);
      this->echoDisplay(this->Setpoint, 1);
      this->echoDisplay("C°", 1, 3);
      this->echoDisplay(map(encCounterFan, 100, 255, 0, 100), 1, 7);
      this->echoDisplay("%", 1, 10);
      this->echoDisplay(this->hermeticContactState?ostiv:nagrev, 2, 0);
      this->echoDisplay(warmcount, 0, 7);
      //this->echoDisplay(this->Output, 0, 9);
    } 

    //сам вывод на дисплей или куда надо
    void echoDisplay(int i) {
      char strt[11];
      sprintf(strt, "%d  ", i);
      u8x8.drawString(0, 0, strt);
      return;
    }

    void echoDisplay(int i, int str) {
      char strt[11];
      sprintf(strt, "%d  ", i);
      u8x8.drawString(0, str, strt);
      return;
    }

    void echoDisplay(int i, int str, int x) {
      char strt[11];
      sprintf(strt, "%d  ", i);
      u8x8.drawString(x, str, strt);
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
          mil = millis();
          //после вывода снова включаем нагрев
          this->warmingFan = true;
        } else {
          this->echo();
          mil = millis();
        }
      }
      // Нагрев фена
      this->warming();
      Testloop++;
      //скорость вращения фена
      if (this->error == 0) {
        if ( this->Input < 15 && this->Setpoint < 10) {
          this->speedfan = 0;
        } else {
          this->speedfan = encCounterFan;
        }
      }
      analogWrite(this->speedfanPin, this->speedfan);
    }

};
