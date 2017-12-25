#include "setup.h"

void setup(){
	/* Setup outputs */
	DDRD |= _BV(5) | _BV(6) | _BV(7);
	//PORTD &= ~_BV(5) & ~_BV(6) & ~_BV(7); //SOURCE 
	PORTD |= _BV(5) | _BV(6) | _BV(7); //SINK
	DDRB |= _BV(0) | _BV(1);
	//PORTB &= ~_BV(0) & ~_BV(1); //SOURCE
	PORTB |=  _BV(0) |  _BV(1); //SINK

	/* Setup inputs */
	DDRD &= /*~_BV(0) & ~_BV(1) &*/ ~_BV(2) & ~_BV(3) & ~_BV(4);
	/* Pull-up resistors */
	PORTD |= /*_BV(0) | _BV(1) |*/ _BV(2) | _BV(3) | _BV(4); 
	
	/* Setup ADC */
	DDRC &= ~_BV(0) & ~_BV(1) & ~_BV(2) & ~_BV(3) & ~_BV(4) & ~_BV(5);
	ADMUX = (1<<REFS0);
	/* Enable the ADC and set the prescaler to 128. */
        ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

        USART_Init(51);
        _delay_ms(3000);

	/* Setup time interrupt */
	TIMSK |= (1 << TOIE0);
	/* Enable interrupts */
	sei();
        ADCSRA |= 1<<ADSC; /* Start ADC Conversion */
	
        /* Set prescaler to 64 and start the timer */
        TCCR0 |= (1 << CS01) | (1 << CS00);

	init();
	
}
