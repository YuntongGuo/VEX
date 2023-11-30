#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
motor RightBack = motor(PORT1, ratio6_1, false);
motor RightMiddle = motor(PORT9, ratio6_1, false);
motor RightFront = motor(PORT17, ratio6_1, true);
motor LeftBack = motor(PORT5, ratio6_1, true);
motor LeftMiddle = motor(PORT8, ratio6_1, true);
motor LeftFront = motor(PORT14, ratio6_1, false);
motor Intake = motor(PORT11, ratio6_1, true);
motor Catapult = motor(PORT20, ratio36_1, false);
rotation CataRotation = rotation(PORT7, false);
digital_out PneumaticA = digital_out(Brain.ThreeWirePort.A);
digital_out PneumaticH = digital_out(Brain.ThreeWirePort.H);
digital_out PneumaticB = digital_out(Brain.ThreeWirePort.B);
distance Distance = distance(PORT16);

// VEXcode generated functions



/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}