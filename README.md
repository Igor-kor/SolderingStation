# Паяльная станция на arduino uno
 Основные компоненты:
- Arduino uno
- Операционный усилитель lm358p
- Симистор bt139
- Отопара moc3021
- Источник питаниям 5 вольт и 24 вольта
- OLED дисплей I2C SSD1306
- Энкодер 
- irfz44n для управления скоростью потока воздуха
- ltv814 (2) оптопара для определения перехода через 0, и (для этого можно заменить на ltv817) для включения irfz44n

Используемые библиотеки:
- для дисплея https://github.com/olikraus/u8g2
- библиотека пид регулятора https://playground.arduino.cc/Code/PIDLibrary/
- ускоренное ядро ардуино https://github.com/AlexGyver/GyverCore

Полезная информация
- по настройке коэфициентов пид регулятора http://lazysmart.ru/osnovy-avtomatiki/nastrojka-pid-regulyatora/
- дискретный регулятор мощности (теория, как в итоге должно работать) https://www.qrz.ru/schemes/contribute/digest/bp169.shtml

Прототип
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/foto1.jpg)
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/foto2.jpg)

В собраном виде
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/foto3.jpg)
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/foto4.jpg)

Для правильной регулировки написал программу на C# показывающую график https://github.com/Igor-kor/ArduinoDiagramm
- До регулировки
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/screenbefore.jpg)
- После регулировки
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/screenafter.jpg)

Схемы
- Определение перехода через 0
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/cheme1.jpg)
- Управление турбиной фена
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/cheme2.jpg)
- Усилитель для термопары
![скрин](https://github.com/Igor-kor/SolderingStantion/blob/master/img/cheme3.png)

