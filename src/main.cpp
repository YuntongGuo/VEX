#include "vex.h"

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// RightBack            motor         1               
// RightMiddle          motor         9               
// RightFront           motor         17              
// LeftBack             motor         5               
// LeftMiddle           motor         8               
// LeftFront            motor         14              
// Intake               motor         11              
// Catapult             motor         20              
// CataRotation         rotation      7               
// PneumaticA           digital_out   A               
// PneumaticH           digital_out   H               
// PneumaticB           digital_out   B               
// Distance             distance      16              
// ---- END VEXCODE CONFIGURED DEVICES ----

using namespace vex;
competition Competition;

/*---------------------------------------------------------------------------*/
/*                             JAR-Template Config                           */
/*                                                                           */
/*  Where all the magic happens. Follow the instructions below to input      */
/*  all the physical constants and values for your robot. You should         */
/*  already have configured your robot manually with the sidebar configurer. */
/*---------------------------------------------------------------------------*/

Drive chassis(

// drive setup 
//ZERO_TRACKER_ODOM
ZERO_TRACKER_ODOM,

//the names of Drive motors into the motor groups 

//Left Motors
motor_group(LeftFront,LeftMiddle,LeftBack),

//Right Motors
motor_group(RightFront,RightMiddle,RightBack),

//PORT NUMBER of inertial sensor
PORT15,

//wheel diameter
3.25,

//External ratio of motor to wheels
0.6,

//Gyro scale when spin the robot 360 degrees.
359,

/*---------------------------------------------------------------------------*/
/*                                  PAUSE!                                   */
/*                                                                           */
/*  The rest of the drive constructor is for robots using POSITION TRACKING. */
/*  If you are not using position tracking, leave the rest of the values as  */
/*  they are.                                                                */
/*---------------------------------------------------------------------------*/

//If you are using ZERO_TRACKER_ODOM, you ONLY need to adjust the FORWARD TRACKER CENTER DISTANCE.

//FOR HOLONOMIC DRIVES ONLY: Input your drive motors by position. This is only necessary for holonomic drives, otherwise this section can be left alone.
//LF:      //RF:    
PORT1,     -PORT2,

//LB:      //RB: 
PORT3,     -PORT4,

//If you are using position tracking, this is the Forward Tracker port (the tracker which runs parallel to the direction of the chassis).
//If this is a rotation sensor, enter it in "PORT1" format, inputting the port below.
//If this is an encoder, enter the port as an integer. Triport A will be a "1", Triport B will be a "2", etc.
3,

//Input the Forward Tracker diameter (reverse it to make the direction switch):
2.75,

//Input Forward Tracker center distance (a positive distance corresponds to a tracker on the right side of the robot, negative is left.)
//For a zero tracker tank drive with odom, put the positive distance from the center of the robot to the right side of the drive.
//This distance is in inches:
5,

//Input the Sideways Tracker Port, following the same steps as the Forward Tracker Port:
1,

//Sideways tracker diameter (reverse to make the direction switch):
-2.75,

//Sideways tracker center distance (positive distance is behind the center of the robot, negative is in front):
5.5

);
int cataMove =3;
double cataMax = CataRotation.angle(deg);
double competitionBegin;

bool PneumaticAState = false;
bool PneumaticHState = false;
bool PneumaticGState = false;
bool PneumaticBState = false;

double PneumaticACooldown = Brain.timer(sec);
double PneumaticHCooldown = Brain.timer(sec);
double PneumaticGCooldown = Brain.timer(sec);
double PneumaticBCooldown = Brain.timer(sec);

bool CatapultOn = false;
double CatapultCooldown = Brain.timer(sec);

int auton_tribal = 0;

int percentExponential(float value, int ExpType =1){
  if(ExpType == 1){
    return pow(value,3)/pow(100,2);
  }else if (ExpType == 2) {
    if(value>1){
      return .2*value+1.2*pow(1.043, value)-1.2;
    }else if (value<-1){
      return -(.2*fabs(value)+1.2*pow(1.043, fabs(value))-1.2);
    }
  }else if (ExpType == 3) {
      if(value>5){
      return 101.8/(1+pow(2.72,-((value-60)/10)));
    }else if (value<-5){
      return -101.8/(1+pow(2.72,-((fabs(value)-60)/10)));
    }
  
  }
  return 0;
}

