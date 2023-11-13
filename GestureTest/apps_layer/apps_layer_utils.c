#include "vl53lmz_api.h"
#include "vl53lmz_plugin_detection_thresholds.h"

#include "apps_layer_utils.h"
#include "platform.h"

#include <string.h>

int8_t 	xtalk64_calibration_stored = 0;
#ifdef APPS_LAYER_VL53LMZ_CALIBRATE_XTALK
uint8_t xtalk_calibration_buffer[VL53LMZ_XTALK_BUFFER_SIZE];
#endif

int apps_layer_init_vl53lmz_sensor(VL53LMZ_Configuration *p_lmz_dev){
	int status = 0;

	// Ideally do a HW reset of the sensor by toggling AVDD & IOVDD
	// Reset_Sensor(&p_lmz_dev->platform);		/* Platform function, not in API */

	// If I2C master does not support "I2C bus clear" feature, need to toggle I2C_SPI_N pin to release I2C bus
	//TODO : Give example code here

	// Finally init sensor (SW reset sequence + sensor boot)
	status = vl53lmz_init(p_lmz_dev);
	if (status != VL53LMZ_STATUS_OK)
	{
		uart_printf("vl53lmz_init failed : %d\n",status);
		return status;
	}

	// Update calibration status
	xtalk64_calibration_stored = 0;

	return status;
}



int apps_layer_vl53lmz_Configure(VL53LMZ_Configuration *p_lmz_dev,
						struct Params_t *p_Params)
{
	int status = 0;

	//uart_printf("===============Program Resolution (offset and xtalk)\n");
	status = vl53lmz_set_resolution(p_lmz_dev, p_Params->Resolution==16 ? VL53LMZ_RESOLUTION_4X4 : VL53LMZ_RESOLUTION_8X8);
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("ERROR at %s(%d) : vl53lmz_set_resolution failed : %d\n",__func__, __LINE__,status);
		return status;
	}

	status = vl53lmz_set_ranging_frequency_hz(p_lmz_dev, (1000/p_Params->RangingPeriod));
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("ERROR at %s(%d) : vl53lmz_set_ranging_period_ms failed : %d\n",__func__, __LINE__,status);
		return status;
	}

	status = vl53lmz_set_integration_time_ms(p_lmz_dev, p_Params->IntegrationTime);
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("ERROR at %s(%d) : vl53lmz_set_integration_time_ms failed : %d\n",__func__, __LINE__,status);
		return status;
	}

	status = vl53lmz_set_xtalk_margin(p_lmz_dev, 50);
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("ERROR at %s(%d) : vl53lmz_set_xtalk_margin failed : %d\n",__func__, __LINE__,status);
		return status;
	}

	// Set Closest target first
	status = vl53lmz_set_target_order(p_lmz_dev, VL53LMZ_TARGET_ORDER_CLOSEST);
	if (status != VL53LMZ_STATUS_OK){
			uart_printf("ERROR at %s(%d) : vl53lmz_set_target_order failed : %d\n",__func__, __LINE__,status);
			return status;
		}

	// Set sharpener
	status = vl53lmz_set_sharpener_percent(p_lmz_dev, 5);
	  if (status != VL53LMZ_STATUS_OK){
	    printf("ERROR at %s(%d) : vl53lmz_set_sharpener_percent failed : %d\n",__func__, __LINE__,status);
	    return(status);
	  }

	if (xtalk64_calibration_stored){
		//uart_printf("===============Program Xtalk\n");
		status = vl53lmz_set_caldata_xtalk(p_lmz_dev, xtalk_calibration_buffer);
		if (status != VL53LMZ_STATUS_OK){
			uart_printf("ERROR at %s(%d) : vl53lmz_set_caldata_xtalk failed : %d\n",__func__, __LINE__,status);
			return status;
		}
		uart_printf("vl53lmz_set_caldata_xtalk succeeded\n");
	}
	return status;
}

