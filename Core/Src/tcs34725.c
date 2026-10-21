#include "tcs34725.h"

#define TCS34725_ADDRESS       (0x29U << 1)
#define TCS34725_COMMAND       0x80U

#define TCS34725_ENABLE        0x00U
#define TCS34725_ATIME         0x01U
#define TCS34725_CONTROL       0x0FU
#define TCS34725_ID            0x12U
#define TCS34725_CDATAL        0x14U

#define TCS34725_PON           0x01U
#define TCS34725_AEN           0x02U

static HAL_StatusTypeDef TCS34725_Write(
    I2C_HandleTypeDef *hi2c,
    uint8_t reg,
    uint8_t value)
{
    uint8_t command = TCS34725_COMMAND | reg;

    return HAL_I2C_Mem_Write(
        hi2c,
        TCS34725_ADDRESS,
        command,
        I2C_MEMADD_SIZE_8BIT,
        &value,
        1,
        100
    );
}

HAL_StatusTypeDef TCS34725_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t id = 0;

    if (HAL_I2C_IsDeviceReady(
            hi2c, TCS34725_ADDRESS, 3, 100) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Read(
            hi2c,
            TCS34725_ADDRESS,
            TCS34725_COMMAND | TCS34725_ID,
            I2C_MEMADD_SIZE_8BIT,
            &id,
            1,
            100) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Common TCS34725-compatible ID values */
    if ((id != 0x44U) && (id != 0x4DU))
    {
        return HAL_ERROR;
    }

    /* Power on */
    if (TCS34725_Write(hi2c, TCS34725_ENABLE,
                       TCS34725_PON) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_Delay(3);

    /* Integration time: approximately 103 ms */
    if (TCS34725_Write(hi2c, TCS34725_ATIME,
                       0xD5U) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Gain: 4x */
    if (TCS34725_Write(hi2c, TCS34725_CONTROL,
                       0x01U) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Enable power and RGBC measurement */
    if (TCS34725_Write(hi2c, TCS34725_ENABLE,
                       TCS34725_PON | TCS34725_AEN) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_Delay(120);

    return HAL_OK;
}

HAL_StatusTypeDef TCS34725_Read(
    I2C_HandleTypeDef *hi2c,
    TCS34725_Data *data)
{
    uint8_t buffer[8];

    if ((data == NULL) || (hi2c == NULL))
    {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Read(
            hi2c,
            TCS34725_ADDRESS,
            TCS34725_COMMAND | TCS34725_CDATAL,
            I2C_MEMADD_SIZE_8BIT,
            buffer,
            sizeof(buffer),
            200) != HAL_OK)
    {
        return HAL_ERROR;
    }

    data->clear = (uint16_t)buffer[0] |
                  ((uint16_t)buffer[1] << 8);

    data->red   = (uint16_t)buffer[2] |
                  ((uint16_t)buffer[3] << 8);

    data->green = (uint16_t)buffer[4] |
                  ((uint16_t)buffer[5] << 8);

    data->blue  = (uint16_t)buffer[6] |
                  ((uint16_t)buffer[7] << 8);

    return HAL_OK;
}
