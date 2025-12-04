#ifndef GUI_H_
#define GUI_H_

// *************************** INCLUDES ************************************ //
#include "main.h"
#include "lcd.h"
#include "adc.h"

// *************************** DEFINES ************************************ //
#define MANUAL 69
#define AUTO   67

// *************************** VARIABLES ************************************ //
extern uint8_t up[8];
extern uint8_t down[8];
extern int Out_Pivots[5];

// ************************ FUNCTION PROTOTYPES **************************** //

void GUI_Init(void);
void Run_GUI(int x_coord, int y_coord, int ctrl_mode, int *Out_Pivots);
uint16_t ADC_Read_Balance(void);
uint16_t ADC_Read_Pince(void);

#endif