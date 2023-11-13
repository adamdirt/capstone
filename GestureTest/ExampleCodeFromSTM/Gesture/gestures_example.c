/********************************************************************************
*
* Copyright (c) 2018 STMicroelectronics - All Rights Reserved
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document s strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*******************************************************************************/

// VL53LMZ driver
#include "vl53lmz_api.h"
// Application library for this example
#include "apps_layer_utils.h"
// Gesture recognition library
#include "gwmz.h"

#define GESTURE_EXAMPLE_VERSION "1.4.0"

//--------------------------------------------------//
//   Definition of default Gesture algo settings    //
//--------------------------------------------------//
#define DEFAULT_GW_DETECTION_LIMITATION				GW_DETECTION_LIMITATION			// Enabled Gesture: GW_DETECTION_LIMITATION = 95 = all / 0 = Horizontal SWIPES(Right/Left) + TAP  + Double TAP
#define DEFAULT_GESTURE_APP_RESOLUTION 				64
#define DEFAULT_GESTURE_APP_RANGING_PERIOD 			50
#define DEFAULT_GESTURE_APP_INTEGRATION_TIME 		10
#define DEFAULT_GESTURE_APP_MAX_DISTANCE_MM 		GW_MAX_DISTANCE_MM				// Gesture max distance software limit
#define DEFAULT_GESTURE_APP_SENSOR_ORIENTATION 		SEN_ORIENTATION					// See in the User Manual
#define DEFAULT_GESTURE_APP_DOUBLE_TAP_THRESHOLD	DOUBLE_TAP_TS_DIFF_THRESHOLD	// Max duration 2 TAPs (msec)
#define DEFAULT_GESTURE_APP_SCREENING_TIME_MS		GW_SCREENING_TIME_MS 			// Duration needed with no other prediction to lock current
#define DEFAULT_GESTURE_APP_ANALYSIS_TIME_MS		GW_ANALYSIS_TIME_MS     		// Time window to compute speed in ms
#define DEFAULT_GESTURE_APP_DEAD_TIME_MS			GW_DEAD_TIME_MS    				// Duration which no new gesture can be predicted
#define DEFAULT_GESTURE_APP_CLOSER_MM				GW_CLOSER_DELTA_MM				// Distance min between two consecutive gestures to trig the "closer" flag
#define DEFAULT_GESTURE_APP_MIN_SPEED_X_MM_S		GW_MIN_SPEED_X_MM_S				// Vx speed Threshold (mm/s)
#define DEFAULT_GESTURE_APP_MIN_SPEED_Y_MM_S		GW_MIN_SPEED_Y_MM_S				// Vy speed Threshold (mm/s)
#define DEFAULT_GESTURE_APP_MIN_SPEED_Z_MM_S		GW_MIN_SPEED_Z_MM_S				// Vz speed Threshold (mm/s)
#define DEFAULT_GESTURE_APP_MAX_SPEED_MM_S			GW_MAX_SPEED_MM_S				// Max Speed for a valid motion
#define DEFAULT_GESTURE_APP_MIN_VX_VY				GW_MIN_VX_ON_VY					// Threshold Ratio VX vs VY
#define DEFAULT_GESTURE_APP_MIN_VX_VZ				GW_MIN_VX_ON_VZ					// Threshold Ratio VX vs VZ
#define DEFAULT_GESTURE_APP_MIN_VY_VX				GW_MIN_VY_ON_VX					// Threshold Ratio VY vs VX
#define DEFAULT_GESTURE_APP_MIN_VY_VZ				GW_MIN_VY_ON_VZ					// Threshold Ratio VY vs VZ
#define DEFAULT_GESTURE_APP_MIN_VZ_VX				GW_MIN_VZ_ON_VX					// Threshold Ratio VZ vs VX
#define DEFAULT_GESTURE_APP_MIN_VZ_VY				GW_MIN_VZ_ON_VY					// Threshold Ratio VZ vs VY
#define DEFAULT_LC_STABLE_THRESHOLD					LC_STABLE_THRESHOLD				// Speed Threshold to detect a stable hand
#define DEFAULT_LC_STABLE_TIME_THRESHOLD			LC_STABLE_TIME_THRESHOLD		// Duration to detect a stable hand
#define DEFAULT_LC_MAXDISTANCE_MM					LC_MAXDISTANCE_MM				// Distance associated to 100% Level Control
#define DEFAULT_LC_MINDISTANCE_MM					LC_MINDISTANCE_MM				// Distance associated to 0% Level Control
#define DEFAULT_GESTURE_APP_MIN_USER_FILTERING_MM	GW_MIN_USER_FILTERING_MM		// min distance to locate the user
#define DEFAULT_GESTURE_APP_MAX_USER_FILTERING_MM	GW_MAX_USER_FILTERING_MM 		// max distance to locate the user
#define DEFAULT_GESTURE_APP_FILTERING_AREA_MM 		GW_FILTERING_AREA_MM			// area ahead the user where gestures will be filtered


