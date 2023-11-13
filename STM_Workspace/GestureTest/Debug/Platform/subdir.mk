################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/platform.c 

OBJS += \
./Platform/platform.o 

C_DEPS += \
./Platform/platform.d 


# Each subdirectory must supply rules for building sources it contributes
Platform/%.o Platform/%.su Platform/%.cyclo: ../Platform/%.c Platform/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Platform -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../VL53L5CX_ULD_API/inc -I../VL53L5CX_ULD_API/src -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32G0xx_HAL_Driver/Src -I"/home/adam/Documents/capstone/CaptstoneRepo/capstone/STM_Workspace/GestureTest/Cortex-M0plus" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Platform

clean-Platform:
	-$(RM) ./Platform/platform.cyclo ./Platform/platform.d ./Platform/platform.o ./Platform/platform.su

.PHONY: clean-Platform

