#ifndef _SETUP_H_
#define _SETUP_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

#define SET_PIN(P,B,V) if(V==0){P&=~_BV(B);}else{P|=_BV(B);}
#define GET_PIN(P,B) !(P & _BV(B));

#define GET_BIT(A,N)
#define SET_BIT(A,N)if((N&0x01)==1){markerArray[vn] |= 1 << (num-((num>>3)<<3));}else{markerArray[vn] &= ~(1 << (num-((num>>3)<<3));}

#define SET_PARAM(VAR,VAL) VAR|=VAL
#define RESET_PARAM(VAR,VAL) VAR&=~VAL
#define IS_PARAM_SET(VAR, VAL) ((VAR&VAL)==VAL)

#define MAX_MARKERS 4
#define MAX_EDGES 4

#define MAX_TIMERS 16
#define MAX_COUNTERS 16
#define MAX_UINT32 4294967296

#define TIMER_ON 0x01
#define TIMER_RESET 0x02
#define TIMER_ONCE 0x04
#define TIMER_OVF 0x08

#define COUNTER_ON 0x01
#define COUNTER_INC 0x02
#define COUNTER_RESET 0x04

#define ADC_CHANNELS 6

	void init(void);
	void setup(void);

#endif
