// ************************************************************************* // 
// File: keyboard.c
// Done by : thomas
// Description :
//
//
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "keyboard.h"

// ************************* SETUP MAIN PROGRAM **************************** //
uint8_t Clavier_MX(void)
{	
    static uint8_t Symbole_Clavier[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'} 
    };

    //static bool Ch_Front = 1;
	for (int ligne = 0; ligne < 4; ligne++){ // check lines
	
		for (int col = 0; col < 4; col++){ 	 // check colons
            Ecrire_I2C(~(0x01 << col));   	 // write 0xF? depending on the col to detect GND

			HAL_Delay(1);
			uint8_t lecture = Lire_I2C();
			if (~lecture & (0x80 >> ligne))  // check if lines are selected
			{
				//if (Ch_Front){
				//	Ch_Front = 0;
					return Symbole_Clavier[col][ligne]; // return the button pressed
				//}
				//else{			
				//	return 0; // aucune touche
				//}
			}
			HAL_Delay(1);
		}
	}	

	//Ch_Front = 1;
    return 0; //nothing pressed
}