#ifdef APPS_LAYER_VL53LMZ_CALIBRATE_XTALK
int perform_calibration(VL53LMZ_Configuration *p_lmz_dev)
{
	int status = 0;
	uart_printf("Xtalk calibration on-going...\n");
	// Run Xtalk calibration (8x8) with 54% target at 600 mm (10 samples)
	status = vl53lmz_calibrate_xtalk(p_lmz_dev, 54, 10, 600);
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("vl53lmz_calibrate_xtalk failed : %d\n",status);
		return status;
	}

	// Read xtalk calibration data and store
	status = vl53lmz_get_caldata_xtalk(p_lmz_dev, xtalk_calibration_buffer);
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("vl53lmz_get_caldata_xtalk failed : %d\n",status);
		return status;
	}
	uart_printf("uint8_t Xtalk_cal_buf[VL53LMZ_XTALK_BUFFER_SIZE] = {\n");
	print_buffer(xtalk_calibration_buffer, VL53LMZ_XTALK_BUFFER_SIZE, PRINT_FORMAT_ARRAY);
	uart_printf("};\n");

	// Update globals
	xtalk64_calibration_stored = 1;

	uart_printf("ok\n");
	return status;
}

int get_calibration(VL53LMZ_Configuration *p_lmz_dev)
{
	int status = 0;
	// Read xtalk calibration data and store
	status = vl53lmz_get_caldata_xtalk(p_lmz_dev, xtalk_calibration_buffer);
	if (status != VL53LMZ_STATUS_OK){
		uart_printf("vl53lmz_get_caldata_xtalk failed : %d\n",status);
		return status;
	}
	uart_printf("uint8_t Xtalk_cal_buf[VL53LMZ_XTALK_BUFFER_SIZE] = {\n");
	print_buffer(xtalk_calibration_buffer, VL53LMZ_XTALK_BUFFER_SIZE, PRINT_FORMAT_ARRAY);
	uart_printf("};\n");

	uart_printf("ok\n");
	return status;
}
#endif


int print_buffer(uint8_t *buffer, uint16_t size, uint8_t format)
{
	int i = 0;
	uint8_t *s = buffer;

	if (size %4 != 0){
		uart_printf("print_buffer failed : data size not a multiple of 4 bytes !\n");
		return -1;
	}

	for(i=0;i<size;i=i+4){
		if (format == PRINT_FORMAT_ARRAY)
			uart_printf("0x%02x, 0x%02x, 0x%02x, 0x%02x,\n",*s,*(s+1),*(s+2),*(s+3));
		else if (format == PRINT_FORMAT_TXT)
			uart_printf("%02X%02X%02X%02X\n",*s,*(s+1),*(s+2),*(s+3));
		s=s+4;
	}
	return 0;
}

/**
 * List of parameters that can be changed by the "set param=value" command
 * This list is used by the Parse_SET() parser function (to be more generic)
 * Add a new entry in this list to expose a new parameter to the command parser
 */
