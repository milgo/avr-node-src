#include "functions.h"
#include "program.h"

uint8_t markerArray[MAX_MARKERS];

volatile uint8_t timerArray[MAX_TIMERS];
volatile uint32_t timerValueArray[MAX_TIMERS];
uint32_t timerTimeArray[MAX_TIMERS];

uint32_t counterValueArray[MAX_COUNTERS];
uint8_t counterArray[MAX_COUNTERS];

uint8_t edgeArray[MAX_EDGES];

volatile uint16_t adcResults[ADC_CHANNELS];
volatile int adcIsrChnl=0;

void setBit(uint8_t* arr, uint8_t num){
	uint8_t vn = num >> 3;
	uint8_t bn = (num - (vn << 3));
	arr[vn] |= 1 << (bn - vn);
}

void resetBit(uint8_t* arr, uint8_t num){
	uint8_t vn = num >> 3;
	uint8_t bn = (num - (vn << 3));
	arr[vn] &= ~(1 << (bn - vn));
}

uint8_t getBit(uint8_t* arr, uint8_t num){
	uint8_t vn = num >> 3;
	uint8_t bn = (num - (vn << 3));
	uint8_t pn = (1 << (bn-vn));
	return (arr[vn] & pn) == pn;
}
volatile uint16_t real_time=0;
void begin(){
	int i;
	for(i=0; i<MAX_TIMERS; i++){
		/*if(IS_PARAM_SET(timerArray[i], TIMER_ON) && IS_PARAM_SET(timerArray[i], TIMER_RESET)){				
			timerValueArray[i]=timerTimeArray[i];
			RESET_PARAM(timerArray[i], TIMER_RESET);
		}*/
		if(IS_PARAM_SET(timerArray[i], TIMER_ON) && timerValueArray[i]<=0){
			SET_PARAM(timerArray[i], TIMER_OVF); /*SIGNAL TIMER OVERFLOW*/
			timerValueArray[i]=timerTimeArray[i];
			real_time=0;
		}
	}
	for(i=0; i<MAX_COUNTERS; i++){
		if(IS_PARAM_SET(counterArray[i], COUNTER_RESET)){				
			counterValueArray[i]=0;
			RESET_PARAM(counterArray[i], COUNTER_RESET);
			RESET_PARAM(counterArray[i], COUNTER_INC);
		}
		else if(IS_PARAM_SET(counterArray[i], COUNTER_INC)){				
			counterValueArray[i]++;
			RESET_PARAM(counterArray[i], COUNTER_INC);
		}		
	}
}

void end(){
	int i;
	for(i=0; i<MAX_TIMERS; i++){
		RESET_PARAM(timerArray[i], TIMER_OVF);
	}
}

void init(){
	int i;
	for(i=0; i<MAX_MARKERS; i++){
		markerArray[i]=0;
	}
	for(i=0; i<MAX_TIMERS; i++){
		timerArray[i]=0;
		timerValueArray[i]=0;
		timerTimeArray[i]=0;
	}
	for(i=0; i<MAX_COUNTERS; i++){
		counterArray[i]=0;
		counterValueArray[i]=0;
	}
	for(i=0; i<MAX_EDGES; i++){
		edgeArray[i]=0;
	}	
}

void O1(int v){
	SET_PIN(PORTD, 5, v)
}

void O2(int v){
	SET_PIN(PORTD, 6, v)
}

void O3(int v){
	SET_PIN(PORTD, 7, v)
}

void O4(int v){
	SET_PIN(PORTB, 0, v)
}

void O5(int v){
	SET_PIN(PORTB, 1, v)
}

int I1(){
	return GET_PIN(PIND, 0);
}
	
int I2(){
	return GET_PIN(PIND, 1);
}
	
int I3(){
	return GET_PIN(PIND, 2);
}

int I4(){
	return GET_PIN(PIND, 3);
}

int I5(){
	return GET_PIN(PIND, 4);
}

void MKS(int n, int v){
	if((v & 0x01)==1){
		setBit(markerArray, n);
	}else{
		resetBit(markerArray, n);
	}
}

int MKG(int n){
	return getBit(markerArray, n);
}

