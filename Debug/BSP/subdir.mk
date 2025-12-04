################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/bsp_i2c.c \
../BSP/bsp_led.c \
../BSP/bsp_uart.c 

OBJS += \
./BSP/bsp_i2c.o \
./BSP/bsp_led.o \
./BSP/bsp_uart.o 

C_DEPS += \
./BSP/bsp_i2c.d \
./BSP/bsp_led.d \
./BSP/bsp_uart.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o BSP/%.su BSP/%.cyclo: ../BSP/%.c BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"/Users/alexandrepanhaleux/Developer/STM32Cube_Workspace/Blinky1/BSP" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP

clean-BSP:
	-$(RM) ./BSP/bsp_i2c.cyclo ./BSP/bsp_i2c.d ./BSP/bsp_i2c.o ./BSP/bsp_i2c.su ./BSP/bsp_led.cyclo ./BSP/bsp_led.d ./BSP/bsp_led.o ./BSP/bsp_led.su ./BSP/bsp_uart.cyclo ./BSP/bsp_uart.d ./BSP/bsp_uart.o ./BSP/bsp_uart.su

.PHONY: clean-BSP

