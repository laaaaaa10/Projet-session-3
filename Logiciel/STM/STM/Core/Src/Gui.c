// ************************************************************************* // 
// File: 12C.c
// Done by : JAvier
// Description :
// used to contoll the lcd:
// menu 1: display all the basic information
// menu 2: menue to controll the motors
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "Gui.h"


// *************************** DEFINES ************************************ //
#define MANUAL 69
#define AUTO   67


// *************************** VARIABLES ************************************ //


// ************************* SETUP MAIN PROGRAM **************************** //

void Run_GUI(int x_coord, int y_coord, int ctrl_mode, int *Out_Pivots) {
    LCD_Clear();

    // ----- MENUE 1 (AUTO) ----- //
    if (ctrl_mode == AUTO) {
        LCD_Print("0:");
        LCD_PrintInt(Out_Pivots[0]); 
        LCD_Print(" 1:");
        LCD_PrintInt(Out_Pivots[1]); 
        LCD_Print(" 2:");
        LCD_PrintInt(Out_Pivots[2]); 
        LCD_Print(" 3:");
        LCD_PrintInt(Out_Pivots[3]); 
        LCD_Print(" 4:");
        LCD_PrintInt(Out_Pivots[4]); 

        // also need to pring the hand sensor and weight sensor
        LCD_Set(0, 2);
        LCD_Print(" X:");
        LCD_PrintInt(Table_pos.x); 
        LCD_Print(" Y:"); 
        LCD_PrintInt(Table_pos.y);

        // can type anything you want on theseline
        LCD_Set(0, 4);
        LCD_Print("Press * to change mode");

        LCD_Set(0, 3);
        LCD_Print("we gay fr     mode = AUTO");
    }
    // ----- MENUE 2 (MANUEL) ----- //
    else  {

        LCD_Print("  ");
        
        LCD_Print("0:");
        LCD_PrintInt(Out_Pivots[0]); 
        LCD_Print(" 1:");
        LCD_PrintInt(Out_Pivots[1]); 
        LCD_Print(" 2:");
        LCD_PrintInt(Out_Pivots[2]); 
        LCD_Print(" 3:");
        LCD_PrintInt(Out_Pivots[3]); 
        LCD_Print(" 4:");
        LCD_PrintInt(Out_Pivots[4]); 


        LCD_Set(0, 2);
        LCD_Print(UP );
        LCD_Print(" 1  2  3  A");

        LCD_Set(0, 1);
        LCD_Print(DOWN ); 
        LCD_Print(" 4  5  6  B");

        // can type anything you want on theseline
        LCD_Set(0, 3);
        LCD_Print("we gay fr     mode = MANUEL");
    }
}