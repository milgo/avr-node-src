#include "functions.h"
#include "setup.h"
#include "program.h"


/* ARDUINO NONO V3 SETUP

	DI1 - PD2
	DI2 - PD3
	DI3 - PD4 
	DI4 - PD5
	
	DO1 - PD6
	DO2 - PD7
	DO3 - PB0
	DO4 - PB1
	
	AI1 - PC0
	AI2 - PC1
	
	AO1 - PB2
	AO2 - PB3
	
*/

#define FORCED_SELECTOR(condition, id, normal, forced) do{\
if(condition == 1){\
if(is_data_forced(id) == 1){\
forced;\
break;}}\
normal;\
}while(0)

#define MAX_DATA_FORCED MAX_DATA/8+1
	
UINT adcResults[ADC_CHANNELS];
UINT adcIsrChnl=0;
uint32_t _data[MAX_DATA];
uint8_t _data_forced[MAX_DATA_FORCED];
uint32_t time = 0;
uint32_t dtime = 0;
uint8_t force_enabled = 0;

void setup()
{
    /* Setup DO outputs */
    //DDRC and PORTC register is all zeros after reset HI-Z
    //Setting DDRC bit to one makes PORTC pin output sink

    /* Setup PWM */
	//OCR1A = 0; // Channel A = 0
    OCR1B = 0; // Channel B = 0
	OCR2A = 0;

    TCCR1A |= (1<<WGM10); // Fast PWM 8bit
	TCCR1A |= (1<<COM1B1); //Clear OC1B on Compare Match, set OC1B at BOTTOM
	TCCR1B |= (1<<WGM12);
    TCCR1B |= (1<<CS10)|(1<<CS11); // Preksaler = 64  fpwm = 976,5Hz

	TCCR2A |= (1<<WGM20); // Fast PWM 8bit
	TCCR2A |= (1<<COM2A1); //Clear OC1B on Compare Match, set OC1B at BOTTOM
    TCCR2B |= (1<<WGM21);
    TCCR2B |= (1<<CS20)|(1<<CS21); // Preksaler = 64  fpwm = 976,5Hz

	/* Setup AO outputs */
    DDRB |= _BV(2) | _BV(3);

	/* Setup DI inputs */
    DDRD &= ~_BV(2) & ~_BV(3) & ~_BV(4) & ~_BV(5);
	/* Pull-up resistors */
    PORTD |= _BV(2) | _BV(3) | _BV(4) |_BV(5);

	/* Setup DO outputs */
	DDRD |= _BV(6) | _BV(7); //Source current
	DDRB |= _BV(0) | _BV(1);

	/* Setup ADC */
    DDRC &= ~_BV(0) & ~_BV(1);
    /* AVCC with external capacitor at AREF pin */
	ADMUX = (1<<REFS0);
	/* Enable the ADC and set the prescaler to 128. */
    ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

	//spi_init_master();
	//w25_init();
	init();

    usart_init(51);//19200
    _delay_ms(3000);

	/* Setup time interrupt */
	TIMSK0 |= (1 << TOIE0);
	/* Enable interrupts */
	sei();
    ADCSRA |= 1<<ADSC; /* Start ADC Conversion */

    /* Set prescaler to 64 and start the timer */
    TCCR0B |= (1 << CS01) | (1 << CS00);
}

void zero_data_forced()
{
	for(int i=0; i<MAX_DATA_FORCED; i++)_data_forced[i]=0;
}

void set_data_forced(uint8_t id, uint8_t forced)
{
    if(forced > 0)
        _data_forced[id/8] |= (1 << (id % 8));
    else
        _data_forced[id/8] &= ~(1 << (id % 8));
}

uint8_t is_data_forced(uint8_t id){
    return (_data_forced[id/8] >> (id%8)) & 0x1;
}

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

void DO1(UINT id, BOOL out)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		SET_BIT(PORTD, 6, out);_data[id] = out,
		SET_BIT(PORTD, 6, _data[id])
	);
}

void DO2(UINT id, BOOL out)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		SET_BIT(PORTD, 7, out);_data[id] = out,
		SET_BIT(PORTD, 7, _data[id])
	);
}

void DO3(UINT id, BOOL out)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		SET_BIT(PORTB, 0, out);_data[id] = out,
		SET_BIT(PORTB, 0, _data[id])
	);
}

void DO4(UINT id, BOOL out)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		SET_BIT(PORTB, 1, out);_data[id] = out,
		SET_BIT(PORTB, 1, _data[id])
	);
}

BOOL DI1(UINT id)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = !GET_BIT(PIND, 2),
	);
	return _data[id];
}

BOOL DI2(UINT id)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = !GET_BIT(PIND, 3),
		
	);
    return _data[id];
}

BOOL DI3(UINT id)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = !GET_BIT(PIND, 4),

	);
    return _data[id];
}

BOOL DI4(UINT id)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = !GET_BIT(PIND, 5),

	);
    return _data[id];
}

UINT AI1(UINT id)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = adcResults[0],

	);
    return _data[id];
}

UINT AI2(UINT id)
{
	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = adcResults[1],

	);
    return _data[id];
}

BOOL AO1(UINT id, BOOL in, UINT out, UINT* error)
{
	if(out<0)out=0;
	if(out>255)out=255;

	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = out;
		OCR1B = out,
		OCR1B = _data[id]
	);

	return 0;
}

void AO2(UINT id, UINT out)
{
	if(out<0)out=0;
	if(out>255)out=255;

	FORCED_SELECTOR
	(
		force_enabled,
		id,
		_data[id] = out;
		OCR2A = out,
		OCR2A = _data[id]
	);
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
        }
        if((int)_data[id] <= 0)
        {
            _data[id] = 0;
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
        //USART_WriteInt(val);
        //USART_WriteChar('\r');
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
