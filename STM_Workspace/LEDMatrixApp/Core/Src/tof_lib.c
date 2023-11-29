/*
 * tof_lib.c
 *
 *  Created on: Nov 29, 2023
 *      Author: adam
 */

#include "tof_lib.h"

VL53LMZ_Configuration Sensor1Cfg;
VL53LMZ_ResultsData Sensor1Results;


uint8_t init_tof(VL53LMZ_Configuration	*config){
	uint8_t status = VL53LMZ_STATUS_OK;

	config->platform.address = VL53LMZ_DEFAULT_I2C_ADDRESS;

	status |= vl53lmz_init(config);
	HAL_Delay(1000);
	//TODO more initialization
	status |= vl53lmz_set_resolution(config, VL53LMZ_RESOLUTION_8X8);
	status |= vl53lmz_set_ranging_frequency_hz(config, 15);

	HAL_Delay(500);

	return status;
}

uint8_t matrix_app_main(){
	// variables
	uint8_t status = VL53LMZ_STATUS_OK;
	uint8_t ranging = 0;

	// initialization
    status |= init_tof(&Sensor1Cfg);

    // begin app
    while (1){

    }
}

