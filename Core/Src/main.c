
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : ARM-Based Currency Screening Prototype
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "tcs34725.h"
#include <stdbool.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PV */

static TCS34725_Data colorData;
static bool colorSensorReady = false;

/*
 * Example reference color ratios only.
 * Calibrate these values using known samples under fixed lighting.
 */
#define REF_R_RATIO       0.33f
#define REF_G_RATIO       0.34f
#define REF_B_RATIO       0.33f
#define RATIO_TOLERANCE   0.15f
#define MIN_CLEAR_VALUE   10U

/* USER CODE END PV */

/**
  * @brief  Application entry point.
  * @retval int
  */
int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();

    /* USER CODE BEGIN 2 */

    /* Turn OFF all RGB LED channels initially */
    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
                      GPIO_PIN_RESET);

    /* Initialize TCS34725 color sensor */
    colorSensorReady = (TCS34725_Init(&hi2c1) == HAL_OK);

    /* USER CODE END 2 */

    /* Infinite loop */
    while (1)
    {
        /* IR sensor is assumed active LOW */
        GPIO_PinState irState =
            HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);

        /*
         * No sample detected:
         * Blue LED ON
         */
        if (irState == GPIO_PIN_SET)
        {
            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5 | GPIO_PIN_6,
                              GPIO_PIN_RESET);

            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_7,
                              GPIO_PIN_SET);

            HAL_Delay(100);
            continue;
        }

        /*
         * Color sensor initialization/read error:
         * Red LED blinks
         */
        if ((!colorSensorReady) ||
            (TCS34725_Read(&hi2c1, &colorData) != HAL_OK))
        {
            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_6 | GPIO_PIN_7,
                              GPIO_PIN_RESET);

            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5,
                              GPIO_PIN_SET);

            HAL_Delay(200);

            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5,
                              GPIO_PIN_RESET);

            HAL_Delay(200);
            continue;
        }

        /*
         * Reject very low color readings.
         * Blue LED indicates insufficient reading.
         */
        if (colorData.clear < MIN_CLEAR_VALUE)
        {
            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5 | GPIO_PIN_6,
                              GPIO_PIN_RESET);

            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_7,
                              GPIO_PIN_SET);

            HAL_Delay(100);
            continue;
        }

        /* Calculate normalized RGB ratios */
        float total = (float)colorData.red
                    + (float)colorData.green
                    + (float)colorData.blue;

        if (total <= 0.0f)
        {
            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5 | GPIO_PIN_6 |
                              GPIO_PIN_7,
                              GPIO_PIN_RESET);

            HAL_Delay(100);
            continue;
        }

        float rRatio = (float)colorData.red / total;
        float gRatio = (float)colorData.green / total;
        float bRatio = (float)colorData.blue / total;

        /*
         * Compare sensor readings with configured reference ratios.
         * This is a prototype color-matching test, not authentication.
         */
        bool referenceMatch =
            (rRatio >= REF_R_RATIO - RATIO_TOLERANCE) &&
            (rRatio <= REF_R_RATIO + RATIO_TOLERANCE) &&

            (gRatio >= REF_G_RATIO - RATIO_TOLERANCE) &&
            (gRatio <= REF_G_RATIO + RATIO_TOLERANCE) &&

            (bRatio >= REF_B_RATIO - RATIO_TOLERANCE) &&
            (bRatio <= REF_B_RATIO + RATIO_TOLERANCE);

        /*
         * Reference color match:
         * Green LED ON
         *
         * Reference color mismatch:
         * Red LED ON
         */

        if (referenceMatch)
        {
            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5 | GPIO_PIN_7,
                              GPIO_PIN_RESET);

            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_6,
                              GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_6 | GPIO_PIN_7,
                              GPIO_PIN_RESET);

            HAL_GPIO_WritePin(GPIOA,
                              GPIO_PIN_5,
                              GPIO_PIN_SET);
        }

        HAL_Delay(200);
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Set RGB LED pins LOW */
    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
                      GPIO_PIN_RESET);

    /* IR sensor output: PA1 */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* RGB LED outputs: PA5, PA6, PA7 */
    GPIO_InitStruct.Pin =
        GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief Error Handler
  * @retval None
  */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add error reporting here */
}
#endif
