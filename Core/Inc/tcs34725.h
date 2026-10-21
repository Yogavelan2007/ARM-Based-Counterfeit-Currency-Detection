
#ifndef TCS34725_H
#define TCS34725_H

#include "main.h"
#include "stm32f4xx_hal_i2c.h"

#define TCS34725_ADDRESS (0x29U << 1)

HAL_StatusTypeDef TCS34725_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef TCS34725_ReadRGB(
    I2C_HandleTypeDef *hi2c,
    uint16_t *red,
    uint16_t *green,
    uint16_t *blue,
    uint16_t *clear
);

#endif
