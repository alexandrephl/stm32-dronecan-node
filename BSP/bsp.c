/*
 * bsp.c
 *
 *  Created on: Dec 5, 2025
 *      Author: alexandrepanhaleux
 */

#include "bsp.h"
#include <stdio.h>
#include <string.h>

#define BMP280_ADDR  0x76   // SDO = GND -> addr 0x76

// ======================
//       BSP INIT
// ======================
void BSP_Init(void) {
    BSP_LED_Init();
    BSP_UART_Init();
    BSP_Button_Init();
    BSP_I2C_Init();
}

// ======================
//        LED
// ======================
void BSP_LED_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
	  /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

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
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&huart2);
}

void BSP_UART_Send(const uint8_t *data, uint16_t len) {
    HAL_UART_Transmit(&huart2, (uint8_t*)data, len, HAL_MAX_DELAY);
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
//        I2C
// ======================

void BSP_I2C_Init(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000; // ou 400000
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c1);
}

HAL_StatusTypeDef BSP_I2C_Read(uint16_t devAddr,
                               uint8_t regAddr,
                               uint8_t *data,
                               uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, devAddr << 1, regAddr,
            I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BSP_I2C_Write(uint16_t devAddr,
                                uint8_t regAddr,
                                uint8_t *data,
                                uint16_t len)
{
    return HAL_I2C_Mem_Write(&hi2c1, devAddr << 1, regAddr,
            I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

void BSP_I2C_TestBMP280(void)
{
    uint8_t id = 0;
    if (HAL_I2C_IsDeviceReady(&hi2c1, BMP280_ADDR << 1, 1, 100) == HAL_OK) {
            BSP_UART_Send((uint8_t*)"OK\r\n", 4);
        } else {
            BSP_UART_Send((uint8_t*)"NOK\r\n", 4);
        }
    HAL_Delay(500);

    if (BSP_I2C_Read(BMP280_ADDR, 0xD0, &id, 1) == HAL_OK) {
        char msg[32];
        sprintf(msg, "BMP280 ID = 0x%02X\r\n", id);
        BSP_UART_Send((uint8_t *)msg, strlen(msg));
    }
    else {
        BSP_UART_Send((uint8_t *)"I2C error\r\n", 11);
    }
}

// ======================
//   INTERRUPT HANDLER
// ======================
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) {
        BSP_LED_Toggle();
    }
}
