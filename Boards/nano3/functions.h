#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "setup.h"
#include "max.h"

uint32_t _data[MAX_DATA];
volatile uint8_t force_enabled;

void set_data_forced(uint8_t id, uint8_t forced);
uint8_t is_data_forced(uint8_t id);
void zero_data_forced(void);

/*{"category":"vardef", "regex":"[0-9]+"}*/
#define UINT uint32_t

/*{"category":"vardef", "regex":"\d"}*/
#define INT uint32_t
/*{"category":"vardef", "regex":"\d"}*/
#define BOOL uint32_t

/*{"category":"output"}*/
void DO1(UINT id, BOOL out);
/*{"category":"output"}*/
void DO2(UINT id, BOOL out);
/*{"category":"output"}*/
void DO3(UINT id, BOOL out);
/*{"category":"output"}*/
void DO4(UINT id, BOOL out);

/*{"category":"input"}*/
BOOL DI1(UINT id);
/*{"category":"input"}*/
BOOL DI2(UINT id);
/*{"category":"input"}*/
BOOL DI3(UINT id);
/*{"category":"input"}*/
BOOL DI4(UINT id);

/*{"category":"input", "regex":"[0-9]+"}*/
UINT AI1(UINT id);
/*{"category":"input"}*/
UINT AI2(UINT id);

/*{"category":"output"}*/
BOOL AO1(UINT id, BOOL in, UINT out, UINT *error);
/*{"category":"output"}*/
void AO2(UINT id, UINT out);

/*{"category":"function"}*/
BOOL FLIP(UINT id, BOOL set, BOOL reset);
/*{"category":"function"}*/
BOOL TMR(UINT id, UINT time, BOOL reset);
/*{"category":"function"}*/
BOOL RISE(UINT id, BOOL pulse);
/*{"category":"function"}*/
BOOL FALL(UINT id, BOOL pulse);
/*{"category":"function"}*/
UINT CNT(UINT id, BOOL pulse, BOOL reset);

/*{"category":"function"}*/
void SND(UINT id, BOOL pulse, INT val);

/*{"category":"function"}*/
INT CGT(UINT id, INT a, INT b);
/*{"category":"function"}*/
INT CLT(UINT id, INT a, INT b);
/*{"category":"function"}*/
INT CEQ(UINT id, INT a, INT b);
/*{"category":"function"}*/
BOOL AND(UINT id, BOOL a, BOOL b);
/*{"category":"function"}*/
BOOL OR(UINT id, BOOL a, BOOL b);
/*{"category":"function"}*/
BOOL NOT(UINT id, BOOL a);


#endif

