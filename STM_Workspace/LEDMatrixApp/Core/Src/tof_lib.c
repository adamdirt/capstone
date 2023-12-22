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

//LED* LastLED; //deprecate?
LED LEDMatrix[MAX_QUADRANT_Y][MAX_QUADRANT_X];

//ActiveLEDFIFO activeLEDs;

//led_color cur_color;
LED reset_command;

brush my_brush;

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

	status |= GW_init(&gest_predictor, &hand_tracker, &sensor_data);

	status |= HT_set_square_resolution(&hand_tracker, &sensor_data, VL53LMZ_RESOLUTION_8X8);

	status |= GW_set_frequency(&gest_predictor, &hand_tracker, &sensor_data, 15);

	//fortest
	status |= SEN_set_orientation(&sensor_data, 4);
	return status;
}

void init_matrix(){
	my_brush.cur_led = &LEDMatrix[0][0];
	my_brush.last_led = &LEDMatrix[0][0];

	my_brush.cur_color = Red;
	my_brush.last_color = Red;

//	reset_command = {255, 255, Black, 0, 0, 0};
	reset_command.row = 255;
	reset_command.column = 255;
	set_led_color(&reset_command, Black);
	reset_command.color = Black;


//	LEDMatrix[0][0].brightness = 255;
	set_led_color(&LEDMatrix[0][0], my_brush.cur_color);

	reset_matrix();

	LED pack_command = {254, 254, Black, 0, 0, 0};

	send_led_data(&pack_command);
	HAL_Delay(500);
}

void set_led_color(LED* led_to_change, led_color new_color){
	led_to_change->color = new_color;
	switch (new_color)
	{
	case Black:
		led_to_change->R = 0;
		led_to_change->G = 0;
		led_to_change->B = 0;
		break;
	case Red:
		led_to_change->R = MAX_BRIGHTNESS;
		led_to_change->G = 0;
		led_to_change->B = 0;
		break;
	case Green:
		led_to_change->R = 0;
		led_to_change->G = MAX_BRIGHTNESS;
		led_to_change->B = 0;
		break;
	case Blue:
		led_to_change->R = 0;
		led_to_change->G = 0;
		led_to_change->B = MAX_BRIGHTNESS;
		break;
	case White:
		led_to_change->R = MAX_BRIGHTNESS;
		led_to_change->G = MAX_BRIGHTNESS;
		led_to_change->B = MAX_BRIGHTNESS;
		break;
	}
	led_to_change->color = new_color;
}

