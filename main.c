#include "program.h"
#include "setup.h"
#include "hdlc.h"
#include "uart.h"
#include "crc32.h"
#include "functions.h"

#define GetNodeValue 0x16
#define GetProgramChecksum 0x17
#define SetForcingEnabled 0x18
#define SetNodeForceEnable 0x19
#define SetNodeForceDisable 0x20
#define GetNodeForceStatus 0x21

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

	u8_t forced = 0;

	reply.command = request->command;
	reply.id = request->id;
	reply.param = request->param;
	reply.data = request->data;

	switch(request->command){
		case GetProgramChecksum:
			reply.data = CHECKSUM;
			break;
		case GetNodeValue: //Get data value
			if(request->param >= MAX_DATA)
				break;
			reply.data = _data[request->param];
			break;
		case SetForcingEnabled: //Set force enabled
			force_enabled = request->param;
			zero_data_forced();
			break;
		case SetNodeForceEnable:
			set_data_forced(request->param, 1);
			if(request->param < MAX_DATA && request->param >=0 )
				_data[request->param] = request->data;
			break;
		case SetNodeForceDisable: //Unforce data id
			set_data_forced(request->param, 0);
			break;
		case GetNodeForceStatus: //Get force data id
			forced = is_data_forced(request->param);
			reply.param = forced;
			reply.data = _data[request->param];
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