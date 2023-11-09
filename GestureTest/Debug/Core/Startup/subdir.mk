################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32g071rbtx.s 

OBJS += \
./Core/Startup/startup_stm32g071rbtx.o 

S_DEPS += \
./Core/Startup/startup_stm32g071rbtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	$(error unable to generate command line)

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32g071rbtx.d ./Core/Startup/startup_stm32g071rbtx.o

.PHONY: clean-Core-2f-Startup

