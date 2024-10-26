#include "main.h"
#include "api_mram.h"

static	uint8_t		mutex_m95xx		= 0;
		uint32_t	m_timeoutTick	= 0;
FLASH_EEPROM_u 		flash_access	= {
	.flash_area	= 0
};

void API_mram_read(uint32_t address, uint8_t *data, uint32_t size);
static uint8_t read_verify_data(uint32_t addr, uint8_t *des, uint16_t size);

/*
 * API_defaultsetting
 */
void API_defaultsetting(uint32_t reg)
{
	if ( reg == SETTING_ADDRESS )
	{
		// servo parameter default value.
	}
	else
	{
		// servo parameter backup value.
	}
	return;
}

/*
 * VOL_defaultsetting
 */
void VOL_defaultsetting(void)
{
	// factory parameter default value.

	return;
}

/*
 *	API_setenvironment
 */
void API_setenvironment(void)
{
	// EEPROM data updated to MCU local memory.

	return;
}

/*
 *	API_flash_m95128
 */
void API_flash_m95128(void)
{
	__IO uint16_t size = 0;
	
	if ( flash_access.flash_area )
	{
		if ( flash_access.area.setting_area )
		{
			size = sizeof(SETTING_DATA_t);
			API_writeflash(SETTING_ADDRESS, size, &setting_data);
			flash_access.area.setting_area = 0;
		}
		
		if ( flash_access.area.reset_area )
		{
			size = sizeof(SETTING_DATA_t);
			API_writeflash(RESET_ADDRESS, size, &reset_data);
			flash_access.area.reset_area = 0;
		}
		
		if ( flash_access.area.volatile_area )
		{
			size = sizeof(VOLATILE_DATA_t);
			API_writeflash(VOLATILE_ADDRESS, size, &volatile_data);
			flash_access.area.volatile_area = 0;
		}
		
		if ( flash_access.area.record_area )
		{
			size = sizeof(RECORD_DATA_t);
			API_writeflash(RECORD_ADDRESS, size, &record_data);
			flash_access.area.record_area = 0;
		}
	}
	return;
}

/*
 *	API_readsetting
 */
void API_readsetting(void)
{
	__IO uint8_t	crc8 = 0;
	__IO uint16_t	size;
	__IO uint16_t	tmp;
	
	size = sizeof(VOLATILE_DATA_t);
	crc8 = read_verify_data(VOLATILE_ADDRESS, (uint8_t *) &volatile_data, size);
	if ( crc8 != volatile_data.checksum )
	{
		VOL_defaultsetting();
		API_writeflash(VOLATILE_ADDRESS, size, &volatile_data);
	}
	
	size = sizeof(RECORD_DATA_t);
	crc8 = read_verify_data(RECORD_ADDRESS, (uint8_t *) &record_data, size);
	if ( crc8 != record_data.checksum )
	{
		memset(&record_data, 0x00, size);
		API_writeflash(RECORD_ADDRESS, size, &record_data);
	}

	size = sizeof(SETTING_DATA_t);
	crc8 = read_verify_data(RESET_ADDRESS, (uint8_t *) &reset_data, size);
	if ( crc8 != reset_data.checksum )
	{
		API_defaultsetting(RESET_ADDRESS);
		API_writeflash(RESET_ADDRESS, size, &reset_data);
	}
	
	crc8 = read_verify_data(SETTING_ADDRESS, (uint8_t *) &setting_data, size);
	if ( crc8 != setting_data.checksum )
	{
		API_defaultsetting(SETTING_ADDRESS);
		API_writeflash(SETTING_ADDRESS, size, &setting_data);
	}
	return;
}

/*
 *	read_vrify_data
 */
static uint8_t read_verify_data(uint32_t addr, uint8_t *des, uint16_t size)
{
	uint8_t crc = 0;
	API_mram_read(addr, des, size);
	API_checksum(des, size, &crc);
	return crc;
}

/*
 *	API_checksum
 */
void API_checksum(uint8_t *array, uint32_t size, uint8_t *checksum)
{
	int i = 0;
	uint8_t crc8 = 0xFF;
  	uint8_t crc_poly = 0x25;
  	size -= 1;   // last byte(checksum)
  	while(size--)
	{
		crc8 ^= *array++;
		for(i = 0; i < 8; i++){
			if(crc8 & 0x80){
				crc8 = (crc8 << 1) ^ crc_poly;
			}else{
				crc8 <<= 1;
			}
		}
	}
	*checksum = crc8 & 0xFF;
	return;
}

/*
 *	API_mram_write
 */
HAL_StatusTypeDef API_mram_write(uint32_t addr, uint32_t size, void *buf)
{
	if ( mutex_m95xx )
		return HAL_BUSY;
	else
		mutex_m95xx = true;
	
	HAL_StatusTypeDef result = HAL_OK;

	// do something...
	
	mutex_m95xx = false;

	return result;
}

/*
 * API_mram_read
 */
void API_mram_read(uint32_t address, uint8_t *data, uint32_t size)
{
	// do something...

	return;
}

uint32_t API_mram_tick(uint8_t val)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint32_t m_tick = HAL_GetTick();
	MSG_FRAME_t msg;
	
	flash_access.flash_area |= val;

	msg.msg = WRITE_EEPROM;
	xQueueSendToBackFromISR(msgQueueHandle,(void *) &msg, &xHigherPriorityTaskWoken);
	/* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
	The macro used to do this is dependent on the port and may be called
	portEND_SWITCHING_ISR. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return m_tick;
}

/*
 * API_writeflash
 */
HAL_StatusTypeDef API_writeflash(uint32_t addr, uint32_t size, void *buf)
{
	HAL_StatusTypeDef result = HAL_OK;

	result = API_mram_write(addr, size, buf);

	return result;
}


