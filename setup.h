#ifndef _SETUP_H_
#define _SETUP_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

#define SET_PIN(P,B,V) if(V==0){P&=~_BV(B);}else{P|=_BV(B);}
#define GET_PIN(P,B) !(P & _BV(B));

#define SET_PARAM(VAR,VAL) VAR|=VAL
#define RESET_PARAM(VAR,VAL) VAR&=~VAL
#define IS_PARAM_SET(VAR, VAL) ((VAR&VAL)==VAL)

#define MAX_UINT32 4294967296

#define TIMER_ON 0x80000000

#define ADC_CHANNELS 6

	void init(void);
	void setup(void);

#endif
