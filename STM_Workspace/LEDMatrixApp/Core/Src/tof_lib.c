/*
 * tof_lib.c
 *
 *  Created on: Nov 29, 2023
 *      Author: adam
 */

#include "tof_lib.h"
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

extern uint8_t ranging;

// ToF structs
VL53LMZ_Configuration Sensor1Cfg;
VL53LMZ_ResultsData Sensor1Results;

// Gesture Library structs
GW_proc_t gest_predictor;
HT_proc_t hand_tracker;
SEN_data_t sensor_data;

uint16_t activeled_index;

LED* LastLED; //deprecate?
LED LEDMatrix[MAX_QUADRANT_Y][MAX_QUADRANT_X];

ActiveLEDFIFO activeLEDs;

led_color cur_color;
LED reset_command;

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
	uint8_t row, col;

	status |= GW_init(&gest_predictor, &hand_tracker, &sensor_data);

	status |= HT_set_square_resolution(&hand_tracker, &sensor_data, VL53LMZ_RESOLUTION_8X8);

	status |= GW_set_frequency(&gest_predictor, &hand_tracker, &sensor_data, 15);

	//fortest
//	status |= SEN_set_orientation(&sensor_data, 0);

	for (row = 0; row < MAX_QUADRANT_Y; row++){
		for (col = 0; col < MAX_QUADRANT_X; col++){
			LEDMatrix[row][col].row = row;
			LEDMatrix[row][col].column = col;
			LEDMatrix[row][col].color = Black;
			LEDMatrix[row][col].brightness = 0;

//			set_led_color(&LEDMatrix[row][col], Black);
		}
	}
	LastLED = &LEDMatrix[0][0];
	cur_color = Blue;

	activeLEDs.first = 0;
	activeLEDs.last = 0;
	activeLEDs.length = 0;

	reset_command.row = 255;
	reset_command.column = 255;
	reset_command.color = Black;

	send_led_data(&reset_command);

	return status;
}

//void set_led_color(LED* led_to_change, led_color new_color){
//	switch (new_color)
//	{
//	case Black:
//		led_to_change->red = 0;
//		led_to_change->green = 0;
//		led_to_change->blue = 0;
//		break;
//	case Red:
//		led_to_change->red = 255;
//		led_to_change->green = 0;
//		led_to_change->blue = 0;
//		break;
//	case Green:
//		led_to_change->red = 0;
//		led_to_change->green = 255;
//		led_to_change->blue = 0;
//		break;
//	case Blue:
//		led_to_change->red = 0;
//		led_to_change->green = 0;
//		led_to_change->blue = 255;
//		break;
//	}
//	led_to_change->color = new_color;
//}

uint8_t send_led_data(LED* updated_LED){
	uint8_t status = 0;
//	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
//							updated_LED->color};
	//test buffer
//	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
//								4, 5, 10};
	//todo update to do row col r g b
	uint8_t rgb_buffer[3] = {updated_LED->color == Red ? updated_LED->brightness : 0,
							updated_LED->color == Green ? updated_LED->brightness : 0,
							updated_LED->color == Blue ? updated_LED->brightness : 0,
	};

	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
								rgb_buffer[0], rgb_buffer[1], rgb_buffer[2]};

	status |= HAL_I2C_Master_Transmit(&hi2c1, ARDUINO_I2C_ADDR, data_buffer, 5, 100);

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
//		valid &= (ranging < 600);
		result = SEN_set_data(pDest, idx, ranging, peak, valid);

		if (result != 0) return result;
	}

	return 0;
}

void convertSingleSensorPos(int x, int y, int z, uint8_t *pos_buffer) {
	float maxX = z*SINE_VALUE;
	float maxY = z*SINE_VALUE;
	float Xinterval = (maxX*2)/MAX_QUADRANT_X; // divide by 13 and we can overlap column 13 of left sensor with column 1 of right sensor
	float Yinterval = (maxY*2)/MAX_QUADRANT_Y;

	//test

	Xinterval *= 0.85;
	Yinterval *= 0.85;

	int row = -1*(y/Yinterval);
	int col = x/Xinterval;
	row += (int) (MAX_QUADRANT_Y/2);
	col += (int) (MAX_QUADRANT_X/2);
	if (row > MATRIX_HEIGHT-1) {
		row = MATRIX_HEIGHT-1;
	}
	if (row < 0) {
		row = 0;
	}
	if (col > MATRIX_WIDTH-1) {
		col = MATRIX_WIDTH-1;
	}
	if (col < 0) {
		col = 0;
	}
	pos_buffer[0] = (uint8_t) row;
	pos_buffer[1] = (uint8_t) col;
}


