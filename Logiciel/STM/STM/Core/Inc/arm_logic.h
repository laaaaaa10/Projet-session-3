// ************** INCLUDES ************** //
#include "main.h"
#include <stdbool.h>

#ifndef ARM_LOGIC_H
#define ARM_LOGIC_H

// ************** FUNCTION PROTOTYPES ************** //
int ARM_LOGIC(int *In_Coords, int *Out_Pivots);

void BASE_ROTATION(int *In_Coords, int *Out_Pivots);
void RARM_ROTATIONS(int *In_Coords, int *Out_Pivots);
void WRIST_ANGLE(int *Out_Pivots);
void HAND_CONTROL(int *Out_Pivots, int Hand_action);
bool VERIFY_PIVOTS(int *Out_Pivots);

#endif
