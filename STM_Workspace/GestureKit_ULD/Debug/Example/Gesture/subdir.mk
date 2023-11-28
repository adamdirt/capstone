################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Example/Gesture/gestures_example.c 

OBJS += \
./Example/Gesture/gestures_example.o 

C_DEPS += \
./Example/Gesture/gestures_example.d 


# Each subdirectory must supply rules for building sources it contributes
Example/Gesture/%.o Example/Gesture/%.su Example/Gesture/%.cyclo: ../Example/Gesture/%.c Example/Gesture/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../VL53LMZ_ULD_API/inc -I../Platform -I../apps_layer -I../GesturesMZ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Example-2f-Gesture

clean-Example-2f-Gesture:
	-$(RM) ./Example/Gesture/gestures_example.cyclo ./Example/Gesture/gestures_example.d ./Example/Gesture/gestures_example.o ./Example/Gesture/gestures_example.su

.PHONY: clean-Example-2f-Gesture