uint8_t send_led_data(LED* updated_LED){
	uint8_t status = 0;
//	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
//							updated_LED->color};
	//test buffer
//	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
//								4, 5, 10};
	//todo update to do row col r g b
//	uint8_t rgb_buffer[3] = {updated_LED->color == Red ? updated_LED->brightness : 0,
//							updated_LED->color == Green ? updated_LED->brightness : 0,
//							updated_LED->color == Blue ? updated_LED->brightness : 0,
//	};


	uint8_t data_buffer[5] = {updated_LED->row, updated_LED->column,
								updated_LED->R, updated_LED->G, updated_LED->B};
//								rgb_buffer[0], rgb_buffer[1], rgb_buffer[2]};

	status |= HAL_I2C_Master_Transmit(&hi2c1, ARDUINO_I2C_ADDR, data_buffer, 5, 100);
	HAL_Delay(5);
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
	Yinterval *= 0.83;

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

//
//uint8_t adamsConversion(float x, float y, uint8_t* pos_buffer){
//	//todo ignore outside of box
//	int ledx_mm, ledy_mm;
//	uint8_t ledx_num, ledy_num; // col,
//	uint8_t out_of_bounds;
//
//	ledx_mm = x + MATRIX_MM_WIDTH_DIV;
//
//		//bounds check
//	out_of_bounds = (x < -MATRIX_MM_WIDTH_DIV || x > MATRIX_MM_WIDTH_DIV ||
//				y < 0 || y > MATRIX_MM_HEIGHT);
//
//	ledy_mm = y - MATRIX_MM_HEIGHT;
//	if (ledy_mm < 0) {ledy_mm = -ledy_mm;}
//
//	if (out_of_bounds) {return 1;}
//	// (ledx,ledy) are coordinates with origin in top left, and +x and +y pointing right and down
//	// now, convert to led
//
//	ledx_num = (uint8_t) (ledx_mm * LED_X_DIV);
//	ledy_num = (uint8_t) (ledy_mm * LED_Y_DIV);
//	// now, BOUNDS CHECKING PLEASE
//
//	if (ledx_num >= MATRIX_WIDTH){ledx_num = MATRIX_WIDTH - 1;}
//	else if (ledx_num < 0){ledx_num = 0;}
//
//	if (ledy_num >= MATRIX_HEIGHT){ledy_num = MATRIX_HEIGHT - 1;}
//	else if (ledy_num < 0){ledy_num = 0;}
//
//	// ok bye
//	pos_buffer[0] = ledy_num; // ie row #
//	pos_buffer[1] = ledx_num; // ie col #
//
//	return 0;
//}


void update_led_matrix(HT_hand_t* cur_hand, led_color new_color){
	uint8_t led_index[2];
	uint8_t row, col;

//	uint8_t out_of_bounds;

	convertSingleSensorPos(cur_hand->hand_x, cur_hand->hand_y, cur_hand->hand_z, led_index);
//	out_of_bounds = adamsConversion(cur_hand->hand_x, cur_hand->hand_y, led_index);

//	if (out_of_bounds){return;}

	row = led_index[0];
	col = led_index[1];

//	send_led_data(&LEDMatrix[row][col]);
//	LastLED = &LEDMatrix[row][col];

	if (LEDMatrix[row][col].color != new_color){ //new led
		if (row > 1 && col > 1){
//			set_led_color(&LEDMatrix[row][col], new_color);
//			FIFO_add(&activeLEDs, &LEDMatrix[row][col]);
			set_led_color(&LEDMatrix[row][col], new_color);
//			LEDMatrix[row][col].color = new_color;
//			LEDMatrix[row][col].brightness = 255;
			send_led_data(&LEDMatrix[row][col]);
		}
	}
}


void reset_matrix(){
	uint8_t row, col;
	send_led_data(&reset_command);
	gest_predictor.gesture.label = GW_NONE;
	HAL_Delay(100);

	for (row = 0; row < MAX_QUADRANT_Y; row++){
			for (col = 0; col < MAX_QUADRANT_X; col++){
				LEDMatrix[row][col].row = row;
				LEDMatrix[row][col].column = col;
//				if((row == 0 && col == 1) || (row == 1 && col == 0) || (row == 1 && col == 1)){
//					LEDMatrix[row][col].R = MAX_BRIGHTNESS;
//					LEDMatrix[row][col].G = MAX_BRIGHTNESS;
//					LEDMatrix[row][col].B = MAX_BRIGHTNESS;
//					send_led_data(&LEDMatrix[row][col]);
//					continue;
//				}
				set_led_color(&LEDMatrix[row][col], Black);
//				LEDMatrix[row][col].color = Black;
//				LEDMatrix[row][col].brightness = 0;
			}
	}
	set_led_color(&LEDMatrix[0][1], White);
	set_led_color(&LEDMatrix[1][0], White);
	set_led_color(&LEDMatrix[1][1], White);
	send_led_data(&LEDMatrix[0][1]);
	send_led_data(&LEDMatrix[1][0]);
	send_led_data(&LEDMatrix[1][1]);
}

uint8_t matrix_app_main(){
	// variables
	uint8_t status = VL53LMZ_STATUS_OK;
//	uint32_t rot_percent = 0;

	// initialization
    status |= init_tof(&Sensor1Cfg);

    status |= init_tracking();

    init_matrix();

	status |= vl53lmz_start_ranging(&Sensor1Cfg);
	ranging = 1;

    // begin app
    while (1){
    	status |= sample_once(&Sensor1Cfg, &Sensor1Results);

    	status |= SEN_CopyRangingData(&sensor_data, &Sensor1Results);

		status |= GW_run(&gest_predictor, &hand_tracker, &sensor_data);
//
		if (gest_predictor.gesture.label == GW_TOWARD){ // brush change
			if (++my_brush.cur_color == 4){my_brush.cur_color = 0;}
			gest_predictor.gesture.label = GW_NONE;
		}
		else if (gest_predictor.gesture.label == GW_UP){
			reset_matrix();
			gest_predictor.gesture.label = GW_NONE;
			continue;
		}
//		if (ranging % 3 == 0) {do_decay();}
//		do_decay();
		if(hand_tracker.hand.found && hand_tracker.hand.hand_z < 600){
			update_led_matrix(&hand_tracker.hand, my_brush.cur_color);
			set_led_color(&LEDMatrix[0][0], my_brush.cur_color);
			send_led_data(&LEDMatrix[0][0]);
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
