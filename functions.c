#include "functions.h"
#include "program.h"
#include "crc32.h"

volatile UINT adcResults[ADC_CHANNELS];
volatile UINT adcIsrChnl=0;
volatile uint32_t _data[MAX_DATA];
volatile uint32_t time = 0;
volatile uint32_t dtime = 0;

volatile union SerialData{
    uint32_t data32;
    uint16_t data16[2];
    char buffer[4];
}serialQuery, serialResponse;

volatile char buffer[4];
volatile uint8_t bufferCounter = 0;

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
        SET_BIT(DDRC, 2, out);
        _data[id] = out;
}

void DO2(UINT id, BOOL out){
        SET_BIT(DDRC, 3, out);
        _data[id] = out;
}

void DO3(UINT id, BOOL out){
        SET_BIT(DDRC, 4, out);
        _data[id] = out;
}

void DO4(UINT id, BOOL out){
        SET_BIT(DDRC, 5, out);
        _data[id] = out;
}

BOOL DI1(UINT id){
        _data[id] = !GET_BIT(PIND, 2);
        return _data[id];
}

BOOL DI2(UINT id){
        _data[id] = !GET_BIT(PIND, 3);
        return _data[id];
}

BOOL DI3(UINT id){
        _data[id] = !GET_BIT(PIND, 4);
        return _data[id];
}

BOOL DI4(UINT id){
        _data[id] = !GET_BIT(PIND, 5);
        return _data[id];
}

BOOL DI5(UINT id){
        _data[id] = !GET_BIT(PIND, 6);
        return _data[id];
}

BOOL DI6(UINT id){
        _data[id] = !GET_BIT(PIND, 7);
        return _data[id];
}

UINT AI1(UINT id){
    _data[id] = adcResults[0];
    return _data[id];
}

UINT AI2(UINT id){
    _data[id] = adcResults[1];
    return _data[id];
}

void AO1(UINT id, UINT out){
    _data[id] = out;
    if(out<0)out=0;
    if(out>255)out=255;
    OCR1A = out;
}

void AO2(UINT id, UINT out){
    _data[id] = out;
    if(out<0)out=0;
    if(out>255)out=255;
    OCR1B = out;
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
                    _data[id] = 0;
                    _data[id] |= TIMER_ON | time;
                    USART_WriteChar('r');
                }

                if((int)_data[id] <= 0)
                {
                    _data[id] = 0;
                    _data[id] |= TIMER_ON | time;
                    USART_WriteChar('t');
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
    serialQuery.buffer[bufferCounter]=data;

    bufferCounter++;
    if(bufferCounter>3)
    {
        bufferCounter=0;

        uint8_t command = (serialQuery.data16[0] >> 8) & 0xFF;
        uint8_t data = (serialQuery.data16[1] >> 8) & 0xFF;

        if(command == 0) //acquire program checksum
        {
            serialResponse.data32 = CHECKSUM;
            int i;
            for(i=3;i>=0;i--)USART_WriteChar(serialResponse.buffer[i]);
        }
        else if(command == 1) //acquire value
        {
            if(data < (uint8_t)MAX_DATA)
            {
                serialResponse.data32 = _data[data];
                int i;
                for(i=3;i>=0;i--)USART_WriteChar(serialResponse.buffer[i]);
            }
        }
    }
}
