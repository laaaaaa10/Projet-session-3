#ifndef INC_UART_Com_H_
#define INC_UART_Com_H_

// ************** INCLUDES ************** // 
#include "main.h"
#include <stdbool.h>

// ************** VARIABLES *************** //
extern uint8_t PICs_8Bit[8];
extern UART_HandleTypeDef huart1;

// ************** FUNCTIONS ************** //
// AIDE LE TRANSMIT
void UART_Send(uint8_t M_0, uint8_t M_1, uint8_t M_2, uint8_t M_3, uint8_t M_4);
void UART_Com(uint8_t V_TX);

// AIDE LE RECEIVE
uint8_t* UART_Receive(void);  // returnE LE POINTEUR À PICs_8Bit
uint8_t UART_Read_1bit(void);
uint8_t UART_Data_Available(void);

#endif
