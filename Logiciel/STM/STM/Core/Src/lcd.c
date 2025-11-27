#include "LCD.h"
#include "string.h"
#include "stm32f1xx_hal.h"

static uint8_t lcd_row = 0;
static uint8_t lcd_col = 0;

// === Fonctions internes ===

static void LCD_EnablePulse(void)
{
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

static void LCD_Send4Bits(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data >> 0) & 0x01);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data >> 1) & 0x01);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data >> 2) & 0x01);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data >> 3) & 0x01);
}

static void LCD_SendCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    LCD_Send4Bits(cmd >> 4);
    LCD_EnablePulse();
    LCD_Send4Bits(cmd & 0x0F);
    LCD_EnablePulse();
}

void LCD_SendData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    LCD_Send4Bits(data >> 4);
    LCD_EnablePulse();
    LCD_Send4Bits(data & 0x0F);
    LCD_EnablePulse();
    lcd_col++;
}

// === Fonctions publiques ===

void LCD_Init(void)
{
    HAL_Delay(50);
    LCD_Send4Bits(0x03);
    LCD_EnablePulse();
    HAL_Delay(5);
    LCD_Send4Bits(0x03);
    LCD_EnablePulse();
    HAL_Delay(1);
    LCD_Send4Bits(0x03);
    LCD_EnablePulse();
    LCD_Send4Bits(0x02); // Mode 4 bits
    LCD_EnablePulse();

    LCD_SendCommand(0x28); // 4 bits, 2 lignes, 5x8
    LCD_SendCommand(0x0F); // Écran ON, curseur ON, clignotement ON
    LCD_SendCommand(0x06); // Auto-incrément
    LCD_Clear();
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    HAL_Delay(2);
    lcd_row = 0;
    lcd_col = 0;
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_row = row;
    lcd_col = col;
    LCD_SendCommand(0x80 | (col + offsets[row]));
    HAL_Delay(2);
}

void LCD_Print(char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
        HAL_Delay(2);
        // Gérer débordement automatique
        if (lcd_col >= 20)
        {
            lcd_col = 0;
            lcd_row++;

            if (lcd_row >= 4) {
            	lcd_row = 0;
                LCD_SendCommand(0x02);
                HAL_Delay(2);
            }
            else{
				LCD_SetCursor(lcd_col, lcd_row);
				HAL_Delay(2);
            }
        }
    }
}


void LCD_CreateChar(uint8_t location, uint8_t *pattern)
{
    location &= 0x07; // max 8 caractères (0-7)
    LCD_SendCommand(0x40 | (location << 3)); // adresse CGRAM
    for (uint8_t i = 0; i < 8; i++)
    {
        LCD_SendData(pattern[i]);
    }
    LCD_Clear();
}

void LCD_SetBack(void)
{
	if ((lcd_col <=19) && (lcd_col > 0) && (lcd_row <=3))
	{
		//LCD_Print("AAA");
		LCD_SetCursor((lcd_col-1), lcd_row);
	}
	else if ((lcd_col == 0) && (lcd_row <= 3) && (lcd_row > 0))
	{
		//LCD_Print("BBB");
		LCD_SetCursor(19, (lcd_row-1));
	}
	else if ((lcd_col == 0) && (lcd_row == 0))
	{
		//LCD_Print("CCC");
		LCD_SetCursor(19, 3);
	}
}




















