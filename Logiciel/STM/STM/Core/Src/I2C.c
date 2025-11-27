// ************** INCLUDES ************** //
#include "I2C.h"
// ************** Lire_I2C ************** //

char Lire_I2C(void){
    uint8_t data = 0;
    //uint16_t Clavier_I2C = 0x0000;
    HAL_I2C_Master_Receive(&hi2c1, 0x28 << 1, &data, 1, 100);
    return data;
}
