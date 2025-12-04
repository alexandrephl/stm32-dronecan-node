/*
 * bsp_led.c
 *
 *  Created on: Dec 2, 2025
 *      Author: alexandrepanhaleux
 */

#include "bsp_led.h"
#include "main.h"   // for LD2_Pin, LD2_GPIO_Port

void BSP_LED_Init(void) {
    // already configured in MX_GPIO_Init()
    // but this makes the API official

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	  /* USER CODE BEGIN MX_GPIO_Init_1 */

	  /* USER CODE END MX_GPIO_Init_1 */

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	  /*Configure GPIO pin : LD2_Pin */
	  GPIO_InitStruct.Pin = LD2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

void BSP_LED_On(void) {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

void BSP_LED_Off(void) {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void BSP_LED_Toggle(void) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