void CataLoad(void){
  //printf("%f",CataRotation.angle(deg));
  while(CataRotation.angle(deg) > 10  && CataRotation.angle(deg)<80){
    Catapult.spin(fwd,-100,vex::velocityUnits::pct);
      //printf("%f\n",CataRotation.angle(deg));

  }
Catapult.stop(hold);
}

void CataRelease(){
  Catapult.spinFor(fwd, -100, deg);
}

void drivetrainControl(){
  double motorForwardSpeed = percentExponential(controller(primary).Axis3.position(pct),2);
  double motorTurnSpeed = percentExponential(controller(primary).Axis1.position(pct)*.85,2);
  // forward and back
  chassis.DriveL.spin(fwd, (motorForwardSpeed + motorTurnSpeed)*12/100,volt);
  chassis.DriveR.spin(fwd,(motorForwardSpeed - motorTurnSpeed)*12/100,volt);
}
void IntakeControl(){
   // intake
    int IntakeSpeed = 0;
    if(controller(primary).ButtonL1.pressing()){
      IntakeSpeed=100;
    }
    else if(controller(primary).ButtonL2.pressing()){
      IntakeSpeed=-100;
    }
    Intake.spin(fwd,IntakeSpeed,vex::velocityUnits::pct);
}
void CatapultControl(){
  // cata
    // catamove = 0 : hold down
    // catamove = 1 : move down
    // catamove = 2 : releasing
    // catamove = 3 : released  
    //printf("cata %d, %f, max is %f\n",cataMove,CataRotation.angle(deg),cataMax);
         // printf("loop-------------------- %f\n",CataRotation.angle(deg));

    if(CataRotation.angle(deg)>=cataMax-3 && CataRotation.angle(deg)<=cataMax+3&&cataMove!=1){
      //printf("current angle: %f\n",CataRotation.angle(deg));
      // end release
      cataMove = 1;      
      
      printf(" %d\n",auton_tribal); 
      //printf("target %f\n",cataMax); 
      //printf("current----------- %f\n",CataRotation.angle(deg));
      printf("end release\n");
       
    }else if(Distance.objectDistance(mm)<=5 && cataMove == 0){
      // start release
      cataMove = 2; 
      Catapult.spin(fwd, -100, pct);
      chassis.DriveL.stop(hold);
      chassis.DriveR.stop(hold);
      auton_tribal = auton_tribal + 1;
      printf("start release\n");
    }else if((CataRotation.angle(deg)<=5||CataRotation.angle(deg)>=355) && (cataMove==1)){
      if(Distance.objectDistance(mm)<=5){
        printf("block diurect fire\n");
        cataMove = 2; 
        Catapult.spin(fwd, -100, pct);
        chassis.DriveL.stop(hold);
        chassis.DriveR.stop(hold);
        auton_tribal = auton_tribal + 1;
        return;

      }
      //rest
      cataMove = 0;
      Catapult.stop(hold); 
      printf("rest\n");
      //Catapult.spinFor(fwd,3,deg);

    }
    
    if(cataMove==1){
      Catapult.spin(fwd,-12,volt);
    }
}
void CatapultStart(){
  if(controller(primary).ButtonR1.pressing() && Brain.timer(sec) - CatapultCooldown>=.5){
    CatapultOn = !CatapultOn;
    CatapultCooldown=Brain.timer(sec);
  }
  if(CatapultOn){
    CatapultControl();
  }else {
    Catapult.stop(coast);
  }
}

