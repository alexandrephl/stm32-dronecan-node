/*
 * bsp_uart.c
 *
 *  Created on: Dec 2, 2025
 *      Author: alexandrepanhaleux
 */

#include "bsp_uart.h"
#include "main.h"   // for huart2 initialization done by CubeMX

void BSP_UART_Init(void) {
    // CubeMX already initializes USART2 in MX_USART2_UART_Init()
    // Just ensure it's ready (optional checks)
    HAL_UART_Init(&huart2);
}

void BSP_UART_SendChar(uint8_t c) {
    HAL_UART_Transmit(&huart2, &c, 1, HAL_MAX_DELAY);
}

void BSP_UART_Send(const uint8_t *data, size_t len) {
    HAL_UART_Transmit(&huart2, (uint8_t*)data, len, HAL_MAX_DELAY);
}

void BSP_UART_SendString(const char *s) {
    while (*s) {
        BSP_UART_SendChar((uint8_t)*s++);
    }
}
