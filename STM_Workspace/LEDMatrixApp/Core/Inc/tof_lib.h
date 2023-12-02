/*
 * tof_lib.h
 *
 *  Created on: Nov 29, 2023
 *      Author: adam
 */

#ifndef INC_TOF_LIB_H_
#define INC_TOF_LIB_H_

// includes
#include "vl53lmz_api.h"
#include "gwmz.h"
#include "htmz.h"

// macros for tof
#define VL53LMZ_8X8_RANGING_FREQ 		((uint8_t) 15U);

// macros for lib
#define GESTURE_APP_RANGING_PERIOD		50;

// others
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define PI 3.14159265
#define SINE_VALUE 0.41421356237
#define MAX_QUADRANT_X 5
#define MAX_QUADRANT_Y 5
#define MATRIX_HEIGHT			5
#define MATRIX_WIDTH			5

typedef enum led_color {Black, Red, Orange, Yellow, Green, Blue, Violet, White} led_color;

typedef struct{
	uint8_t row;
	uint8_t column;

	led_color color;
} LED;


uint8_t matrix_app_main();
void convertSingleSensorPos(int x, int y, int z, uint8_t *pos_buffer);

uint8_t init_tof(VL53LMZ_Configuration *config);

uint8_t SEN_CopyRangingData(SEN_data_t* pDest, VL53LMZ_ResultsData *pRangingData);
int* select_target_index(int *target_indices, int zone_index, VL53LMZ_ResultsData *pRangingData);

/*
 * @param data_buff[5] : list of the packet data to be sent
 */
uint8_t send_led_data(LED* updated_LED);

#endif /* INC_TOF_LIB_H_ */
