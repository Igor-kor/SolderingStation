#include "Display.h"
#include "Config.h"
#include "u8x8_font_ikor.h"

Display display;

// Объект u8x8 живёт только здесь: остальные модули работают через Display
static U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void Display::begin() {
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_ikor);
  // статические метки
  drawText("C*", 0, 3);
  drawText("C*", 1, 3);
  drawText("%", 1, 13);
}

void Display::drawText(const char* s, uint8_t row, uint8_t col) {
  u8x8.drawString(col, row, s);
}

void Display::drawInt(int value, uint8_t row, uint8_t col) {
  char buf[8];
  itoa(value, buf, 10);
  u8x8.drawString(col, row, buf);
}

void Display::showStateLabel(PGM_P label) {
  u8x8.clearLine(2);
  // строка лежит во flash — копируем в буфер для вывода
  char buf[17];
  strncpy_P(buf, label, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';
  drawText(buf, 2, 0);
}

void Display::clearPidLine() {
  u8x8.clearLine(3);
}

void Display::drawPid(const char* key, const char* value) {
  u8x8.clearLine(3);
  drawText(value, 3, 0);
  drawText(key, 3, 7);
}

// Строка 1: уставка температуры, уставка и текущая скорость вентилятора
void Display::updateRow1(int setpoint, int fanTarget, int fanCurrent) {
  u8x8.clearLine(1);
  drawInt(setpoint, 1, 0);
  drawInt(fanTarget, 1, 7);
  drawInt(fanCurrent, 1, 10);
  drawText("%", 1, 13);
}

// Строки с текущими значениями — не чаще TIME_ECHO:
// постоянная перерисовка через I2C тормозит главный цикл
void Display::update(int currentTemp, int heaterOutput) {
  if (millis() - lastUpdate < TIME_ECHO) return;
  lastUpdate = millis();
  drawInt(currentTemp, 0, 0);
  // затираем старшие цифры при уменьшении разрядности
  if (currentTemp < 100) drawText(" ", 0, 2);
  if (currentTemp < 10) drawText(" ", 0, 1);
  drawInt(heaterOutput, 0, 9);
  if (heaterOutput < 100) drawText(" ", 0, 11);
  if (heaterOutput < 10) drawText(" ", 0, 10);
}
