/* important info:
// forarm = 25.5 cm
// upper arm = 17.8 cm
Z
|__ Y
 \
  X   
// *the robot base pivot is at 0,0
// *the angles of a pivot point are relative to the previous segment*
// *the angle is the total ratio 360 degrees = to X rotations of the motor*
// minmax angle for motor 0 (base rotation) = inifinite
// minmax angle for motor 1 (first pivot point) = 45..200 degrees
// minmax angle for motor 2 (second pivot point) = 45..300 degrees
// minmax angle for motor 3 (wrist) = 45..300 degrees
// minmax angle for motor 4 (hand open/close) = 0(close)..125(open) degrees 
*/

// *************************** INCLUDES ************************************* // 
#include "main.h"
#include "arm_logic.h"
#include <stdbool.h>
#include <math.h>



// *************************** VARIABLES ************************************ //
int Pivot0 = 0;      // base rotation angle
int Pivot1 = 0;      // first pivot point angle
int Pivot2 = 0;      // second pivot point angle
int Pivot3 = 0;      // wrist angle
int Pivot4 = 0;      // hand angle

// output angles for the 5 motors (base rotation, pivot1, pivot2, wrist, hand)
int Out_Pivots[5] = {Pivot0, Pivot1, Pivot2, Pivot3, Pivot4};
int In_Coords[3]  = {In_X, In_Y, In_Z}; // desired coordinates for the arm to reach (x,y,z)
int Distance;                           // distance from base to point in space



//************************* SETUP MAIN PROGRAM *******************************
// the logic of the 2 pivot points and hand control is in this function
// this function takes in x,y,z coordinates
// to get the hand to a certain position in space
void ARM_LOGIC(int *In_Coords, int *Out_Pivots){
    BASE_ROTATION(In_Coords, Out_Pivots);
    RARM_ROTATIONS(In_Coords, Out_Pivots);
    WRIST_ANGLE(Out_Pivots);
    HAND_CONTROL(Hand_action); //(prob will do a logic that it does X when ti reaches its desired coords)
    
    if(!VERIFY_PIVOTS(Out_Pivots)){
        // return error code
        return -1;
    }
    return 0;
}



//***************************** FUNCTIONS *************************************
// ----- BASE ROTATION (PIV 0)----- //
// controls the base rotation of the arm
// the base rotation:
// the robot it at 0,0 (if we look from top down)
// to get the correct base rotation angle we do
// atan2(y,x) = base rotation angle
void BASE_ROTATION(int *In_Coords, int *Out_Pivots){
    float x = In_Coords[0];
    float y = In_Coords[1];
    
    Out_Pivots[1] = atan2(y, x) * (180.0 / M_PI); // convert radians to degrees
    // need to gear ratio to return correct pivot0 value
}

// ----- ARM ROTATIONS (PIV 1 & 2) ----- //
// controls the first pivot point of the arm

// the 2 main pivot points:
// we do some trigonometry to get the angles of the 2 pivot points
// we start by getting the distance from the base to the point in space
// d = sqrt(x^2 + y^2)
// now with d, we can get the angles using the law of cosines
// we can change the arm distance if for example the pivot 1 is at 90 degrees
// and the pivot 2 is at 360 degrees, the arm is as close as possible to the base 
// (technically it would be folded on itself)

// if pivot 1 is at 180 degrees and pivot 2 is at 180 degrees, the arm is fully extended
// so if we have the distance d,
// we can compute r = sqrt(d^2 + z^2)
// check if r is reachable
// use law of cosines: elbow_angle = acos((L1^2 + L2^2 - r^2)/(2*L1*L2))
// compute shoulder_angle = atan2(z, d) - atan2(L2*sin(elbow_angle), L1 + L2*cos(elbow_angle))

void ARM_ROTATIONS(int *In_Coords, int *Out_Pivots) {
    int x = In_Coords[0];
    int y = In_Coords[1];
    int z = In_Coords[2];

    int Distance = sqrt(x*x + y*y);
    int r = sqrt(d*d + z*z); //r means ...

    Out_Pivots[1] = ...; // pivot1
    Out_Pivots[2] = ...; // pivot2
}

// ----- WRIST ANGLE (PIV 3)----- //
// controlls the joint of the hand, primarly keeps it faced down releative
// to pivot point 1 and 2

// imagin angle of pivot 1 (in that case its 45 degrees down from horzontal)
// and angle of pivot 2 (in that case its 135 degrees relative to pivot 1)
// - to get the hand to be down (0 degrees relative to horizontal)
//   you need to do 45 + (135 - 90) = 90 degrees of rotation from the base
//   so the hand angle is function of the two pivot angles
void WRIST_ANGLE(int *Out_Pivots){
    // get pivot 1 angle
    // get pivot 2 angle
    // do math to get hand angle
}

// ----- HAND CONTROL (PIV 4) ----- //
// controls the opening and closing of the hand
void HAND_CONTROL(Hand_action){

}

// ----- VERIFY PIVOTS ----- //
// makes sure the pivot angles are within the min/max range
bool VERIFY_PIVOTS(int *Out_Pivots) {
    if (Out_Pivots[1] < 45 || Out_Pivots[1] > 200) return false;
    if (Out_Pivots[2] < 45 || Out_Pivots[2] > 300) return false;
    if (Out_Pivots[3] < 45 || Out_Pivots[3] > 300) return false;
    if (Out_Pivots[4] < 0  || Out_Pivots[4] > 125) return false;
    return true;
}





