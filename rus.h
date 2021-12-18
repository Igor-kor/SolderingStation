#ifndef LANGRUS_H
#define LANGRUS_H

/*
online converter https://onlineutf8tools.com/convert-utf8-to-hexadecimal
replace 0x.. to \x..
and delete \x0d for get 1 byte symbol (u8g2 printing space symbol befor 2 byte symbol)
*/

#define S_WARMING "\x9d\x90\x93\xa0\x95\x92"
#define S_WAITING "\x9e\x96\x98\x94\x90\x9d\x98\x95"
#define S_COOLING "\x9e\xa1\xa2\xab\x92\x90\x9d\x98\x95"
#define S_SETTING "\x9d\xb0\x81\x82\x80\xbe\xb9\xba\xb0"
#endif
