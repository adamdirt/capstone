################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../VL53L5CX_ULD_API/src/vl53l5cx_api.c \
../VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.c \
../VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.c \
../VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.c 

OBJS += \
./VL53L5CX_ULD_API/src/vl53l5cx_api.o \
./VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.o \
./VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.o \
./VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.o 

C_DEPS += \
./VL53L5CX_ULD_API/src/vl53l5cx_api.d \
./VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.d \
./VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.d \
./VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.d 


# Each subdirectory must supply rules for building sources it contributes
VL53L5CX_ULD_API/src/%.o VL53L5CX_ULD_API/src/%.su VL53L5CX_ULD_API/src/%.cyclo: ../VL53L5CX_ULD_API/src/%.c VL53L5CX_ULD_API/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Platform -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../VL53L5CX_ULD_API/inc -I../VL53L5CX_ULD_API/src -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32G0xx_HAL_Driver/Src -I"/home/adam/Documents/capstone/CaptstoneRepo/capstone/STM_Workspace/GestureTest/Cortex-M0plus" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-VL53L5CX_ULD_API-2f-src

clean-VL53L5CX_ULD_API-2f-src:
	-$(RM) ./VL53L5CX_ULD_API/src/vl53l5cx_api.cyclo ./VL53L5CX_ULD_API/src/vl53l5cx_api.d ./VL53L5CX_ULD_API/src/vl53l5cx_api.o ./VL53L5CX_ULD_API/src/vl53l5cx_api.su ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.cyclo ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.d ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.o ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.su ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.cyclo ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.d ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.o ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.su ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.cyclo ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.d ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.o ./VL53L5CX_ULD_API/src/vl53l5cx_plugin_xtalk.su

.PHONY: clean-VL53L5CX_ULD_API-2f-src

