/*
 * bmp280.c
 *
 *  Created on: Dec 8, 2025
 *      Author: alexandrepanhaleux
 */

#include "bmp280.h"

#define BMP280_ADDR  0x76

static uint8_t bmp280_read8(uint8_t reg)
{
    uint8_t val = 0;
    BSP_I2C_Read(BMP280_ADDR, reg, &val, 1);
    return val;
}

static uint16_t bmp280_read16(uint8_t reg)
{
    uint8_t buf[2];
    BSP_I2C_Read(BMP280_ADDR, reg, buf, 2);
    return (buf[0] << 8) | buf[1];
}

uint8_t BMP280_Init(BMP280 *dev)
{
    uint8_t id = bmp280_read8(BMP280_REG_ID);
    if (id != 0x58)
        return 0; // FAIL

    // Read calibration
    dev->calib.dig_T1 = bmp280_read16(0x88);
    dev->calib.dig_T2 = (int16_t)bmp280_read16(0x8A);
    dev->calib.dig_T3 = (int16_t)bmp280_read16(0x8C);

    // Set normal mode, temp oversampling x1
    uint8_t ctrl = 0x27;
    BSP_I2C_Write(BMP280_ADDR, BMP280_REG_CTRL, &ctrl, 1);

    return 1;
}

uint8_t BMP280_ReadTemperature(BMP280 *dev, float *temperature)
{
    uint8_t buf[3];
    if (BSP_I2C_Read(BMP280_ADDR, BMP280_REG_TEMP_MSB, buf, 3) != HAL_OK)
        return 0;

    int32_t adc = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);

    // Compensation formula
    int32_t var1 = ((((adc >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
                    ((int32_t)dev->calib.dig_T2)) >> 11;

    int32_t var2 = (((((adc >> 4) - ((int32_t)dev->calib.dig_T1)) *
                       ((adc >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) *
                     ((int32_t)dev->calib.dig_T3)) >> 14;

    dev->t_fine = var1 + var2;

    *temperature = (dev->t_fine * 5 + 128) >> 8;
    *temperature /= 100.0f;

    return 1;
}
