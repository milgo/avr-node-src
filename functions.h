#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "setup.h"
#include "max.h"

typedef char* String128;

/*VARDEF*/ #define UINT uint16_t
/*VARDEF*/ #define INT int
/*VARDEF*/ #define STR String128
/*VARDEF*/ #define BOOL uint8_t


/*OUTPUT*/ void DO1(BOOL out);
/*OUTPUT*/ void DO2(BOOL out);
/*OUTPUT*/ void DO3(BOOL out);
/*OUTPUT*/ void DO4(BOOL out);
/*OUTPUT*/ void DO5(BOOL out);

/*INPUT*/ BOOL DI1(void);
/*INPUT*/ BOOL DI2(void);
/*INPUT*/ BOOL DI3(void);
/*INPUT*/ BOOL DI4(void);
/*INPUT*/ BOOL DI5(void);

/*INPUT*/ UINT AI1(void);
/*INPUT*/ UINT AI2(void);
/*INPUT*/ UINT AI3(void);
/*INPUT*/ UINT AI4(void);
/*INPUT*/ UINT AI5(void);

/*FUNCTION,UNIQUE*/ BOOL FLIP(UINT id, BOOL set, BOOL reset);
/*FUNCTION,UNIQUE*/ BOOL TMR(UINT id, UINT time, BOOL reset);
/*FUNCTION,UNIQUE*/ BOOL RISE(UINT id, BOOL pulse);
/*FUNCTION,UNIQUE*/ BOOL FALL(UINT id, BOOL pulse);
/*FUNCTION,UNIQUE*/ UINT CNT(UINT id, BOOL pulse, BOOL reset);

/*FUNCTION*/ void SND(BOOL pulse, INT val);


/*FUNCTION*/ INT CGT(INT a, INT b);
/*FUNCTION*/ INT CLT(INT a, INT b);
/*FUNCTION*/ INT CEQ(INT a, INT b);
/*FUNCTION*/ BOOL AND(BOOL a, BOOL b);
/*FUNCTION*/ BOOL OR(BOOL a, BOOL b);
/*FUNCTION*/ BOOL NOT(BOOL a);


#endif

