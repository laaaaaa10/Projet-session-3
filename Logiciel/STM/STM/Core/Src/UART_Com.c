// ************** INCLUDES ************** //
#include "main.h"
#include "UART_Com.h"

// ************** VARIABLES ************** //
uint8_t PICs_8Bit[8];

// ************** Transmit ************** //
void UART_Send(uint8_t M_0, uint8_t M_1, uint8_t M_2, uint8_t M_3, uint8_t M_4){
    uint8_t CheckSum;
    UART_Com('G');
    UART_Com('O');
    UART_Com(M_0);
    UART_Com(M_1);
    UART_Com(M_2);
    UART_Com(M_3);
    UART_Com(M_4);

    CheckSum = ('G' + 'O' + M_0 + M_1 + M_2 + M_3 + M_4);
    UART_Com(CheckSum);
}

void UART_Com(uint8_t V_TX){
    HAL_UART_Transmit(&huart1, &V_TX, 1, 1);
}

// ************** Receive ************** //
// call read_UART 8 times for each bite of the PIC, then return it in the main
uint8_t *UART_Recive(void){
    uint8_t i;
    for(i = 0; i < 8; i++){
        PICs_8Bit[i] = UART_Read_1bit();
    }
    return PICs_8Bit;
}

int UART_Read_1bit(void){
    char ucCaract;
    HAL_UART_Receive(&huart1, (uint8_t *)&ucCaract, 1, HAL_MAX_DELAY);
    return ucCaract;
}