/* VL53LMZ variables -----------------------------------------------------------------*/
VL53LMZ_Configuration 	LMZDev;
VL53LMZ_ResultsData 	RangingData;
volatile int 			IntrCount;
uint32_t 				frame_count = 0; //Counter for every new data ready
extern int8_t 			xtalk64_calibration_stored;
extern uint8_t 			xtalk_calibration_buffer[VL53LMZ_XTALK_BUFFER_SIZE];


/* UART command variables: (defined in main.c) -----------------------------------------------------------------*/
extern volatile int UartComm_CmdReady;
extern int SC_HandleCmd(const char *Buffer);
extern char UartComm_RXBuffer[];
extern void UartComm_Start();


/* Gesture library variables -----------------------------------------------------------------*/
GW_proc_t gest_predictor;
HT_proc_t hand_tracker;
SEN_data_t sensor_data;


struct Params_t Params = {
						.gesture_gui					= 0,
						.Resolution						= DEFAULT_GESTURE_APP_RESOLUTION,
						.RangingPeriod					= DEFAULT_GESTURE_APP_RANGING_PERIOD,
						.IntegrationTime				= DEFAULT_GESTURE_APP_INTEGRATION_TIME,
						.SensorOrientation				= DEFAULT_GESTURE_APP_SENSOR_ORIENTATION,
						.ranging_ignore_dmax_mm 		= DEFAULT_GESTURE_APP_MAX_DISTANCE_MM,

						//Level Control algo tuning parameters
						.lc_stable_threshold 			= DEFAULT_LC_STABLE_THRESHOLD,
						.lc_stable_time_threshold 		= DEFAULT_LC_STABLE_TIME_THRESHOLD,
						.lc_maxDistance_mm 				= DEFAULT_LC_MAXDISTANCE_MM,
						.lc_minDistance_mm 				= DEFAULT_LC_MINDISTANCE_MM,

						//Gesture Algo tuning params
						// -1 => use default algo value
						.gesture_selection 				= DEFAULT_GW_DETECTION_LIMITATION,
						.double_tap_ts_threshold 		= DEFAULT_GESTURE_APP_DOUBLE_TAP_THRESHOLD,
						.screening_ms 					= DEFAULT_GESTURE_APP_SCREENING_TIME_MS,
						.analysis_ms 					= DEFAULT_GESTURE_APP_ANALYSIS_TIME_MS,
						.dead_ms 						= DEFAULT_GESTURE_APP_DEAD_TIME_MS,
						.closer_mm 						= DEFAULT_GESTURE_APP_CLOSER_MM,
						.min_speed_x_mm_s 				= DEFAULT_GESTURE_APP_MIN_SPEED_X_MM_S,
						.min_speed_y_mm_s 				= DEFAULT_GESTURE_APP_MIN_SPEED_Y_MM_S,
						.min_speed_z_mm_s 				= DEFAULT_GESTURE_APP_MIN_SPEED_Z_MM_S,
						.max_speed_mm_s 				= DEFAULT_GESTURE_APP_MAX_SPEED_MM_S,
						.min_vx_vy 						= DEFAULT_GESTURE_APP_MIN_VX_VY,
						.min_vx_vz 						= DEFAULT_GESTURE_APP_MIN_VX_VZ,
						.min_vy_vx 						= DEFAULT_GESTURE_APP_MIN_VY_VX,
						.min_vy_vz 						= DEFAULT_GESTURE_APP_MIN_VY_VZ,
						.min_vz_vx 						= DEFAULT_GESTURE_APP_MIN_VZ_VX,
						.min_vz_vy 						= DEFAULT_GESTURE_APP_MIN_VZ_VY,
						.min_user_filtering_mm			= DEFAULT_GESTURE_APP_MIN_USER_FILTERING_MM,
						.max_user_filtering_mm			= DEFAULT_GESTURE_APP_MAX_DISTANCE_MM + DEFAULT_GESTURE_APP_FILTERING_AREA_MM, //DEFAULT_GESTURE_APP_MAX_USER_FILTERING_MM,
						.filtering_area_mm				= DEFAULT_GESTURE_APP_FILTERING_AREA_MM,

};

