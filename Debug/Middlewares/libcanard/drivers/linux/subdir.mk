################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/libcanard/drivers/linux/linux.c 

OBJS += \
./Middlewares/libcanard/drivers/linux/linux.o 

C_DEPS += \
./Middlewares/libcanard/drivers/linux/linux.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/libcanard/drivers/linux/%.o Middlewares/libcanard/drivers/linux/%.su Middlewares/libcanard/drivers/linux/%.cyclo: ../Middlewares/libcanard/drivers/linux/%.c Middlewares/libcanard/drivers/linux/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DQ_SPY -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/SENSORS" -I../Core/Inc -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/BSP" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-libcanard-2f-drivers-2f-linux

clean-Middlewares-2f-libcanard-2f-drivers-2f-linux:
	-$(RM) ./Middlewares/libcanard/drivers/linux/linux.cyclo ./Middlewares/libcanard/drivers/linux/linux.d ./Middlewares/libcanard/drivers/linux/linux.o ./Middlewares/libcanard/drivers/linux/linux.su

.PHONY: clean-Middlewares-2f-libcanard-2f-drivers-2f-linux

