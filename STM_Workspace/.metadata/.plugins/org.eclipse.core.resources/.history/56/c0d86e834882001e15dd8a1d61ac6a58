/*
 * tof_lib.h
 *
 *  Created on: Nov 7, 2023
 *      Author: adam
 */

#ifndef INC_TOF_LIB_H_
#define INC_TOF_LIB_H_

#include "vl53l5cx_api.h"
#include "vl53l5cx_plugin_motion_indicator.h"

#include "gwmz.h"
#include "htmz.h"

/*
 * @brief defines for tests
 */

//#define DO_IS_ALIVE_TEST
//#define DO_MOTION_TEST
#define DO_DISTANCE_TEST
//#define DO_GESTURE_TEST



/*
 * @brief Initialize the vl53l5cx sensor and required peripherals.
 * @return (uint8_t) status : 0 if OK
 */
uint8_t init_tof(VL53L5CX_Configuration	*config);

/*
 * @brief With given sensor configuration and sensor results
 * @param (VL53L5CX_Configuration) *config : VL53L5CX configuration structure.
 * @param (VL53L5CX_ResultsData) *results : VL53L5CX results structure.
 * @return (uint8_t) status : 0 if OK
 */
uint8_t sample_once(VL53L5CX_Configuration *config, VL53L5CX_ResultsData *results);

/*
 * @brief determines if motion is detected in current frame.
 * @param (VL53L5CX_ResultsData) *results : VL53L5CX results structure.
 * @return (uint8_t) cur_motion : 0 if no motion, 1 otherwise.
 */
uint8_t detect_motion(VL53L5CX_ResultsData *results);

uint8_t SEN_CopyRangingData(SEN_data_t* sensor_data, VL53L5CX_ResultsData *results);


#endif /* INC_TOF_LIB_H_ */
