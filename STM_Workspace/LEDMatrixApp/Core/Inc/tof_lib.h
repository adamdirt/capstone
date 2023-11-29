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

// macros
#define VL53LMZ_8X8_RANGING_FREQ 		((uint8_t) 15U);


uint8_t matrix_app_main();

uint8_t init_tof(VL53LMZ_Configuration *config);



#endif /* INC_TOF_LIB_H_ */
