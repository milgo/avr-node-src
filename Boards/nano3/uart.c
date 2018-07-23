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
	// Not necessary; initialize anyway
	DDRD |= _BV(PD1);
	DDRD &= ~_BV(PD0);

	// Set baud rate; lower byte and top nibble
	UBRR0H = ((ubrr_value) & 0xF00);
	UBRR0L = (uint8_t) ((ubrr_value) & 0xFF);

	UCSR0B |= 1U << RXCIE0 | 1U << UDRIE0;    // enable rx and tx interrupts
	UCSR0B |= 1U << RXEN0 | 1U << TXEN0;      // enable receiver/transmitter
	UCSR0C |= 1U << UCSZ01 | 1U << UCSZ00;    // 8 bit data frames,no parity, 1 stop bit
}

ISR(USART_RX_vect)
{
	while ( !(UCSR0A & (1<<RXC0)) );
	if (uart_rx_write_pos == UART_RX_CIRCLE_BUFFER_SIZE)
		uart_rx_write_pos = 0;
	uart_rx_circle_buffer[uart_rx_write_pos++] = UDR0;
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
		UCSR0B &= ~(1<<UDRIE0); //disable transmit interrupt
		 // TX complete interrupt enable
		UCSR0B |= (1<<TXCIE0);
		return;
	}

	uart_tx_read_pos++;

	if (uart_tx_read_pos == UART_TX_CIRCLE_BUFFER_SIZE)
	{
		uart_tx_read_pos = 0;
	}

	UDR0 = uart_tx_circle_buffer[uart_tx_read_pos];
	tx_finished = FALSE;

}

/* Transmit complete */
ISR(USART_TX_vect)
{
	UCSR0B &= ~(1<<TXCIE0);
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
	UCSR0B |= (1<<UDRIE0);
	return TRUE;
}