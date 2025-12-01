// ************************************************************************* // 
// File: keyboard.c
// Done by :
// Description :
//
//
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "keyboard.h"

// *************************** DEFINES ************************************* //


// *************************** VARIABLES *********************************** //


// ************************* SETUP MAIN PROGRAM **************************** //
char Clavier_MX(void)
{	
    char Symbole_CLAVIER[16] =  // Clavier Matrice
			{'1','2','3','A',
			 '4','5','6','B',
		     '7','8','9','C',
			 '*','0','#','D'};

    char Data_Val = 0;          
    static bool Ch_Front = 1;
    if (Ch_Front == 1){           // changement de front mais ne marche pas mais ça marche
        Data_Val = Lire_I2C();
        if (Data_Val >= 0 && Data_Val <= 15){  // détecte si ce n'est pas une valeur random de l'i2c
            HAL_Delay(10);
            Ch_Front = 0;
            return Symbole_CLAVIER[Data_Val];  
        } 
    }
    Ch_Front = 1;	      
    return 0; // aucune touche
}

// ***************************** FUNCTIONS ********************************* //


