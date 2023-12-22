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
#define SENSOR2_I2C_ADDRESS			((uint16_t)0x62)

// macros for lib
#define GESTURE_APP_RANGING_PERIOD		50;

// others
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define LPn1_Pin GPIO_PIN_0
#define LPn1_GPIO_Port GPIOC
#define LPn2_Pin GPIO_PIN_1
#define LPn2_GPIO_Port GPIOC


#define PI 3.14159265
#define SINE_VALUE 0.41421356237
#define SINE_TEST 0.2679491924
#define SINE_TEST_2 SINE_TEST
#define MAX_QUADRANT_X 25
#define MAX_QUADRANT_Y 20
#define	MATRIX_HEIGHT  20
#define MATRIX_WIDTH   25
#define ARDUINO_I2C_ADDR 		0x08 << 1
#define NUM_LEDS 	MAX_QUADRANT_X * MAX_QUADRANT_Y
#define NUM_COLORS 				4
#define MAX_BRIGHTNESS 255
#define MATRIX_MM_WIDTH		(float) 460
#define MATRIX_MM_WIDTH_DIV 230
#define MATRIX_MM_HEIGHT	(float) 380
// in form led per mm.
#define LED_X_DIV 			MATRIX_WIDTH/MATRIX_MM_WIDTH
#define LED_Y_DIV 			MATRIX_HEIGHT/MATRIX_MM_HEIGHT

typedef enum led_color {Black, Red, Green, Blue, White} led_color;

typedef struct{
	uint8_t row;
	uint8_t column;

	led_color color;
	led_color R;
	led_color G;
	led_color B;
} LED;

typedef struct{
	LED* active[NUM_LEDS];
	uint16_t length;
	uint16_t first; //first item in fifo
	uint16_t last; // last item in fifo
} ActiveLEDFIFO;

typedef struct{
	led_color cur_color;
	led_color last_color;

	LED* cur_led; // row col
	LED* last_led; // row col
} brush;

void set_led_color(LED* led_to_change, led_color new_color);
uint8_t matrix_app_main();
void convertSingleSensorPos(int x, int y, int z, uint8_t *pos_buffer);
/*
 * proc: convert origin from center to top left, and switch y+ direction, then do LED divide
 * returns 1 on out of bounds error
 */
uint8_t adamsConversion(float x, float y, uint8_t* pos_buffer);
//void FIFO_add(ActiveLEDFIFO* fifo, LED* new_led);
uint8_t init_tof(VL53LMZ_Configuration *config);
void reset_matrix();

uint8_t SEN_CopyRangingData(SEN_data_t* pDest, VL53LMZ_ResultsData *pRangingData);
int* select_target_index(int *target_indices, int zone_index, VL53LMZ_ResultsData *pRangingData);
//void set_led_color(LED* led_to_change, led_color new_color);
/*
 * @param data_buff[5] : list of the packet data to be sent
 */
uint8_t send_led_data(LED* updated_LED);

#endif /* INC_TOF_LIB_H_ */
