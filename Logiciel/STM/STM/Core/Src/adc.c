// ************************************************************************* //
// File: ADC.c
// Done by : Jessica Di Stefano
// Description :
//
//
// ************************************************************************* //

// *************************** INCLUDES ************************************ //
#include "main.h"
#include "adc.h"
#include <stdbool.h>
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;


//************************* SETUP MAIN PROGRAM *******************************
uint16_t ADC_Read_Raw(void)
{
    uint16_t adcVal = 0;

    HAL_ADC_Start(&hadc1);

    // Attendre conversion complète
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

    // Lire valeur brute (0–4095)
    adcVal = HAL_ADC_GetValue(&hadc1);

    // HAL_ADC_Stop(&hadc1);

    return adcVal;
}