struct SetParam_t SetableParams[]={
		{ .Name= "Resolution",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, Resolution), .size=1 },
		{ .Name= "gesture_gui",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, gesture_gui), .size=1 },
		{ .Name= "RangingPeriod",							.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, RangingPeriod), .size=1 },
		{ .Name= "IntegrationTime",							.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, IntegrationTime), .size=1 },
		{ .Name= "screening_ms",							.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, screening_ms), .size=1 },
		{ .Name= "analysis_ms",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, analysis_ms), .size=1 },
		{ .Name= "dead_ms",									.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, dead_ms), .size=1 },
		{ .Name= "closer_mm",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, closer_mm), .size=1 },
		{ .Name= "min_speed_x_mm_s",						.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, min_speed_x_mm_s), .size=1 },
		{ .Name= "min_speed_y_mm_s",						.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, min_speed_y_mm_s), .size=1 },
		{ .Name= "min_speed_z_mm_s",						.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, min_speed_z_mm_s), .size=1 },
		{ .Name= "double_tap_ts_threshold",					.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, double_tap_ts_threshold), .size=1 },
		{ .Name= "min_vx_vy",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%f",   .ParamOffset = offsetof(struct Params_t, min_vx_vy), .size=1 },
		{ .Name= "min_vx_vz",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%f",   .ParamOffset = offsetof(struct Params_t, min_vx_vz), .size=1 },
		{ .Name= "min_vy_vx",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%f",   .ParamOffset = offsetof(struct Params_t, min_vy_vx), .size=1 },
		{ .Name= "min_vy_vz",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%f",   .ParamOffset = offsetof(struct Params_t, min_vy_vz), .size=1 },
		{ .Name= "min_vz_vx",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%f",   .ParamOffset = offsetof(struct Params_t, min_vz_vx), .size=1 },
		{ .Name= "min_vz_vy",								.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%f",   .ParamOffset = offsetof(struct Params_t, min_vz_vy), .size=1 },
		{ .Name= "gesture_selection",						.Scanner= (int(*)(const char*, const char *,...))sscanf,    .ScanFmt="%d",   .ParamOffset = offsetof(struct Params_t, gesture_selection), .size=1 },
};

char UartComm_TmpBuffer[512];

struct BaseCommand_t{
    const char *Name;
    int (*Parse)(const struct BaseCommand_t *pCmd, const char *Buffer); /** !< Parser is invoked with the command and string right after the command itself*/
    const char *Help;
    const char *Syntax;
    const char *Example;
    int NoAnswer; /**!< When set , successful command do not issue "ok"
                    use it for command that always echo back some answer bad or not */
};

/**
 * ARRAY size of a static declared array
 */
#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(a)  ((sizeof(a) / sizeof(a[0])))
#endif

extern struct Params_t Params;
extern CommandData_t CommandData;

static int Parse_Enable( const struct BaseCommand_t *pCmd, const char *Buffer){
	(void)pCmd;
	(void)Buffer;
	CommandData.start = 1;
    return 0;
}
static int Parse_Disable( const struct BaseCommand_t *pCmd, const char *Buffer){
	(void)pCmd;
	(void)Buffer;
	CommandData.stop = 1;
	return 0;
}

int Parse_SET(const struct BaseCommand_t *pCmd, const char *Buffer){
    int n;
    char *Find;
    char *ParamValue;
    int Status=-1;
    uint8_t *ParamsPtr;
    int i,j;

    Find=strchr(Buffer, '=');
    if( Find == NULL ){
        goto done;
    }
    ParamValue = Find +1 ;
    for( i=0; i< ARRAY_SIZE(SetableParams); i++){
        int ParamLen;
        ParamLen=strlen(SetableParams[i].Name);
        if( strncmp(Buffer+1,SetableParams[i].Name, ParamLen) == 0 ){
            // ok we have it
        	ParamsPtr =(uint8_t*)&Params;
        	ParamsPtr+=SetableParams[i].ParamOffset;
        	// Go through each element (comma separated) if size is not 1
        	for(j=0;j<SetableParams[i].size;j++){
        		if (j!=0) {
        			// Search for the next comma
        			Find=strchr(ParamValue, ',');
        			if( Find == NULL ){
						goto done;
					}
        			// Jump to the string just after the comma
					ParamValue = Find +1 ;
				}
				n = SetableParams[i].Scanner( ParamValue, SetableParams[i].ScanFmt, ParamsPtr );
				if( SetableParams[i].Checker != NULL ){
					// Dedicate checker
					Status=SetableParams[i].Checker( &SetableParams[i], ParamsPtr );
				}
				else{
					// simply use the n retrun
					if( n==1 ){
//						CommandData.set = 1;
						Status = 0;
					}
					else{
//						ParseError("fail: top decode argument value"); //ToDo
					}
				}
				// Manage data types : only %f (4 bytes), %d (4 bytes), %u (1 byte) supported
				if( SetableParams[i].ScanFmt[1]=='f'){
					ParamsPtr += 4;
				}
				else if( SetableParams[i].ScanFmt[1]=='u'){
					ParamsPtr += 1;
				}
				else{
					ParamsPtr += 4;
				}
        	}
            // done we have find params
            break;
        }
    }
    // parama name not found
    if( i >= ARRAY_SIZE(SetableParams) ){
//    	ParseError("unknown param name"); //ToDo
    }
done:
    return Status;
}

static int Parse_Params( const struct BaseCommand_t *pCmd, const char *Buffer){
    int i,j;
    char *TmpStr;
    int n;
    uart_printf("size : %d\n",ARRAY_SIZE(SetableParams));
    for( i=0; i<ARRAY_SIZE(SetableParams); i++){
        	TmpStr=UartComm_TmpBuffer;
            uint8_t *ParamsPtr =(uint8_t*)&Params;
            ParamsPtr+=SetableParams[i].ParamOffset;

            if( SetableParams[i].Scanner==sscanf){
                sprintf(TmpStr, "%s=",SetableParams[i].Name);
                TmpStr=strstr(TmpStr,"=")+1; // move passed skip argname=
                // Go through each element (comma separated) if size is not 1
                for(j=0;j<SetableParams[i].size;j++){
    				// Separate each element by a comma
                	if (j!=0) {
    					sprintf(TmpStr,",");
    					TmpStr++;
    				}
                	// Manage data types : only %f (4 bytes), %d (4 bytes), %u (1 byte) supported
                	if( SetableParams[i].ScanFmt[1]=='f'){
    					n = sprintf(TmpStr, SetableParams[i].ScanFmt, *((float *)ParamsPtr));
    					ParamsPtr = ParamsPtr+4;
    				}
                	else if( SetableParams[i].ScanFmt[1]=='u'){
    					n = sprintf(TmpStr, SetableParams[i].ScanFmt, *((uint8_t *)ParamsPtr));
    					ParamsPtr = ParamsPtr+1;
    				}
    				else{
    					n = sprintf(TmpStr, SetableParams[i].ScanFmt, *((uint32_t *)ParamsPtr));
    					ParamsPtr = ParamsPtr+4;
    				}
    				TmpStr = TmpStr+n;
                }
                sprintf(TmpStr, "\n");
                uart_printf(UartComm_TmpBuffer);
            }
        }
        return 0;
    }

static int Parse_Calibrate(const struct BaseCommand_t *pCmd, const char *Buffer)
{
	int n;
	int Cmd=-1; // 0 = valid
	char *TmpBuffer;
	(void)pCmd;
	TmpBuffer = SB_TmpBuffer();
	n=sscanf(Buffer, "%s", TmpBuffer);

	if( n<1 ){
		uart_printf("%s", "missing arg\n");
		return -1;
	}

	if( strcmp(TmpBuffer,"xtalk")==0 )
	{
		Cmd = 0;
		CommandData.calibrate = 1;
	}
	else
	{  // add here new item to be parse
	}

	// no parsing of target
	if( Cmd == -1 ){
		uart_printf("Calibrate: invalid/wrong calibration data type (%s) : only xtalk supported\n", TmpBuffer);
		return -2;
	}

	return  0;
}

static int Parse_GetCalData(const struct BaseCommand_t *pCmd, const char *Buffer)
{
	int n;
	int Cmd=-1; // 0 = valid
	char *TmpBuffer;

	(void)pCmd;
	TmpBuffer = SB_TmpBuffer();
	n=sscanf(Buffer, "%s", TmpBuffer);

	if( n<1 ){
		uart_printf("%s", "missing arg\n");
		return -1;
	}

	if( strcmp(TmpBuffer,"xtalk")==0 )
	{
		Cmd = 0;
		CommandData.get_caldata = 1;
	}
	else
	{  // add here new item to be parse
	}

	// no parsing of target
	if( Cmd == -1 ){
		uart_printf("GetCalData: invalid/wrong calibration data type (%s) : only xtalk supported", TmpBuffer);
		return -2;
	}

	return  0;
}

static int Parse_Help( const struct BaseCommand_t *pCmd, const char *Unused);

struct BaseCommand_t BaseCmd[]={
		{ .Name= "enable", .Parse = Parse_Enable,
				.Help="Enable sensor ranging.",
				.Syntax="'enable'",
				.Example="'enable' => Enable sensor ranging"
		},
		{ .Name= "disable", .Parse = Parse_Disable,
				.Help="Disable sensor ranging.",
				.Syntax="'disable'",
				.Example="'disable' => Disable sensor ranging",
		},
		{ .Name= "params", .Parse = Parse_Params,
				.Help="Show all input parameters",
				.Syntax="'params'",
				.Example="'params' => Show all input parameters",
				.NoAnswer = 1,
		},
		{ .Name= "set", .Parse = Parse_SET,
				.Help="Set a parameter. See params command to know available parameters and current values.",
				.Syntax="'set param_name=value'",
				.Example="'set RangingPeriod=1'",
		},
		{ .Name= "help", .Parse = Parse_Help,
		        		.Help="Displays this help",
		                .NoAnswer = 1,
		},
		{ .Name= "calibrate", .Parse = Parse_Calibrate,
				.Help="Calibrate the device : only xtalk supported",
				.Syntax="'calibrate xtalk'",
				.NoAnswer = 1,
		},
		{ .Name= "get_caldata", .Parse = Parse_GetCalData,
				.Help="Gets Calibration data from the device",
				.Syntax="'get_caldata xtalk'",
				.NoAnswer = 1
		},
};

static int Parse_Help( const struct BaseCommand_t *pCmd, const char *Unused){
    size_t i;
    char *TmpBuffer;

    (void)pCmd;
    (void)Unused;
    TmpBuffer = SB_TmpBuffer();

    for(i=0; i<ARRAY_SIZE(BaseCmd);i++){
    	TmpBuffer[0]=0;    //star clean string
        strcat(TmpBuffer, BaseCmd[i].Name);
        if( BaseCmd[i].Help != NULL  ){
            strcat(TmpBuffer, "\t");
            strcat(TmpBuffer, BaseCmd[i].Help);
        }
        if( BaseCmd[i].Syntax != NULL  ){
            strcat(TmpBuffer, "\n\tSyntax:\t");
            strcat(TmpBuffer, BaseCmd[i].Syntax);
        }
        if( BaseCmd[i].Example != NULL  ){
			strcat(TmpBuffer, "\n\tExample:\t");
			strcat(TmpBuffer, BaseCmd[i].Example);
		}
        strcat(TmpBuffer, "\n");
        uart_printf(TmpBuffer);
    }
    return 0;
}


static const char StrCmdOk[]="ok\n";

int SC_HandleCmd(const char *Buffer){
    int Status =-1;
    int CmdLen;
    int CmdDecoded=0;
    size_t i;

    for( i=0; i<ARRAY_SIZE(BaseCmd);i++){
        int CmdRet;
        CmdLen=strlen(BaseCmd[i].Name);
        if( strncmp( Buffer, BaseCmd[i].Name, CmdLen) == 0 &&
                (Buffer[CmdLen]==' ' || Buffer[CmdLen]=='\t' || Buffer[CmdLen]==0 )
          ){
        	CmdRet=BaseCmd[i].Parse(&BaseCmd[i], Buffer+CmdLen);
            if( CmdRet == 0 ){
                Status =0;
                if( BaseCmd[i].NoAnswer == 0 )
                	uart_printf(StrCmdOk);
            }
            CmdDecoded=1;
            break;
        }
        // set command
    }
    if( CmdDecoded == 0 ){
        // TODO command not decoded ?
    	// ParseError("unknown command %s", Buffer);
    }
    return Status;
}