// Commands for the FSM example
// The User can use the command "help" in the terminal to get all details on the commands
CommandData_t CommandData = {	.start=0,
								.stop=0,
								.get_caldata=0,
								.calibrate=0,
};


// Function to get the first target data
int* select_target_index(int *target_indices, int zone_index, VL53LMZ_ResultsData *pRangingData)
{
	(void)pRangingData;
	target_indices[0] = zone_index * VL53LMZ_NB_TARGET_PER_ZONE;
	return target_indices;
}

// Format data from LMZ driver to Gesture algorithm
int SEN_CopyRangingData(SEN_data_t* pDest, VL53LMZ_ResultsData *pRangingData)
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


// Function used to print the algo results on the UART port and the data are formatted to be read in a terminal (Tera Term for example)
// The serial baudrate is 921600 by default in this example (see main.c)
void print_uart(){
	if (gest_predictor.gesture.ready)
		{
			// Motion Gesture
			MZ_TRACE("\nGesture(%d) Detected : ", gest_predictor.gesture.label);
			if (gest_predictor.gesture.label == GW_LEFT)
			{
				MZ_TRACE(" LEFT Swipe <= \n");
			}
			else if (gest_predictor.gesture.label == GW_RIGHT)
			{
				MZ_TRACE(" RIGHT Swipe => \n");
			}
			//-------------------------------
			else if (gest_predictor.gesture.label == GW_DOWN)
			{
				MZ_TRACE(" DOWN Swipe \n");
			}
			else if (gest_predictor.gesture.label == GW_UP)
			{
				MZ_TRACE(" UP Swipe \n");
			}
			else if (gest_predictor.gesture.label == GW_TOWARD)
			{
				MZ_TRACE(" TAP []   \n");
			}
			//-------------------------------
			else if (gest_predictor.gesture.label == GW_DOUBLETAP)
			{
				MZ_TRACE(" Double TAP  [[]]   \n");
			}
			else if (gest_predictor.gesture.label == GW_AWAY)
			{
				MZ_TRACE(" Away \n");
			}

			MZ_TRACE("\n");
		}

		if (gest_predictor.lc_state == LC_UNDER_CONTROL)
		{
			MZ_TRACE("LC %3lu, Z = %6.1f, Polar: r = %6.1f, Theta = %4.1f\n", gest_predictor.gesture.lc_currentLevel, hand_tracker.hand.hand_z, gest_predictor.gesture.lc_polar_r, gest_predictor.gesture.lc_polar_theta);
		}
}

