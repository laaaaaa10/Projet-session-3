/* important info:
// forarm = 25.5 cm
// upper arm = 17.8 cm
Z
|__ Y
 \
  X   
// (pos are in cm)
// *the robot base pivot is at 0,0,10 
// *the angles of a pivot point are relative to the previous segment*
// *the angle is the total ratio 360 degrees = to 5 rotations of the motor*
// minmax angle for motor 0 (base rotation) = -169..169
// minmax angle for motor 1 (first pivot point) = 0..131 degrees 
// minmax angle for motor 2 (second pivot point) = 300..60 degrees
// minmax angle for motor 3 (wrist) = 300..60 degrees
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
int Distance;                     // distance from base to point in space
// length of each arm
float L1 = 17.8;
float L2 = 25.5;


//************************* SETUP MAIN PROGRAM *******************************
// the logic of the 2 pivot points and hand control is in this function
// this function takes in x,y coordinates
// to get the hand to a certain position in space
int ARM_LOGIC(int *In_Coords, int *Out_Pivots){
    int Pivots[5] = {0,0,0,0,0};

    BASE_ROTATION(In_Coords, Pivots); {
        float bx = (float)In_Coords[0];
        float by = (float)In_Coords[1];
        Pivots[0] = (int)roundf(atan2f(by, bx) * 180.0f / 3.14159f);
    }

    if (ARM_ROTATIONS(In_Coords, Pivots) != 0) {
        return -1;
    }
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
    Pivots[0] = (int)roundf(atan2f(y, x) * 180.0f / 3.14159f);
}

// ----- ARM ROTATIONS (PIV 1 & 2) ----- //
// controls the first pivot point of the arm

// the 2 main pivot points:
// we do some trigonometry to get the angles of the 2 pivot points
// we start by getting the distance from the base to the point in space
// d = sqrt(x^2 + y^2)
// now with d, we can get the angles using the law of cosines
// we can change the arm distance if for example the pivot 1 is at 90 degrees
// and the pivot 2 is at 0(or 360) degrees, the arm is as close as possible to the base 
// (technically it would be folded on itself)

// if pivot 1 is at 90 degrees and pivot 2 is at 180 degrees, the arm is fully extended upwards
// the furdest it can reach is when pivot 2 is at 45 and pivot 1 is at ____ 
// so if we have the distance d,
// we can compute r = sqrt(d^2 + z^2)
// check if r is reachable
// use law of cosines: elbow_angle = acos((L1^2 + L2^2 - r^2)/(2*L1*L2))
// compute shoulder_angle = atan2(z, d) - atan2(L2*sin(elbow_angle), L1 + L2*cos(elbow_angle))
int ARM_ROTATIONS(int *In_Coords, int *Pivots) {
    float x = (float) In_Coords[0];
    float y = (float) In_Coords[1];
    
    // Horizontal distance from base to target
    float d = hypotf(x, y);
    
    // Vertical offset: wrist at 15cm, shoulder at 10cm → z = 5cm above shoulder;
    float z = 10.0f;
    float compensation;
    
    if (d <= 15.0f) {
        // 10→15: error 4→5
        compensation = -4.0f - (d - 10.0f) * 0.2f;
    } else if (d <= 25.0f) {
        // 15→25: error 5→0
        compensation = -5.0f + (d - 15.0f) * 0.5f;
    } else if (d <= 35.0f) {
        // 25→35: error 0→-3
        compensation = (d - 25.0f) * 0.3f;
    } else {
        // 35+: stays -3
        compensation = 3.0f;
    }
    
    z += compensation;

    // 3D distance in vertical plane from shoulder to wrist
    float r = hypotf(d, z);
    
    float L1f = L1;
    float L2f = L2;
    
    if (r > (L1f + L2f) - 0.001f) return -1;
    if (r < fabsf(L1f - L2f) + 0.001f) return -1;
    
    // Internal angle between the two arms (this equals Pivot 2)
    float cos_internal = (L1f*L1f + L2f*L2f - r*r) / (2.0f * L1f * L2f);
    if (cos_internal > 1.0f) cos_internal = 1.0f;
    if (cos_internal < -1.0f) cos_internal = -1.0f;
    float internal_rad = acosf(cos_internal);
    float pivot2_deg = internal_rad * (180.0f / 3.14159265f);
    
    // Shoulder angle calculation
    // Angle to target from shoulder (line of sight)
    float alpha = atan2f(z, d);
    
    // Triangle angle at shoulder (law of cosines, not atan2)
    float cos_beta = (L1f*L1f + r*r - L2f*L2f) / (2.0f * L1f * r);
    if (cos_beta > 1.0f) cos_beta = 1.0f;
    if (cos_beta < -1.0f) cos_beta = -1.0f;
    float beta = acosf(cos_beta);
    
    // Shoulder angle (elbow-up configuration)
    float pivot1_rad = alpha + beta;
    float pivot1_deg = pivot1_rad * (180.0f / 3.14159265f);
    
    int s = (int) roundf(pivot1_deg);
    int e = (int) roundf(pivot2_deg);
    
    if (s < 0) s = 0;
    if (s > 131) s = 131;
    if (e < 0) e = 0;
    if (e > 400) e = 400;
    
    Pivots[1] = s;
    Pivots[2] = e;
    return 0;
}

// ----- WRIST ANGLE (PIV 3)----- //
// controlls the joint of the hand, primarly keeps it faced down releative
// to pivot point 1 and 2

// imagin angle of pivot 1 (in that case its 45 degrees)
// and angle of pivot 2  (in that case its 100 degrees)
// - to get the hand to be down (90 degrees relative to horizontal)
//   you need to do __________
void WRIST_ANGLE(int *Pivots){
    // Wrist angle = always keep hand down relative to arm
    // Minimal implementation: try to keep end-effector "down".
    // Use the same conventions as Pivots: Pivots[1] is shoulder deg, Pivots[2] is motor-style (180 - elbow_deg).
    // Simple heuristic: wrist = 270 - (shoulder + elbow_motor). Keep as integer.
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
// controls the opening and closing of the hand
void HAND_CONTROL(int *Pivots, int Hand_action){
    // minimal clamp behavior
    if (Hand_action < 0) Hand_action = 0;
    if (Hand_action > 125) Hand_action = 125;
    Pivots[4] = Hand_action;
}

// ----- TRANSLATE PIVOTS ----- //
// translates the degres into teh correct value between (0 to 205)
//
// Using the table endpoints (first and last rows) to build a linear mapping
// PWM 0 -> angle0_0, PWM 205 -> angle0_205 (same for pivots 1 and 2).
// We produce integer PWM values (0..205).
//
// Endpoints derived from the table you supplied:
// pivot0: PWM0 = -169, PWM205 = 165
// pivot1: PWM0 = 131,  PWM205 = 0
// pivot2: PWM0 = 34,   PWM205 = 375
static inline int linear_deg_to_pwm(int deg, int deg0, int deg205){
    // denom = deg205 - deg0
    int denom = deg205 - deg0;
    if (denom == 0) return 0;
    // compute numerator with 64-bit to avoid overflow: (deg - deg0) * 205
    long long numer = (long long)(deg - deg0) * 205LL;
    long long absden = (denom >= 0) ? denom : - (long long)denom;
    // do rounding toward nearest:
    long long adj = absden / 2;
    long long pwm;
    if (denom > 0) {
        if (numer >= 0) pwm = (numer + adj) / denom;
        else pwm = - ( ( -numer + adj ) / denom );
    } else {
        // denom < 0: result sign flips
        if (numer >= 0) pwm = - ( ( numer + adj ) / absden );
        else pwm = ( ( -numer + adj ) / absden );
    }
    // clamp
    if (pwm < 0) pwm = 0;
    if (pwm > 205) pwm = 205;
    return (int)pwm;
}

// inverse helper: pwm -> deg using endpoints
// inverse: pwm -> deg using endpoints deg0 @ 0 and deg205 @ 205
int pwm_to_deg(int pwm, int deg0, int deg205) {
    // deg = deg0 + (deg205 - deg0) * pwm / 205  (rounded to nearest)
    int denom = 205;
    long long numer = (long long)(deg205 - deg0) * (long long)pwm;
    long long adj = denom / 2;
    long long frac = (numer >= 0) ? ( (numer + adj) / denom ) : ( - ( ( -numer + adj ) / denom ) );
    long long out = (long long)deg0 + frac;
    return (int)out;
}

void PIV_TRANSLATE(int *Pivots, int *Out_Pivots){
    // pivot0: deg0 = -169, deg205 = 165
    Out_Pivots[0] = linear_deg_to_pwm(Pivots[0], -169, 165);
    
    // pivot1: Piecewise linear from calibration table
    {
        int deg = Pivots[1];
        int pwm;
        if (deg >= 90) {
            pwm = 50 + (131 - deg) * 43 / 41;
        } else {
            pwm = 93 + (90 - deg) * 49 / 90;
        }
        if (pwm < 0) pwm = 0;
        if (pwm > 205) pwm = 205;
        Out_Pivots[1] = pwm;
    }
    
    // pivot2: deg0 = 34, deg205 = 375
    Out_Pivots[2] = linear_deg_to_pwm(Pivots[2], 34, 375);
    
    // pivot3: 249° → PWM 50, 90° → PWM 160
    {
        int deg = Pivots[3];
        int pwm = 50 + (249 - deg) * 110 / 159;
        if (pwm < 0) pwm = 0;
        if (pwm > 205) pwm = 205;
        Out_Pivots[3] = pwm;
    }
    
    // pivot4: hand
    if (Pivots[4] < 0) Out_Pivots[4] = 0;
    else if (Pivots[4] > 125) Out_Pivots[4] = 125;
    else Out_Pivots[4] = Pivots[4];
}

// ----- VERIFY PIVOTS ----- //// makes sure the pivot angles are within the min/max range
bool VERIFY_PIVOTS(int *Out_Pivots) {
// -------------------------------------------------------------
// NOTE ABOUT VERIFY_PIVOTS:
//
// Out_Pivots[] contains *PWM values* after translation,
// We convert back to degrees using the inverse linear mapping
// (derived from the same endpoints) and compare to the mechanical ranges.
    // motor0: -169..169
    int a0 = pwm_to_deg(Out_Pivots[0], -169, 165);
    if (a0 < -169 || a0 > 169) return false;

    // motor1: 0..131
    int a1 = pwm_to_deg(Out_Pivots[1], 131, 0);
    if (a1 < 0 || a1 > 131) return false;

    // motor2: allowed if >=300 or <=60 (wrapped)
    int a2 = pwm_to_deg(Out_Pivots[2], 34, 375);
    if (!((a2 >= 300 && a2 <= 360) || (a2 >= 0 && a2 <= 60))) return false;

    // motor3: wrist same wrapped check using pivot2 mapping
    int a3 = pwm_to_deg(Out_Pivots[3], 34, 375);
    if (!((a3 >= 300 && a3 <= 360) || (a3 >= 0 && a3 <= 60))) return false;

    // motor4: 0..125 (already PWM mapped directly)
    int a4 = Out_Pivots[4];
    if (a4 < 0 || a4 > 125) return false;

    return true;
}
