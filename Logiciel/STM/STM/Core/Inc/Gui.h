#ifndef GUI_H_
#define GUI_H_

// *************************** INCLUDES ************************************ //
#include "main.h"
#include "lcd.h"

// *************************** VARIABLES ************************************ //
LCD_Init();

extern int Out_Pivots[5];

// ************************ FUNCTION PROTOTYPES **************************** //
// ----- SETUP CUSTOM CHARACTERS HERE ----- //
// custum up symbole
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
// custum down symbole
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

    LCD_CreateChar(1, up);
    LCD_CreateChar(2, down);

    // idk what they are but tthe where in my lcd custom char code
    //Operandes op;
    //static char result_str[6];

void Run_GUI(int x_coord, int y_coord, int ctrl_mode, int *Out_Pivots);

#endif
