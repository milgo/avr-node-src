// winbond.h

#ifndef WINBOND_H
#define WINBOND_H

#include "setup.h"
#include "spi.h"

#define SPI_FREQ        1000000
#define SPI_MODE        0
#define SPI_NBIT        8

#define WE_INST         0x06
#define WD_INST         0x04
#define R_INST          0x03
#define W_INST          0x02
#define C_ERASE_INST    0x60

#define DUMMY_ADDR      0x00
#define WAIT_TIME       1

#define ADDR_BMASK2     0x00ff0000
#define ADDR_BMASK1     0x0000ff00
#define ADDR_BMASK0     0x000000ff

#define ADDR_BSHIFT2    16
#define ADDR_BSHIFT1    8
#define ADDR_BSHIFT0    0

    void w25_init(void);
    int w25_read_byte(int addr);                                 // takes a 24-bit (3 bytes) address and returns the data (1 byte) at that location
    //int w25_readByte(int a2, int a1, int a0);                   // takes the address in 3 separate bytes A[23,16], A[15,8], A[7,0]
    void w25_read_stream(int addr, char* buf, int count);        // takes a 24-bit address, reads count bytes, and stores results in buf
    void w25_write_byte(int addr, int data);                     // takes a 24-bit (3 bytes) address and a byte of data to write at that location
    //void w25_writeByte(int a2, int a1, int a0, int data);       // takes the address in 3 separate bytes A[23,16], A[15,8], A[7,0]
    void w25_write_stream(int addr, char* buf, int count);       // write count bytes of data from buf to memory, starting at addr
    void w25_chip_erase(void);                                       // erase all data on chip
    void w25_write_enable(void);                                     // write enable
    void w25_write_disable(void);                                    // write disable
    void w25_chip_enable(void);                                      // chip enable
    void w25_chip_disable(void);                                     // chip disable

#endif