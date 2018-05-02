// winbond.cpp

#include "winbond.h"

void w25_init(void) {
    //this->format(SPI_NBIT, SPI_MODE);
    //this->frequency(SPI_FREQ);
	DDRB |= _BV(0);
    w25_chip_disable();
}


// READING
int w25_read_byte(int addr) {
    w25_chip_enable();
    spi_tranceiver(R_INST);
    spi_tranceiver((addr & ADDR_BMASK2) >> ADDR_BSHIFT2);
    spi_tranceiver((addr & ADDR_BMASK1) >> ADDR_BSHIFT1);
    spi_tranceiver((addr & ADDR_BMASK0) >> ADDR_BSHIFT0);
    int response = spi_tranceiver(DUMMY_ADDR);
    w25_chip_disable();
    return response;
}
/*int w25_readByte(int a2, int a1, int a0) {
   w25_chipEnable();
   spi_tranceiver(R_INST);
   spi_tranceiver(a2);
   spi_tranceiver(a1);
   spi_tranceiver(a0);
   int response = spi_tranceiver(DUMMY_ADDR);
    w25_chipDisable();
    return response;
}*/
void w25_read_stream(int addr, char* buf, int count) {
    if (count < 1)
        return;
    w25_chip_enable();
    spi_tranceiver(R_INST);
    spi_tranceiver((addr & ADDR_BMASK2) >> ADDR_BSHIFT2);
    spi_tranceiver((addr & ADDR_BMASK1) >> ADDR_BSHIFT1);
    spi_tranceiver((addr & ADDR_BMASK0) >> ADDR_BSHIFT0);
    for (int i = 0; i < count; i++)
        buf[i] =  spi_tranceiver(DUMMY_ADDR);
    w25_chip_disable();
}

// WRITING
void w25_write_byte(int addr, int data) {
    w25_write_enable();
    w25_chip_enable();
    spi_tranceiver(W_INST);
    spi_tranceiver((addr & ADDR_BMASK2) >> ADDR_BSHIFT2);
    spi_tranceiver((addr & ADDR_BMASK1) >> ADDR_BSHIFT1);
    spi_tranceiver((addr & ADDR_BMASK0) >> ADDR_BSHIFT0);
    spi_tranceiver(data);
    w25_chip_disable();
    w25_write_disable();
    _delay_ms(WAIT_TIME);
}
/*void w25_writeByte(int a2, int a1, int a0, int data) {
    w25_writeEnable();
    w25_chipEnable();
    spi_tranceiver(W_INST);
    spi_tranceiver(a2);
    spi_tranceiver(a1);
    spi_tranceiver(a0);
    spi_tranceiver(data);
    w25_chipDisable();
    w25_writeDisable();
    _delay_ms(WAIT_TIME);
}*/
void w25_write_stream(int addr, char* buf, int count) {
    if (count < 1)
        return;
    w25_write_enable();
    w25_chip_enable();
    spi_tranceiver(W_INST);
    spi_tranceiver((addr & ADDR_BMASK2) >> ADDR_BSHIFT2);
    spi_tranceiver((addr & ADDR_BMASK1) >> ADDR_BSHIFT1);
    spi_tranceiver((addr & ADDR_BMASK0) >> ADDR_BSHIFT0);
    for (int i = 0; i < count; i++)
        spi_tranceiver(buf[i]);
    w25_chip_disable();
    w25_write_disable();
    _delay_ms(WAIT_TIME);
}

//ERASING
void w25_chip_erase() {
    w25_write_enable();
    w25_chip_enable();
    spi_tranceiver(C_ERASE_INST);
    w25_chip_disable();
    w25_write_disable();
    _delay_ms(WAIT_TIME);
}

//ENABLE/DISABLE (private functions)
void w25_write_enable() {
    w25_chip_enable();
    spi_tranceiver(WE_INST);
    w25_chip_disable();
}
void w25_write_disable() {
    w25_chip_enable();
    spi_tranceiver(WD_INST);
    w25_chip_disable();
}
void w25_chip_enable() {
    SET_BIT(PORTB,0,0);
}
void w25_chip_disable() {
    SET_BIT(PORTB,0,1);
}