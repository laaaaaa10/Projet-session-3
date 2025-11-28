// ************** INCLUDES ************** // 
#include "main.h"
#include <stdbool.h>

// ************** DEFINES *************** //
#ifndef INC_MEM_TAC_H_
#define INC_MEM_TAC_H_

// ************** VARIABLES *************** //
// ************** STRUCTURE *************** //
typedef struct {
    char x;
    char y;
} Point;

// ************** FUNCTION ************** //
Point Simple_Tab(int *data);
Point Lire_Tab(int *KKK);

#endif
