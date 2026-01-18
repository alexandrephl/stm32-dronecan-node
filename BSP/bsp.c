/******************************************************************************
* @file    bsp.c
* @brief   Board Support Package
*
*          Provides low-level hardware abstraction for:
*            - CAN
*            - I2C
*            - UART
*            - LEDs and board-specific peripherals
*
*          This layer isolates STM32 HAL usage from the application logic.
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/

#include "bsp.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include <stdbool.h>


/* External peripherals handles */
extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

/* Private define ------------------------------------------------------------*/
#define BMP280_ADDR  0x76   // SDO = GND -> addr 0x76
#define CANARD_CAN_FRAME_EFF 0x80000000;

/* BSP INIT ------------------------------------------------------------------*/

/**
* @brief Initializes all board-level peripherals.
*
* @param None
* @retval None
*/
void BSP_Init(void) {
    BSP_LED_Init();
    BSP_UART_Init();
    BSP_Button_Init();
    BSP_I2C_Init();
}

/* LED -----------------------------------------------------------------------*/

/**
* @brief Initializes the onboard LED GPIO.
*
* @param None
* @retval None
*/
void BSP_LED_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Enable GPIO clock for LED pin */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure LED pin as push-pull output */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
* @brief Turns the LED on.
*/
void BSP_LED_On(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

/**
* @brief Turns the LED off.
*/
void BSP_LED_Off(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/**
* @brief Toggles the LED state.
*/
void BSP_LED_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

/* UART ----------------------------------------------------------------------*/

/**
* @brief Initializes UART used for debug output.
*
* @param None
* @retval None
*/
void BSP_UART_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Enable clocks for GPIO and USART2 */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure UART TX/RX pins */
    GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure UART peripheral */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&huart2);
}

/**
* @brief Sends data over UART.
*
* @param data Pointer to data buffer
* 		 len  Number of bytes to send
* @retval None
*/
void BSP_UART_Send(const uint8_t *data, uint16_t len) {
	/* Blocking transmit is acceptable for debug output */
    HAL_UART_Transmit(&huart2, (uint8_t*)data, len, HAL_MAX_DELAY);
}

/* BUTTON --------------------------------------------------------------------*/

/**
* @brief Initializes the user button with interrupt.
*
* @param None
* @retval None
*/
void BSP_Button_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Enable GPIO clock for button */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure button pin as external interrupt */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Enable and configure EXTI interrupt */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* I2C -----------------------------------------------------------------------*/

/**
* @brief Initializes I2C bus for sensor communication.
*
* @param None
* @retval None
*/
void BSP_I2C_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks for GPIO and I2C */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* Configure I2C SCL/SDA pins */
    GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure I2C peripheral */
    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 400000;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c1);
}

/**
* @brief Reads data from an I2C device register.
*
* @param devAddr Device I2C address
* @param regAddr Register address
* @param data    Pointer to receive buffer
* @param len     Number of bytes to read
* @retval HAL status
*/
HAL_StatusTypeDef BSP_I2C_Read(uint16_t devAddr,
                               uint8_t regAddr,
                               uint8_t *data,
                               uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, devAddr << 1, regAddr,
            I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

/**
* @brief Writes data to an I2C device register.
*
* @param devAddr Device I2C address
* @param regAddr Register address
* @param data    Pointer to transmit buffer
* @param len     Number of bytes to write
* @retval HAL status
*/
HAL_StatusTypeDef BSP_I2C_Write(uint16_t devAddr,
                                uint8_t regAddr,
                                uint8_t *data,
                                uint16_t len)
{
    return HAL_I2C_Mem_Write(&hi2c1, devAddr << 1, regAddr,
            I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

/* CAN -----------------------------------------------------------------------*/

/**
* @brief Sends a CAN frame using STM32 HAL.
*
* This function is responsible for:
*            - Preparing an extended CAN identifier
*            - Waiting for a free transmit mailbox
*            - Transmitting the CAN frame
*
* @param can_id Extended CAN identifier
* @param data   Pointer to payload data
* @param len    Payload length in bytes
* @retval None
*/
void BSP_CAN_SendFrame(uint32_t can_id, const uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef header;
    uint32_t mailbox;

    /* DroneCAN uses 29-bit extended identifiers */
    header.IDE = CAN_ID_EXT;
    header.ExtId = can_id & 0x1FFFFFFF;
    header.RTR = CAN_RTR_DATA;
    header.DLC = len;
    header.TransmitGlobalTime = DISABLE;

    /* Wait until a transmit mailbox is available */
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
    {
        // wait
    }

    /* Submit frame to CAN peripheral */
    HAL_CAN_AddTxMessage(&hcan1, &header, data, &mailbox);
}

bool BSP_CAN_ReceiveFrame(uint32_t* can_id_out, uint8_t* data_out, uint8_t* len_out)
{
    CAN_RxHeaderTypeDef header;
    uint8_t data[8];

    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) == 0)
        return false;

    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &header, data) != HAL_OK)
        return false;

    if (header.IDE == CAN_ID_EXT){
        *can_id_out = header.ExtId | CANARD_CAN_FRAME_EFF;
    }
    else
        *can_id_out = header.StdId;

    *len_out = header.DLC;
    memcpy(data_out, data, *len_out);

    return true;
}

/* INTERRUPTS -----------------------------------------------------------------------*/

/**
* @brief GPIO EXTI callback.
*
* @param GPIO_Pin GPIO pin number
* @retval None
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) {
        BSP_LED_Toggle();
    }
}
