#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <U8g2lib.h>

// OLED SSD1306 128x32 (I2C), полноэкранный буфер u8g2.
// Главный экран: верхняя строка (состояние, уставка с подчёркиванием,
// мини-шкала мощности, цель вентилятора), текущая температура крупно
// по центру. Экран настроек: коэффициент ПИД крупно по центру.
class Display {
  public:
    void begin();
    void setStateLabel(PGM_P label);                    // UTF-8 строка во flash
    void showPid(const char* key, const char* value);   // экран настроек
    void update(int currentTemp, int setpoint, int fanTarget, int heaterOutput);

  private:
    void redraw();
    void drawMainFrame();
    void drawPidFrame();

    enum Screen { SCREEN_MAIN, SCREEN_PID };
    Screen screen = SCREEN_MAIN;
    char stateLabel[24] = "";      // текущая подпись состояния (UTF-8)
    char pidKey[2] = "P";
    char pidValue[12] = "";
    // последние выведенные значения: перерисовка только при изменении
    int dTemp = -1, dSetpoint = -1, dFan = -1, dBarW = -1;
    unsigned long lastDraw = 0;
};

extern Display display;

#endif
