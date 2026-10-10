#ifndef LANGRUS_H
#define LANGRUS_H

/*
Подписи состояний — однобайтовая кодировка под шрифт u8x8_font_ikor
(кириллица: UTF-8 -> hex, удалить байты 0xD0/0xD1; u8g2 печатал
пробел перед двухбайтными символами).
Конвертер: https://onlineutf8tools.com/convert-utf8-to-hexadecimal
Строки хранятся во flash (PROGMEM), а не в ОЗУ.
*/

#define S_WARMING PSTR("\x9d\x90\x93\xa0\x95\x92")
#define S_WAITING PSTR("\x9e\x96\x98\x94\x90\x9d\x98\x95")
#define S_COOLING PSTR("\x9e\xa1\xa2\xab\x92\x90\x9d\x98\x95")
#define S_SETTING PSTR("\x9d\xb0\xa1\xa2\xa0\xbe\xb9\xba\xb0")

#endif
