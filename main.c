#include "main.h"
#include "dac.h"
#include "BT_UART.h"

void SystemClock_Config(void);

char left_motor[3] = "000";
char right_motor[3] = "000";

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    GPIO_init();
    BT_UART_init();

    while (1)
    {
        // Drive left
        GPIOB->ODR &= ~(GPIO_PIN_1);
        //DAC_set_voltage(left_motor);
        GPIOB->ODR |= GPIO_PIN_1;

        // Drive right
        GPIOB->ODR &= ~(GPIO_PIN_2);
        //DAC_set_voltage(right_motor);
        GPIOB->ODR |= GPIO_PIN_2;

        // OFF left
        left_motor[0] = '0';
        left_motor[1] = '0';
        left_motor[2] = '0';
        GPIOB->ODR &= ~(GPIO_PIN_1);
        //DAC_set_voltage(left_motor);
        GPIOB->ODR |= GPIO_PIN_1;

        HAL_Delay(50);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_RCCEx_EnableMSIPLLMode();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