int TMR(int num, int time, int reset){
	/* Setup new timer */
	if(!IS_PARAM_SET(timerArray[num],TIMER_ON)){
		timerArray[num] |= TIMER_ON;
		timerValueArray[num] = time;
		timerTimeArray[num] = time;
		//USART_WriteString("SET TIMER\r");
	}
	else{
		/* If we want to change time */
		/*if(timerTimeArray[num] != time){
			timerTimeArray[num] = time;
		}*/
		/* Timer is going to be reset at the beggining of next loop */
		if((reset & 0x01) == 1){
			//SET_PARAM(timerArray[num], TIMER_RESET);
			timerValueArray[num]=timerTimeArray[num];
		}
	}
	return (IS_PARAM_SET(timerArray[num], TIMER_OVF))?1:0;
}

int ANG(int num){
	if((num-1 < ADC_CHANNELS) && (num-1 >= 0)){
		return adcResults[num-1];
	}
	return 0;
}

int FLP(int num, int set, int reset){
	if(set==1 && reset==0){
		MKS(num, 1);
	}
	if(reset==1){
		MKS(num, 0);
	}
	return MKG(num);
}

int EDG(int num, int pulse, int rise, int reset){
	
	if((reset & 0x01) == 1){
		MKS(num, 0);
		resetBit(edgeArray, num);	
	}
	
	if((getBit(edgeArray, num)==0) && (rise & 0x01) == 1 && (pulse & 0x01) == 1){
		MKS(num, 1);		
	}
	if((getBit(edgeArray, num)==1) && (rise & 0x01) == 0 && (pulse & 0x01) == 0){
		MKS(num, 1);		
	}
	
	if((pulse & 0x01) == 1){		
		setBit(edgeArray, num);
	}else{
		resetBit(edgeArray, num);
	}
	
	return MKG(num);
}

int CNT(int num, int pulse, int reset){
	if((pulse & 0x01) == 1){
		if(counterValueArray[num] < MAX_UINT32){
			SET_PARAM(counterArray[num], COUNTER_INC);
		}else{
			SET_PARAM(counterArray[num], COUNTER_RESET);
		}
	}
	if((reset & 0x01) == 1){
		SET_PARAM(counterArray[num], COUNTER_RESET);
	}
	return counterValueArray[num];
}

void SND(int pulse, int val){
	if((pulse & 0x01)==1){
		USART_WriteInt(val);
		USART_WriteChar('\r');
	}
}

int CGT(int a, int b){
	return a>b;
}

int CLT(int a, int b){
	return a<b;
}

int CEQ(int a, int b){
	return (a==b)?1:0;
}

int MOD(int a, int b){
	return a % b;
}
	
int ADD(int a, int b){
	return a+b;
}	
	
int AND(int a, int b){
	return (a & 0x01) & (b & 0x01);
}

int OR(int a, int b){
	return (a & 0x01) | (b & 0x01);
}

int NOT(int a){
	return !(a & 0x01);
}

/* Timer0 overflow interrupt */
//int c=0;
//int on=0;
ISR (TIMER0_OVF_vect)
{
	TCNT0 += 6; /* That makes timer to overflow every 1ms */
	/*c++;
	if(c >1000){
		if(on == 0){on=1;}
		else{on=0;}
		c = 0;
	}
	O2(on);*/
	real_time++;
	int i;
	for(i=0; i<MAX_TIMERS; i++){
		if(IS_PARAM_SET(timerArray[i], TIMER_ON) && timerValueArray[i]>0){			
			timerValueArray[i]--;	
		}
	}
    /* Event is executed every 4ms here */
}

ISR(ADC_vect)
{
    // Save conversion result.
    adcResults[adcIsrChnl] = ADC;
	USART_WriteString("[");
	USART_WriteInt(adcIsrChnl);
	USART_WriteString("]:");	
	USART_WriteInt(adcResults[adcIsrChnl]);
		USART_WriteChar(',');
    // Next channel.
    if (++adcIsrChnl > ADC_CHANNELS-1)
    {
        adcIsrChnl = 0;
    }

	ADMUX = (1<<REFS0) | (adcIsrChnl & 0b00001111);
	
    // Start the next conversion.    
    ADCSRA |= (1<<ADSC);
}

void USART_RecvInt(char data)
{
	USART_WriteChar(data);
}