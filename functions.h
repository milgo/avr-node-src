#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "setup.h"

#define SET_PIN(P,B,V) if(V==0){P&=~_BV(B);}else{P|=_BV(B);}
#define GET_PIN(P,B) !(P & _BV(B));

#define GET_BIT(A,N)
#define SET_BIT(A,N)if((N&0x01)==1){markerArray[vn] |= 1 << (num-((num>>3)<<3));}else{markerArray[vn] &= ~(1 << (num-((num>>3)<<3));}

#define SET_PARAM(VAR,VAL) VAR|=VAL
#define RESET_PARAM(VAR,VAL) VAR&=~VAL
#define IS_PARAM_SET(VAR, VAL) ((VAR&VAL)==VAL)

/* Multiply this by 8 to get the real num of available variables */
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

	void O1(int v);
	void O2(int v);
	void O3(int v);
	void O4(int v);
	void O5(int v);
	
	int I1(void);
	int I2(void);
	int I3(void);
	int I4(void);
	int I5(void);
	
	void MKS(int n, int v);
	int MKG(int n);
	
	int FLP(int num, int set, int reset);
	int TMR(int num, int time, int reset);
	int ANG(int num);
	
	void SND(int pulse, int val);
	
	int EDG(int num, int pulse, int rise, int reset);
	int CNT(int num, int pulse, int reset);
	
	int CGT(int a, int b);
	int CLT(int a, int b);
	int CEQ(int a, int b);
	
	int MOD(int a, int b);
	int SHR(int a, int b);
	
	int ADD(int a, int b);
	
	int AND(int a, int b);
	int OR(int a, int b);
	int NOT(int a);


#endif

