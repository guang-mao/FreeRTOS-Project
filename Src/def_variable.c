#include "main.h"

RUNTIME_DATA_t							runtime_data;

/*
 *	Softstart struct variable
 */
RUNTIME_SOFTSTARTER_t					runtime_ss;

/*
 *	MRAM struct variable
 */
SETTING_DATA_t 							setting_data, reset_data;
VOLATILE_DATA_t							volatile_data;
RECORD_DATA_t							record_data;

/*
 *	Suntries variable
 */
__IO		bool						adc_init_cplt = false, iap_doublecheck = false;
			uint8_t						bootstage 	= 0;
			uint8_t						adc_cvrt_state = 0;
			uint32_t					bootup_tick = 0x7FFFFFFF, firstCMD_tick = 0x7FFFFFFF;
			uint16_t  					Speed	= 0;
			uint16_t 					adc1[NUMBERofADC1], adc2[NUMBERofADC2];
			int16_t 					ABS_CENTER;
			int32_t						i_term = 0;
			
/*
 *	RTOS
 */
			QueueHandle_t				msgQueueHandle	= NULL;
			TaskHandle_t				xAPI_TaskHandle = NULL, xProcessMsgHandle = NULL;
