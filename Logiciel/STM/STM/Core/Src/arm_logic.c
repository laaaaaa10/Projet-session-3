// ************************************************************************* // 
// File: arm_logic.c
// Done by : Javier
// Description :
// send coords and return the pwm of each motors
// this is the most interesting code ive made so far
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
#include "UART_Com.h"
#include "lcd.h"
#include <stdbool.h>
#include <math.h>

// *************************** DEFINES ************************************ //
#define PI 3.14159f

#define OPEN   1
#define AUTO   67

// *************************** VARIABLES ************************************ //
extern UART_HandleTypeDef huart1;

// length of each arm
float L1 = 17.8;
float L2 = 25.5;

// coordinates
float x;
float y;
float z;

// calculated values
float distance;
float height;
float reach;
float compensation;

int Pivots[5] = {0,0,0,0,0};
int prev_hand_pwm = 0;

// estim delay variables
float Old_x; 
float Old_y; 
int  estim_distance;
int  Estim_delay;

//************************* HELPER FUNCTION *******************************
// Calculates IK and sends to PIC - returns 0 on success, -1 on failure
static int MOVE_ARM(int *Out_Pivots, int delay_ms) {
    BASE_ROTATION(Pivots);

    if (ARM_ROTATIONS(Pivots) != 0) {
        return -1; // KEEP: prevents impossible IK math
    }    

    WRIST_ANGLE(Pivots);
    PIV_TRANSLATE(Pivots, Out_Pivots);

    if (!VERIFY_PIVOTS(Out_Pivots)) {
        return -1; // KEEP: prevents out-of-range motor commands
    }

    // sends the pivots value to the PIC
    UART_Send(
        (uint8_t)Out_Pivots[0],
        (uint8_t)Out_Pivots[1],
        (uint8_t)Out_Pivots[2],
        (uint8_t)Out_Pivots[3],
        (uint8_t)Out_Pivots[4]
    );

    HAL_Delay(delay_ms);
    return 0;
}

//************************* SETUP MAIN PROGRAM *******************************
// controlls every parts of the arm
int ARM_LOGIC(int x_coord, int y_coord, int z_coord, bool hand_inst, int *Out_Pivots) {
    bool was_auto = (z_coord == AUTO);
    
    x = (float)y_coord;
    y = (float)x_coord;

    // Move to position (at z=10 if AUTO, else z_coord)
    z = (z_coord == AUTO) ? 5.5f : (float)z_coord;
    ESTIMATE_DELAY();

    // Linear interpolation: 2 steps if distance > 10cm to help move straight
    if (estim_distance > 7) {
        float final_x = x;
        float final_y = y;
        float final_z = z;
        int half_delay = (Estim_delay / 2);
        
        // mid way pos depending on the distance
        if (estim_distance > 7) {
            x = Old_x + (final_x - Old_x) * 0.65f;
            y = Old_y + (final_y - Old_y) * 0.65f;
            z = final_z + (float)estim_distance - 5.0f;
        } 
        
        MOVE_ARM(Out_Pivots, half_delay);

        // final position - this one matters
        x = final_x;
        y = final_y;
        z = final_z;
        
        if (MOVE_ARM(Out_Pivots, half_delay) != 0) {
            return -1; // target unreachable
        }
    } else {
        if (MOVE_ARM(Out_Pivots, Estim_delay) != 0) {
            return -1; // target unreachable
        }
    }

    // If AUTO, lower to (grab position)
    if (was_auto) {
        z = 4.0f;
        MOVE_ARM(Out_Pivots, 800);  
    }
    
    // controll the hand (keeps the arm at the same pos)
    HAND_CONTROL(Out_Pivots, hand_inst);
    UART_Send(
        (uint8_t)Out_Pivots[0],
        (uint8_t)Out_Pivots[1],
        (uint8_t)Out_Pivots[2],
        (uint8_t)Out_Pivots[3],
        (uint8_t)Out_Pivots[4]
    );
    HAL_Delay(500);  // wait for hand to grab/release
    
    // If AUTO, raise back up after grabbing
    if (was_auto) {
        z = 15.0f;
        MOVE_ARM(Out_Pivots, 800);  // ignore error, just skip if unreachable
    }
    
    // Update previous pivots at the end
    Old_x = x;
    Old_y = y;

    return 0;
}


// ***************************** FUNCTIONS ************************************* //
// ----- BASE ROTATION (PIV 0) ----- //
// Calculates the base rotation angle to point the arm toward the target
// Uses atan2(y,x) to get the angle in the horizontal plane
void BASE_ROTATION(int *Pivots){
    Pivots[0] = (int)roundf(atan2f(y, x) * 180.0f / PI) + 3;
}


