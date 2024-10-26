/************************************************************************
Title:	  as5600.c - Driver for AMS AS5600 12-Bit Programmable Contactless
                                         Potentiometer
Author:   Nicholas Morrow <nickhudspeth@gmail.com>
File:     as5600.c
Software: STM32Fxxx_HAL_Driver, CMSIS-CORE
Hardware: STM32Fxxx
License:  The MIT License (MIT)
Usage:    Refer to the header file as5600.h for a description of the routines.
          See also example test_as5600.c, if available.
LICENSE:
    Copyright (C) 2018 Nicholas Morrow

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
************************************************************************/

/**********************    INCLUDE DIRECTIVES    ***********************/

#include "as5600.h"
/**********************    GLOBAL VARIABLES    ***********************/
AS5600_TypeDef 	*as5600_instance;
uint8_t 		i2c_rx_buf[2] = {0x00};

/*******************    FUNCTION IMPLEMENTATIONS    ********************/
AS5600_TypeDef *AS5600_New(void) {
    AS5600_TypeDef *a = (AS5600_TypeDef *)calloc(1, sizeof(AS5600_TypeDef));
    return a;
}

HAL_StatusTypeDef AS5600_Init(AS5600_TypeDef *a) {
    HAL_StatusTypeDef status = HAL_OK;
    return status;
}

HAL_StatusTypeDef AS5600_GetRawAngle(AS5600_TypeDef *const a,
                                     uint16_t *const angle) {
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Read_DMA(a->i2cHandle, a->i2cAddr, AS5600_REGISTER_RAW_ANGLE_HIGH, I2C_MEMADD_SIZE_8BIT, &i2c_rx_buf[0], 2);
	return status;
}


