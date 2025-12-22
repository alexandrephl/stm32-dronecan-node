################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/libcanard/examples/SimpleNode/simple_node.c 

OBJS += \
./Middlewares/libcanard/examples/SimpleNode/simple_node.o 

C_DEPS += \
./Middlewares/libcanard/examples/SimpleNode/simple_node.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/libcanard/examples/SimpleNode/%.o Middlewares/libcanard/examples/SimpleNode/%.su Middlewares/libcanard/examples/SimpleNode/%.cyclo: ../Middlewares/libcanard/examples/SimpleNode/%.c Middlewares/libcanard/examples/SimpleNode/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DQ_SPY -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/SENSORS" -I../Core/Inc -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/BSP" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-libcanard-2f-examples-2f-SimpleNode

clean-Middlewares-2f-libcanard-2f-examples-2f-SimpleNode:
	-$(RM) ./Middlewares/libcanard/examples/SimpleNode/simple_node.cyclo ./Middlewares/libcanard/examples/SimpleNode/simple_node.d ./Middlewares/libcanard/examples/SimpleNode/simple_node.o ./Middlewares/libcanard/examples/SimpleNode/simple_node.su

.PHONY: clean-Middlewares-2f-libcanard-2f-examples-2f-SimpleNode

