// ************** INCLUDES ************** // 
#include "main.h"
#include "Keyboard.h"
#include <stdbool.h>
#include "UART_Com.h"

// ************** EXTERN **************** //
extern UART_HandleTypeDef huart1;

// ************** DEFINES *************** //
#ifndef INC_UART_Com_H_
#define INC_UART_Com_H_

// ************** FUNCTION ************** //
void UART_Send(uint8_t M_0, uint8_t M_1, uint8_t M_2, uint8_t M_3, uint8_t M_4);
void UART_Com(uint8_t V_TX);

int UART_Rec(void);
int Lire_UART(void);

#endif
