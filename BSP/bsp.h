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

void BSP_Init(void);

// LED
void BSP_LED_Init(void);
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle(void);

// UART
#include "stm32f4xx_hal.h"

void BSP_UART_Init(void);
void BSP_UART_Send(uint8_t *data, uint16_t len);

// Button
void BSP_Button_Init(void);

#endif
