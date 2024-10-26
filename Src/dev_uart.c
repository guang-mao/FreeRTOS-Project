#include "main.h"
#include "api_uartcomm.h"

uint8_t		rxBuf[UART_BUFFER_SIZE];
/*==================================================================================================
Name:
Function: Rearrange the data in the UART receive buffer.
		  And return the length of the data received this time.	
==================================================================================================*/
uint16_t USART_Receive(UART_HandleTypeDef *huart, const uint8_t *srcBuf, uint8_t *dstBuf)
{
	uint16_t size;
	// do something...
	return size;
}

void uart_InterruptHandler( UART_HandleTypeDef *huart, uint8_t *fe_cnt)
{
	uint8_t Length = 0;
	uint16_t checksumCal = 0, checksumTail = 0;
	if ( huart->Instance == USART1 )
	{
		Length = USART_Receive(huart, rxBuf, runtime_data.uart_rx_buffer);
		if ( Length < 2 )
		{
			return;
		}

		checksumCal = API_checksum16_protocol(runtime_data.uart_rx_buffer, (Length - 2));
		checksumTail = (runtime_data.uart_rx_buffer[Length - 2] << 8) + runtime_data.uart_rx_buffer[Length - 1];
		
		if ( ( Length == 16 ) && ( checksumCal == checksumTail ) )
		{
			API_rs232comm(Length);
			*fe_cnt = 0;
		}
		memset(&runtime_data.uart_rx_buffer[0], 0x00, UART_BUFFER_SIZE);
	}
}
