/*
 * bsp.c
 *
 *  Created on: Dec 5, 2025
 *      Author: alexandrepanhaleux
 */

#include "bsp.h"

// Global peripherals
UART_HandleTypeDef huart2;

// ======================
//       BSP INIT
// ======================
void BSP_Init(void) {
    BSP_LED_Init();
    BSP_UART_Init();
    BSP_Button_Init();
}

// ======================
//        LED
// ======================
void BSP_LED_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
	  /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void BSP_LED_On(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

void BSP_LED_Off(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

void BSP_LED_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

// ======================
//        UART
// ======================
void BSP_UART_Init(void) {

    __HAL_RCC_USART2_CLK_ENABLE();

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&huart2);
}

void BSP_UART_Send(uint8_t *data, uint16_t len) {
    HAL_UART_Transmit(&huart2, data, len, HAL_MAX_DELAY);
}

// ======================
//        Button
// ======================
void BSP_Button_Init(void) {

    __HAL_RCC_GPIOC_CLK_ENABLE();   // bouton sur GPIOC

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// ======================
//   INTERRUPT HANDLER
// ======================
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) {
        BSP_LED_Toggle();
    }
}
