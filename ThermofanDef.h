#ifndef THERMOFANDEF_H
#define THERMOFANDEF_H

//Максимальная температура на потенциометре
#ifndef TEMP_MAX
#define TEMP_MAX 600
#endif

//Температура при достижении которой отключится нагрев(для аварийного охлаждения);
#ifndef TEMP_MAX_OFF
#define TEMP_MAX_OFF 600
#endif

//Температура после которой фен снова начнет свою работу после аварийного охлаждения
#ifndef TEMP_MIN_ON
#define TEMP_MIN_ON 80
#endif

// Время вывода мс(вывод на дисплей сильно замедляет ардуину)
#ifndef TIME_ECHO
#define TIME_ECHO 100
#endif

#ifndef WARMTICK
#define WARMTICK 25
#endif

// для энкодера
#ifndef ENC_LEFT
#define ENC_LEFT 1
#endif

#ifndef ENC_RIGHT
#define ENC_RIGHT 0
#endif

#ifndef MILLIS_CHANGE_DIRECTION
#define MILLIS_CHANGE_DIRECTION 100
#endif

// пин энкодера
#ifndef ENC_A
#define ENC_A 2
#endif

// пин энкодера
#ifndef ENC_B
#define ENC_B 4
#endif

// тип энкодера, 0 или 1
#ifndef ENC_TYPE
#define ENC_TYPE 1
#endif

// Пин термопары(операционного усилителя)
#define THERMOCOUPLE_PIN A0
// Пин оптопары нагрева фена
#define OPTRON_PIN 9
#define SPEED_FAN_PIN 5
//#define POTENTIOMETER_PIN A1

#endif
