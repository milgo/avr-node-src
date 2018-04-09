#include "program.h"
#include "setup.h"
#include "hdlc.h"
#include "uart.h"
#include "crc32.h"
#include "functions.h"

static void tx_u8(uint8_t data)
{
	//How do we send bytes
    while(!uart_tx_byte(data))
    {
        ;
    }
}

void on_rx_uart_circle_buffer_data(uint8_t data)
{
	//Feed hdlc when there is data in circle buffer
	hdlc_on_rx_u8(data);
}

void hdlc_on_rx_frame(const u8_t * data, size_t nr_of_bytes)
{
	DeviceDataFrame *request = (DeviceDataFrame*)data;
	DeviceDataFrame reply;

	u16_t id = U16_TO_LITTLE_ENDIAN(request->id);
	//u32_t data =

	switch(request->command){
		case 0x17:
			reply.command = request->command;
			reply.id = id;
			reply.param = request->param;
			reply.data = CHECKSUM;
			hdlc_tx_frame((u8_t*)&reply, sizeof(DeviceDataFrame));
			break;
		case 0x16:
			if(request->param >= MAX_DATA)
				break;
			reply.command = request->command;
			reply.id = id;
			reply.param = request->param;
			reply.data = _data[request->param];
			hdlc_tx_frame((u8_t*)&reply, sizeof(DeviceDataFrame));
			break;
	}
}

int main(void)
{
	setup();
	//DO1(0,0);

	hdlc_init(&tx_u8, &hdlc_on_rx_frame);

    while (1)
    {
		//hdlc_tx_frame(test_string, 5);
		//tx_u8_array(test_string, 27);
		//_delay_ms(100);

        loop();

		read_from_uart_rx_circle_buffer();
    }
}