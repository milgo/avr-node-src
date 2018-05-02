#include "spi.h"

void spi_init_master (void)
{
    DDRB |= (1<<5)|(1<<3);
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

unsigned char spi_tranceiver (unsigned char data)
{
    SPDR = data;
    while(!(SPSR & (1<<SPIF) ));
    return(SPDR);
}
