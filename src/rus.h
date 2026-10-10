#ifndef LANGRUS_H
#define LANGRUS_H

/*
Подписи состояний — однобайтовые коды под шрифт u8x8_font_ikor:
кириллица (нижние регистры) занимает слоты 0x90-0xAF, код буквы =
0x90 + (позиция в алфавите а..я), т.е. а=0x90, н=0x9d, р=0xa0, я=0xaf.
(В шрифте есть и уменьшенные копии некоторых букв со 0xb0 — они
использованы в S_SETTING.)
Строки хранятся во flash (PROGMEM), а не в ОЗУ.
*/

#define S_WARMING PSTR("\x9d\x90\x93\xa0\x95\x92")
#define S_WAITING PSTR("\x9e\x96\x98\x94\x90\x9d\x98\x95")
#define S_COOLING PSTR("\x9e\xa1\xa2\xab\x92\x90\x9d\x98\x95")
#define S_SETTING PSTR("\x9d\xb0\xa1\xa2\xa0\xbe\xb9\xba\xb0")
#define S_OVERHEAT PSTR("\x9f\x95\xa0\x95\x93\xa0\x95\x92")

#endif
