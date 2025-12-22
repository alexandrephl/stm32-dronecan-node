/******************************************************************************
* @file    bmp280.c
* @brief   BMP280 pressure and temperature sensor driver
*
*          Implements sensor initialization, calibration loading,
*          and compensated temperature and pressure measurements
*          according to the Bosch BMP280 datasheet.
*
*          Output units:
*            - Temperature: degrees Celsius (°C)
*            - Pressure: hectopascal (hPa)
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/

#include "bmp280.h"
#include <stdio.h>
#include <string.h>

#define BMP280_ADDR  0x76 /* I2C address (SDO tied to GND) */

/* low-level helpers --------------------------------------------------------*/

/**
* @brief Reads an 8-bit register from the BMP280.
*
* @param reg Register address
* @retval Register value
*/
static uint8_t bmp280_read8(uint8_t reg)
{
    uint8_t val = 0;
    BSP_I2C_Read(BMP280_ADDR, reg, &val, 1);
    return val;
}

/**
* @brief Reads a 16-bit little-endian register from the BMP280.
*
* @param reg Register address
* @retval Register value
*/
static uint16_t bmp280_read16(uint8_t reg)
{
    uint8_t buf[2];
    BSP_I2C_Read(BMP280_ADDR, reg, buf, 2);
    return (buf[1] << 8) | buf[0];
}

/* Sensor init -------------------------------------------------------------*/
/**
* @brief Initializes the BMP280 sensor.
*
* @param dev Pointer to BMP280 device structure
* @retval 1 on success, 0 on failure
*/
uint8_t BMP280_Init(BMP280 *dev)
{
	/* Verify sensor ID */
    uint8_t id = bmp280_read8(BMP280_REG_ID);
    if (id != 0x58)
        return 0;

    /* Read temperature calibration coefficients */
    dev->calib.dig_T1 = bmp280_read16(0x88);
    dev->calib.dig_T2 = (int16_t)bmp280_read16(0x8A);
    dev->calib.dig_T3 = (int16_t)bmp280_read16(0x8C);

    /* Read pressure calibration coefficients */
    dev->calib.dig_P1 = bmp280_read16(0x8E);
    dev->calib.dig_P2 = (int16_t)bmp280_read16(0x90);
    dev->calib.dig_P3 = (int16_t)bmp280_read16(0x92);
    dev->calib.dig_P4 = (int16_t)bmp280_read16(0x94);
    dev->calib.dig_P5 = (int16_t)bmp280_read16(0x96);
    dev->calib.dig_P6 = (int16_t)bmp280_read16(0x98);
    dev->calib.dig_P7 = (int16_t)bmp280_read16(0x9A);
    dev->calib.dig_P8 = (int16_t)bmp280_read16(0x9C);
    dev->calib.dig_P9 = (int16_t)bmp280_read16(0x9E);

    /* Configure sensor:
     *  - Normal mode
     *  - Temperature oversampling x1
     *  - Pressure oversampling x1
     */
    uint8_t ctrl = 0x27;
    BSP_I2C_Write(BMP280_ADDR, BMP280_REG_CTRL, &ctrl, 1);

    return 1;
}

/* Temperature -------------------------------------------------------------*/

/**
* @brief Reads and compensates temperature.
*
*        Uses calibration data and compensation formulas
*        provided in the BMP280 datasheet.
*
* @param dev         Pointer to BMP280 device structure
* @param temperature Pointer to store temperature in °C
* @retval 1 on success, 0 on failure
*/
uint8_t BMP280_ReadTemperature(BMP280 *dev, float *temperature)
{
    uint8_t buf[3];
    if (BSP_I2C_Read(BMP280_ADDR, BMP280_REG_TEMP_MSB, buf, 3) != HAL_OK)
        return 0;

    /* Assemble 20-bit raw ADC value */
    int32_t adc = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);

    /* Temperature compensation (datasheet algorithm) */
    int32_t var1 = ((((adc >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
                    ((int32_t)dev->calib.dig_T2)) >> 11;

    int32_t var2 = (((((adc >> 4) - ((int32_t)dev->calib.dig_T1)) *
                       ((adc >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) *
                     ((int32_t)dev->calib.dig_T3)) >> 14;

    /* Store t_fine for pressure compensation */
    dev->t_fine = var1 + var2;

    /* Convert to °C */
    *temperature = (dev->t_fine * 5 + 128) >> 8;
    *temperature /= 100.0f;

    return 1;
}

/* Pressure ---------------------------------------------------------------*/

/**
* @brief Reads and compensates pressure.
*
* @param dev      Pointer to BMP280 device structure
* @param pressure Pointer to store pressure in hPa
* @retval 1 on success, 0 on failure
*/
uint8_t BMP280_ReadPressure(BMP280 *dev, float *pressure)
{
    uint8_t buf[3];
    if (BSP_I2C_Read(BMP280_ADDR, BMP280_REG_PRESS_MSB, buf, 3) != HAL_OK)
        return 0;

    /* Assemble 20-bit raw ADC value */
    int32_t adc_P = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);

    /* Pressure compensation (64-bit math required) */
    int64_t var1, var2, p;
    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * dev->calib.dig_P6;
    var2 = var2 + ((var1 * dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * dev->calib.dig_P3) >> 8) + ((var1 * dev->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * dev->calib.dig_P1 >> 33;

    /* Prevent division by zero */
    if (var1 == 0) {
        return 0; // avoid division by zero
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)dev->calib.dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)dev->calib.dig_P8 * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);

    /* Convert to hPa */
    *pressure = p / 25600.0f;

    return 1;
}

/* Combined Read ---------------------------------------------------------------*/

/**
* @brief Reads temperature and pressure in one call.
*
* @param dev         Pointer to BMP280 device structure
* @param temperature Pointer to temperature output (°C)
* @param pressure    Pointer to pressure output (hPa)
* @retval HAL_OK on success, 0 on failure
*/
uint8_t BMP280_ReadValues(BMP280 *dev, float *temperature, float *pressure)
{

	/* Read temperature (updates t_fine) */
    if (!BMP280_ReadTemperature(dev, temperature)) {
        return 0;
    }

    /* Read pressure */
    if (!BMP280_ReadPressure(dev, pressure))
            return 0;

    return HAL_OK;
}
