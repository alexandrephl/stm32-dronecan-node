################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QS/qs.c \
../QS/qs_port.c 

OBJS += \
./QS/qs.o \
./QS/qs_port.o 

C_DEPS += \
./QS/qs.d \
./QS/qs_port.d 


# Each subdirectory must supply rules for building sources it contributes
QS/%.o QS/%.su QS/%.cyclo: ../QS/%.c QS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DQ_SPY -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/QS" -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/BSP" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QS

clean-QS:
	-$(RM) ./QS/qs.cyclo ./QS/qs.d ./QS/qs.o ./QS/qs.su ./QS/qs_port.cyclo ./QS/qs_port.d ./QS/qs_port.o ./QS/qs_port.su

.PHONY: clean-QS