// This function is used by the GUI to get and parse the data sent on the UART
// !!! If the format is changed, the GUI can't work anymore !!! //
void print_evk(long timestamp_ms){
	//display by row
	//only working with a resolution 64
	int LC_on = 0; //Hand has been detected
	int i=0;
	char print_buffer[2048];

	if (frame_count < 99999)
			frame_count++;
		else
			frame_count = 0;

	LC_on = (gest_predictor.lc_state == LC_UNDER_CONTROL) ? 1 : 0;

	//Global data format
	int size_global = 159;
	sprintf(&print_buffer[0],"RAN,%5ld,%3d,%10ld,%4.0f,"
			"%4.0f,%4.0f,%5.0f,%5.0f,%5.0f,"
			"%4d,%1d,%10ld,%10ld,%1d,"
			"%1d,%4.0f,%3ld,%5.1f,%5.1f,"
			"%4d,%4.0f,%1d,%1d,%1d,"
			"%1d,%1d,%1d,%1d,%1d,"
			"%1d,,,,,"
			",,,,,"
			",,,,,"
			",,,,,",
			frame_count,//1
			LMZDev.streamcount,//2
			timestamp_ms,//3
			hand_tracker.hand.hand_x,//4
			hand_tracker.hand.hand_y,//5
			hand_tracker.hand.hand_z,//6
			gest_predictor.speed_x_mm_s,//7
			gest_predictor.speed_y_mm_s,//8
			gest_predictor.speed_z_mm_s,//9
			gest_predictor.gesture.label,//10
			gest_predictor.gesture.ready,//11
			gest_predictor.gesture.start_time_ms,//12
			gest_predictor.gesture.end_time_ms,//13
			gest_predictor.gesture.current,//14
			gest_predictor.gesture.probation,//15
			gest_predictor.gesture.distance_mm,//16
			gest_predictor.gesture.lc_currentLevel,//17
			gest_predictor.gesture.lc_polar_r,//18
			gest_predictor.gesture.lc_polar_theta,//19
			//GW_comp_t _computer structure:
			gest_predictor._computer._label,//20
			gest_predictor._computer._distance_mm,//21
			gest_predictor._computer._closer,//22
			gest_predictor._computer._consistent,//23
			gest_predictor._computer._dead,//24
			gest_predictor._computer._ignore,//25
			gest_predictor._computer._same,//26
			gest_predictor._computer._single,//27
			gest_predictor._computer._valid,//28
			LC_on,//29
			hand_tracker.hand.found//30
			);



	//Zone data format
	//RangingData.target_status			-> 2 digits + ',' = 3
	//RangingData.nb_target_detected	-> 2 digits + ',' = 3
	//RangingData.distance_mm			-> 7 digits + ',' = 8
	//RangingData.signal_per_spad		-> 6 digits + ',' = 7
	//hand_tracker.hand.hand_map		-> 1 digit  + ',' = 2
	//sensor_data.valid			        -> 1 digit  + ',' = 2
	//Total size for 1 zone = zone_size = 22
	int size_zone = 22;
	int char_pos = 0;
	for (i=0; i<Params.Resolution; i++){
		char_pos = size_zone*i+size_global;
		sprintf(&print_buffer[char_pos],",%2d",(RangingData.target_status[i]));
		char_pos = char_pos + 3;
		sprintf(&print_buffer[char_pos],",%2d",(RangingData.nb_target_detected[i]));
		char_pos = char_pos + 3;
		sprintf(&print_buffer[char_pos],",%4.0f",(float) (RangingData.distance_mm[i]/4));//%7.2f
		char_pos = char_pos + 5;//8
		sprintf(&print_buffer[char_pos],",%6.0f",(float) (RangingData.signal_per_spad[i]/2048.0));
		char_pos = char_pos + 7;
		sprintf(&print_buffer[char_pos],",%1d",(hand_tracker.hand.hand_map[i]));
		char_pos = char_pos + 2;
		sprintf(&print_buffer[char_pos],",%1d",(sensor_data.valid[i]));
	}
	sprintf(&print_buffer[size_global+size_zone*Params.Resolution],"\n");
	printf(print_buffer);
}


