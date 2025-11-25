/* important info:
// forarm = 25.5 cm
// upper arm = 17.8 cm
Z
|__ Y
 \
  X   
// Gear ratio: 5:1 (motor:arm)
// (pos are in cm)
// *the robot base pivot is at 0,0,10 
// *the angles of a pivot point are relative to the previous segment*
// *the angle is the total ratio 360 degrees = to 5 rotations of the motor*
// minmax angle for motor 0 (base rotation) = inifinite
// minmax angle for motor 1 (first pivot point) = 45..200 degrees
// minmax angle for motor 2 (second pivot point) = 45..300 degrees
// minmax angle for motor 3 (wrist) = 45..300 degrees
// minmax angle for motor 4 (hand open/close) = 0(close)..125(open) degrees 
//
// note that Z is not going to work, it will constantly be at 5cm from the buildplate
*/

// *************************** INCLUDES ************************************* // 
#include "main.h"
#include "arm_logic.h"
#include <stdbool.h>
#include <math.h>


// *************************** VARIABLES ************************************ //
// output angles for the 5 motors (base rotation, pivot1, pivot2, wrist, hand)
int Pivots[5]     = {0, 0, 0, 0, 0};
int Out_Pivots[5] = {0, 0, 0, 0, 0};
int In_Coords[3]  = {0, 0, 5};   // x,y,z (z=5cm fixed)
int Distance;                     // distance from base to point in space

// length of each arm
float L1 = 17.8;
float L2 = 25.5;


//************************* SETUP MAIN PROGRAM *******************************
// the logic of the 2 pivot points and hand control is in this function
// this function takes in x,y,z coordinates
// to get the hand to a certain position in space
int ARM_LOGIC(int *In_Coords, int *Pivots){
    BASE_ROTATION(In_Coords, Pivots);
    ARM_ROTATIONS(In_Coords, Pivots);
    WRIST_ANGLE(Pivots);
    //HAND_CONTROL(Hand_action); //(prob will do a logic that it does X when ti reaches its desired coords)
    PIV_TRANSLATE(Pivots,Out_Pivots);
    
    if (!VERIFY_PIVOTS(Out_Pivots)) {
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
void BASE_ROTATION(int *In_Coords, int *Pivots){
    float x = In_Coords[0];
    float y = In_Coords[1];
    
    // gets the rotation from the coords
    Pivots[0] = atan2(y, x);
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

void ARM_ROTATIONS(int *In_Coords, int *Pivots) {
    int x = In_Coords[0];
    int y = In_Coords[1];
    int Distance = sqrt(x*x + y*y);
    
    // ----- checks if can reach -----
    if (Distance > (L1 + L2)) {
        return -1;   // too far
    }
    if (Distance < fabsf(L1 - L2)) {
        return -1;   // too close
    }

    // ----- elbow angle -----
    // elbow = acos((L1^2 + L2^2 - d^2) / (2*L1*L2))
    // 1.0f or -1.0f = Clamp float for acos safety
    float cos_elbow = (L1*L1 + L2*L2 - Distance*Distance) / (2.0f * L1 * L2);
    if (cos_elbow > 1.0f) cos_elbow = 1.0f;
    if (cos_elbow < -1.0f) cos_elbow = -1.0f;
    float elbow_deg = acosf(cos_elbow) * 180.0f / 3.14159f;


    // ----- shoulder angle -----
    // shoulder = angle_to_target - acos((d^2 + L1^2 - L2^2)/(2*L1*d))
    float cos_sh_tri = (Distance*Distance + L1*L1 - L2*L2) / (2.0f * L1 * Distance);
    if (cos_sh_tri > 1.0f) cos_sh_tri = 1.0f;
    if (cos_sh_tri < -1.0f) cos_sh_tri = -1.0f;
    float sh_tri_deg = acosf(cos_sh_tri) * 180.0f / 3.14159f;

    // idfk what that does
    float angle_to_target_deg = atan2f(y, x) * 180.0f / 3.14159f;
    float shoulder_deg = angle_to_target_deg - sh_tri_deg;

    Pivots[1] = (int)roundf(shoulder_deg);
    Pivots[2] = (int)roundf(180.0f - elbow_deg); 

    return 0;
}

// ----- WRIST ANGLE (PIV 3)----- //
// controlls the joint of the hand, primarly keeps it faced down releative
// to pivot point 1 and 2

// imagin angle of pivot 1 (in that case its 125 degrees)
// and angle of pivot 2  (in that case its 135 degrees)
// - to get the hand to be down (90 degrees relative to horizontal)
//   you need to do 180 - (pivot1 + pivot2) = 90
void WRIST_ANGLE(int *Pivots){
    // Wrist angle = always keep hand down relative to arm
    Pivots[3] = 180 - (Pivots[1] + Pivots[2]);
}

// ----- HAND CONTROL (PIV 4) ----- //
// controls the opening and closing of the hand
void HAND_CONTROL(int *Pivots, int Hand_action){

}
// ----- TRANSLATE PIVOTS ----- //
// translates the degres into teh correct value between (0 to 205)
//0 being _____ and 205 being _____   (motors are PWM)
// and does the gear ratio multiplication (x5)
void PIV_TRANSLATE(int *Pivots, int *Out_Pivots){
    // Pivot1
    Out_Pivots[1] = (int)((float)(Pivots[1] - 45) / (200 - 45) * 205) * 5;
    // Pivot2
    Out_Pivots[2] = (int)((float)(Pivots[2] - 45) / (300 - 45) * 205) * 5;
    // Pivot3
    Out_Pivots[3] = (int)((float)(Pivots[3] - 45) / (300 - 45) * 205) * 5;
    // Pivot4
    Out_Pivots[4] = (int)((float)(Pivots[4] - 0) / 125 * 205) * 5
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





