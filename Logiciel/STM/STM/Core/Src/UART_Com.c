// ************************************************************************* // 
// File: UART_Com
// Done by : Thomas
// Description :
// use to comunicate between the PIC16F88 and the STM32F103
// 
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "UART_Com.h"

// *************************** DEFINES ************************************* //


// *************************** VARIABLES *********************************** //
extern UART_HandleTypeDef huart1;
uint8_t PICs_8Bit[8];

// ************************* SETUP MAIN PROGRAM **************************** //
// ----- Transmit ----- //
void UART_Send(uint8_t M_0, uint8_t M_1, uint8_t M_2, uint8_t M_3, uint8_t M_4){
    uint8_t CheckSum;
    UART_Com('G');
    UART_Com('O');
    UART_Com(M_0);
    UART_Com(M_1);
    UART_Com(M_2);
    UART_Com(M_3);
    UART_Com(M_4);

    CheckSum = (uint8_t)('G' + 'O' + M_0 + M_1 + M_2 + M_3 + M_4);
    UART_Com(CheckSum);
}

void UART_Com(uint8_t V_TX){
    HAL_UART_Transmit(&huart1, &V_TX, 1, 1);
}

// ----- Receive ----- //
// read 8 bytes (blocking) and return pointer to buffer
uint8_t* UART_Receive(void){
    for (uint8_t i = 0; i < 8; i++){
        PICs_8Bit[i] = UART_Read_1bit();
    }
    return PICs_8Bit;
}

// ***************************** FUNCTIONS ********************************* //
// ----- read one bit at a time ----- //
uint8_t UART_Read_1bit(void){
    uint8_t ucCaract = 0;
    // Use 1000 ms timeout for robustness during debug; reduce later if needed
    if (HAL_UART_Receive(&huart1, &ucCaract, 1, 250) != HAL_OK) {
        // on timeout or error return 0 (NUL). You can change behaviour if desired.
        return 0;
    }
    return ucCaract;
}