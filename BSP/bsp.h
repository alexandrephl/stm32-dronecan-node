/*
 * bsp.h
 *
 *  Created on: Dec 5, 2025
 *      Author: alexandrepanhaleux
 */

#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

//global handles
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

void BSP_Init(void);

// LED
void BSP_LED_Init(void);
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle(void);

// UART
void BSP_UART_Init(void);
void BSP_UART_Send(const uint8_t *data, uint16_t len);

// Button
void BSP_Button_Init(void);

// i2c
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

#endif
