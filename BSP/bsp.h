/******************************************************************************
* @file    bsp.h
* @brief   Board Support Package public interface
*
*          Declares board-level hardware abstraction functions for:
*            - CAN transmission
*            - I2C access
*            - UART logging
*            - LEDs and basic peripherals
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/

#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/*global handles*/
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

void BSP_Init(void);

/*LED*/
void BSP_LED_Init(void);
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle(void);

/*UART*/
void BSP_UART_Init(void);
void BSP_UART_Send(const uint8_t *data, uint16_t len);

/*Button*/
void BSP_Button_Init(void);

/*i2c*/
void BSP_I2C_Init(void);
HAL_StatusTypeDef BSP_I2C_Read(uint16_t devAddr,
                               uint8_t regAddr,
                               uint8_t *data,
                               uint16_t len);
HAL_StatusTypeDef BSP_I2C_Write(uint16_t devAddr,
                                uint8_t regAddr,
                                uint8_t *data,
                                uint16_t len);
void BSP_I2C_TestBMP280(void);

/*CAN*/
void BSP_CAN_SendFrame(uint32_t can_id, const uint8_t *data, uint8_t len);
bool BSP_CAN_ReceiveFrame(uint32_t* can_id_out, uint8_t* data_out, uint8_t* len_out);

#endif
