#include "uart.h"

volatile uint8_t uart_rx_write_pos = 0;
volatile uint8_t uart_rx_read_pos = 0;
volatile uint8_t uart_rx_circle_buffer[UART_RX_CIRCLE_BUFFER_SIZE];
volatile uint8_t uart_rx_read_ready_flag = 0;

volatile uint8_t uart_tx_write_pos = 0;
volatile uint8_t uart_tx_read_pos = 0;
volatile uint8_t uart_tx_circle_buffer[UART_TX_CIRCLE_BUFFER_SIZE];
volatile bool tx_finished = TRUE;

void usart_init(uint16_t ubrr_value)
{

   /* Set Baud rate */
   UBRRL = ubrr_value;
   UBRRH = (ubrr_value>>8);
   /* Asynchronous mode, No Parity, 1 StopBit, char size 8 */

   UCSRC=(1<<URSEL)|(3<<UCSZ0);

   /* Enable The receiver and transmitter */
   UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
}

ISR(USART_RXC_vect)
{
	while ( !(UCSRA & (1<<RXC)) );
	if (uart_rx_write_pos == UART_RX_CIRCLE_BUFFER_SIZE)
		uart_rx_write_pos = 0;
	uart_rx_circle_buffer[uart_rx_write_pos++] = UDR;
	uart_rx_read_ready_flag = 1;
}

void read_from_uart_rx_circle_buffer(void)
{
	if (uart_rx_read_ready_flag == 1)
	{
		for(;;)
		{
			if(uart_rx_read_pos == uart_rx_write_pos)
			{
				uart_rx_read_ready_flag=0;
				break;
			}

			if(uart_rx_read_pos == UART_RX_CIRCLE_BUFFER_SIZE)
			{
				uart_rx_read_pos=0;
			}

			on_rx_uart_circle_buffer_data(uart_rx_circle_buffer[uart_rx_read_pos++]);
		}
	}
}

/* Data Register Empty */
ISR(USART_UDRE_vect)
{
	if(uart_tx_read_pos == uart_tx_write_pos)
	{
		UCSRB &= ~(1<<UDRIE); //disable transmit interrupt
		 // TX complete interrupt enable
		UCSRB |= (1<<TXCIE);
		return;
	}

	uart_tx_read_pos++;

	if (uart_tx_read_pos == UART_TX_CIRCLE_BUFFER_SIZE)
	{
		uart_tx_read_pos = 0;
	}

	UDR = uart_tx_circle_buffer[uart_tx_read_pos];
	tx_finished = FALSE;

}

/* Transmit complete */
ISR(USART_TXC_vect)
{
	UCSRB &= ~(1<<TXCIE);
	tx_finished = TRUE;
	//DO1(0,1);
}

bool tx_uart_circle_buffer_write(uint8_t data)
{
	uint8_t write_pos = uart_tx_write_pos;
	if (write_pos == UART_TX_CIRCLE_BUFFER_SIZE-1)
	{
		write_pos = 0;
	}
	else
	{
		write_pos++;
	}

	if(write_pos == uart_tx_read_pos)
	{
		//buffer is full
		return FALSE;
	}

	uart_tx_circle_buffer[write_pos] = data;
	uart_tx_write_pos = write_pos;

	return TRUE;
}

bool uart_tx_byte(uint8_t data)
{
	//DO1(0,0);
	if(!tx_uart_circle_buffer_write(data))
		return FALSE;

	//UDRE // Data register empty flag
	UCSRB |= (1<<UDRIE);
	return TRUE;
}