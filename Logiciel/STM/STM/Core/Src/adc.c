// ************************************************************************* //
// File: ADC.c
// Done by : Jessica Di Stefano
<<<<<<< HEAD
// Description : C'est le ADC la
=======
// Description : used to get the wieght of the cylinders
>>>>>>> 0659e4b144fb942262f9cbb755db25425938f9f3
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

	HAL_ADC_Start(&hadc1);
    uint16_t adcVal = 0;

    HAL_ADC_Start(&hadc1);

    // Attendre conversion complète
    HAL_ADC_PollForConversion(&hadc1, 10);

    // Lis le raw genre 4095
    adcVal = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    return adcVal;
}
