#include "uart.h"

void USART_Init(uint16_t ubrr_value)
{

   /* Set Baud rate */
   UBRRL = ubrr_value;
   UBRRH = (ubrr_value>>8);
   /* Asynchronous mode, No Parity, 1 StopBit, char size 8 */

   UCSRC=(1<<URSEL)|(3<<UCSZ0);

   /* Enable The receiver and transmitter */
   UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
}

char USART_ReadChar()
{
	while(!(UCSRA & (1<<RXC))){}
	return UDR;
}

void USART_WriteChar(char data)
{
	while(!(UCSRA & (1<<UDRE))){}
	UDR=data;
}

char num[10];

void USART_WriteInt(int value)
{
	num[0]=0;
	itoa( value, num, 10 );
	USART_WriteString(num);
}


void USART_WriteString(char s[])
{
	int i=0;
	
	while (s[i] != 0x00)
	{
		USART_WriteChar(s[i]);
		i++;
	}
}