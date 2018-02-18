#include <PID_v1.h>

//пин с термопары фена
int sensorPin = A0; // select the input pin for the potentiometer
//потенциометр для температуры фена
int tempPoten = A1;
//значение температуры с термопары фена
int sensorValue = 0; // variable to store the value coming from the sensor
//пин оптрона фена
int hotAirTen = 9;
//вывод заданной температуры фена
int echoPoten = 0;
//предыдущая выставленная температура фена
int oldpoten = 0;

//для индикатора
const byte latchPin = 6;
const byte clockPin = 7;
const byte dataPin  = 5;

//пид регулировка нагрева
double Setpoint, Input, Output;
//оптимальные значения 0.5 0 0.7
//как настраивать http://full-chip.net/arduino-proekty/110-pid-regulyator-dlya-nagrevaohlazhdeniya.html
PID fanpid(&Input, &Output, &Setpoint,0.5,0,0.7, DIRECT);

//количество индикаторов
const int  countIndicator = 3;
//цифры для вывода на индикаторах
byte arrayNumber[] = {0b11000000, 0b11111001, 0b10100100,
                      0b10110000, 0b10011001, 0b10010010,
                      0b10000010, 0b11111000, 0b10000000,
                      0b10010000
                     };

//вывод значений на индикаторе
void echoNumber(int number, int indicatorCount = 1, int point = 0) {
  Serial.println(number);
  digitalWrite(latchPin, LOW);
  for (int i = indicatorCount; i > 0; i--) {
    byte pointByte = 0b0;
    if (point > 0 && (point  == (indicatorCount - i))) {
      pointByte = 0b10000000;
    }
    shiftOut(dataPin, clockPin, MSBFIRST, arrayNumber[number % 10] - pointByte);
    number /= 10;
  }
  digitalWrite(latchPin, HIGH);
}

//подсчет разрядов для чисел флоат
int countRank(float number) {
  int intNumber = number;
  int rank = 0;
  while (intNumber % 10 > 0 || intNumber / 10 > 0) {
    intNumber /= 10;
    rank++;
  }
  return rank;
}

//вывод значения с точкой
//void echoNumber(float number, int indicatorCount = 1) {
//  int point = indicatorCount - countRank(number);
//  echoNumber((int)((float)pow(10, point) * number), indicatorCount, point);
//}

//функция оверсэмплинга
uint16_t getOversampled() {
unsigned long int result = 0;
for (byte z = 0; z < 64; z++) { //делаем 64 замера
result += analogRead(sensorPin); //складываем всё вместе
}
return result >> 6; //делаем побитовый сдвиг для полученного значения (64 это 2 в 6-ой степени, поэтому »6)
}

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(hotAirTen, OUTPUT);
  fanpid.SetMode(AUTOMATIC);
  fanpid.SetSampleTime(80);
  Serial.begin(9600);
}

#define CYCLE_1_TIME 10    // время цикла 1 ( * 2 = 10 мс)

byte  timerCount1 = 0;    // счетчик таймера 1

void loop() {
  //считываем с пина значение температуры
  sensorValue = getOversampled();
  //расчет температуры с термопары К типа
  int tcel = abs(int(206.36*sensorValue*(5.0 / 1023.0) -13.263));
  //считыаем значение с потенциометра, делим на 2 чтобы было до 512 градусов
//  int tpoten = analogRead(tempPoten)/2;
    int tpoten = 700;
  //устанавливаем считанные значения в класс пид регулятора
  Setpoint = tpoten;
  Input = tcel;
  //делаем расчет значения
  fanpid.Compute();

  if ( timerCount1 >= CYCLE_1_TIME ) {
    timerCount1= 0; 
    // код программы вызывается каждые 10 мс
    //вывод значения с потенциометра если мы его изменили
  if(abs(tpoten - oldpoten)>5){
    echoPoten = 100;
  }
  oldpoten = tpoten;
  if(echoPoten < 1){
    echoNumber(tcel, countIndicator);
  }else{
    echoPoten--;
    echoNumber(tpoten, countIndicator);
  }
  }
  timerCount1++;

  //если температура задана ниже 20 то не будем ничего нагревать
  if(tpoten < 20){
     analogWrite(hotAirTen,LOW);
    return;
  }
  //нагрев тена 
  analogWrite(hotAirTen, Output);
}
