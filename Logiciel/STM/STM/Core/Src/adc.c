//////////////////////////////////////////////////////////////////////////////////////////////////
/// ADC FAIT PAR : JESSICA DI STEFANO                                                          ///
/// MODIFIÉ PAR :                                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////
/// CONFIG :                                                                                   ///
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "adc.h"

uint16_t ADC_Read_Raw(void)
{
    uint16_t adcVal = 0;

    HAL_ADC_Start(&hadc);

    // attendre conversion complète
    HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

    // lire valeur brute (0–4095)
    adcVal = HAL_ADC_GetValue(&hadc);

    HAL_ADC_Stop(&hadc);

    return adcVal;
}





