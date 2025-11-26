// ************** INCLUDES ************** //
#include "main.h"

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
// call read_UART 8 times for each bite of the PIC, thenretrun it in the main
int UART_Rec(void){
    char Char_tab[8];
    for (int i = 0; i < 8; i++){
        Char_tab[i] = Lire_UART();
    }
    return Char_tab;
}

int Lire_UART(void){
    char ucCaract;
    HAL_UART_Receive(&huart1, (uint8_t *)&ucCaract, 1, HAL_MAX_DELAY);
    return ucCaract;
}


