/*
 * tof_lib.c
 *
 *  Created on: Nov 29, 2023
 *      Author: adam
 */

#include "tof_lib.h"
#include <math.h>

extern UART_HandleTypeDef huart6;

// ToF structs
VL53LMZ_Configuration Sensor1Cfg;
VL53LMZ_ResultsData Sensor1Results;

// Gesture Library structs
GW_proc_t gest_predictor;
HT_proc_t hand_tracker;
SEN_data_t sensor_data;


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

uint8_t init_tracking(){
	uint8_t status = 0;

	status |= HT_init(&hand_tracker, &sensor_data);

	status |= HT_set_square_resolution(&hand_tracker, &sensor_data, VL53LMZ_RESOLUTION_8X8);

	status |= HT_set_frequency(&hand_tracker, &sensor_data, 15);

	return status;
}

uint8_t send_led_data(LED* updated_LED){
	uint8_t status = 0;
	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
							updated_LED->red, updated_LED->blue, updated_LED->green};

	status |= HAL_UART_Transmit(&huart6, data_buffer, 5, 1000);

	return status;
}

int* select_target_index(int *target_indices, int zone_index, VL53LMZ_ResultsData *pRangingData)
{
	(void) pRangingData;
	target_indices[0] = zone_index * VL53LMZ_NB_TARGET_PER_ZONE;
	return target_indices;
}

uint8_t sample_once(VL53LMZ_Configuration* config, VL53LMZ_ResultsData* results){
	uint8_t status = VL53LMZ_STATUS_OK;
	uint8_t is_ready = 0;

	while(is_ready == 0){ // poll ;)
		status |= vl53lmz_check_data_ready(config, &is_ready);
	}
	vl53lmz_get_ranging_data(config, results);

	return status;
}

uint8_t SEN_CopyRangingData(SEN_data_t* pDest, VL53LMZ_ResultsData *pRangingData)
{
	float ranging, peak;
	bool valid;
	int idx, result, nb_targets, target_idx[3], target_status;

	if (pDest == NULL || pRangingData == NULL) return -1;

	pDest->timestamp_ms = GET_TIME_STAMP();

	for (idx = 0; idx < pDest->info.nb_zones; idx++)
	{
		select_target_index(target_idx, idx, pRangingData);

		ranging = pRangingData->distance_mm[target_idx[0]]/4.0; // Signed 14.2

		peak = pRangingData->signal_per_spad[target_idx[0]]/2048.0; // Unsigned 21.11
		nb_targets = pRangingData->nb_target_detected[idx];
		target_status = pRangingData->target_status[target_idx[0]];

		valid = (nb_targets > 0)
				&& (target_status == 4 || target_status == 5 || target_status == 6 || target_status == 9 || target_status == 10);
		result = SEN_set_data(pDest, idx, ranging, peak, valid);

		if (result != 0) return result;
	}

	return 0;
}

void convertSingleSensorPos(int x, int y, int z, int *pos_buffer) {
	float maxX = z*SINE_VALUE;
	float maxY = maxX;
	float Xinterval = (maxX*2)/MAX_QUADRANT_X; // divide by 13 and we can overlap column 13 of left sensor with column 1 of right sensor
	float Yinterval = (maxY*2)/MAX_QUADRANT_Y;
	int row = -1*(y/Yinterval);
	int col = x/Xinterval;
	row += (int) (MAX_QUADRANT_Y/2);
	col += (int) (MAX_QUADRANT_X/2);
	if (row > MAX_QUADRANT_Y-1) {
		row = MAX_QUADRANT_Y-1;
	}
	if (row < 0) {
		row = 0;
	}
	if (col > MAX_QUADRANT_X-1) {
		col = MAX_QUADRANT_X-1;
	}
	if (col < 0) {
		col = 0;
	}
	pos_buffer[0] = (uint8_t) row;
	pos_buffer[1] = (uint8_t) col;
}


uint8_t matrix_app_main(){
	// variables
	uint8_t status = VL53LMZ_STATUS_OK;
	uint16_t ranging = 0;

	// initialization
    status |= init_tof(&Sensor1Cfg);

    status |= init_tracking();

	status |= vl53lmz_start_ranging(&Sensor1Cfg);

    // begin app
    while (1){
    	ranging++;

    	status |= sample_once(&Sensor1Cfg, &Sensor1Results);

    	status |= SEN_CopyRangingData(&sensor_data, &Sensor1Results);

		status |= HT_run(&hand_tracker, &sensor_data);

		if(hand_tracker.hand.found){
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		}
		else {
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}


    	if(ranging > 500){
    		vl53lmz_stop_ranging(&Sensor1Cfg);

    		while(1){
    			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    			HAL_Delay(1000);
    		}
    	}
    }
}




