#ifndef LCD_H
#define LCD_H

// *************************** INCLUDES ************************************ //
#include "main.h"

// ***************************** FUNCTIONS ********************************* //
void LCD_Init(void);
void LCD_Cmd(uint8_t cmd);
void LCD_Write(uint8_t data);
void LCD_Print(char *text);
void LCD_PrintInt(int num);
void LCD_Clear(void);
void LCD_Set(uint8_t col, uint8_t row);
void LCD_CreateChar(uint8_t loc, uint8_t map[]);

#endif