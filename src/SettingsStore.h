#ifndef SETTINGSSTORE_H
#define SETTINGSSTORE_H

#include <Arduino.h>

// Настройки, сохраняемые в EEPROM
struct Settings {
  int setpoint;      // уставка температуры, °C
  int fanSpeed;      // уставка скорости вентилятора, %
  float kp, ki, kd;  // коэффициенты ПИД
};

// Хранение настроек одним блоком: magic + версия + данные + контрольная сумма
class SettingsStore {
  public:
    void load();      // чтение; при первом запуске — миграция со старого формата
    void save() const;

    Settings settings;

  private:
    void setDefaults();
    void sanitize();
    void migrateFromLegacy();
};

extern SettingsStore store;

#endif
