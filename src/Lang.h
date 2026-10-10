#ifndef LANG_H
#define LANG_H

// Язык подписей на дисплее. Для английского раскомментировать LANG_ENG
// и пересобрать проект — точка выбора языка только эта.
// #define LANG_ENG

#ifdef LANG_ENG
#include "eng.h"
#else
#include "rus.h"
#endif

#endif
