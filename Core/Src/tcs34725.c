
#include "tcs34725.h"

#define COMMAND_BIT       0x80U
#define AUTO_INCREMENT    0x20U

#define REG_ENABLE        0x00U
#define REG_ATIME         0x01U
#define REG_CONTROL       0x0FU
#define REG_ID            0x12U
#define REG_CDATAL        0x14U

#define ENABLE_PON        0x01U
#define ENABLE_AEN        0x02U

static HAL_StatusTypeDef WriteRegister(
    I2C_HandleTypeDef *hi2c,
    uint8_t reg,
    uint8_t value)
{
    uint8_t data[2] = {
        (uint8_t)(COMMAND_BIT | reg),
        value
    };

    return HAL_I2C_Master_Transmit(
        hi2c, TCS34725_ADDRESS, data, 2, 100);
}

static HAL_StatusTypeDef ReadRegisters(
    I2C_HandleTypeDef *hi2c,
    uint8_t reg,
    uint8_t *data,
    uint16_t length)
{
    uint8_t command =
        (uint8_t)(COMMAND_BIT | AUTO_INCREMENT | reg);

    HAL_StatusTypeDef status;

    status = HAL_I2C_Master_Transmit(
        hi2c, TCS34725_ADDRESS, &command, 1, 100);

    if (status != HAL_OK)
        return status;

    return HAL_I2C_Master_Receive(
        hi2c, TCS34725_ADDRESS, data, length, 100);
}

HAL_StatusTypeDef TCS34725_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t id = 0;

    if (ReadRegisters(hi2c, REG_ID, &id, 1) != HAL_OK)
        return HAL_ERROR;

    /* Expected TCS34725 ID */
    if (id != 0x44U && id != 0x4DU && id != 0x10U)
        return HAL_ERROR;

    /* Integration time: approximately 101 ms */
    if (WriteRegister(hi2c, REG_ATIME, 0xD5U) != HAL_OK)
        return HAL_ERROR;

    /* Gain: 1x */
    if (WriteRegister(hi2c, REG_CONTROL, 0x00U) != HAL_OK)
        return HAL_ERROR;

    /* Power on */
    if (WriteRegister(hi2c, REG_ENABLE, ENABLE_PON) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(3);

    /* Enable ADC */
    if (WriteRegister(
            hi2c, REG_ENABLE, ENABLE_PON | ENABLE_AEN) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(120);

    return HAL_OK;
}

HAL_StatusTypeDef TCS34725_ReadRGB(
    I2C_HandleTypeDef *hi2c,
    uint16_t *red,
    uint16_t *green,
    uint16_t *blue,
    uint16_t *clear)
{
    uint8_t data[8];

    if (ReadRegisters(hi2c, REG_CDATAL, data, 8) != HAL_OK)
        return HAL_ERROR;

    *clear = ((uint16_t)data[1] << 8) | data[0];
    *red   = ((uint16_t)data[3] << 8) | data[2];
    *green = ((uint16_t)data[5] << 8) | data[4];
    *blue  = ((uint16_t)data[7] << 8) | data[6];

    return HAL_OK;
}
