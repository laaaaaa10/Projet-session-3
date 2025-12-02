// ************************************************************************* // 
// File: Me_Tac.c
// Done by :
// Description :
//
//
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "Mem_Tac.h"

// ************************* SETUP MAIN PROGRAM **************************** //
Point Lire_Tab(int *Coord_Table){
    Point Coord_Mem = {0, 0};
    
    // Tableau 2D de Points
    Point Tableau_Mem[7][3] = {
        {{-3,17},{1,17},{5,17}},
        {{-3,20},{1,20},{5,20}},
        {{-3,23},{1,23},{5,23}},
        {{-3,26},{1,26},{5,26}},
        {{-3,29},{1,29},{5,29}},
        {{-3,32},{1,32},{5,32}},
        {{-3,35},{1,35},{5,35}} 
    };

    Point Tab = Simple_Tab(Coord_Table);

    // Bounds check - return {0,0} if invalid
    if (Tab.x < 0 || Tab.x > 6 || Tab.y < 0 || Tab.y > 2){
        return Coord_Mem;
    }

    Coord_Mem = Tableau_Mem[Tab.x][Tab.y];

    return Coord_Mem;
}

// ***************************** FUNCTIONS ********************************* //
Point Simple_Tab(int *data){
    Point Tab = {0, 0};

    // y index from data[2]
    if ((data[2] > 0x9E) && (data[2] < 0xA2)) Tab.y = 0;
    else if ((data[2] > 0x66) && (data[2] < 0x6A)) Tab.y = 1;
    else if ((data[2] > 0x36) && (data[2] < 0x3A)) Tab.y = 2;

    // x index from data[3]
    if ((data[3] > 0xD5) && (data[3] < 0xD9)) Tab.x = 0;
    else if ((data[3] > 0xBF) && (data[3] < 0xC3)) Tab.x = 1;
    else if ((data[3] > 0xAC) && (data[3] < 0xB0)) Tab.x = 2;
    else if ((data[3] > 0x96) && (data[3] < 0x9A)) Tab.x = 3;
    else if ((data[3] > 0x7C) && (data[3] < 0x80)) Tab.x = 4;
    else if ((data[3] > 0x66) && (data[3] < 0x6A)) Tab.x = 5;
    else if ((data[3] > 0x4C) && (data[3] < 0x50)) Tab.x = 6;

    return Tab;
}
