#ifndef ARM_LOGIC_H
#define ARM_LOGIC_H

// ************** INCLUDES ************** //
#include "main.h"
#include "UART_Com.h"
#include <stdbool.h>

// ************ VARIABLES *************** //
// output angles for the 5 motors (base rotation, pivot1, pivot2, wrist, hand)
extern int Pivots[5];
extern int Out_Pivots[5];

// ************** FUNCTION PROTOTYPES ************** //
int ARM_LOGIC(int x_coord, int y_coord, int z_coord, bool hand_inst, int *Out_Pivots);
int pwm_to_deg(int pwm, int deg0, int deg205);
int ARM_ROTATIONS(int *Pivots);

void BASE_ROTATION(int *Pivots);
void WRIST_ANGLE(int *Pivots);
void PIV_TRANSLATE(int *Pivots, int *Out_Pivots);
void ESTIMATE_DELAY(void);
void HAND_CONTROL(int *Out_Pivots, bool hand_state);

bool VERIFY_PIVOTS(int *Out_Pivots);

#endif
