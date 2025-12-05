// ************************************************************************* //
// File: ADC.c
// Done by : Jessica Di Stefano
// Description : used to get the wieght of the cylinders
//
//
// ************************************************************************* //

// *************************** INCLUDES ************************************ //
#include "main.h"
#include "adc.h"
#include <stdbool.h>
#include <stdio.h>


extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

//************************* SETUP MAIN PROGRAM *******************************
uint16_t ADC_Read_Balance(void)
{
	HAL_ADC_Start(&hadc1);
    uint16_t adcBalance = 0;

    // Attendre conversion complète
    HAL_ADC_PollForConversion(&hadc1, 10);  // timeout de 10 ms :-)

    // Lis le raw genre 4095
    adcBalance = HAL_ADC_GetValue(&hadc1);  // ajoute la lecture dans adcBalance

    HAL_ADC_Stop(&hadc1);

    return adcBalance;
}


uint16_t ADC_Read_Pince(void)
{
    HAL_ADC_Start(&hadc2);
    uint16_t adcPince = 0;

    HAL_ADC_PollForConversion(&hadc2, 10);  // timeout de 10 ms !!!

    // Lis la valeur raw
    adcPince = HAL_ADC_GetValue(&hadc2);    // ajoute la lecture dans adcPince

    HAL_ADC_Stop(&hadc2);

    return adcPince;
}
