#include "functions.h"
#include "program.h"

volatile UINT adcResults[ADC_CHANNELS];
volatile UINT adcIsrChnl=0;
volatile uint32_t _data[MAX_DATA];
volatile uint32_t time = 0;
volatile uint32_t dtime = 0;

void begin()
{
    if(time>0)
    {
        dtime = time;
        time = 0;
    }
}

void end()
{
    dtime = 0;
}

void init()
{
        for(int i=0; i<MAX_DATA; i++)_data[i]=0;
}

void DO1(UINT id, BOOL out){
        SET_PIN(PORTD, 5, out)
}

void DO2(UINT id, BOOL out){
        SET_PIN(PORTD, 6, out)
}

void DO3(UINT id, BOOL out){
        SET_PIN(PORTD, 7, out)
}

void DO4(UINT id, BOOL out){
        SET_PIN(PORTB, 0, out)
}

void DO5(UINT id, BOOL out){
        SET_PIN(PORTB, 1, out)
}

BOOL DI1(UINT id){
	return GET_PIN(PIND, 0);
}
	
BOOL DI2(UINT id){
	return GET_PIN(PIND, 1);
}
	
BOOL DI3(UINT id){
	return GET_PIN(PIND, 2);
}

BOOL DI4(UINT id){
	return GET_PIN(PIND, 3);
}

BOOL DI5(UINT id){
	return GET_PIN(PIND, 4);
}

UINT AI1(UINT id){
    return adcResults[0];
}

UINT AI2(UINT id){
    return adcResults[1];
}

UINT AI3(UINT id){
    return adcResults[2];
}

UINT AI4(UINT id){
    return adcResults[3];
}

UINT AI5(UINT id){
    return adcResults[4];
}

UINT UCONST(UINT cnst){
    return cnst;
}

BOOL TMR(UINT id, UINT time, BOOL reset)
{
        if(!IS_PARAM_SET(_data[id], TIMER_ON))
        {
                _data[id] |= TIMER_ON | time;
	}
        else
        {
                if((reset & 0x01) == 1)
                {
                    _data[id] |= TIMER_ON | time;
		}

                if(_data[id] <= 0)
                {
                    _data[id] |= TIMER_ON | time;
                    return 1;
                }
                else
                {
                    _data[id] = _data[id] - dtime;
                }

	}
        return 0;
}

BOOL FLIP(UINT id, BOOL set, BOOL reset)
{
	if(set==1 && reset==0){
                _data[id]=1;
	}
	if(reset==1){
                _data[id]=0;
	}
        return _data[id];
}


BOOL RISE(UINT id, BOOL pulse)
{
    BOOL prev = _data[id];
    _data[id] = pulse & 0x01;
    return (prev < _data[id]);
}


BOOL FALL(UINT id, BOOL pulse)
{
    BOOL prev = _data[id];
    _data[id] = pulse & 0x01;
    return (prev > _data[id]);
}


UINT CNT(UINT id, BOOL pulse, BOOL reset)
{
        if((pulse & 0x01) == 1)
        {
            if(_data[id] < MAX_UINT32)
            {
                _data[id]++;
            }
	}
        if((reset & 0x01) == 1)
        {
                _data[id]=0;
	}
        return _data[id];
}

void SND(UINT id, BOOL pulse, INT val){
	if((pulse & 0x01)==1){
		USART_WriteInt(val);
		USART_WriteChar('\r');
	}
}

INT CGT(UINT id, INT a, INT b){
	return a>b;
}

INT CLT(UINT id, INT a, INT b){
	return a<b;
}

INT CEQ(UINT id, INT a, INT b){
	return (a==b)?1:0;
}

BOOL AND(UINT id, BOOL a, BOOL b){
	return (a & 0x01) & (b & 0x01);
}

BOOL OR(UINT id, BOOL a, BOOL b){
	return (a & 0x01) | (b & 0x01);
}

BOOL NOT(UINT id, BOOL a){
	return !(a & 0x01);
}

/* Timer0 overflow interrupt */
ISR (TIMER0_OVF_vect)
{
        TCNT0 = 6; /* That makes timer to overflow every 1ms */

        /*int i;
        for(i=0; i<MAX_FUNC_TMR; i++)
        {
                if(IS_PARAM_SET(TMR_FUNC_DATA[i], TIMER_ON) && TMR_FUNC_VALUE[i]>0)
                {
                        TMR_FUNC_VALUE[i]--;
		}
        }
        */
        time++;
}

ISR(ADC_vect)
{
    // Save conversion result.
    adcResults[adcIsrChnl] = ADC;
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
