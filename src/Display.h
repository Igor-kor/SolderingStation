#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>

// OLED SSD1306 128x32, I2C, текстовый режим u8x8: 16 колонок x 4 строки
class Display {
  public:
    void begin();
    void showStateLabel(const char* label);            // строка 2
    void clearPidLine();                               // строка 3
    void drawPid(const char* key, const char* value);  // строка 3
    void updateRow1(int setpoint, int fanTarget, int fanCurrent);
    void update(int currentTemp, int heaterOutput);

  private:
    void drawText(const char* s, uint8_t row, uint8_t col);
    void drawInt(int value, uint8_t row, uint8_t col);
    unsigned long lastUpdate = 0;
};

extern Display display;

#endif
