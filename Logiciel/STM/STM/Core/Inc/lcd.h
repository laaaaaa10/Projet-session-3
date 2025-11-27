#ifndef __LCD_H__
#define __LCD_H__

#include "main.h"

#define LCD_RS_Pin GPIO_PIN_9
#define LCD_RS_GPIO_Port GPIOB
#define LCD_RW_Pin GPIO_PIN_10
#define LCD_RW_GPIO_Port GPIOB
#define LCD_EN_Pin GPIO_PIN_11
#define LCD_EN_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_12
#define LCD_D4_GPIO_Port GPIOB
#define LCD_D5_Pin GPIO_PIN_13
#define LCD_D5_GPIO_Port GPIOB
#define LCD_D6_Pin GPIO_PIN_14
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_15
#define LCD_D7_GPIO_Port GPIOB

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *str);
void LCD_CreateChar(uint8_t location, uint8_t *pattern);
void LCD_SendData(uint8_t data);
void LCD_SetBack(void);

#endif
