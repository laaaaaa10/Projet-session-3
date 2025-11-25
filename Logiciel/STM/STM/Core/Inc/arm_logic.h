// ************** INCLUDES ************** //
#include "main.h"
#include <stdbool.h>

#ifndef ARM_LOGIC_H
#define ARM_LOGIC_H

// ************** FUNCTION PROTOTYPES ************** //
int ARM_LOGIC(int *In_Coords, int *Pivots);

void BASE_ROTATION(int *In_Coords, int *Pivots);
void ARM_ROTATIONS(int *In_Coords, int *Pivots);
void WRIST_ANGLE(int *Pivots);
void HAND_CONTROL(int *Pivots, int Hand_action);
void PIV_TRANSLATE(int *Pivots, int *Out_Pivots)
bool VERIFY_PIVOTS(int *Out_Pivots);

#endif
