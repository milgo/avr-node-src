#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "setup.h"

typedef char* String128;

/*VARDEF*/ #define INT int
/*VARDEF*/ #define REAL float
/*VARDEF*/ #define STR String128
/*VARDEF*/ #define BOOL unsigned char


/*FNCDEF*/ void O1(INT v);
/*FNCDEF*/ void O2(INT v);
/*FNCDEF*/ void O3(INT v);
/*FNCDEF*/ void O4(INT v);
/*FNCDEF*/ void O5(INT v);

/*FNCDEF*/ INT I1(void);
/*FNCDEF*/ INT I2(void);
/*FNCDEF*/ INT I3(void);
/*FNCDEF*/ INT I4(void);
/*FNCDEF*/ INT I5(void);

/*FNCDEF*/ void MKS(INT n, INT v);
/*FNCDEF*/ INT MKG(INT n);

/*FNCDEF*/ INT FLP(INT num, INT set, INT reset);
/*FNCDEF*/ INT TMR2(INT num, INT time, INT reset);
/*FNCDEF*/ INT ANG(INT num);

/*FNCDEF*/ void SND(INT pulse, INT val);

/*FNCDEF*/ INT EDG(INT num, INT pulse, INT rise, INT reset);
/*FNCDEF*/ INT CNT(INT num, INT pulse, INT reset);

/*FNCDEF*/ INT CGT(INT a, INT b);
/*FNCDEF*/ INT CLT(INT a, INT b);
/*FNCDEF*/ INT CEQ(INT a, INT b);

/*FNCDEF*/ INT MOD(INT a, INT b);
/*FNCDEF*/ INT SHR(INT a, INT b);

/*FNCDEF*/ INT ADD(INT a, INT b);

/*FNCDEF*/ INT AND(INT a, INT b);
/*FNCDEF*/ INT OR(INT a, INT b);
/*FNCDEF*/ INT NOT(INT a);


#endif

