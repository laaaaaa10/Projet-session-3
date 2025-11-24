// ************** INCLUDES ************** //
#include "main.h"

// ************** DEFINES ************** //
#define LCD_DELAY 2

// ************** PIN CONTROL ************** //
#define LCD_RS(x) HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define LCD_EN(x) HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)

// ************** FUNCTIONS ************** //
void LCD_Init(void);
void LCD_Cmd(uint8_t cmd);
void LCD_Write(uint8_t data);
void LCD_Print(char *text);
void LCD_Clear(void);
void LCD_Set(uint8_t col, uint8_t row);
void LCD_CreateChar(uint8_t loc, uint8_t map[]);

// ************** INTERNAL ************** //
static void LCD_Nibble(uint8_t nibble);
static void LCD_Pulse(void);

// ************** INITIALIZATION ************** //
//the hex are commands that are in the stupid lcd datasheet
void LCD_Init(void) {

    HAL_Delay(50); // wait for LCD to power up

    LCD_Nibble(0x03); HAL_Delay(5);
    LCD_Nibble(0x03); HAL_Delay(5);
    LCD_Nibble(0x03); HAL_Delay(1);
    LCD_Nibble(0x02); // 4-bit mode

    LCD_Cmd(0x28); // 2-line, 5x8 font
    LCD_Cmd(0x0C); // display ON, cursor OFF
    LCD_Cmd(0x06); // auto-increment
    LCD_Cmd(0x01); // clear
    HAL_Delay(2);
}

// ************** COMMAND FUNCTION ************** //
void LCD_Cmd(uint8_t cmd) {
    LCD_RS(0);
    LCD_Nibble(cmd >> 4);
    LCD_Nibble(cmd & 0x0F);
    HAL_Delay(LCD_DELAY);
}

// ************** WRITE FUNCTION ************** //
void LCD_Write(uint8_t data) {
    LCD_RS(1);
    LCD_Nibble(data >> 4);
    LCD_Nibble(data & 0x0F);
    HAL_Delay(LCD_DELAY);
}

// ************** PRINT FUNCTION ************** //
void LCD_Print(char *text) {
    while (*text) LCD_Write(*text++);
}

// ************** CLEAR FUNCTION ************** //
void LCD_Clear(void) {
    LCD_Cmd(0x01);
    HAL_Delay(2);
}

// ************** CUSTOM CHARACTER FUNCTION ************** //
void LCD_CreateChar(uint8_t loc, uint8_t map[]) {
    if (loc < 8) {
        LCD_Cmd(0x40 + (loc * 8));   // Set CGRAM address
        for (uint8_t i = 0; i < 8; i++)
            LCD_Write(map[i]);       // Write 8 bytes (pattern)
        LCD_Set(0, 0);               // Return to DDRAM
    }
}

// ************** CURSOR POSITION ************** //
void LCD_Set(uint8_t col, uint8_t row) {
    uint8_t addr[] = {0x00, 0x40, 0x14, 0x54};
    LCD_Cmd(0x80 | (col + addr[row]));
}

// ************** LOW LEVEL ************** //
static void LCD_Nibble(uint8_t nibble) {
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    LCD_Pulse();
}

static void LCD_Pulse(void) {
    LCD_EN(1);
    HAL_Delay(1);
    LCD_EN(0);
    HAL_Delay(1);
}
