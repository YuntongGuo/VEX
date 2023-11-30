# VEX Robotics 57249C
This is the code Write ups for 57249 C
The autonomous code using Jar Template

# User Control

```c++
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
```
In the user control, each loop is 20 milliseconds between to prevent overuse of resource.
The code called four functions for drive, intake, catapult, and pneumatic wings.
### Drivetrain Control
```c++
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
```
The percentExpenential function takes in a integer and return a value modified by a exponential function. 
```c++
void drivetrainControl(){
  double motorForwardSpeed = percentExponential(controller(primary).Axis3.position(pct),2);
  double motorTurnSpeed = percentExponential(controller(primary).Axis1.position(pct)*.85,2);
  // forward and back
  chassis.DriveL.spin(fwd, (motorForwardSpeed + motorTurnSpeed)*12/100,volt);
  chassis.DriveR.spin(fwd,(motorForwardSpeed - motorTurnSpeed)*12/100,volt);
}
```
I read the value on two axis, then decrease the value of turning by multiply .85
Then I put the result value in percent Exponential function to create exponential drive. It make the bot easier to control. 
Use the minus and plus to combine the turning and forward. 
Then convert this value into voltage and spin the motor by voltage.
### Intake
```c++
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
```
For intake control, if the L1 button is pressed the intake will spin positive at max speed to the intake triball, while the L2 button pressed, the intake will spin negative direction and remove the triball on the bot. 
### Catapult
```c++
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
```
I use the cataMove Global value to store the current state of the catapult.
In the code, I have an if else statement that calculate the action that catapult should do when the button pressed or an specific angle has reached based on current state of the catapult. 
If the catapult released, and button pressed, catapult start moving down by spinning motor
If the catapult complete the release(reach the released angle), change the state to released and stop the motor.
If in hold down mode, when button pressed, the motor will spin and release the catapult. 
Then change the state to releasing. 
If the catapult reach the angle that is holding down, change the state to hold down and hold the motor. 
In a separate if statement, If the state is moving catapult down, spin the motor at max speed. 

### Pneumatics Wing
```c++
bool PneumaticAState = false;
double PneumaticACooldown = Brain.timer(sec);

```
The first variable is to store the current state of side wings
2nd is to store the time of last change in wings to prevent waste of air. 

```c++
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
```
If the button is pressed and the time from last release/retract the pneumatics wings is greater than .7 second(Cooldown). 
Then judge whether current side wings is released or retracted. 
If is released, then call Pneumatics off function to retract the wings
If retracted then call Pneumatics on function to release the wings
Then change the state of the side.
# Autonomous
Using [JAR-Template](https://jacksonarearobotics.github.io/JAR-Template/)
Jackson area robotics
I configure the bot to the template 
In the autonomous part, I call the function for moving the bot 
forward for specific distance or Turn an accurate degree. 
```c++
  chassis.drive_distance(24);
  chassis.turn_to_angle(90);

```
## Far side (4 tribal)
```c++
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
```
set volt for drive and turn.
intake to hold the tribal
drive forward, 
drop the tribal and move backward,
turn to the bar move forward and grab the triball
open Pneumatics and push 2 tribal in to goal (include the one dropped)
move backward turn and drop the triball . 
extake and push it into goal.
intake stop.
```c++
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
```
go back and turn to the tribal on the side. 
move forward to grab it.
go back, turn to the goal and extake
push it in.
move back. turn to the elevation bar.
move at full speed to touch it.



## close side
```c++
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
```
SET voltage
star intake to grab tribal
move back and expand Pneumatic
wait expand
move forwarded and turning to unload
retract Pneumatic
```c++
  chassis.turn_to_angle(60);
  chassis.drive_distance(-17);
  chassis.right_swing_to_angle(93);
  Intake.stop();
  Intake.spin(fwd,-7,volt);
  chassis.drive_distance(-20,chassis.desired_heading,10,6,1,100,300);
  chassis.drive_distance(12);
```
turn toward goal.
move forward
turn toward the side of goal
extake and push it in
move back 

```c++
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
```
move to the elevation bar.
push the tribal under the bar.
move back and go to the other side of the bar to touch the elevation bar.
this is to avoid pass the elevation bar(lead to lose auton)

# Skill
## Driving Skill
use the same code as User Control

## Programming Skill
use a distance sensor to detect the tribal on the puncher. 
count the number of punches. After 44 punches, move to the other side and expand Pneumatics and push for three times.
from a different angle.
```c++
void auton_skill(){
  cataMax = CataRotation.angle(deg);
  chassis.drive_max_voltage = 12;
  chassis.right_swing_to_angle(-65);
  PneumaticA.set(true);
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
}
```

















