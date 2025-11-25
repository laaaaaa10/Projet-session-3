// ************** INCLUDES ************** //
#include <UART_Com.h>

// ************** FUNCTION ************** //
void UART_Envoit(uint8_t M_0)
{
}

void UART_STM(uint8_t V_TX)
{
    uint8_t buffer[1];
    buffer[0] = V_TX;

    HAL_UART_Transmit(&huart1, buffer, 1, 1);
}