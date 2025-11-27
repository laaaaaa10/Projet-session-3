//////////////////////////////////////////////////////////////////////////////////////////////////
/// ADC FAIT PAR : JESSICA DI STEFANO                                                          ///
/// MODIFIÉ PAR :                                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////
/// CONFIG :                                                                                   ///
/////////////////////////////////////////////////////////////////////////////////////////////////
/*

void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;

    HAL_ADC_Init(&hadc1);

    // Configure PA2 -> ADC1_IN2
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
/// FONCTION POUR LIRE L'ADC :                                                                 ///
//////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t read_adc_raw()
{
    HAL_ADC_Start(&hadc1);                          // Démarre la conversion ADC
    HAL_ADC_PollForConversion(&hadc1, 10);          // Attend max 10 ms
    return HAL_ADC_GetValue(&hadc1);                // Retourne la valeur (0–4095)
}
//////////////////////////////////////////////////////////////////////////////////////////////////
*/
