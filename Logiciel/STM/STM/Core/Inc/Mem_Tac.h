#ifndef INC_MEM_TAC_H_
#define INC_MEM_TAC_H_

// ************** INCLUDES ************** // 
#include "main.h"
#include <stdbool.h>

// ************** VARIABLES *************** //
typedef struct {
    char x;
    char y;
} Point;

// ************** FUNCTIONS ************** //
Point Simple_Tab(int *data);
Point Lire_Tab(int *KKK);

#endif