// ----- ARM ROTATIONS (PIV 1 & 2) ----- //
// Calculates shoulder and elbow angles using inverse kinematics
// First gets horizontal distance, then adds vertical offset (height)
// 'reach' is the 3D straight-line distance from shoulder to target wrist position
// Uses law of cosines to solve the triangle formed by upper arm, forearm, and reach
// Returns -1 if target is unreachable (too far or too close)
int ARM_ROTATIONS(int *Pivots) {    
    // Horizontal distance from base to target
    distance = hypotf(x, y);
    
    height = z;
    // used later to have a 2 step movement
    if ((int)height == AUTO) height = 12.0f;
    
    // Vertical offset and also need to apply
    // compensation based on distance
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
    
    // KEEP: prevents impossible IK solutions that would break the arm
    if (reach > (L1 + L2) + 5.0f) return -1;  // too far
    if (reach < fabsf(L1 - L2) + 0.001f) return -1;  // too close
    
    // Calculate elbow angle using law of cosines
    // This is the internal angle between upper arm and forearm
    float cos_internal = (L1*L1 + L2*L2 - reach*reach) / (2.0f * L1 * L2);
    if (cos_internal > 1.0f) cos_internal = 1.0f;
    if (cos_internal < -1.0f) cos_internal = -1.0f;
    float internal_rad = acosf(cos_internal);
    float pivot2_deg = internal_rad * (180.0f / PI);
    
    // Calculate shoulder angle
    // alpha = angle to target from horizontal
    // beta = angle offset due to arm triangle geometry
    float alpha = atan2f(height, distance);
    
    float cos_beta = (L1*L1 + reach*reach - L2*L2) / (2.0f * L1 * reach);
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
    
    // Distance-based offset to keep gripper pointing down
    // Compensates for mechanical nonlinearity in the arm
    float offset;
    if (distance <= 15.0f) {
        offset = 17.0f;
    } else if (distance <= 25.0f) {
        // Steep slope: 17° at d=15 → 36° at d=25
        offset = 17.0f + (distance - 15.0f) * 1.9f;
    } else {
        // Flat slope: 36° at d=25 → 41° at d=40
        offset = 36.0f + (distance - 25.0f) * 0.33f;
    }
    
    // Wrist angle = base formula + offset
    int wrist = (int)roundf(265.0f - (float)shoulder - (float)elbow + offset);
    
    // Normalize to 0-360
    while (wrist < 0) wrist += 360;
    while (wrist >= 360) wrist -= 360;
    
    Pivots[3] = wrist;
}


// ----- HAND CONTROL (PIV 4) ----- //
// Controls gripper opening:
// 0 Deg = fully opened, 90 Deg = fully closed 
// Angle = 0.439024 × pwm  (pwm = 0 to 205)
// later will add a ststem to know if tis holdding something
void HAND_CONTROL(int *Out_Pivots, bool hand_state) {
    if (hand_state == OPEN) {
        Out_Pivots[4] = 0;
    } else {
        Out_Pivots[4] = 205;
    }
    prev_hand_pwm = Out_Pivots[4];  // save for next time
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
    
    long numer = (long)(deg - deg0) * 205LL;
    long absden = (denom >= 0) ? denom : - (long)denom;
    long adj = absden / 2;
    long pwm;
    
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
    long numer = (long long)(deg205 - deg0) * (long long)pwm;
    long adj = denom / 2;
    long frac = (numer >= 0) ? ( (numer + adj) / denom ) : ( - ( ( -numer + adj ) / denom ) );
    long out = (long long)deg0 + frac;
    return (int)out;
}

void PIV_TRANSLATE(int *Pivots, int *Out_Pivots){
    int deg = Pivots[1];
    int pwm;

    // pivot0: base rotation, deg0 = -169, deg205 = 165    
    Out_Pivots[0] = linear_deg_to_pwm(Pivots[0], -169, 165);
    
    // pivot1: shoulder, piecewise linear from calibration table
    if (deg >= 90) {
        pwm = 50 + (131 - deg) * 43 / 41;
    } 
    else {
        pwm = 93 + (90 - deg) * 49 / 90;
    }
    if (pwm < 0) pwm = 0;
    if (pwm > 205) pwm = 205;
    Out_Pivots[1] = pwm;

    // pivot2: elbow, deg0 = 34, deg205 = 375    
    Out_Pivots[2] = linear_deg_to_pwm(Pivots[2], 26, 375);
    
    // pivot3: wrist, 327° → PWM 0, 25° → PWM 205
    Out_Pivots[3] = linear_deg_to_pwm(Pivots[3], 327, 25);
    
    // pivot4: keep PREVIOUS hand state for now
    Out_Pivots[4] = prev_hand_pwm;
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

    // resteurns good if everythign is good
    return true;
}


// ----- ESTIMATE DELAY ----- //
//for each 1cm it should take abought 0.5 sec + 1 sec for safety
void ESTIMATE_DELAY(void) {
    estim_distance = (int)hypotf(Old_x-x, Old_y-y);

    // i genuenly dont know how i cam up with that but it works
    Estim_delay = (int)(estim_distance * 350);
    
    // caps it to not be too long
    if (Estim_delay > 4000) Estim_delay = 4000;
}