// ************** INCLUDES ************** // 
#include "main.h"
#include "Keyboard.h"
#include <stdbool.h>

// ************** EXTERN **************** //
extern UART_HandleTypeDef huart1;
extern uint8_t PICs_8Bit[8]; // for main.c access

// ************** DEFINES *************** //
#ifndef INC_UART_Com_H_
#define INC_UART_Com_H_

// ************** FUNCTION ************** //
void UART_Send(uint8_t M_0, uint8_t M_1, uint8_t M_2, uint8_t M_3, uint8_t M_4);
void UART_Com(uint8_t V_TX);

uint8_t* UART_Recive(void);  // returns pointer to PICs_8Bit
int UART_Read_1bit(void);

#endif
