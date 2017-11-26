#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "setup.h"
#include "max.h"

typedef char* String128;

volatile uint32_t _data[MAX_DATA];

/*VARDEF*/ #define UINT uint32_t
/*VARDEF*/ #define INT uint32_t
/*VARDEF*/ #define BOOL uint32_t


/*OUTPUT*/ void DO1(UINT id, BOOL out);
/*OUTPUT*/ void DO2(UINT id, BOOL out);
/*OUTPUT*/ void DO3(UINT id, BOOL out);
/*OUTPUT*/ void DO4(UINT id, BOOL out);
/*OUTPUT*/ void DO5(UINT id, BOOL out);

/*INPUT*/ BOOL DI1(UINT id);
/*INPUT*/ BOOL DI2(UINT id);
/*INPUT*/ BOOL DI3(UINT id);
/*INPUT*/ BOOL DI4(UINT id);
/*INPUT*/ BOOL DI5(UINT id);

/*INPUT*/ UINT AI1(UINT id);
/*INPUT*/ UINT AI2(UINT id);
/*INPUT*/ UINT AI3(UINT id);
/*INPUT*/ UINT AI4(UINT id);
/*INPUT*/ UINT AI5(UINT id);

/*FUNCTION*/ BOOL FLIP(UINT id, BOOL set, BOOL reset);
/*FUNCTION*/ BOOL TMR(UINT id, UINT time, BOOL reset);
/*FUNCTION*/ BOOL RISE(UINT id, BOOL pulse);
/*FUNCTION*/ BOOL FALL(UINT id, BOOL pulse);
/*FUNCTION*/ UINT CNT(UINT id, BOOL pulse, BOOL reset);

/*FUNCTION*/ void SND(UINT id, BOOL pulse, INT val);

/*FUNCTION*/ INT CGT(UINT id, INT a, INT b);
/*FUNCTION*/ INT CLT(UINT id, INT a, INT b);
/*FUNCTION*/ INT CEQ(UINT id, INT a, INT b);
/*FUNCTION*/ BOOL AND(UINT id, BOOL a, BOOL b);
/*FUNCTION*/ BOOL OR(UINT id, BOOL a, BOOL b);
/*FUNCTION*/ BOOL NOT(UINT id, BOOL a);


#endif

