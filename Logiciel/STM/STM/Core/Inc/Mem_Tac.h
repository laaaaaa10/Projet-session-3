#ifndef INC_MEM_TAC_H_
#define INC_MEM_TAC_H_

// ************** INCLUDES ************** // 
#include "main.h"
#include <stdbool.h>

// ************** VARIABLES *************** //
typedef struct {
    int x;
    int y;
} Point;

// ************** FUNCTIONS ************** //
Point Simple_Tab(int *data);
Point Lire_Tab(int *coord_Table);

#endif
