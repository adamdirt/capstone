/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "platform.h"

extern I2C_HandleTypeDef 	hi2c1;
extern volatile int IntrCount;

int LMZ_platform_init(VL53LMZ_Platform	*p_platform)
{

	p_platform->address = 0x52;
	return 0;
}

uint8_t RdByte(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	uint8_t status = 0;
	uint8_t data_write[2];
	uint8_t data_read[1];

	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 2, 100);
	status = HAL_I2C_Master_Receive(&hi2c1, p_platform->address, data_read, 1, 100);
	*p_value = data_read[0];
	//uart_printf("read 1 byte\n");
	return status;
}

uint8_t WrByte(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	uint8_t data_write[3];
	uint8_t status = 0;

	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	data_write[2] = value & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 3, 100);
	//uart_printf("write 1 byte\n");
	return status;
}

uint8_t WrMulti(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = HAL_I2C_Mem_Write(&hi2c1, p_platform->address, RegisterAdress,
			I2C_MEMADD_SIZE_16BIT, p_values, size, 65535);
	//uart_printf("write %d bytes\n",size);
	return status;
}

uint8_t RdMulti(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status;
	uint8_t data_write[2];
	data_write[0] = (RegisterAdress>>8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;

	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 2, 10);
	status += HAL_I2C_Master_Receive(&hi2c1, p_platform->address, p_values, size, 400);
    //uart_printf("read %d bytes\n",size);

	return status;
}

uint8_t Reset_Sensor(
		VL53LMZ_Platform *p_platform)
{
	uint8_t status = 0;
	
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	
	/* Set pin LPN to LOW */
//	HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_RESET);
	/* Set pin AVDD to LOW */
	/* Set pin VDDIO  to LOW */
	/* Set 0 to pin DUT_PWR */
//	HAL_GPIO_WritePin(PWR_EN_C_GPIO_Port, PWR_EN_C_Pin, GPIO_PIN_RESET);
	WaitMs(p_platform, 100);

	/* Set pin LPN of to HIGH */
//	HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_SET);
	/* Set pin AVDD of to HIGH */
	/* Set pin VDDIO of  to HIGH */
	/* Set 1 to pin DUT_PWR */
//	HAL_GPIO_WritePin(PWR_EN_C_GPIO_Port, PWR_EN_C_Pin, GPIO_PIN_RESET);
	WaitMs(p_platform, 100);

	return status;
}


void SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;

	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4)
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);

		memcpy(&(buffer[i]), &tmp, 4);
	}
}

uint8_t WaitMs(
		VL53LMZ_Platform *p_platform,
		uint32_t TimeMs)
{
	HAL_Delay(TimeMs);
	return 0;
}

int wait_for_LMZ_interrupt(VL53LMZ_Platform *p_dev)
{
	(void)p_dev;

	__WFI(); // Wait For Interrupt
	if(IntrCount != 0) {
		IntrCount = 0;
		return 1;
	}

	return 0;
}

// Returns a temporary storage big enough to dump the parameters into (about 700 bytes so far)
static char SB_PrivateBuffer[1024];
char* SB_TmpBuffer(void)
{
	return SB_PrivateBuffer;
}
