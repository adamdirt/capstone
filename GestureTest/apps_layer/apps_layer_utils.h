/********************************************************************************
*
* Copyright (c) 2018 STMicroelectronics - All Rights Reserved
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*******************************************************************************/

/*
 * apps_layer_utils.h
 */

#ifndef __APPS_LAYER_UTILS_H
#define __APPS_LAYER_UTILS_H


#include "vl53lmz_plugin_xtalk.h"


#define APPS_LAYER_VL53LMZ_CALIBRATE_XTALK
/* To select strongest (default) or closest target first */
#define DEV_PSORT_CFG_IDX ((uint16_t)0xae64)

struct pipe_grp__sort__cfg_t {
	uint8_t sort__target_order;
	uint8_t sort__cfg__pad_0;
	uint8_t sort__cfg__pad_1;
	uint8_t sort__cfg__pad_2;
};

#define DEV_DSS_CFG_IDX ((uint16_t)0xad38)
/** @struct dci_grp__dss_cfg_t
*   @brief DSS Configuration Parameters
*   @{
*/
struct dci_grp__dss_cfg_t {
	/**
	 * - max_value = 4095
	 * - min_value = 0
	 * - format = unsigned 12.4
	 * - units = Mcps
	 * - resolution = 0.0625
	*/
	uint16_t dss__lower_target_rate_mcps;
	/**
	 * - max_value = 4095
	 * - min_value = 0
	 * - format = unsigned 12.4
	 * - units = Mcps
	 * - resolution = 0.0625
	*/
	uint16_t dss__upper_target_rate_mcps;
	/**
	 * - max_value = 4095
	 * - min_value = 0
	 * - format = unsigned 12.4
	 * - units = effective SPAD's
	 * - resolution = 0.0625
	*/
	uint16_t dss__initial_effective_spads;
	/**
	 * - max_value = 4095
	 * - min_value = 0
	 * - format = unsigned 12.4
	 * - units = effective SPAD's
	 * - resolution = 0.0625
	*/
	uint16_t dss__min_effective_spads;
	/**
	 * - max_value = 4095
	 * - min_value = 0
	 * - format = unsigned 12.4
	 * - units = effective SPAD's
	 * - resolution = 0.0625
	*/
	uint16_t dss__max_effective_spads;
	/**
	 * - max_value = 2
	 * - min_value = 0
	 * - format = unsigned 2.0
	*/
	uint8_t dss__additional_steps;
	uint8_t dss__mode;
	/**
	 * - max_value = 1
	 * - min_value = 0
	 * - format = unsigned 1.7
	 * - resolution = 0.0078
	*/
	uint8_t dss__spatial_mode;
	uint8_t dss_cfg__spare_0;
	uint8_t dss_cfg__spare_1;
	uint8_t dss_cfg__spare_2;
};


union dci_union__block_header_u {
	uint32_t bytes;
	struct {

		uint32_t p_type : 4;

		uint32_t b_size__p_rep : 12;

		uint32_t b_idx__p_idx : 16;
	};
};

#define DEV_RNG_REPEAT_CFG_IDX ((uint16_t)0x5458)
struct dci_grp__rng_repeat_cfg_t {
	uint16_t rng_repeat__ranging_rate_hz;
	uint16_t rng_repeat__fw_per_range_overhead_us;
};
#define  DEV_FW_GRP__ANALOGUE_DYNAMIC_IDX  0xB39C
struct fw_grp__analogue_dynamic_t {
	uint16_t pll_fm_depth;
	uint16_t pll_fm_freq;
	uint8_t vcsel_sel_ctrl_0;
	uint8_t vcsel_sel_ctrl_1;
	uint8_t vcsel_mon_ctrl;
	uint8_t vcselcp_sel_ovrcur_ctrl;
	uint8_t vcselcp_clk_range_sel;
	uint8_t sequencer_output_delay;
	uint8_t vcselcp_bootup_fsm_ext_en;
	uint8_t vcsel_atest1_sel;
	uint8_t vcselcp_mini_sel;
	uint8_t vcselcp_reg_sel;
	uint8_t analogue_dynamic_pad_0;
	uint8_t analogue_dynamic_pad_1;
};

/**
 * Parameter parser description
 *
 * give scanner/formatter ptr etc ...
 */
typedef struct SetParam_t * pSetParam_t;

