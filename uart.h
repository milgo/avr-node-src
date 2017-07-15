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

void USART_Init(uint16_t ubrr_value);
void USART_WriteChar(char data);
void USART_WriteString(char s[]);
void USART_WriteInt(int value);
char USART_ReadChar(void);

extern void USART_RecvInt(char data);

#endif 
