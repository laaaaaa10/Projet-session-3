// ************** INCLUDES ************** //
#include "main.h"

// ************** DEFINES *************** //
#ifndef INC_UART_Com_H_
#define INC_UART_Com_H_

// ************** EXTERN **************** //
extern UART_HandleTypeDef huart1;

// ************** FUNCTION ************** //
void UART_Envoit(uint8_t M_0, uint8_t M_1, uint8_t M_2, uint8_t M_3, uint8_t M_4);
void UART_STM(uint8_t V_TX);

#endif
