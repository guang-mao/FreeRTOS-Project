#include "main.h"

/**
*	API_Bootup
**/
void API_Bootup(void)
{
	if ( runtime_data.bootup != true )
	{
		/*	convert position uint	*/
		int32_t current_pos = (int32_t) ( ( (float) ( runtime_data.rel_deg ) - y ) / x );
		
		int32_t elapse = HAL_GetTick() - bootup_tick;
		
		switch ( bootstage )
		{
			case SETUP: {
				if ( elapse > 200 )
				{
					UARTxInit(&huart1);

					// do something...

					runtime_data.ppmin_tick 		= HAL_GetTick();
					runtime_data.ppm2mc_cmd			= current_pos;
					motor_data.Hallstep 			= GetHall();
					bootstage 						= STAYPLACE;
				}
				break;
			}
			case STAYPLACE: {
				if ( elapse > 250 )
				{
					runtime_data.pwmduty_limit = ( ( TIM1->ARR + 1 ) * volatile_data.pwmduty_limit ) / 100;
					switch ( setting_data.start_up )
					{
						default: {
						case STARTUP_RTZ:
							/* Will not be executed	*/
							break;
						}
						case STARTUP_RTS: {
							/* Will not be executed	*/
							break;
						}
						case STARTUP_BRAKE: {
							/* Will not be executed	*/
							break;
						}
						case STARTUP_FREE: {
							/* The servo will release if there is no signal after power on.	*/
							if ( runtime_data.raw_cmd == 0 )
							{
								runtime_data.ppm2ss_cmd	= 0;
								runtime_data.motor_stop = true;
								// do something...
							}
							break;
						}
					}
					bootup_tick	= HAL_GetTick();
					bootstage	= ENDPLACE;
				}
				break;
			}
			case ENDPLACE: {
				if ( ( ( (int32_t) runtime_data.ppm2ss_cmd + 1000 ) > current_pos ) &&
					 ( ( (int32_t) runtime_data.ppm2ss_cmd - 1000 ) < current_pos ) )
				{
					runtime_data.bootup 			= true;
					// do something...
				}
				break;
			}
		}
	}
	return;
}

/**
*	API_softstarter
**/
void API_softstarter(void)
{
	/*	Motor Motion Planning	*/
	return;
}

/**
*	API_motorctrl
**/
void API_motorctrl(void)
{
	/* The Main motor control algorithm */
	return;
}

/**
*	API_protection_mechanism
**/
void API_protection_mechanism(void)
{
	// do something...
	return;
}

/**
*	API_cmdacception
**/
void API_cmdacception(uint16_t tmp)
{
	
	runtime_data.raw_cmd = tmp;

	// do something...

	API_cmdProcessing();
	return;
}

/**
* API_cmdProcessing
**/
void API_cmdProcessing(void)
{
	uint32_t	ppm2ss_cmd	= 0;
	uint16_t	raw_cmd		= 0;

	if ( runtime_data.raw_cmd <= runtime_data.cmd_upperbound && runtime_data.raw_cmd >= runtime_data.cmd_lowerbound )
	{

		raw_cmd = runtime_data.raw_cmd;
		
		ppm2ss_cmd = ABS_CENTER * 10 + (int32_t) ( ( raw_cmd - ABS_CENTER ) * 10 );

		if ( ppm2ss_cmd > runtime_data.deg_upperbound )			runtime_data.ppm2ss_cmd = runtime_data.deg_upperbound;
		else if ( ppm2ss_cmd < runtime_data.deg_lowerbound )	runtime_data.ppm2ss_cmd = runtime_data.deg_lowerbound;
		else													runtime_data.ppm2ss_cmd = ppm2ss_cmd;
	}
	return;
}

void cmdlostSwitchList(const uint8_t mode)
{
	__IO int32_t cmd_lost_deg = 0;
	switch ( mode )
	{
		case CMDLOST_RTZ: {
			/* This will not be executed	*/
			break;
		}
		case CMDLOST_RTS: {
			/* This will not be executed	*/
			break;
		}
		case CMDLOST_BRAKE: {
			/* This will not be executed	*/
			break;
		}
		case CMDLOST_FREE: {
			runtime_data.ppm2ss_cmd 		= 0;
			runtime_data.raw_cmd			= 0;
			runtime_data.motor_stop 		= true;
			// do something...
			break;
		}
	}
	return;
}

/**
* API_cmdlostcheck
**/
void API_cmdlostcheck(void)
{
	if ( runtime_data.startup_get_signal != true )
	{
		return;
	}
	
	int32_t cmd_diff_tick = HAL_GetTick() - runtime_data.ppmin_tick;
	
	if ( cmd_diff_tick < 0 )
	{
		cmd_diff_tick  = 0;
	}
	
	if ( cmd_diff_tick > runtime_data.cmd_timeout )
	{
		if ( runtime_data.signal_lost == false )
		{ // set to lost signal condition
			runtime_data.signal_lost = true;
			cmdlostSwitchList(setting_data.cmd_lost);
		}
	}
	else
	{
		if ( runtime_data.signal_lost == true )
		{ // return to NORMAL operation
			runtime_data.motor_stop 			= false;
			runtime_data.signal_lost			= false;
			
			// do something...
		}
	}
	return;
}