void PnumaticsControl(){
  // Pnumatics
    if(controller(primary).ButtonR2.pressing()&&Brain.timer(sec)-PneumaticACooldown>=.3&&Brain.timer(sec)-PneumaticHState>=.3){
      PneumaticACooldown= Brain.timer(sec);
      PneumaticHCooldown= Brain.timer(sec);
      if(PneumaticAState || PneumaticHState){
        PneumaticAState = !PneumaticAState;
        PneumaticHState = !PneumaticHState;
        PneumaticA.set(false);
        PneumaticH.set(false);
      }else if(!(PneumaticAState&&PneumaticHState)){
        PneumaticAState = !PneumaticAState;
        PneumaticHState = !PneumaticHState;
        PneumaticA.set(true);
        PneumaticH.set(true);
      }
    }
}
void PnumaticAControl(){
  // Pnumatics
    if(controller(primary).ButtonY.pressing()&&Brain.timer(sec)-PneumaticACooldown>=.3){
      PneumaticACooldown= Brain.timer(sec);
      printf("use\n");
      if(PneumaticAState){
        PneumaticAState = !PneumaticAState;
        PneumaticA.set(false);
      }else if(!PneumaticAState){
        PneumaticAState = !PneumaticAState;
        PneumaticA.set(true);
      }
    }
}
void PnumaticBControl(){
  // Pnumatics
    if(controller(primary).ButtonA.pressing()&&Brain.timer(sec)-PneumaticBCooldown>=.3){
      PneumaticBCooldown = Brain.timer(sec);
      printf("use\n");
      if(PneumaticBState){
        PneumaticBState = !PneumaticBState;
        PneumaticB.set(false);
      }else if(!PneumaticBState){
        PneumaticBState = !PneumaticBState;
        PneumaticB.set(true);
      }
    }
}
void autoPnumaticB(){
  if(Brain.timer(sec)-competitionBegin>104 && PneumaticBState){
  
  while(1) {
    PneumaticB.set(true);
    this_thread::sleep_for(100);
  }
  }
}
void highHang(){
  if(controller(primary).ButtonX.pressing()){
    chassis.DriveL.spin(fwd,-12,volt);
    chassis.DriveR.spin(fwd,-12,volt);
    wait(.5,sec);
    if(!PneumaticBState){
      PneumaticBState = !PneumaticBState;
    }
    
    PneumaticB.set(true);
    chassis.DriveL.spin(fwd,12,volt);
    chassis.DriveR.spin(fwd,12,volt);
    wait(.9,sec);
    chassis.DriveL.stop(hold);
    chassis.DriveR.stop(hold);
  }
}

