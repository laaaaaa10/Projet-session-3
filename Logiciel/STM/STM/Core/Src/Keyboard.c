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
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'} 
    };

    //static bool Ch_Front = 1;
	for (int ligne = 0; ligne < 4; ligne++){ //check les caca
		// Lire les colonnes (entrée)
		for (int col = 0; col < 4; col++){ //check les pipi
            Ecrire_I2C(~(0x01 << col));   // écrie 0xF? selon la col pour détecter gnd 
			HAL_Delay(1);                // delai de configuration de l'i2c
			uint8_t lecture = Lire_I2C();
			if (~lecture & (0x80 >> ligne))  // check les pipi selectionner
			{
<<<<<<< HEAD
				return Symbole_Clavier[col][ligne];
=======
				//if (Ch_Front){
				//	Ch_Front = 0;
					return Symbole_Clavier[col][ligne]; 
				//}
				//else{			
				//	return 0; // aucune touche
				//}
>>>>>>> aee04b05bda67c0ac354960acd4d81bf657f8a75
			}
			HAL_Delay(1);	 // delai de configuration de l'i2c
		}
	}	
	//Ch_Front = 1;
    return 0; // aucune touche
}
