#include "SettingsStore.h"
#include "Config.h"
#include <EEPROM.h>
#include <math.h>

SettingsStore store;

namespace {

const uint16_t SETTINGS_MAGIC = 0x5354;   // 'ST'
const uint8_t SETTINGS_VERSION = 1;

struct Stored {
  uint16_t magic;
  uint8_t version;
  Settings settings;
  uint8_t checksum;   // сумма байтов поля settings
};

// Адреса старого формата (версии до 1.4.4 включительно):
// строка версии по адресу 0, значения с шагом 8 байт
const int LEGACY_ADDR_SETPOINT = 250;
const int LEGACY_ADDR_FANSPEED = 258;
const int LEGACY_ADDR_KP = 266;
const int LEGACY_ADDR_KI = 274;
const int LEGACY_ADDR_KD = 282;

uint8_t checksumOf(const Settings& s) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(&s);
  uint8_t sum = 0;
  for (uint8_t i = 0; i < sizeof(Settings); i++) {
    sum += p[i];
  }
  return sum;
}

} // namespace

void SettingsStore::setDefaults() {
  settings.setpoint = 200;
  settings.fanSpeed = 100;
  settings.kp = 0.5;
  settings.ki = 0.2;
  settings.kd = 0.01;
}

void SettingsStore::sanitize() {
  if (settings.setpoint < SETPOINT_MIN) settings.setpoint = SETPOINT_MIN;
  if (settings.setpoint > SETPOINT_MAX) settings.setpoint = SETPOINT_MAX;
  if (settings.fanSpeed < 0) settings.fanSpeed = 0;
  if (settings.fanSpeed > 100) settings.fanSpeed = 100;
  if (isnan(settings.kp) || isnan(settings.ki) || isnan(settings.kd)) {
    setDefaults();
  }
}

// Старый формат: по адресу 0 длина + строка версии вида "1.4.4",
// значения лежат по фиксированным адресам с 250. Если строка похожа на
// версию — забираем значения (это либо сохранённые пользователем, либо
// записанные старой прошивкой заводские), иначе — заводские значения.
void SettingsStore::migrateFromLegacy() {
  uint8_t len = EEPROM.read(0);
  bool legacy = (len >= 1 && len <= 10);
  for (uint8_t i = 0; legacy && i < len; i++) {
    uint8_t c = EEPROM.read(1 + i);
    if (c != '.' && (c < '0' || c > '9')) {
      legacy = false;
    }
  }
  if (legacy) {
    EEPROM.get(LEGACY_ADDR_SETPOINT, settings.setpoint);
    EEPROM.get(LEGACY_ADDR_FANSPEED, settings.fanSpeed);
    float f;
    EEPROM.get(LEGACY_ADDR_KP, f); settings.kp = f;
    EEPROM.get(LEGACY_ADDR_KI, f); settings.ki = f;
    EEPROM.get(LEGACY_ADDR_KD, f); settings.kd = f;
    sanitize();
  } else {
    setDefaults();
  }
}

void SettingsStore::load() {
  Stored stored;
  EEPROM.get(0, stored);
  if (stored.magic == SETTINGS_MAGIC && stored.version == SETTINGS_VERSION
      && stored.checksum == checksumOf(stored.settings)) {
    settings = stored.settings;
    sanitize();
    return;
  }
  migrateFromLegacy();
  save();
}

void SettingsStore::save() const {
  Stored stored;
  stored.magic = SETTINGS_MAGIC;
  stored.version = SETTINGS_VERSION;
  stored.settings = settings;
  stored.checksum = checksumOf(settings);
  EEPROM.put(0, stored);
}
