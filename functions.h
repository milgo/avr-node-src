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
/*VARDEF*/ #define REAL float
/*VARDEF*/ #define STR String128
/*VARDEF*/ #define BOOL uint8_t


/*OUTPUT*/ void DO1(BOOL out);
/*OUTPUT*/ void DO2(BOOL out);
/*OUTPUT*/ void DO3(BOOL out);
/*OUTPUT*/ void DO4(BOOL out);
/*OUTPUT*/ void DO5(BOOL out);

/*OUTPUT*/ void AO1(UINT out);

/*INPUT*/ BOOL DI1(void);
/*INPUT*/ BOOL DI2(void);
/*INPUT*/ BOOL DI3(void);
/*INPUT*/ BOOL DI4(void);
/*INPUT*/ BOOL DI5(void);

/*INPUT*/ INT AI1(void);
/*INPUT*/ INT AI2(void);
/*INPUT*/ INT AI3(void);
/*INPUT*/ INT AI4(void);
/*INPUT*/ INT AI5(void);
/*INPUT*/ INT AI5(void);

/*FNCDEF,UNIQUE*/ INT FLP(UINT id, BOOL set, BOOL reset);
/*FNCDEF,UNIQUE*/ INT TMR(UINT id, UINT time, BOOL reset);
/*FNCDEF,UNIQUE*/ INT EDG(UINT id, BOOL pulse);
/*FNCDEF,UNIQUE*/ UINT CNT(UINT id, BOOL pulse, BOOL reset);

/*FNCDEF*/ void SND(BOOL pulse, INT val);


/*FNCDEF*/ INT CGT(INT a, INT b);
/*FNCDEF*/ INT CLT(INT a, INT b);
/*FNCDEF*/ INT CEQ(INT a, INT b);
/*FNCDEF*/ BOOL AND(BOOL a, BOOL b);
/*FNCDEF*/ BOOL OR(BOOL a, BOOL b);
/*FNCDEF*/ BOOL NOT(BOOL a);


#endif

