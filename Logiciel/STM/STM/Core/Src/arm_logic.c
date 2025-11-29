// ************************************************************************* // 
// File: arm_logic.c
// Done by : Javier
// Description :
// send coordfs and return the pwm of each motors
//
// ************************************************************************* // 

/* important info:
// forarm = 25.5 cm
// upper arm = 17.8 cm
Z
|__ Y
 \
  X   
// (pos are in cm)
// the robot base pivot is at 0,0,10 
// the angles of a pivot point are relative to the previous segment
// the angle is the total ratio 360 degrees = to 5 rotations of the motor
// minmax angle for motor 0 (base rotation) = -169..169
// minmax angle for motor 1 (first pivot point) = 0..131 degrees 
// minmax angle for motor 2 (second pivot point) = 300..60 degrees
// minmax angle for motor 3 (wrist) = 300..60 degrees
// minmax angle for motor 4 (hand open/close) = 0(close)..125(open) degrees */

// *************************** INCLUDES ************************************* // 
#include "main.h"
#include "arm_logic.h"
#include <stdbool.h>
#include <math.h>

// *************************** DEFINES ************************************ //
#define PI 3.14159f

// *************************** VARIABLES ************************************ //
// length of each arm
float L1 = 17.8;
float L2 = 25.5;

// coordinates
float x;
float y;

// calculated values
float distance;
float height;
float reach;
float compensation;

int Pivots[5] = {0,0,0,0,0};


//************************* SETUP MAIN PROGRAM *******************************
// controlls every parts of the arm
int ARM_LOGIC(int *In_Coords, int *Out_Pivots){
    
    x = (float)In_Coords[0];
    y = (float)In_Coords[1];

    BASE_ROTATION(Pivots);

    if (ARM_ROTATIONS(In_Coords, Pivots) != 0) {
        return -1; // return error code
    }

    WRIST_ANGLE(Pivots);
    PIV_TRANSLATE(Pivots,Out_Pivots);

    if (!VERIFY_PIVOTS(Out_Pivots)) {
        return -1; // return error code
    }

    return 0;
}

// ***************************** FUNCTIONS ************************************* //
// ----- BASE ROTATION (PIV 0) ----- //
// Calculates the base rotation angle to point the arm toward the target
// Uses atan2(y,x) to get the angle in the horizontal plane
void BASE_ROTATION(int *Pivots){
    Pivots[0] = (int)roundf(atan2f(y, x) * 180.0f / PI);
}

// ----- ARM ROTATIONS (PIV 1 & 2) ----- //
// Calculates shoulder and elbow angles using inverse kinematics
// First gets horizontal distance, then adds vertical offset (height)
// 'reach' is the 3D straight-line distance from shoulder to target wrist position
// Uses law of cosines to solve the triangle formed by upper arm, forearm, and reach
// Returns -1 if target is unreachable (too far or too close)
int ARM_ROTATIONS(int *In_Coords, int *Pivots) {    
    // Horizontal distance from base to target
    distance = hypotf(x, y);
    
    // Vertical offset and also need to apply
    // compensation based on distance
    height = 10.0f;
    
    // 10→15: error 4→5
    if      (distance <= 15.0f) {compensation = -4.0f - (distance - 10.0f) * 0.2f;} 
    // 15→25: error 5→0  
    else if (distance <= 25.0f) {compensation = -5.0f + (distance - 15.0f) * 0.5f;} 
    // 25→35: error 0→-3
    else if (distance <= 35.0f) {compensation = (distance - 25.0f) * 0.3f;} 
    // 35+: stays -3
    else                        {compensation = 3.0f;}
    
    height += compensation;

    // 3D distance in vertical plane from shoulder to wrist
    reach = hypotf(distance, height);
    
    float L1f = L1;
    float L2f = L2;
    
    // Check if target is reachable
    if (reach > (L1f + L2f) - 0.001f) return -1;  // too far
    if (reach < fabsf(L1f - L2f) + 0.001f) return -1;  // too close
    
    // Calculate elbow angle using law of cosines
    // This is the internal angle between upper arm and forearm
    float cos_internal = (L1f*L1f + L2f*L2f - reach*reach) / (2.0f * L1f * L2f);
    if (cos_internal > 1.0f) cos_internal = 1.0f;
    if (cos_internal < -1.0f) cos_internal = -1.0f;
    float internal_rad = acosf(cos_internal);
    float pivot2_deg = internal_rad * (180.0f / PI);
    
    // Calculate shoulder angle
    // alpha = angle to target from horizontal
    // beta = angle offset due to arm triangle geometry
    float alpha = atan2f(height, distance);
    
    float cos_beta = (L1f*L1f + reach*reach - L2f*L2f) / (2.0f * L1f * reach);
    if (cos_beta > 1.0f) cos_beta = 1.0f;
    if (cos_beta < -1.0f) cos_beta = -1.0f;
    float beta = acosf(cos_beta);
    
    // Shoulder angle = target angle + geometry offset
    float pivot1_rad = alpha + beta;
    float pivot1_deg = pivot1_rad * (180.0f / PI);
    
    int s = (int) roundf(pivot1_deg);
    int e = (int) roundf(pivot2_deg);
    
    // Clamp to motor limits
    if (s < 0) s = 0;
    if (s > 131) s = 131;
    if (e < 0) e = 0;
    if (e > 400) e = 400;
    
    Pivots[1] = s;
    Pivots[2] = e;
    return 0;
}