typedef int (*ScannerType)(const char* Buff, const char *fmt, ...);
typedef int (*CheckerType)(const pSetParam_t SetParam, const void *Param);
typedef char* (*PrinterType)(char* buffer, const pSetParam_t SetParam, const uint8_t *Param);

struct SetParam_t {
    const char *Name;     // Parameter name
    ScannerType Scanner;  //!< if NULL, default to sscanf
    char *ScanFmt;        //!< scanner argument ptr ie sscanf fmt string. If NULL, default to "%d"
    int ParamOffset;      //!<offset in @ref DevParams_t to param
    CheckerType Checker;  //!< checker : if null return nob 0 form scanner as validation
    PrinterType Printer;  //!< printer : print name=value
    int size;
};

struct Params_t {
	int gesture_gui;				// Enable a specific data logging on the Uart for the GUI
	uint32_t Resolution;			// Sensor resolution, only 64 is available
	int RangingPeriod;				// Ranging period in ms
	int IntegrationTime;			// Integration time in ms
	int SensorOrientation;			// See User Manual
	int ranging_ignore_dmax_mm; 	// Any range above this value is not considered by the gesture library
	//Level Control algo tuning parameters
	float lc_stable_threshold;		// Speed Threshold to detect a stable hand
	long lc_stable_time_threshold;	// Duration to detect a stable hand
	int lc_maxDistance_mm;			// Distance associated to 100% Level Control
	int lc_minDistance_mm;			// Distance associated to 0% Level Control
	//Gesture Algo tuning params
	int gesture_selection;			// Only 0 supported : Horizontal SWIPES(Right/Left) + TAP  + Double TAP // More gesture coming soon
	long double_tap_ts_threshold;	// Max duration 2 TAPs (msec)
	int screening_ms;       		// Duration needed with no other prediction to lock current
    int analysis_ms;        		// Time window to compute speed in ms
	int dead_ms;            		// Duration which no new gesture can be predicted
	int closer_mm;					// Min distance difference to declare a new motion as "closer"
	int min_speed_x_mm_s;			// Vx speed Threshold (mm/s)
	int min_speed_y_mm_s;			// Vy speed Threshold (mm/s)
	int min_speed_z_mm_s;			// Vz speed Threshold (mm/s)
	int max_speed_mm_s;				// Max Speed for a valid motion
	float min_vx_vy;				// Threshold Ratio VX vs VY
	float min_vx_vz;				// Threshold Ratio VX vs VZ
	float min_vy_vx;				// Threshold Ratio VY vs VX
	float min_vy_vz;				// Threshold Ratio VY vs VZ
	float min_vz_vx;				// Threshold Ratio VZ vs VX
	float min_vz_vy;				// Threshold Ratio VZ vs VY
	//User Filtering
	int min_user_filtering_mm;		// min distance to locate the user
	int max_user_filtering_mm;		// max distance to locate the user
	int filtering_area_mm;			// area ahead the user where gestures will be filtered
};


#define MAX_COMMAND_BUFFER_SIZE 776
typedef struct {
	int start;
	int stop;
	int get_caldata;
	uint8_t buffer[MAX_COMMAND_BUFFER_SIZE];
	int calibrate;
} CommandData_t;

/* VL53LMZ calibration defines -------------------------------------------------------*/
#define MAX_CALBRATION_WAIT_IN_MS 60000
#define CALIBRATION_READY_CHECK_INTERVAL_IN_MS 50
#define DCI_GRP__INT_TIMING_CONFIG_OFFSET		0xB9F8

#define SINGLE_RANGE_CONTROL_IDX				0xCD5C
#define DCI_PRESENCE_2_CONFIG_IDX 				0xCF78


/* VL53LMZ buffer print defines ------------------------------------------------------*/
#define PRINT_FORMAT_ARRAY 0
#define PRINT_FORMAT_TXT 1



/* VL53LMZ related function prototypes -----------------------------------------------*/

int apps_layer_init_vl53lmz_sensor(VL53LMZ_Configuration *p_lmz_dev);
int apps_layer_change_config_in_place(VL53LMZ_Configuration *p_lmz_dev, uint8_t *config,struct Params_t *p_Params);
int print_buffer(uint8_t *buffer, uint16_t size, uint8_t format);

int perform_calibration(VL53LMZ_Configuration *p_lmz_dev);
int get_calibration(VL53LMZ_Configuration *p_lmz_dev);

int apps_layer_vl53lmz_Configure(VL53LMZ_Configuration *p_lmz_dev,
						struct Params_t *p_Params);


/* Others -------------------------------*/


#endif
