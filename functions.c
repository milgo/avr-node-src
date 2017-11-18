#include "functions.h"
#include "program.h"

volatile UINT adcResults[ADC_CHANNELS];
volatile UINT adcIsrChnl=0;
volatile UINT real_time=0;

UINT CNT_FUNC_DATA[MAX_FUNC_CNT];
BOOL FALL_FUNC_DATA[MAX_FUNC_FALL];
BOOL RISE_FUNC_DATA[MAX_FUNC_RISE];
BOOL FLIP_FUNC_DATA[MAX_FUNC_FLIP];
volatile UINT TMR_FUNC_DATA[MAX_FUNC_TMR];
volatile UINT TMR_FUNC_VALUE[MAX_FUNC_TMR];

void begin()
{

}

void end()
{

}

void init(){
	int i;
        for(i=0; i<MAX_FUNC_CNT; i++)CNT_FUNC_DATA[i]=0;
        for(i=0; i<MAX_FUNC_FALL; i++)FALL_FUNC_DATA[i]=0;
        for(i=0; i<MAX_COUNTERS; i++)CNT_FUNC_DATA[i]=0;
        for(i=0; i<MAX_FUNC_RISE; i++)RISE_FUNC_DATA[i]=0;
        for(i=0; i<MAX_FUNC_TMR; i++){
            TMR_FUNC_DATA[i]=0;
            TMR_FUNC_VALUE[i]=0;
        }
}

void DO1(BOOL out){
        SET_PIN(PORTD, 5, out)
}

void DO2(BOOL out){
        SET_PIN(PORTD, 6, out)
}

void DO3(BOOL out){
        SET_PIN(PORTD, 7, out)
}

void DO4(BOOL out){
        SET_PIN(PORTB, 0, out)
}

void DO5(BOOL out){
        SET_PIN(PORTB, 1, out)
}

BOOL DI1(){
	return GET_PIN(PIND, 0);
}
	
BOOL DI2(){
	return GET_PIN(PIND, 1);
}
	
BOOL DI3(){
	return GET_PIN(PIND, 2);
}

BOOL DI4(){
	return GET_PIN(PIND, 3);
}

BOOL DI5(){
	return GET_PIN(PIND, 4);
}

UINT AI1(){
    return adcResults[0];
}

UINT AI2(){
    return adcResults[1];
}

UINT AI3(){
    return adcResults[2];
}

UINT AI4(){
    return adcResults[3];
}

UINT AI5(){
    return adcResults[4];
}

BOOL TMR(UINT id, UINT time, BOOL reset)
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

                if(TMR_FUNC_VALUE[id] == 0)
                {
                    TMR_FUNC_VALUE[id] = time;
                    return 1;
                }

	}
        return 0;
}

BOOL FLIP(UINT id, BOOL set, BOOL reset)
{
	if(set==1 && reset==0){
                FLIP_FUNC_DATA[id]=1;
	}
	if(reset==1){
                FLIP_FUNC_DATA[id]=0;
	}
        return FLIP_FUNC_DATA[id];
}


BOOL RISE(UINT id, BOOL pulse)
{
    BOOL prev = RISE_FUNC_DATA[id];
    RISE_FUNC_DATA[id] = pulse & 0x01;
    return (prev < RISE_FUNC_DATA[id]);
}


BOOL FALL(UINT id, BOOL pulse)
{
    BOOL prev = FALL_FUNC_DATA[id];
    FALL_FUNC_DATA[id] = pulse & 0x01;
    return (prev > FALL_FUNC_DATA[id]);
}


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

void SND(BOOL pulse, INT val){
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
