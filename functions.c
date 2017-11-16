#include "functions.h"
#include "program.h"

volatile uint8_t timerArray[MAX_TIMERS];
volatile uint32_t timerValueArray[MAX_TIMERS];
uint32_t timerTimeArray[MAX_TIMERS];

uint32_t counterValueArray[MAX_COUNTERS];
uint8_t counterArray[MAX_COUNTERS];


volatile uint16_t adcResults[ADC_CHANNELS];
volatile int adcIsrChnl=0;

volatile uint16_t real_time=0;
void begin()
{
	int i;
        for(i=0; i<MAX_TIMERS; i++)
        {
                if(IS_PARAM_SET(TMR_FUNC_DATA[i], TIMER_ON) && TMR_FUNC_VALUE[i]<=0)
                {
                        SET_PARAM(TMR_FUNC_DATA[i], TIMER_OVF); /*SIGNAL TIMER OVERFLOW*/
                        TMR_FUNC_VALUE[i]=TMR_FUNC_TIME[i];
			real_time=0;
		}
	}
}

void end()
{
        UINT i;
        for(i=0; i<MAX_FUNC_TMR; i++)
        {
                RESET_PARAM(TMR_FUNC_DATA[i], TIMER_OVF);
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

void DO1(int v){
	SET_PIN(PORTD, 5, v)
}

void DO2(int v){
	SET_PIN(PORTD, 6, v)
}

void DO3(int v){
	SET_PIN(PORTD, 7, v)
}

void DO4(int v){
	SET_PIN(PORTB, 0, v)
}

void DO5(int v){
	SET_PIN(PORTB, 1, v)
}

int DI1(){
	return GET_PIN(PIND, 0);
}
	
int DI2(){
	return GET_PIN(PIND, 1);
}
	
int DI3(){
	return GET_PIN(PIND, 2);
}

int DI4(){
	return GET_PIN(PIND, 3);
}

int DI5(){
	return GET_PIN(PIND, 4);
}

int AI1(){
    return adcResults[0];
}

int AI2(){
    return adcResults[1];
}

int AI3(){
    return adcResults[2];
}

int AI4(){
    return adcResults[3];
}

int AI5(){
    return adcResults[4];
}

int AI5(){
    return adcResults[5];
}

UINT TMR_FUNC_DATA[MAX_FUNC_TMR];
UINT TMR_FUNC_VALUE[MAX_FUNC_TMR];
UINT TMR(UINT id, UINT time, BOOL reset)
{
        if(!IS_PARAM_SET(TMR_FUNC_DATA[id],TIMER_ON))
        {
                TMR_FUNC_DATA[id] |= TIMER_ON;
                TMR_FUNC_VALUE[id] = time;
	}
        else
        {
                if((reset & 0x01) == 1)
                {
                        TMR_FUNC_VALUE[id]=time;
		}
	}
        return (IS_PARAM_SET(TMR_FUNC_DATA[id], TIMER_OVF))?1:0;
}

BOOL FLP_FUNC_DATA[MAX_FUNC_FLP];
BOOL FLP(INT id, BOOL set, BOOL reset)
{
	if(set==1 && reset==0){
                FLP_FUNC_DATA[id]=1;
	}
	if(reset==1){
                FLP_FUNC_DATA[id]=0;
	}
        return FLP_FUNC_DATA[id];
}

BOOL RISE_EDG_DATA[MAX_FUNC_RISE_EDG];
BOOL RISE_EDG(INT id, BOOL pulse)
{
    BOOL prev = RISE_EDG_DATA[id];
    RISE_EDG_DATA[id] = pulse & 0x01;
    return (prev < RISE_EDG_DATA[id]);
}

BOOL FALL_EDG_DATA[MAX_FUNC_FALL_EDG];
BOOL FALL_EDG(INT id, BOOL pulse)
{
    BOOL prev = FALL_EDG_DATA[id];
    FALL_EDG_DATA[id] = pulse & 0x01;
    return (prev > FALL_EDG_DATA[id]);
}

UINT CNT_FUNC_DATA[MAX_FUNC_CNT];
UINT CNT(UINT id, BOOL pulse, BOOL reset)
{
        if((pulse & 0x01) == 1)
        {
            if(CNT_FUNC_DATA[id] < MAX_UINT32)
            {
                CNT_FUNC_DATA[id]++;
            }
	}
        if((reset & 0x01) == 1)
        {
                CNT_FUNC_DATA[id]=0;
	}
        return CNT_FUNC_DATA[id];
}

void SND(INT pulse, INT val){
	if((pulse & 0x01)==1){
		USART_WriteInt(val);
		USART_WriteChar('\r');
	}
}

int CGT(INT a, INT b){
	return a>b;
}

int CLT(INT a, INT b){
	return a<b;
}

int CEQ(INT a, INT b){
	return (a==b)?1:0;
}

BOOL AND(BOOL a, BOOL b){
	return (a & 0x01) & (b & 0x01);
}

BOOL OR(BOOL a, BOOL b){
	return (a & 0x01) | (b & 0x01);
}

BOOL NOT(BOOL a){
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
        for(i=0; i<MAX_FUNC_TMR; i++){
                if(IS_PARAM_SET(TMR_FUNC_DATA[i], TIMER_ON) && TMR_FUNC_VALUE[i]>0){
                        TMR_FUNC_VALUE[i]--;
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