// ----- WRIST ANGLE (PIV 3)----- //
// Keeps the gripper pointing straight down regardless of arm position
// Compensates for the combined angles of shoulder and elbow
// Formula: 270° - shoulder - elbow keeps gripper vertical
void WRIST_ANGLE(int *Pivots){
    int shoulder = Pivots[1];
    int elbow = Pivots[2];
    
    // Keep gripper pointing straight down
    int wrist = 270 - shoulder - elbow;
    
    // Normalize to 0-360
    while (wrist < 0) wrist += 360;
    while (wrist >= 360) wrist -= 360;
    
    Pivots[3] = wrist;
}

// ----- HAND CONTROL (PIV 4) ----- //
// Controls gripper opening: 0 = fully closed, 125 = fully open
void HAND_CONTROL(int *Pivots, int Hand_action){
    if (Hand_action < 0) Hand_action = 0;
    if (Hand_action > 125) Hand_action = 125;
    Pivots[4] = Hand_action;
}

// ----- TRANSLATE PIVOTS ----- //
// Converts angle values (degrees) into PWM motor control values (0-205)
// Uses linear interpolation between calibration table endpoints
// Different pivots have different angle-to-PWM mappings based on physical calibration
// i genuenly dont know how this magic works so i wont even bother tryign to
// understand what gpt cooked
static inline int linear_deg_to_pwm(int deg, int deg0, int deg205){
    int denom = deg205 - deg0;
    if (denom == 0) return 0;
    long long numer = (long long)(deg - deg0) * 205LL;
    long long absden = (denom >= 0) ? denom : - (long long)denom;
    long long adj = absden / 2;
    long long pwm;
    if (denom > 0) {
        if (numer >= 0) pwm = (numer + adj) / denom;
        else pwm = - ( ( -numer + adj ) / denom );
    } else {
        if (numer >= 0) pwm = - ( ( numer + adj ) / absden );
        else pwm = ( ( -numer + adj ) / absden );
    }
    if (pwm < 0) pwm = 0;
    if (pwm > 205) pwm = 205;
    return (int)pwm;
}

// Inverse function: converts PWM back to degrees for verification
int pwm_to_deg(int pwm, int deg0, int deg205) {
    int denom = 205;
    long long numer = (long long)(deg205 - deg0) * (long long)pwm;
    long long adj = denom / 2;
    long long frac = (numer >= 0) ? ( (numer + adj) / denom ) : ( - ( ( -numer + adj ) / denom ) );
    long long out = (long long)deg0 + frac;
    return (int)out;
}

void PIV_TRANSLATE(int *Pivots, int *Out_Pivots){
    int deg = Pivots[1];
    int pwm;

    // pivot0: base rotation, deg0 = -169, deg205 = 165    
    Out_Pivots[0] = linear_deg_to_pwm(Pivots[0], -169, 165); {
        if (deg >= 90) {
            pwm = 50 + (131 - deg) * 43 / 41;
        } 
        else {
            pwm = 93 + (90 - deg) * 49 / 90;
        }
        if (pwm < 0) pwm = 0;
        if (pwm > 205) pwm = 205;

        Out_Pivots[1] = pwm;     // pivot1: shoulder, uses piecewise linear from calibration table
    }

    // pivot2: elbow, deg0 = 34, deg205 = 375    
    Out_Pivots[2] = linear_deg_to_pwm(Pivots[2], 34, 375); {
        int deg = Pivots[3];
        int pwm = 50 + (249 - deg) * 110 / 159;
        if (pwm < 0) pwm = 0;
        if (pwm > 205) pwm = 205;

        Out_Pivots[3] = pwm;    // pivot3: wrist, 249° → PWM 50, 90° → PWM 160
    }
    
    // pivot4: gripper, direct mapping (0-125)
    if (Pivots[4] < 0) Out_Pivots[4] = 0;
    else if (Pivots[4] > 125) Out_Pivots[4] = 125;
    else Out_Pivots[4] = Pivots[4];
}

// ----- VERIFY PIVOTS ----- //
// Converts PWM values back to degrees and checks if they're within mechanical limits
// Returns false if any motor would be commanded outside its safe range
bool VERIFY_PIVOTS(int *Out_Pivots) {
    // motor0: base rotation, -169..169 degrees
    int a0 = pwm_to_deg(Out_Pivots[0], -169, 165);
    if (a0 < -169 || a0 > 169) return false;

    // motor1: shoulder, 0..131 degrees
    int a1 = pwm_to_deg(Out_Pivots[1], 131, 0);
    if (a1 < 0 || a1 > 131) return false;

    // motor2: elbow, wrapped range (300-360 or 0-60 degrees)
    int a2 = pwm_to_deg(Out_Pivots[2], 34, 375);
    if (!((a2 >= 300 && a2 <= 360) || (a2 >= 0 && a2 <= 60))) return false;

    // motor3: wrist, wrapped range (300-360 or 0-60 degrees)
    int a3 = pwm_to_deg(Out_Pivots[3], 34, 375);
    if (!((a3 >= 300 && a3 <= 360) || (a3 >= 0 && a3 <= 60))) return false;

    // motor4: gripper, 0..125 (already in correct range)
    int a4 = Out_Pivots[4];
    if (a4 < 0 || a4 > 125) return false;

    return true;
}