int current_auton_selection = 0;
bool auto_started = false;
void pre_auton(void) {
  // Initializing Robot Configuration. 
  vexcodeInit();
  default_constants();
  Intake.stop(hold);
}
void farside_auton(){
  chassis.drive_max_voltage = 12;
  chassis.turn_max_voltage = 12;
  Intake.spin(fwd,10,volt);
  chassis.drive_distance(-45);
  Intake.stop();
  Intake.spin(fwd,-7,volt);
  chassis.drive_max_voltage = 12;
  chassis.drive_distance(20);
  chassis.turn_to_angle(-45);
  Intake.spin(fwd,12,volt);
  chassis.drive_max_voltage = 12;
  chassis.drive_distance(-35);
  chassis.right_swing_to_angle(-95);
  // change to -90
  chassis.drive_distance(-4);
  PneumaticA.set(true);
  PneumaticH.set(true);
  wait(.2, sec);
  chassis.drive_distance(35,chassis.desired_heading,10,6,1,100,1500);
  PneumaticA.set(false);
  PneumaticH.set(false);
  chassis.drive_distance(-10);
  chassis.set_heading(0);
  chassis.turn_max_voltage=8;
  chassis.turn_to_angle(180);
  Intake.spin(fwd,-10,volt);
  chassis.drive_distance(-20,chassis.desired_heading,12,6,1,100,500);
  Intake.stop();
  chassis.turn_max_voltage = 12;
  chassis.drive_distance(20);
  chassis.turn_to_angle(-35);
  Intake.spin(fwd,12,volt);
  chassis.drive_distance(-20);
  chassis.drive_distance(25);
  chassis.turn_max_voltage = 8;
  chassis.turn_to_angle(180);
  Intake.spin(fwd,-12,volt);
  chassis.drive_distance(-20,chassis.desired_heading,12,6,1,100,600);
  chassis.turn_max_voltage=12;
  chassis.drive_distance(20);
  chassis.right_swing_to_angle(-40);
  chassis.drive_distance(-50);
}
void drop(){
  Intake.spin(fwd,10,volt);
  chassis.drive_distance(-10);
  Intake.stop();  
  Intake.spin(fwd,-8,volt);
  chassis.drive_max_voltage = 12;
  chassis.drive_distance(20);

}
void closeside_auton(){
  chassis.set_coordinates(0, 0,0);
  chassis.drive_max_voltage = 12;
  chassis.turn_max_voltage = 12;
  Intake.spin(fwd,10,volt);
  chassis.drive_distance(-7);
  PneumaticH.set(true);
  wait(.3, sec);
  chassis.set_heading(45);
  //chassis.turn_to_angle(0,8);
  chassis.drive_distance(10,0,3,3);
  PneumaticH.set(false);
  wait(.2,sec);
  chassis.turn_to_angle(60);
  chassis.drive_distance(-17);
  chassis.right_swing_to_angle(93);
  Intake.stop();
  Intake.spin(fwd,-7,volt);
  chassis.drive_distance(-20,chassis.desired_heading,10,6,1,100,300);
  chassis.drive_distance(12);
  chassis.set_heading(0);
  chassis.turn_to_angle(125);
  chassis.drive_distance(-30);
  chassis.turn_to_angle(-90);
  chassis.drive_distance(28);
  chassis.drive_distance(-28);
  chassis.turn_to_angle(0);
  chassis.drive_distance(-25);
  chassis.turn_to_angle(105);
  chassis.drive_distance(-30,chassis.desired_heading,10,6,1,100,1000);
}
void auton_skill(){
          printf("start run -----------------------------\n");

  cataMax = CataRotation.angle(deg);
        printf("%f\n",cataMax);

  chassis.drive_max_voltage = 12;
  chassis.right_swing_to_angle(-65);
  PneumaticA.set(true);
  // while(auton_tribal<4){
    
  //   //printf("current %f\n",CataRotation.angle(deg));

  //   CatapultControl();
  //   wait(20, msec);
  // }
  // wait(2, sec);
      Catapult.spin(fwd, -100, pct);
  wait(40, sec);
  PneumaticA.set(false);
  Catapult.stop(coast);
  printf("end\n");
  chassis.drive_distance(-20);
  chassis.turn_to_angle(-135);
  chassis.drive_distance(13);
  chassis.turn_to_angle(133);
  chassis.drive_distance(80);
  chassis.set_heading(0);
  chassis.right_swing_to_angle(-45);
    PneumaticA.set(true);
  PneumaticH.set(true);

  chassis.drive_distance(15);
  chassis.right_swing_to_angle(-80);
  chassis.drive_distance(30,chassis.desired_heading,12,6,1,100,1000);
  chassis.drive_distance(-20);
    chassis.drive_distance(30,chassis.desired_heading,12,6,1,100,1000);
    chassis.drive_distance(-18);
    PneumaticA.set(false);
    PneumaticH.set(false);
    chassis.right_swing_to_angle(180);
    chassis.drive_max_voltage = 6;
    chassis.turn_max_voltage = 5;
    chassis.drive_distance(30);
    chassis.left_swing_to_angle(-45);
    PneumaticA.set(true);
  PneumaticH.set(true);
    chassis.drive_distance(10);
    chassis.drive_distance(20,chassis.desired_heading,12,6,1,100,1000);
        chassis.drive_max_voltage = 12;
    chassis.drive_distance(-20);
    chassis.turn_to_angle(-90);
            chassis.drive_max_voltage = 6;

    chassis.drive_distance(15);
    chassis.left_swing_to_angle(0);
                chassis.drive_max_voltage = 12;

        chassis.drive_distance(10);

      chassis.drive_distance(20,chassis.desired_heading,12,6,1,100,1000);

  return;
  chassis.turn_to_angle(-90);
  PneumaticA.set(true);
  PneumaticH.set(true);
  chassis.drive_distance(20);
  //start pushing
  chassis.swing_max_voltage = 3;
  chassis.turn_max_voltage = 3;

  chassis.drive_max_voltage = 5;
  chassis.turn_to_angle(-135);
  chassis.drive_distance(15);
  chassis.right_swing_to_angle(180);
  chassis.left_swing_to_angle(-90);
  chassis.drive_distance(15);
  chassis.left_swing_to_angle(0);
  // boom
  chassis.drive_max_voltage = 12;
  chassis.drive_distance(20);
  chassis.DriveL.spin(fwd,12,volt);
  chassis.DriveR.spin(fwd,12,volt);
  wait(1, sec);
  chassis.DriveL.stop();
  chassis.DriveR.stop();
}
void autonomous(void) {
  auto_started = true;
  farside_auton();
  //closeside_auton();
  //auton_skill();
  //drop();
}


void usercontrol(void) {
  cataMax = CataRotation.angle(deg);
  competitionBegin = Brain.timer(sec);
  //printf("catamax: %d",cataMax);
  // User control code here, inside the loop
  while (1) {
    drivetrainControl();    
    IntakeControl();
    CatapultStart();
    PnumaticsControl();    
    PnumaticAControl();
    PnumaticBControl();
    autoPnumaticB();
    
    highHang();
    // Sleep the task for a short amount of time to
    // prevent wasted resources.
    wait(20, msec);
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
