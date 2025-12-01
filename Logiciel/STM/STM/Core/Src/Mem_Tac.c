// ************************************************************************* // 
// File: Me_Tac.c
// Done by :
// Description :
//
//
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "Mem_Tac.h"

// *************************** DEFINES ************************************* //


// *************************** VARIABLES *********************************** //


// ************************* SETUP MAIN PROGRAM **************************** //
Point Lire_Tab(int *KKK){
    Point Coord_Mem = {0,0};   // Initialisation de la variable qui contiendra la coordonnée finale

    // Tableau 2D de Points représentant des coordonnées x et y pour chaque position
    // 7 lignes (colonnes logiques), 3 colonnes (lignes logiques)
    Point Tableau_Mem[7][3] = {
        {{17,-3},{17,1},{17,5}},  // 1ère colonne : x=17, y varie
        {{20,-3},{20,1},{20,5}},  // 2ème colonne : x=20
        {{23,-3},{23,1},{23,5}},  // 3ème colonne : x=23
        {{26,-3},{26,1},{26,5}},  // 4ème colonne : x=26
        {{29,-3},{29,1},{29,5}},  // 5ème colonne : x=29
        {{32,-3},{32,1},{32,5}},  // 6ème colonne : x=32
        {{35,-3},{35,1},{35,5}}   // 7ème colonne : x=35
    };

    // Récupération de l'indice correspondant aux données fournies dans KKK
    // Tab contient les indices x et y pour accéder au tableau
    Point Tab = Simple_Tab(KKK);   // Appel à Simple_Tab pour traduire les valeurs de KKK en indices Tab.x et Tab.y

    // Vérification de sécurité pour éviter un dépassement du tableau
    if (Tab.y < 0 || Tab.y > 2 || Tab.x < 0 || Tab.x > 6){
        Coord_Mem.x = 0;
        Coord_Mem.y = 0;
        return Coord_Mem; // valeur par défaut si les indices sont hors limites
    }

    // Récupération de la coordonnée correspondante dans le tableau
    Coord_Mem = Tableau_Mem[Tab.y][Tab.x];

    return Coord_Mem;  // Retourne la coordonnée finale
}

// ***************************** FUNCTIONS ********************************* //
// ----- Lire_Tableau ----- //
// Fonction qui convertit les valeurs du tableau data en indices x et y
Point Simple_Tab(int *data){
    Point Tab = {0,0};  // Initialisation des indices

    // Détermination de l'indice y en fonction de la valeur data[2]
    if ((data[2] > 0x7E) && (data[2] < 0xA2)) Tab.y = 0; // A
    if ((data[2] > 0x66) && (data[2] < 0x6A)) Tab.y = 1; // B
    if ((data[2] > 0x36) && (data[2] < 0x3A)) Tab.y = 2; // C

    // Détermination de l'indice x en fonction de la valeur data[3]
    if ((data[3] > 0xD5) && (data[3] < 0xD9)) Tab.x = 0; // 1
    if ((data[3] > 0xBF) && (data[3] < 0xC3)) Tab.x = 1; // 2
    if ((data[3] > 0xAC) && (data[3] < 0xB0)) Tab.x = 2; // 3
    if ((data[3] > 0x96) && (data[3] < 0xA0)) Tab.x = 3; // 4
    if ((data[3] > 0x7C) && (data[3] < 0x80)) Tab.x = 4; // 5
    if ((data[3] > 0x66) && (data[3] < 0x70)) Tab.x = 5; // 6
    if ((data[3] > 0x4C) && (data[3] < 0x50)) Tab.x = 6; // 7

    return Tab; // Retourne les indices x et y
}
