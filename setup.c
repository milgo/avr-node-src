#include "setup.h"

void setup()
{
    /* Setup DO outputs */
    //DDRC and PORTC register is all zeros after reset HI-Z
    //Setting DDRC bit to one makes PORTC pin output sink

    /* Setup AO outputs */
    DDRB |= _BV(1) | _BV(2);
    /* Setup PWM - TIMER1 */
    TCCR1A |= (1<<WGM10); // Fast PWM 8bit
    TCCR1B |= (1<<WGM12);
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1); //Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM
    TCCR1B |= (1<<CS10)|(1<<CS11); // Preksaler = 64  fpwm = 976,5Hz
    OCR1A = 0; // Channel A = 0
    OCR1B = 0; // Channel B = 0

	/* Setup inputs */
    DDRD &= ~_BV(2) & ~_BV(3) & ~_BV(4) & ~_BV(5) & ~_BV(6) & ~_BV(7);
	/* Pull-up resistors */
    PORTD |= _BV(2) | _BV(3) | _BV(4) |_BV(5) | _BV(6) | _BV(7);
	
	/* Setup ADC */
    DDRC &= ~_BV(0) & ~_BV(1);
    /* AVCC with external capacitor at AREF pin */
	ADMUX = (1<<REFS0);
	/* Enable the ADC and set the prescaler to 128. */
    ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

	spi_init_master();
	w25_init();
	init();

    usart_init(51);//19200
    _delay_ms(3000);

	/* Setup time interrupt */
	TIMSK |= (1 << TOIE0);
	/* Enable interrupts */
	sei();
    ADCSRA |= 1<<ADSC; /* Start ADC Conversion */

    /* Set prescaler to 64 and start the timer */
    TCCR0 |= (1 << CS01) | (1 << CS00);
}
