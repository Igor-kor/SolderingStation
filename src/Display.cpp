#include "Display.h"
#include "Config.h"

Display display;

// Объект u8g2 живёт только здесь: остальные модули работают через Display.
// Полноэкранный буфер 128x32 = 512 байт ОЗУ
static U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// мини-шкала мощности в середине верхней строки
static const uint8_t POWER_BAR_X = 50;
static const uint8_t POWER_BAR_W = 28;

void Display::begin() {
  // SSD1306 поддерживает 400 кГц: полная перерисовка ~13 мс вместо ~50
  u8g2.setBusClock(400000);
  u8g2.begin();
}

void Display::drawMainFrame() {
  // Верхняя строка: слева состояние (подпись UTF-8)
  u8g2.setFont(u8g2_font_5x8_t_cyrillic);
  u8g2.drawUTF8(0, 8, stateLabel);
  // Справа: уставка температуры (подчёркнута) и цель вентилятора
  char sbuf[8], fbuf[8];
  itoa(dSetpoint, sbuf, 10);
  itoa(dFan, fbuf, 10);
  strcat(fbuf, "%");
  uint8_t fanW = u8g2.getStrWidth(fbuf);
  uint8_t setW = u8g2.getStrWidth(sbuf);
  uint8_t fanX = 127 - fanW;
  uint8_t setX = fanX - 4 - setW;
  u8g2.drawStr(setX, 8, sbuf);
  u8g2.drawHLine(setX, 9, setW);       // подчёркивание: это уставка
  u8g2.drawStr(fanX, 8, fbuf);
  // В середине строки — мини-шкала мощности нагревателя (выход ПИД)
  u8g2.drawFrame(POWER_BAR_X, 3, POWER_BAR_W, 4);
  u8g2.drawBox(POWER_BAR_X + 1, 4, dBarW, 2);
  // Текущая температура — крупно, по центру экрана
  char tbuf[8];
  itoa(dTemp, tbuf, 10);
  u8g2.setFont(u8g2_font_logisoso22_tn);
  uint8_t tempW = u8g2.getStrWidth(tbuf);
  // группа «°C» (кольцо рисуем сами — в цифровом шрифте только цифры):
  // кольцо Ø5 + зазор + буква C шириной 5
  const uint8_t degW = 15;
  uint8_t x0 = (128 - (tempW + 3 + degW)) / 2;
  u8g2.drawStr(x0, 31, tbuf);
  u8g2.drawCircle(x0 + tempW + 5, 14, 2, U8G2_DRAW_ALL);
  u8g2.setFont(u8g2_font_5x8_t_cyrillic);
  u8g2.drawStr(x0 + tempW + 10, 31, "C");
}

void Display::drawPidFrame() {
  u8g2.setFont(u8g2_font_5x8_t_cyrillic);
  u8g2.drawUTF8(0, 8, stateLabel);     // «настройка»
  u8g2.drawStr(121, 8, pidKey);        // P / I / D справа
  // Значение крупно по центру; знак и десятичную точку рисуем сами —
  // в цифровом шрифте есть только цифры
  u8g2.setFont(u8g2_font_logisoso22_tn);
  const char* v = pidValue;            // формат dtostrf: [-]d.ddd
  bool neg = (v[0] == '-');
  if (neg) v++;
  const char* dot = strchr(v, '.');
  uint8_t ilen = dot ? (uint8_t)(dot - v) : strlen(v);
  char intPart[8];
  strncpy(intPart, v, ilen);
  intPart[ilen] = '\0';
  const char* fracPart = dot ? dot + 1 : "";
  uint8_t intW = u8g2.getStrWidth(intPart);
  uint8_t fracW = u8g2.getStrWidth(fracPart);
  int16_t total = intW + fracW + (neg ? 10 : 0) + (dot ? 7 : 0);
  int16_t x = (128 - total) / 2;
  if (neg) {
    u8g2.drawBox(x, 20, 7, 3);         // минус
    x += 10;
  }
  u8g2.drawStr(x, 31, intPart);
  x += intW;
  if (dot) {
    u8g2.drawBox(x + 1, 28, 4, 3);     // точка
    x += 7;
  }
  u8g2.drawStr(x, 31, fracPart);
}

void Display::redraw() {
  u8g2.clearBuffer();
  if (screen == SCREEN_MAIN) {
    drawMainFrame();
  } else {
    drawPidFrame();
  }
  u8g2.sendBuffer();
  lastDraw = millis();
}

void Display::setStateLabel(PGM_P label) {
  strncpy_P(stateLabel, label, sizeof(stateLabel) - 1);
  stateLabel[sizeof(stateLabel) - 1] = '\0';
  screen = SCREEN_MAIN;
  redraw();
}

void Display::showPid(const char* key, const char* value) {
  if (screen == SCREEN_PID && strcmp(pidKey, key) == 0 && strcmp(pidValue, value) == 0) {
    return;   // не изменилось — не перерисовываем (вызывается каждый цикл)
  }
  strcpy(pidKey, key);
  strcpy(pidValue, value);
  screen = SCREEN_PID;
  redraw();
}

void Display::update(int currentTemp, int setpoint, int fanTarget, int heaterOutput) {
  if (screen == SCREEN_PID) return;    // экран настроек обновляет только showPid
  int barW = map(heaterOutput, 0, WARMTICK, 0, POWER_BAR_W - 2);
  if (currentTemp == dTemp && setpoint == dSetpoint && fanTarget == dFan && barW == dBarW) {
    return;
  }
  // правка уставок энкодером — сразу; текущие значения — не чаще TIME_ECHO
  bool edited = (setpoint != dSetpoint) || (fanTarget != dFan);
  if (!edited && millis() - lastDraw < TIME_ECHO) return;
  dTemp = currentTemp;
  dSetpoint = setpoint;
  dFan = fanTarget;
  dBarW = barW;
  redraw();
}
