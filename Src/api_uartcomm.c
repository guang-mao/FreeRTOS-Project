/********************************************************************************
*	Copyright (c) 2018, Meyer Yu - All Rights Reserved														*
*																																								*
*	THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR				*
*	DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF MEYER YU												*
*																																								*
********************************************************************************/

#include "main.h"
#include "api_uartcomm.h"

/**
*	API_UARTTransmit
**/
void API_UARTTransmit(MSG_FRAME_t *msg)
{
	if ( SetToSendMode(&huart1, &msg->data[0], msg->length ) != HAL_OK )
	{
		/*	Don't block if the enqueue fails.
		Put the message at the front of queue 
		for resending */
		xQueueSendToFront(msgQueueHandle,(void *) &msg, 0);
	}
	return;
}

void API_rs232comm(const uint8_t Length) {
	/*
		buffer[ 0] Header   1/2
		buffer[ 1] Header   2/2
		buffer[ 2] Command  1/4 Group
		buffer[ 3] Command  2/4 Command
		buffer[ 4] Command  3/4 Reserved
		buffer[ 5] Command  4/4 ID
		buffer[ 6] Data     1/8
		buffer[ 7] Data     2/8
		buffer[ 8] Data     3/8
		buffer[ 9] Data     4/8
		buffer[10] Data     5/8
		buffer[11] Data     6/8
		buffer[12] Data     7/8
		buffer[13] Data     8/8
		buffer[14] Checksum 1/2
		buffer[15] Checksum 2/2
	*/
	uint8_t tx_frame[16] = {0x00};
	struct __attribute__((packed))
	{
		uint8_t header1;
		uint8_t header2;
		uint8_t group;
		uint8_t command;
		uint8_t rw;
		uint8_t id;
	} rx_frame;

	memcpy(&rx_frame, &runtime_data.uart_rx_buffer[0], 6);
	/*
	 *	Channel type
	 */
	if ( rx_frame.header1 == 0xB9 && rx_frame.header2 == 0x42 )
	{
		switch ( rx_frame.command )
		{

		}
	}
	return;
}

unsigned int API_checksum16_protocol(unsigned char *data, unsigned char length)
{
	int j;
	unsigned int reg_crc = 0;
	for(j = 0; j < length; j++){
		reg_crc += *data++;
	}
	reg_crc &= (0xffff);

	return reg_crc;
}

static inline void pushTxframe(uint8_t* txf, uint8_t length)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	MSG_FRAME_t msg;
	uint16_t crc16 = 0;
	txf[0] = 0xBC;
	txf[1] = 0x73;
	txf[2] = 0x00;
	txf[5] = 0xFF;
	crc16 = API_checksum16_protocol(txf, length - 2);
	txf[length - 2] = (uint8_t) ( ( crc16 >> 8 ) & 0x00FF );
	txf[length - 1] = (uint8_t) ( ( crc16      ) & 0x00FF );
	
	msg.msg = UART_TRANSMIT;
	memcpy(&msg.data[0], txf ,length);
	msg.length = length;
	
	xQueueSendToBackFromISR(msgQueueHandle,(void *) &msg, &xHigherPriorityTaskWoken);
	/* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
	The macro used to do this is dependent on the port and may be called
	portEND_SWITCHING_ISR. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}
