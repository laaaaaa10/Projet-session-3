#ifndef INC_MEM_TAC_H_
#define INC_MEM_TAC_H_

// ************** INCLUDES ************** // 
#include "main.h"
#include <stdbool.h>

// ************** VARIABLES *************** //
typedef struct {
    float  x;
    float  y;
} Point;

// ************** FUNCTIONS ************** //
Point Simple_Tab(uint8_t *data);
Point Lire_Tab(uint8_t *coord_Table);

#endif
