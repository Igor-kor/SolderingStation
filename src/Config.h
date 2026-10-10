#ifndef CONFIG_H
#define CONFIG_H

// ---------- Отладка ----------
// Включать при отладке; в релизе закомментировано
// #define DEBUG_SERIAL
// #define DEBUG_PID_SERIAL

// ---------- Версия прошивки ----------
#define FW_VERSION "1.5.0"

// ---------- Пины ----------
#define ENC_A_PIN 2         // энкодер, канал A (INT0)
#define ENC_B_PIN 4         // энкодер, канал B
#define ZERO_CROSS_PIN 3    // детектор нуля LTV814 (INT1)
#define SAVE_BUTTON_PIN 7   // кнопка сохранения (активная низкая);
                           // зажать при включении — режим настройки ПИД
#define ENC_BUTTON_PIN 8    // кнопка энкодера (активная низкая):
                           // переключение температура/вентилятор
#define OPTRON_PIN 9        // оптосимистор MOC3021 нагревателя;
                           // пин 9 = PB1, ISR диммера пишет в порт напрямую
#define REED_PIN 10         // геркон подставки (внешняя подтяжка на плате)
#define SPEED_FAN_PIN 5     // ШИМ вентилятора (оптопара + MOSFET)
#define THERMOCOUPLE_PIN A0 // термопара через ОУ LM358

// ---------- Диммер ----------
// Тиков Timer2 между переходами через ноль; тик = 256 тактов = 16 мкс
// при 16 МГц, 500 * 16 мкс = 8 мс — полупериод сети 50 Гц
// (на 60 Гц не влезает: полупериод 8.33 мс)
#ifndef WARMTICK
#define WARMTICK 500
#endif

// ---------- Термопара (линеаризация) ----------
// T = коррекция(ADC): TC_SCALE * adc * 5/1023 - TC_OFFSET,
// коррекция(x) = x - (TC_CORR_K * x - TC_CORR_B)
#define TC_SCALE 206.36
#define TC_OFFSET 13.263
#define TC_CORR_K 0.2087
#define TC_CORR_B 23.2

// ---------- Машина состояний ----------
#define TEMP_COOLING_TO_WAITING 50  // ниже — переход в ожидание
#define TEMP_WAITING_TO_COOLING 60  // выше — переход в охлаждение (гистерезис)
#define SETPOINT_MIN 10
#define SETPOINT_MAX 500

// ---------- Аварийная защита ----------
#define TEMP_OVERHEAT_OFF 600     // перегрев: нагрев выключить, вентилятор на 100%
#define TEMP_OVERHEAT_RESUME 80   // остыли ниже — вернуться к работе

// ---------- Энкодер ----------
#define ENC_LEFT 1
#define ENC_RIGHT 0
#define ENC_ACCEL_MS 12        // поворот быстрее этого — ускорение
#define ENC_ACCEL_BASE 14      // шаг ускорения: (14 - мс)/2
#ifndef MILLIS_CHANGE_DIRECTION
#define MILLIS_CHANGE_DIRECTION 100
#endif

// ---------- Дисплей ----------
// Период обновления текущих значений, мс
#ifndef TIME_ECHO
#define TIME_ECHO 100
#endif

#endif
