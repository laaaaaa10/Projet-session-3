// ************************************************************************* // 
// File: Mem_Tac.c
// Done by : Thomas
// Description :
// use to transform the hex values of the table into cardinal coordinates
//
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "Mem_Tac.h"

// ************************* SETUP MAIN PROGRAM **************************** //
Point Lire_Tab(uint8_t* Coord_Table){
    Point Coord_Mem = {0, 0};
    
    // Tableau 2D de Points
    Point Tableau_Mem[7][3] = {
        {{-3,17},{0,17},{6,17}},
        {{-3,20},{0,20},{6,20}},
        {{-3,23},{0,23},{6,23}},
        {{-3,26},{0,26},{6,26}},
        {{-3,29},{0,29},{6,29}},
        {{-3,32},{0,32},{6,32}},
        {{-3,35},{0,35},{6,35}} 
    };

    Point Tab = Simple_Tab(Coord_Table);

    // Bounds check - return {0,0} if invalid
    if ((Tab.x < 0) || (Tab.y < 0)){  // FIX: was && (would miss single -1)
        return Coord_Mem;
    }

    Coord_Mem = Tableau_Mem[Tab.x][Tab.y];

    return Coord_Mem;
}

// ***************************** FUNCTIONS ********************************* //
Point Simple_Tab(uint8_t* data){
    Point Tab = {-1, -1};

    if ((data[3] == 0xFF) || (data[4] == 0xFF)) return Tab;  // FIX: was data[4] twice

    // y index from data[4]
    if       (data[4] >  130)                     Tab.y = 0;
    else if ((data[4] <= 130) && (data[4] > 80))  Tab.y = 1;
    else if  (data[4] <= 80 )                     Tab.y = 2;

    // x index from data[3]
    if       (data[3] >  200)                     Tab.x = 0;
    else if ((data[3] <= 200) && (data[3] > 180)) Tab.x = 1;
    else if ((data[3] <= 180) && (data[3] > 170)) Tab.x = 2;
    else if ((data[3] <= 170) && (data[3] > 150)) Tab.x = 3;
    else if ((data[3] <= 150) && (data[3] > 120)) Tab.x = 4;
    else if ((data[3] <= 120) && (data[3] > 100)) Tab.x = 5;
    else if  (data[3] <= 100)                     Tab.x = 6;
    
    return Tab;
}