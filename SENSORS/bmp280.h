/*
 * bmp280.h
 *
 *  Created on: Dec 8, 2025
 *      Author: alexandrepanhaleux
 */

#ifndef BMP280_H_
#define BMP280_H_

#include <stdint.h>
#include "bsp.h"

#define BMP280_REG_ID     0xD0
#define BMP280_REG_RESET  0xE0
#define BMP280_REG_CTRL   0xF4
#define BMP280_REG_CONFIG 0xF5

#define BMP280_REG_TEMP_MSB 0xFA

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
} BMP280_Calib;

typedef struct {
    BMP280_Calib calib;
    int32_t t_fine;
} BMP280;

uint8_t BMP280_Init(BMP280 *dev);
uint8_t BMP280_ReadTemperature(BMP280 *dev, float *temperature);

#endif /* BMP280_H_ */
