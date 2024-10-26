#include "main.h"

MOTOR_TYPE_t motor_data;

uint8_t pwm_table[4][8] = {

};

uint32_t blockCommutationTable[4][8] = {

};

uint8_t GetHall(void)
{
	uint8_t hall;
	uint8_t regU, regV, regW;
	// do something...
	hall = regU | regV | regW;
	return hall;
}

inline static void BlockCommutationSetDuty(MOTOR_TYPE_t *pMotor)
{
	// do something...
	return;
}

void TimerSetModeRelease(void)
{
	// do something...
	return;
}

void TimerSetModeBrake(void)
{
	// do something...
	return;
}

void BlockCommutate(MOTOR_TYPE_t *pMotor)
{
	int8_t direction = pMotor->dir;

	// do something...
	
	BlockCommutationSetDuty(pMotor);
	return;
}

/**
* HAL_GPIO_EXTI_Callback
**/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if( ( GPIO_Pin == HALL_U_Pin ) || ( GPIO_Pin == HALL_V_Pin ) || ( GPIO_Pin == HALL_W_Pin ) )
	{
		motor_data.Hallstep = GetHall();
	}
	return;
}
