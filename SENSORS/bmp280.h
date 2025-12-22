/******************************************************************************
* @file    bmp280.h
* @brief   BMP280 sensor driver interface
*
*          Provides function prototypes and data structures required
*          to initialize and read temperature and pressure from the
*          Bosch BMP280 sensor.
*
*          Output units:
*            - Temperature: degrees Celsius (°C)
*            - Pressure: hectopascal (hPa)
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/

#ifndef BMP280_H_
#define BMP280_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "bsp.h"
#include "stm32f4xx_hal.h"

/* private defines */
#define BMP280_REG_ID     0xD0
#define BMP280_REG_RESET  0xE0
#define BMP280_REG_CTRL   0xF4
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_TEMP_MSB 0xFA
#define BMP280_REG_PRESS_MSB   0xF7

/* Calibration structure -----------------------------------------------------*/
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_Calib;

/* BMP28- evice state -------------------------------------------------------*/
typedef struct {
    BMP280_Calib calib;
    int32_t t_fine;
} BMP280;

/* Public API ---------------------------------------------------------------*/
uint8_t BMP280_Init(BMP280 *dev);
uint8_t BMP280_ReadTemperature(BMP280 *dev, float *temperature);
uint8_t BMP280_ReadValues(BMP280 *dev, float *temperature, float *pressure);

#endif /* BMP280_H_ */