void update_led_matrix(HT_hand_t* cur_hand, led_color new_color){
	uint8_t led_index[2];
	uint8_t row, col;

	convertSingleSensorPos(cur_hand->hand_x, cur_hand->hand_y, cur_hand->hand_z, led_index);
	row = led_index[0];
	col = led_index[1];

//	send_led_data(&LEDMatrix[row][col]);
//	LastLED = &LEDMatrix[row][col];

	if (LEDMatrix[row][col].color != new_color){ //new led
//		set_led_color(&LEDMatrix[row][col], new_color);
//		FIFO_add(&activeLEDs, &LEDMatrix[row][col]);
		LEDMatrix[row][col].color = new_color;
		LEDMatrix[row][col].brightness = 255;
		send_led_data(&LEDMatrix[row][col]);
	}
}

void FIFO_add(ActiveLEDFIFO* fifo, LED* new_led){
	if (++(fifo->last) == NUM_LEDS){ // get and increment last index, check if at max, reset if true
		fifo->last = 0;
	}
	fifo->active[fifo->last] = new_led; // last should be between 0 and num_led-1
	fifo->length++;
}

void FIFO_pop(ActiveLEDFIFO* fifo){
	if(++(fifo->first) == NUM_LEDS){ // get and increment first index, check if at max, reset if true
		fifo->first = 0;
	}
	fifo->length--;
}

void do_decay(){
	if (activeLEDs.length == 0) {return;}

	uint16_t i = activeLEDs.first;

	while (i != activeLEDs.last){
		activeLEDs.active[i]->brightness-=3;
		send_led_data(activeLEDs.active[i]);
		if (++i == NUM_LEDS){i = 0;}
	}

	if (activeLEDs.active[activeLEDs.first]->brightness < 20){
		activeLEDs.active[activeLEDs.first]->color = Black;
		activeLEDs.active[activeLEDs.first]->brightness = 0;
		send_led_data(activeLEDs.active[activeLEDs.first]);
		FIFO_pop(&activeLEDs);
	}

}

void reset_matrix(){
	send_led_data(&reset_command);
	gest_predictor.gesture.label = GW_NONE;
	HAL_Delay(50);

}

uint8_t matrix_app_main(){
	// variables
	uint8_t status = VL53LMZ_STATUS_OK;
//	uint32_t rot_percent = 0;

	// initialization
    status |= init_tof(&Sensor1Cfg);

    status |= init_tracking();

	status |= vl53lmz_start_ranging(&Sensor1Cfg);
	ranging = 1;

    // begin app
    while (1){
    	status |= sample_once(&Sensor1Cfg, &Sensor1Results);

    	status |= SEN_CopyRangingData(&sensor_data, &Sensor1Results);

		status |= GW_run(&gest_predictor, &hand_tracker, &sensor_data);
//
		if (gest_predictor.gesture.label == GW_TOWARD){ // brush change
			if (++cur_color == 4){cur_color = 0;}
			gest_predictor.gesture.label = GW_NONE;
		}
//		else if (gest_predictor.gesture.label == GW_UP){
//			reset_matrix();
//			continue;
//		}
//		if (ranging % 3 == 0) {do_decay();}
//		do_decay();
		if(hand_tracker.hand.found && hand_tracker.hand.hand_z < 600){
			update_led_matrix(&hand_tracker.hand, cur_color);
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

//Level control attempt
//    		if(gest_predictor.lc_state == LC_UNDER_CONTROL){
//    			rot_percent = gest_predictor.gesture.lc_currentLevel;
//    		}

		}
		else {
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}


    	if(!ranging){
    		while(1){
    			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    			HAL_Delay(1000);
    		}
    	}
    }
}