#include "program.h"
#include "setup.h"

#define UART_CIRCLE_BUFFER_SIZE 32

volatile int uart_write_pos = 0;
volatile int uart_read_pos = 0;
volatile char uart_circle_buffer[UART_CIRCLE_BUFFER_SIZE];
volatile uint8_t uart_read_ready_flag = 0;

ISR(USART_RXC_vect);

ISR(USART_RXC_vect)
{
	while ( !(UCSRA & (1<<RXC)) );
	if (uart_write_pos == UART_CIRCLE_BUFFER_SIZE)
		uart_write_pos = 0;
	uart_circle_buffer[uart_write_pos++] = UDR;
	uart_read_ready_flag = 1;
}

void on_uart_circle_buffer_data(char data)
{
	USART_WriteChar(data);
}

char read_from_uart_circle_buffer()
{
	if (uart_read_ready_flag == 1)
	{
		for(;;)
		{
			if(uart_read_pos == uart_write_pos)
			{
				break;
			}

			if(uart_read_pos == UART_CIRCLE_BUFFER_SIZE)
			{
				uart_read_pos=0;
			}

			on_uart_circle_buffer_data(uart_circle_buffer[uart_read_pos++]);
		}

		uart_read_ready_flag=0;
	}
}

int main(void)
{
	setup();
    while (1)
    {
		//USART_WriteChar('a');
        loop();
		read_from_uart_circle_buffer();
    }
}