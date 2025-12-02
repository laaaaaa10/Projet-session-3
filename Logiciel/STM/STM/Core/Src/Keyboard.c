// ************************************************************************* // 
// File: keyboard.c
// Done by :
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
        {'1','2','3','A'},  // Row 0
        {'4','5','6','B'},  // Row 1
        {'7','8','9','C'},  // Row 2
        {'*','0','#','D'}   // Row 3
    };

    //static bool Ch_Front = 1;
	for (int ligne = 0; ligne < 4; ligne++){
		// Lire les colonnes (entrée)
		for (int col = 0; col < 4; col++){
            Ecrire_I2C(~(0x01 << col));
			HAL_Delay(5);
			uint8_t lecture = Lire_I2C();
			if (~lecture & (0x80 >> ligne))
			{
				//if (Ch_Front){
				//	Ch_Front = 0;
					return Symbole_Clavier[col][ligne];
				//}
				//else{			
				//	return 0; // aucune touche
				//}
			}
			HAL_Delay(5);	
		}
	}	
	//Ch_Front = 1;
    return 0; // aucune touche
}
