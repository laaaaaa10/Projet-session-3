// ************************************************************************* // 
// File: Gui.c
// Done by : Javier
// Description :
// used to control the lcd:
// menu 1: display all the basic information
// menu 2: menu to control the motors
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "Gui.h"

// *************************** DEFINES ************************************ //
#define MANUAL 69
#define AUTO   67

// *************************** VARIABLES ************************************ //
// Custom up symbol
uint8_t up[8] = {
    0b11011,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00000
};

// Custom down symbol
uint8_t down[8] = {
    0b00000,
    0b01100,
    0b10010,
    0b10101,
    0b10010,
    0b10110,
    0b01010,
    0b00000
};

// ************************* SETUP MAIN PROGRAM **************************** //
void GUI_Init(void) {
    LCD_Init();
    LCD_CreateChar(1, up);
    LCD_CreateChar(2, down);
}

void Run_GUI(int x_coord, int y_coord, int ctrl_mode, int *Out_Pivots) {
    static int Ancient_Mode = AUTO;
    if (Ancient_Mode != ctrl_mode){
        LCD_Clear();
        Ancient_Mode = ctrl_mode;
    }

    // ----- MENU 1 (AUTO) ----- //
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

        LCD_Set(0, 1);
        LCD_Print("X:");
        LCD_PrintInt(x_coord); 
        LCD_Print(" Y:"); 
        LCD_PrintInt(y_coord);

        LCD_Set(0, 2);
        LCD_Print(" * to change mode");

        LCD_Set(0, 3);
        LCD_Print("we gay   mode = AUTO");
    }
    
    // ----- MENU 2 (MANUAL) ----- //
    else  {
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

        LCD_Set(0, 1);
        LCD_Write(1);
        LCD_Print(" 1    2    3    A");

        LCD_Set(0, 2);
        LCD_Write(2); 
        LCD_Print(" 4    5    6    B");

        LCD_Set(0, 3);
        LCD_Print("gay    mode = MANUAL");
    }
}