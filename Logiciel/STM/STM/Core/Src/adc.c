//////////////////////////////////////////////////////////////////////////////////////////////////
/// ADC FAIT PAR : JESSICA DI STEFANO                                                          ///
/// MODIFIÉ PAR :                                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////
/// CONFIG :                                                                                   ///
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "adc.h"

// Handle ADC statique
static ADC_HandleTypeDef hadc;

// --- Initialisation ADC ---
void ADC_Init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // GPIOA pour PA3

    // Configurer PA3 en analogique
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Config ADC
    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    HAL_ADC_Init(&hadc);
}

// --- Lecture PA3 ---
uint32_t ADC_Read(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_3;           
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // bon pour cellule de charge
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    HAL_ADC_Start(&hadc);
    if (HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY) == HAL_OK)
        return HAL_ADC_GetValue(&hadc); // valeur brute 0-4095
    return 0; // erreur
}




