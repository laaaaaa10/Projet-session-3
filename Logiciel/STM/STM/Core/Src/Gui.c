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
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00000
};
uint8_t up_I[8] = {
  0B11011,
  0B10001,
  0B01010,
  0B11011,
  0B11011,
  0B11011,
  0B11011,
  0B11011
};

// Custom down symbol
uint8_t down[8] = {
  0B00000,
  0B00100,
  0B00100,
  0B00100,
  0B00100,
  0B10101,
  0B01110,
  0B00100
};

uint8_t down_I[8] = {
  0B11111,
  0B11011,
  0B11011,
  0B11011,
  0B11011,
  0B01010,
  0B10001,
  0B11011
};
uint8_t Pince_1[8] = {
  0B00000,
  0B11100,
  0B11110,
  0B00000,
  0B00000,
  0B11110,
  0B11100,
  0B00000
};
uint8_t Pince_2[8] = {
  0B00000,
  0B00000,
  0B10011,
  0B11110,
  0B11110,
  0B10011,
  0B00000,
  0B00000
};
// ************************* SETUP MAIN PROGRAM ****************************** //
void GUI_Init(void) {
    LCD_Init();
    LCD_CreateChar(1, up);
    LCD_CreateChar(2, down);
    LCD_CreateChar(3, up_I);
    LCD_CreateChar(4, down_I);
    LCD_CreateChar(5, Pince_1);
    LCD_CreateChar(6, Pince_2);
}

void Run_GUI(int x_coord, int y_coord, int ctrl_mode, int *Out_Pivots) {
<<<<<<< HEAD
=======
    
    
>>>>>>> 68f80785b26e597ffd214222aba6b11e806628c7

    static int Ancient_Mode = AUTO;
    static uint32_t timer_1 = 0;
    static AAA = 0;

    if (Ancient_Mode != ctrl_mode){
        LCD_Clear();
        Ancient_Mode = ctrl_mode;
        timer_1 = 0;
    }

        if (HAL_GetTick() - timer_1 >= 1000) {
          // Une seconde est passée
          LCD_Clear();
          AAA = ~AAA;
          timer_1 = HAL_GetTick();   // reset pour la prochaine seconde
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
<<<<<<< HEAD
        LCD_PrintInt(x_coord);
=======
        LCD_PrintInt(x_coord); 
>>>>>>> 68f80785b26e597ffd214222aba6b11e806628c7
        LCD_Print(" Y:"); 
        LCD_PrintInt(y_coord);

        LCD_Set(0, 2);
        LCD_Print(" * to change mode");

        LCD_Set(0, 3);
        LCD_Print("we gay   mode = AUTO");
    }
    
    // ----- MENU 2 (MANUAL) ----- //
    else  {

        LCD_Set(0, 0);
        LCD_Print("0:");
        LCD_PrintInt(Out_Pivots[0]); 
        LCD_Set(0, 1);
        LCD_Print("1:");
        LCD_PrintInt(Out_Pivots[1]); 
        LCD_Set(0, 2);
        LCD_Print("2:");
        LCD_PrintInt(Out_Pivots[2]); 
        LCD_Set(0, 3);
        LCD_Print("3:");
        LCD_PrintInt(Out_Pivots[3]); 

        LCD_Set(6, 0); 
        LCD_Print("-1");
        if (AAA) LCD_Write(1);
        else LCD_Write(3);
        LCD_Print(" -4");
        if (AAA) LCD_Write(2);
        else LCD_Write(4);

        LCD_Set(6, 1); 
        LCD_Print("-2");
        if (AAA) LCD_Write(1);
        else LCD_Write(3);
        LCD_Print(" -5");
        if (AAA) LCD_Write(2);
        else LCD_Write(4);

        LCD_Set(6, 2); 
        LCD_Print("-3");
        if (AAA) LCD_Write(1);
        else LCD_Write(3);
        LCD_Print(" -6");
        if (AAA) LCD_Write(2);
        else LCD_Write(4);

        LCD_Set(6, 3); 
        LCD_Print("-A");
        if (AAA) LCD_Write(1);
        else LCD_Write(3);
        LCD_Print(" -B");
        if (AAA) LCD_Write(2);
        else LCD_Write(4);

        LCD_Set(14, 0);
        LCD_Write(6); 
        LCD_Write(5);
        LCD_Print("=");
        LCD_PrintInt(Out_Pivots[4]); 

        LCD_Set(14, 3);
        LCD_Print("mode=M");
    }
}
