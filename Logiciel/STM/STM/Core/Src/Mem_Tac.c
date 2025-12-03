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
Point Lire_Tab(uint8_t* Coord_Table){
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
    if ((Tab.x < 0) && (Tab.y < 0)){
        return Coord_Mem;
    }

    Coord_Mem = Tableau_Mem[Tab.x][Tab.y];

    return Coord_Mem;
}

// ***************************** FUNCTIONS ********************************* //
Point Simple_Tab(uint8_t* data){
    Point Tab = {-1, -1};

    if ((data[4] == 255) && (data[4] == 255)) return Tab; // Vérif si pas de poids

    // y index from data[2]
    if       (data[4] >  130)                     Tab.y = 0;
    else if ((data[4] <= 130) && (data[4] > 80))  Tab.y = 1;
    else if  (data[4] <= 80 )                     Tab.y = 2;

    // x index from data[4]
    if       (data[3] >  220)                     Tab.x = 0;
    else if ((data[3] <= 220) && (data[3] > 210)) Tab.x = 1;
    else if ((data[3] <= 210) && (data[3] > 190)) Tab.x = 2;
    else if ((data[3] <= 190) && (data[3] > 160)) Tab.x = 3;
    else if ((data[3] <= 160) && (data[3] > 140)) Tab.x = 4;
    else if ((data[3] <= 140) && (data[3] > 100)) Tab.x = 5;
    else if  (data[3] <= 100)                     Tab.x = 6;
    
    return Tab;
}
