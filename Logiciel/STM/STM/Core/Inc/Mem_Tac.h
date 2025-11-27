// ************** INCLUDES ************** // 
#include "main.h"
#include <stdbool.h>

// ************** DEFINES *************** //
#ifndef INC_MEM_TAC_H_
#define INC_MEM_TAC_H_

// ************** VARIABLES *************** //

// ************** STRUCTURE *************** //
typedef struct {
    int x;
    int y;
} Point;

// ************** FUNCTION ************** //
Point Simple_Tab(int *data);
char Lire_Tab(int *KKK);

#endif
