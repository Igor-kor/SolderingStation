//Максимальная температура на потенциометре
#define TEMP_MAX 600
//Температура при достижении которой отключится нагрев(для аварийного охлаждения);
#define TEMP_MAX_OFF 500
//Температура после которой фен снова начнет свою работу после аварийного охлаждения
#define TEMP_MIN_ON 50
// Время вывода мс(вывод на дисплэй сильно замедляет ардуину)
#define TIME_ECHO 1000
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
int Testloop = 0;
//iarduino_OLED myOLED(0x3C);   // Объявляем объект myOLED, указывая адрес дисплея на шине I2C: 0x3C или 0x3D.
extern uint8_t MediumFont[];  // Подключаем шрифт MediumFont.
uint32_t       mil = 0;

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

    // Пин потенциометра температуры нагрева
    const int potentiometerPin = A1;
    // Пин оптопары нагрева фена
    const int optronPin = 9;

    // Пин геркона
    const int hermeticContactPin = 10;
    // Значение геркона
    bool hermeticContactState = false;

    //включен ли нагрев фена
    bool warmingFan = true;
    //пин кнопки включения нагрева фена todo: не используется сейчас
    //    const int warmingButton = 4;

    //пин индикатора нагрева фена todo: теперь не нужен, есть дисплэй
    const int warmingLed = 13;

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
      // оптимально 80
      this->fanpid->SetSampleTime(80);
      //      this->fanpid->SetOutputLimits(0,250);
    //  myOLED.begin(); // Инициируем работу с дисплеем.
    //  myOLED.setFont(MediumFont);
      //отключаем автообновление, так будет быстрее вывод всего экрана
   //   myOLED.autoUpdate(false);
      //      myOLED.invText();
       // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
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

    // получение температуры с термопары фена
    void getTenTemperature() {
      this->thermocoupleValue =  abs(int(206.36 * this->getOversampled(this->thermocouplePin) * (5.0 / 1023.0) - 13.263));
      this->Input = this->thermocoupleValue;
    }

    //чтение значения с потенциометра температуры
    void readPotentiometr() {
      this->Setpoint = map(this->getOversampled(this->potentiometerPin), 0, 1024, 0, TEMP_MAX) ;
    }

    void readhermeticContactState() {
      this->hermeticContactState = !getOversampledDigital(this->hermeticContactPin);
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
      }
      if (this->error == 1) {
        if (this->warmingFan == false && this->Input < TEMP_MIN_ON) {
          this->warmingFan = true;
          this->error = 0;
        }
      }
      if (this->Setpoint < 20 || this->hermeticContactState  || !this->warmingFan ) {
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

    //вывод
   
    void loopth() {
      // Считыываем состояние геркона
      this->readhermeticContactState();
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
//          this->echo();
          mil = millis();
          //после вывода снова включаем нагрев
          this->warmingFan = true;
        } else {
//          this->echo();
          mil = millis();
        }
          display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  char str[11];
  sprintf(str, "%d", Testloop);
  display.write(str[0]);
  display.write(str[1]);
  display.write(str[2]);
  display.write(str[3]);
  display.write(str[4]);
  display.write(str[5]);
  display.write(str[6]);
  display.write(str[7]);
  display.write(str[8]);
  display.write(str[9]);
  display.write(str[10]);
  display.display();
      }
      // Нагрев фена
      this->warming();
      Testloop++;
    }

};
