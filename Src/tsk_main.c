/********************************************************************************
*	Copyright (c) 2021, MKS Servo - All Rights Reserved														*
*																																								*
*	THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR				*
*	DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF MKS Servo												*
*																																								*
********************************************************************************/
#include "main.h"
#include "api_uartcomm.h"

#define	DELAY_MS		1

void ADCxInit(ADC_TypeDef *ADCx);
void PWMxInit(TIM_TypeDef *TIMx);
void CLKxInit(TIM_TypeDef *TIMx);

/**
* tsk_main
**/
void tsk_main(void *argument)
{
	/*
	 *	MRAM Init
	 */
	API_readsetting();
	API_setenvironment();
	
	/*
	 *	INJ ADC Init
	 */
	if ( LL_ADC_IsEnabled (ADC1) == 0 )
	{
		ADCxInit (ADC1);
  	}

	/*
	 * Clock Init
	 */
	CLKxInit(TIM11);
	
	/*
	 *	AS5600
	 */
	as5600_instance				= AS5600_New();
	as5600_instance->i2cHandle	= &hi2c1;
	as5600_instance->i2cAddr	= ( ( AS5600_SLAVE_ADDRESS ) << 1 ) + 1;
	if ( AS5600_Init(as5600_instance) != HAL_OK )
	{
		unlock_busy_entry();
	}
	
	/*
	 *	PWM Init
	 */
	PWMxInit(TIM1);
	
	/*
	 *	REG ADC Init, Circular reading.
	 */
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adc2, NUMBERofADC2);
	
	/*
	 *	Suntries
	 */
	bootup_tick = HAL_GetTick();
}

/**
*	ADCxInit
**/
void ADCxInit(ADC_TypeDef *ADCx)
{
	__IO uint32_t counter = 0U;
	if ( ADCx == ADC1 )
	{
		if ( ( ADCx->CR2 & ADC_CR2_ADON ) != ADC_CR2_ADON )
		{  
			/* Enable the Peripheral */
			ADCx->CR2 |=  ADC_CR2_ADON;
			
			/* Delay for ADC stabilization time */
			/* Compute number of CPU cycles to wait for */
			counter = ( ADC_STAB_DELAY_US * ( SystemCoreClock / 1000000U ) );
			while ( counter != 0U )
			{
				counter--;
			}
		}
		
		// do something...
	}
	return;
}

/**
*	PWMxInit
**/
void PWMxInit(TIM_TypeDef *TIMx)
{
	// do something...
	
	return;
}

/**
*	PPMxInit
**/
void PPMxInit(TIM_TypeDef *TIMx)
{
	// do something...

	return;
}

/**
*	CLKxInit
**/
void CLKxInit(TIM_TypeDef *TIMx)
{
	LL_TIM_EnableCounter(TIMx);
	return;
}

/**
*	UARTxInit
**/
void UARTxInit(UART_HandleTypeDef *huart)
{
	MX_USART1_UART_Init();
	SetToReceiveMode(huart, rxBuf, UART_BUFFER_SIZE);
	return;
}

/**
*	read_as5600_pos
**/
void read_as5600_pos(void)
{
	HAL_StatusTypeDef status;
	
	status = AS5600_GetRawAngle(as5600_instance, &runtime_data.raw_deg);
	if ( status != HAL_OK )
	{
		unlock_busy_entry();
	}

	return;
}

/**
*	update_position_data
**/
void update_position_data(void)
{
	int32_t pos			= 0;
	int32_t elapse	= HAL_GetTick() - bootup_tick;
	
	runtime_data.raw_deg = ( i2c_rx_buf[0] << 8 ) | ( i2c_rx_buf[1] );
	
	if ( runtime_data.raw_deg < 4096 && elapse > 50 ) 
	{
		/*	calculate servo position value	*/
		runtime_data.rel_deg = pos;
	}
	return;
}

/**
*	update_sensor_data
**/
void update_sensor_data(void)
{
	static uint32_t current = 0, voltage = 0;
	uint32_t vref = 0;
	int16_t temp = 0;
	
	/*
	 *	Current Update
	 */
	runtime_data.current = current;
	
	/*
	 *	Voltage Update
	 */
	runtime_data.voltage = voltage;
	
	/*
	 *	Temperature Update
	 */
	runtime_data.temperature = temp;

	/*
	 *	Vref Update
	 */ 
	runtime_data.vref = vref;
	
	return;
}

/**
*	API_Task
**/
void API_Task(void *argument)
{
	tsk_main(NULL);
	while (1)
	{
		/*	wait notification...	*/
		if ( ulTaskNotifyTake(pdTRUE, portMAX_DELAY) )
		{
			if ( bootstage >= ENDPLACE )
			{
				API_softstarter();
				API_motorctrl();
			}
		}
	}
}

/**
*	API_msg_process
**/
void API_msg_process(void *argument)
{
	while (1)
	{
		MSG_FRAME_t msg;
		xQueueReceive( msgQueueHandle, &msg, portMAX_DELAY );
		switch ( msg.msg )
		{
			case NONE: {
				// do nothing...
				break;
			}
			case READ_AS5600: {
				read_as5600_pos();
				break;
			}
			case UPDATE_POS: {
				update_position_data();
				break;
			}
			case UART_TRANSMIT: {
				API_UARTTransmit(&msg);
				break;
			}
			case PROTECT_SENS_UPDATE: {
				update_sensor_data();
				API_protection_mechanism();
				break;
			}
			case WRITE_EEPROM: {
				API_flash_m95128();
				break;
			}
		}
	}
};

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	MSG_FRAME_t msg;
	/* xHigherPriorityTaskWoken must be initialised to pdFALSE.  If calling
	vTaskNotifyGiveFromISR() unblocks the handling task, and the priority of
	the handling task is higher than the priority of the currently running task,
	then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	msg.msg = UPDATE_POS;
	xQueueSendToBackFromISR(msgQueueHandle,(void *) &msg, &xHigherPriorityTaskWoken);
	/* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
	The macro used to do this is dependent on the port and may be called
	portEND_SWITCHING_ISR. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/* xHigherPriorityTaskWoken must be initialised to pdFALSE.  If calling
	vTaskNotifyGiveFromISR() unblocks the handling task, and the priority of
	the handling task is higher than the priority of the currently running task,
	then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	MSG_FRAME_t msg;

	msg.msg = PROTECT_SENS_UPDATE;
	xQueueSendToBackFromISR(msgQueueHandle,(void *) &msg, &xHigherPriorityTaskWoken);
	/* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
	The macro used to do this is dependent on the port and may be called
	portEND_SWITCHING_ISR. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	
}

void unlock_busy_entry(void)
{
	// do something...

	return;
}
