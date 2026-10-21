#ifndef TCS34725_H
#define TCS34725_H

#include "main.h"

typedef struct
{
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} TCS34725_Data;

HAL_StatusTypeDef TCS34725_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef TCS34725_Read(I2C_HandleTypeDef *hi2c,
                                TCS34725_Data *data);

#endif
