/*
 * bsp_uart.h
 *
 *  Created on: Dec 2, 2025
 *      Author: alexandrepanhaleux
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stddef.h>

void BSP_UART_Init(void);
void BSP_UART_SendChar(uint8_t c);
void BSP_UART_Send(const uint8_t *data, size_t len);
void BSP_UART_SendString(const char *s);

extern UART_HandleTypeDef huart2;

#endif /* BSP_UART_H_ */
