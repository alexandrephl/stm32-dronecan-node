################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.c 

OBJS += \
./Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.o 

C_DEPS += \
./Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/libcanard/drivers/avr/avr-can-lib/demo/%.o Middlewares/libcanard/drivers/avr/avr-can-lib/demo/%.su Middlewares/libcanard/drivers/avr/avr-can-lib/demo/%.cyclo: ../Middlewares/libcanard/drivers/avr/avr-can-lib/demo/%.c Middlewares/libcanard/drivers/avr/avr-can-lib/demo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DQ_SPY -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/SENSORS" -I../Core/Inc -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/BSP" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-libcanard-2f-drivers-2f-avr-2f-avr-2d-can-2d-lib-2f-demo

clean-Middlewares-2f-libcanard-2f-drivers-2f-avr-2f-avr-2d-can-2d-lib-2f-demo:
	-$(RM) ./Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.cyclo ./Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.d ./Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.o ./Middlewares/libcanard/drivers/avr/avr-can-lib/demo/main.su

.PHONY: clean-Middlewares-2f-libcanard-2f-drivers-2f-avr-2f-avr-2d-can-2d-lib-2f-demo

