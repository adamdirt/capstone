################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../apps_layer/apps_layer_utils.c 

OBJS += \
./apps_layer/apps_layer_utils.o 

C_DEPS += \
./apps_layer/apps_layer_utils.d 


# Each subdirectory must supply rules for building sources it contributes
apps_layer/%.o apps_layer/%.su apps_layer/%.cyclo: ../apps_layer/%.c apps_layer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/adam/Documents/capstone/CaptstoneRepo/capstone/STM_Workspace/LEDMatrixApp/VL53LMZ_ULD_API/inc" -I"/home/adam/Documents/capstone/CaptstoneRepo/capstone/STM_Workspace/LEDMatrixApp/Platform" -I"/home/adam/Documents/capstone/CaptstoneRepo/capstone/STM_Workspace/LEDMatrixApp/GesturesMZ" -I"/home/adam/Documents/capstone/CaptstoneRepo/capstone/STM_Workspace/LEDMatrixApp/apps_layer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-apps_layer

clean-apps_layer:
	-$(RM) ./apps_layer/apps_layer_utils.cyclo ./apps_layer/apps_layer_utils.d ./apps_layer/apps_layer_utils.o ./apps_layer/apps_layer_utils.su

.PHONY: clean-apps_layer

