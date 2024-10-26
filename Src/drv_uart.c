/*
 * drv_uart.cpp
 *
 *  Created on: 2021年9月9日
 *      Author: ece1234
 */
#include "main.h"
//#include "drv_uart.hpp"

typedef struct
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;

/*==================================================================================================
Name:
Function: Change UART receive state
==================================================================================================*/
void ReceiveStart(UART_HandleTypeDef *huart)
{
	// do something...
}

/*==================================================================================================
Name:
Function: UART receive initization
==================================================================================================*/
void SetToReceiveMode(UART_HandleTypeDef *huart, const void *pData, unsigned long DataLength )
{
	ReceiveStart(huart);
	// do something...
}

/*==================================================================================================
Name:
Function: Change UART transmit state
==================================================================================================*/
HAL_StatusTypeDef SetToSendMode(UART_HandleTypeDef *huart, const void *pData,unsigned long DataLength)
{
	// do something...
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	ReceiveStart(huart);
}

void Serial_PutString(uint8_t *s)
{
	uint16_t length = 0;
  	while (s[length] != '\0')
  	{
    	length++;
 	}
  	SetToSendMode(&huart1, s, length);
}












