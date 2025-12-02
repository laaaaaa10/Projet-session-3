#ifndef ADC_H
#define ADC_H

#include "stm32f0xx_hal.h"

// Initialise l'ADC et PA3
void ADC_Init(void);

// Lit PA3 et renvoie la valeur brute ADC (0-4095)
uint32_t ADC_Read(void);

#endif // ADC_H

