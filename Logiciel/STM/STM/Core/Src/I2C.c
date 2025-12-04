// ************************************************************************* // 
// File: 12C.c
// Done by : Thomas
// Description :
// used to conver 2 wires into 8 for the keyboard 
//
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "I2C.h"

// ************************* SETUP MAIN PROGRAM **************************** //
uint8_t Lire_I2C(void)
{
    uint8_t data = 0;

    // On lit le data
    HAL_I2C_Master_Receive(&hi2c1, PCF8574_ADDR << 1, &data, 1, 100);

    return data;
}

void Ecrire_I2C(uint8_t value)
{
    HAL_I2C_Master_Transmit(&hi2c1, PCF8574_ADDR << 1, &value, 1, 100);
}
