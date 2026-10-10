#ifndef LANGRUS_H
#define LANGRUS_H

/*
Подписи состояний — обычный UTF-8, PROGMEM. Дисплей работает через
u8g2 drawUTF8 и шрифт u8g2_font_5x8_t_cyrillic, никакой ручной
перекодировки байтов не требуется.
*/

#define S_WARMING PSTR("Нагрев")
#define S_WAITING PSTR("Ожидание")
#define S_COOLING PSTR("Остывание")
#define S_SETTING PSTR("Настройка")
#define S_OVERHEAT PSTR("Перегрев")

#endif
