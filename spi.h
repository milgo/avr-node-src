#ifndef __SPI_H__
#define __SPI_H__

#include "setup.h"

void spi_init_master (void);
uint8_t spi_tranceiver (uint8_t data);

#endif // #ifndef __SPI_H__
