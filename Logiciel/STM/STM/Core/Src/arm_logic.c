// ************** INCLUDES ************** // 
#include "main.h"
#include "arm_logic.h"
#include <stdbool.h>

// ************** VARIABLES ************* //



// ************** FUNCTIONS ************* //
//************************* HAND ANGLE (keeps it down)*****************************
// controlls the joint of the hand, primarly keeps it faced down releative
// to pivot point 1 and 2

// imagin angle of pivot 1 (in that case its 45 degrees down from horzontal)
// and angle of pivot 2 (in that case its 135 degrees relative to pivot 1)
// - to get the hand to be down (0 degrees relative to horizontal)
//   you need to do 45 + (135 - 90) = 90 degrees of rotation from the base
//   so the hand angle is function of the two pivot angles
HAND_ANGLE:
    // get pivot 1 angle
    // get pivot 2 angle
    // do math to get hand angle
return; 

//************************* HAND CONTROL *****************************************
// controls the opening and closing of the hand
// also sets the hand angle to keep hand facing down
HAND_CONTROL:
    // open or close hand
    // call HAND_ANGLE to keep hand down
return;

//************************* BASE ROTATION ****************************************
// controls the base rotation of the arm
BASE_ROTATION:
    // rotate base
return;

//************************* FIRST PIVOT POINT (1) *************************************
// controls the first pivot point of the arm
FIRST_PIVOT_POINT:
    // move first pivot point
return;

//************************* SECOND PIVOT POINT (2) ************************************
// controls the second pivot point of the arm
SECOND_PIVOT_POINT:
    // move second pivot point
return;

//************************* PIVOTS CONTROLL *****************************************
// the logic of the 2 pivot points and hand control is in this function
// this function takes in x,y,z coordinates
// to get the hand to a certain position in space

// the base rotation:
// the robot it at 0,0 (if we look from top down)
// to get the correct base rotation angle we do
// atan2(y,x) = base rotation angle

// the 2 main pivot points:
// we do some trigonometry to get the angles of the 2 pivot points
// we start by getting the distance from the base to the point in space
// d = sqrt(x^2 + y^2)

// now with d, we can get the angles using the law of cosines
// we can change the arm distance if for example the pivot 1 is at 90 degrees
// and the pivot 2 is at 0 degrees, the arm is as close as possible to the base
// if pivot 1 is at 0 degrees and pivot 2 is at 180 degrees, the arm is fully extended
// so if we have the distance d,
// we can compute r = sqrt(d^2 + z^2)
// check if r is reachable
// use law of cosines: elbow_angle = acos((L1^2 + L2^2 - r^2)/(2*L1*L2))
// compute shoulder_angle = atan2(z, d) - atan2(L2*sin(elbow_angle), L1 + L2*cos(elbow_angle))
PIVOTS_CONTROL:
    // get base rotation angle
    // get distance d
    // compute pivot angles using law of cosines
return;

//************************* SETUP MAIN PROGRAM *******************************