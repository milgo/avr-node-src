/*
 * uart.h
 *
 * Created: 2016-07-02 23:28:45
 *  Author: Milosz
 */ 

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include "defines.h"

#define UART_RX_CIRCLE_BUFFER_SIZE 32
#define UART_TX_CIRCLE_BUFFER_SIZE 32

#define U16_TO_LITTLE_ENDIAN(v) ((v & 0xFF00)) >> 8 | ((v & 0xFF) << 8)

typedef struct _devicedataframe_struct
{
    u16_t id;
    u8_t command;
    u8_t param;
    u32_t data;
}DeviceDataFrame;


void usart_init(uint16_t ubrr_value);
void read_from_uart_rx_circle_buffer(void);
bool tx_uart_circle_buffer_write(uint8_t data);
bool uart_tx_byte(uint8_t data);
void on_rx_uart_circle_buffer_data(uint8_t data);

ISR(USART_UDRE_vect);
ISR(USART_TXC_vect);
ISR(USART_RXC_vect);
ISR(USART_RXC_vect);

#endif 
