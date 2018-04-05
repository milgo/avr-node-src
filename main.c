#include "program.h"
#include "setup.h"

#define UART_RX_CIRCLE_BUFFER_SIZE 32
#define UART_TX_CIRCLE_BUFFER_SIZE 32

#define TRUE 1
#define FALSE 0
#define bool uint8_t

volatile uint8_t uart_rx_write_pos = 0;
volatile uint8_t uart_rx_read_pos = 0;
volatile uint8_t uart_rx_circle_buffer[UART_RX_CIRCLE_BUFFER_SIZE];
volatile uint8_t uart_rx_read_ready_flag = 0;

volatile uint8_t uart_tx_write_pos = 0;
volatile uint8_t uart_tx_read_pos = 0;
volatile uint8_t uart_tx_circle_buffer[UART_TX_CIRCLE_BUFFER_SIZE];
volatile bool tx_finished = TRUE;

static void tx_u8(uint8_t data);

ISR(USART_RXC_vect);

ISR(USART_RXC_vect)
{
	while ( !(UCSRA & (1<<RXC)) );
	if (uart_rx_write_pos == UART_RX_CIRCLE_BUFFER_SIZE)
		uart_rx_write_pos = 0;
	uart_rx_circle_buffer[uart_rx_write_pos++] = UDR;
	uart_rx_read_ready_flag = 1;
}

void on_rx_uart_circle_buffer_data(uint8_t data)
{
	tx_u8(data);
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

//----------------------------------------------------------------------------------

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
	DO1(0,1);
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
	DO1(0,0);
	if(!tx_uart_circle_buffer_write(data))
		return FALSE;

	//UDRE // Data register empty flag
	UCSRB |= (1<<UDRIE);
	return TRUE;
}

static void tx_u8(uint8_t data)
{
    while(!uart_tx_byte(data))
    {
        ;
    }//do something when buffer is full
	//like for instance check if buffer is big enought to fit the frame
}

void tx_u8_array(uint8_t * data, uint8_t nr_of_bytes)
{
    while(nr_of_bytes != 0)
    {
		tx_u8(*data++);
        nr_of_bytes--;
    }
}

int main(void)
{
	setup();
	DO1(0,0);

	uint8_t* test_string = "abcdefghijklmnopqrstuvwxyz|";

    while (1)
    {

		tx_u8_array(test_string, 27);
		//_delay_ms(100);

        loop();

		read_from_uart_rx_circle_buffer();
    }
}