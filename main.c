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
	u8_t forced = 0;

	reply.command = request->command;
	reply.id = id;
	reply.param = request->param;
	reply.data = request->data;

	switch(request->command){
		case 0x17: // Get Checksum
			reply.data = CHECKSUM;
			break;
		case 0x16: //Get data value
			if(request->param >= MAX_DATA)
				break;
			reply.data = _data[request->param];
			break;
		case 0x18: //Set force enabled
			force_enabled = request->param;
			zero_data_forced();
			break;
		case 0x19: //Force data id
			set_data_forced(request->param, 1);
			if(request->param < MAX_DATA && request->param >=0 )
				_data[request->param] = request->data;
			break;
		case 0x20: //Unforce data id
			set_data_forced(request->param, 0);
			break;
		case 0x21: //Get force data id
			forced = is_data_forced(request->param);
			reply.data = forced;			
			break;
	}
	
	hdlc_tx_frame((u8_t*)&reply, sizeof(DeviceDataFrame));
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