# Моя DIY паяльная станция (тестовый рабочий вариант)

Необходимо переделать управление потоком фоздуха, пока задается в скетче скорость.

 Основные компоненты:
- Arduino uno
- Операционный усилитель lm358p
- Симистор bt139
- Отопара moc3021
- Источник питаниям 5 вольт и 24 вольта
- OLED дисплей I2C SSD1306
- 10КОм потенциометры 
- irfz44n для управления скоростью потока воздуха
- ltv814 (2) оптопара для определения перехода через 0, и (для этого можно заменить на ltv817) для включения irfz44n

Используемые библиотеки:
- для дисплея iarduino_OLED https://iarduino.ru/file/340.html
- библиотека пид регулятора https://playground.arduino.cc/Code/PIDLibrary/
- ускоренное ядро ардуино https://github.com/AlexGyver/GyverCore

Полезная информация
- по дисплею https://wiki.iarduino.ru/page/OLED_trema/
- по настройке коэфициентов пид регулятора http://lazysmart.ru/osnovy-avtomatiki/nastrojka-pid-regulyatora/
- дискретный регулятор мощности (теория, как в итоге должно работать) https://www.qrz.ru/schemes/contribute/digest/bp169.shtml