// Recommended Initialization to use the Gesture library
int gesture_library_init_configure(void)
{
	int status = 0;

	//----------------//
	// MANDATORY CODE //
	//----------------//
	//Initialize Gesture algo and use the default parameters
	status = GW_init(&gest_predictor, &hand_tracker, &sensor_data);
	if (status != 0)
	{
		MZ_TRACE("GW_init failed : %d\n", status);
		return status;
	}

	status = HT_set_square_resolution(&hand_tracker, &sensor_data, Params.Resolution);
	if (status != 0)
	{
		MZ_TRACE("HT_set_square_resolution failed : %d\n",status);
		return status;
	}

	status = GW_set_frequency(&gest_predictor, &hand_tracker, &sensor_data, 1000.0/Params.RangingPeriod);
	if (status != 0)
	{
		MZ_TRACE("GW_set_frequency failed : %d\n",status);
		return status;
	}


	//---------------//
	// OPTIONAL CODE //
	//---------------//
	// TO CHANGE THE ALGO PARAMETERS //
	// !!! Gesture recognition performances are not guaranteed if default parameter values are changed !!!///
	status = GW_set_params(&gest_predictor,
			Params.gesture_selection,
			Params.analysis_ms,
			Params.dead_ms,
			Params.screening_ms,
			Params.closer_mm,
			Params.min_speed_x_mm_s,
			Params.min_speed_y_mm_s,
			Params.min_speed_z_mm_s,
			Params.max_speed_mm_s,
			Params.ranging_ignore_dmax_mm,
			Params.min_vx_vy,
			Params.min_vx_vz,
			Params.min_vy_vx,
			Params.min_vy_vz,
			Params.min_vz_vx,
			Params.min_vz_vy,
			-1, // This parameter can't be changed
			Params.min_user_filtering_mm,
			Params.max_user_filtering_mm,
			Params.filtering_area_mm,
			Params.lc_stable_threshold,
			Params.lc_stable_time_threshold,
			Params.lc_maxDistance_mm,
			Params.lc_minDistance_mm,
			Params.double_tap_ts_threshold);
	if (status != 0)
	{
		MZ_TRACE("GW_set_params failed : %d\n",status);
		return status;
	}
	// Set Frequency has to be run again after changing the Gesture params
	status = GW_set_frequency(&gest_predictor, &hand_tracker, &sensor_data, 1000.0/Params.RangingPeriod);
	if (status != 0)
	{
		MZ_TRACE("GW_set_frequency failed : %d\n",status);
		return status;
	}

	// SENSOR ORIENTATION CHANGE : more details in the User Manual //
	status = SEN_set_orientation(&sensor_data, Params.SensorOrientation);
	if (status != 0)
	{
		MZ_TRACE("SEN_set_orientation failed : %d\n",status);
		return status;
	}

	return status;
}

