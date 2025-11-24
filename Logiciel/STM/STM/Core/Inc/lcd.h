// ************** INCLUDES ************** //
#include "main.h"

// ************** DEFINES *************** //
#ifndef INC_LCD_H_
#define INC_LCD_H_

// ************** FUNCTION ************** //
void LCD_Init(void);
void LCD_Cmd(uint8_t cmd);
void LCD_Write(uint8_t data);
void LCD_Print(char *text);
void LCD_Clear(void);
void LCD_Set(uint8_t col, uint8_t row);
void LCD_CreateChar(uint8_t loc, uint8_t map[]);

#endif