// --------------------- //
// Main Application Code //
// --------------------- //
// This is an implementation example of the Gesture library including :
// - Initialization : Sensor + library
// - VL53LMZ Ranging
// - Gesture Algo processing
// - Xtalk calibration example has been added
int LMZ_gesture_app_main(int start_ranging, uint8_t *xtalk_calib_buf)
{
	/* Variables */
	int status = 0;
	int interrupt_status = 0;
	int ranging = 0;

	MZ_TRACE("VL53LMZ Gesture Demo, version: %s\n", GESTURE_EXAMPLE_VERSION);
	MZ_TRACE("Gesture Library version: %s\n", GW_VERSION);

	/* Initialize platform structure */
    status = LMZ_platform_init(&LMZDev.platform);
	if (status < 0)
	{
		MZ_TRACE("LMZ_platform_init failed\n");
		return -1;
	}


    /* Initialize sensor */
	status = vl53lmz_init(&LMZDev);
	if (status != VL53LMZ_STATUS_OK)
	{
		printf("vl53lmz_init failed : %d\n",status);
		return status;
	}

	// Xtalk Calibration init //
	// Copy the xtalk_calib_buf defined in main()
	// The xtalk buffer will be loaded after the command "enable"
	if (xtalk_calib_buf)
	{
		xtalk64_calibration_stored = 1;
		memcpy(xtalk_calibration_buffer, xtalk_calib_buf, sizeof(xtalk_calibration_buffer) );
	}

	while (1)
	{
		// Manage command through UART
		if (UartComm_CmdReady)
		{
			SC_HandleCmd(UartComm_RXBuffer);
			UartComm_CmdReady = 0;
			UartComm_Start(); // Restart RX
		}

		// START //
		// All initializations are done after a start command: "enable"
		if (CommandData.start)
		{
			if (ranging == 1)
			{
				MZ_TRACE("Sensor already started\n");
				CommandData.start = 0;
				continue;
			}
			else
			{
				// Initialize the Gesture library
				status = gesture_library_init_configure();
				if (status != 0)
				{
					printf("gesture_library_configure failed : %d\n",status);
					return status;
				}
			}

			// Configure VL53LMZ
			status = apps_layer_vl53lmz_Configure(&LMZDev, &Params);
			if (status != VL53LMZ_STATUS_OK)
			{
				MZ_TRACE("ERROR at %s(%d) : VL53LMZ_Configure failed : %d\n",__func__, __LINE__,status);
				return status;
			}

			// Start ranging
			status = vl53lmz_start_ranging(&LMZDev);
			if (status != VL53LMZ_STATUS_OK)
			{
				MZ_TRACE("ERROR at %s(%d) : vl53lmz_start_ranging failed : %d\n",__func__, __LINE__,status);
				return -1;
			}
			// Reset interrupt counter as interrupt line has toggled (twice) during sttof_init call
			IntrCount = 0;
			ranging = 1;
			CommandData.start = 0;
		}

		// STOP //
		// The stop command: "disable" stops the sensor (stop ranging)
		else if (CommandData.stop)
		{
			if (ranging == 0)
			{
				CommandData.stop = 0;
				continue;
			}
			status = vl53lmz_stop_ranging(&LMZDev);
			if (status != VL53LMZ_STATUS_OK)
			{
				MZ_TRACE("ERROR at %s(%d) : vl53lmz_stop_ranging failed : %d\n",__func__, __LINE__,status);
				break;
			}
			// Reset flags
			ranging = 0;
			CommandData.stop = 0;
		}

		// After the start command, the ranging is enabled
		if (ranging)
		{
			interrupt_status = wait_for_LMZ_interrupt(&LMZDev.platform);
			if (interrupt_status == 0)
			{
				continue;
			}
			status = vl53lmz_get_ranging_data(&LMZDev, &RangingData);
			if (status == 0)
			{
				SEN_CopyRangingData(&sensor_data, &RangingData);
				if (status != 0)
				{
					MZ_TRACE("SEN_CopyRangingData failed : %d\n",status);
					return status;
				}

				status = GW_run(&gest_predictor, &hand_tracker, &sensor_data);
				if (status != 0)
				{
					MZ_TRACE("GW_run failed : %d\n",status);
					return status;
				}
				// Print gesture algo and range output
				if (Params.gesture_gui == 1) // gesture_gui is changed by the GUI, by default print_uart() is used
					print_evk(sensor_data.timestamp_ms);
				else
					print_uart();
			} else {
				// Ranging error
				MZ_TRACE("Ranging Error : %d !\n",status);
			}
		}
		else
		{
			// XTALK CALIBRATION //
			/* !!! You have to stop ranging to perform the calibration or get the calibration buffer - send the command "disable" !!!*/
			/* Perform Calibration */
			if (CommandData.calibrate)
			{
				status = perform_calibration(&LMZDev);
				if (status != VL53LMZ_STATUS_OK)
				{
					uart_printf("perform_calibration failed : %d\n",status);
				}
				else
				{
					uart_printf("perform_calibration succeeded");
				}
				CommandData.calibrate = 0;
			}
			/* Get Calibration buffer */
			if (CommandData.get_caldata)
			{
				status = get_calibration(&LMZDev);
				if (status != VL53LMZ_STATUS_OK)
				{
					uart_printf("get_calibration failed : %d\n",status);
				}
				else
				{
					uart_printf("get_calibration succeeded\n");
				}
				CommandData.get_caldata = 0;
			}
		}

	}
	return status;
}


