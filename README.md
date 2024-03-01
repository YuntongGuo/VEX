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
# this is code note for 57249C
### the driver control part is same all the time so only write about the autonomos code

# Springfield


## Vertical Back wings
we add vertical back wings, now we have both front horizontal and back vertical wings. The benefit of back wing is they don't bent horizaontal when it's pushing tribal.

Thus, I change the skill autonomous to use back wings instead of front wings.

The back wings can also use as descore, it's longer than hang, so it cam better descore.

## Programming Skills 
```c++
  chassis.set_coordinates(-1, 7, 90);
  chassis.turn_to_angle(45);
  chassis.drive_distance(-12);
  chassis.right_swing_to_angle(90);
  chassis.drive_distance(3);
  chassis.drive_distance(-20, chassis.desired_heading, 12, 10, 1, 100, 1000);
  chassis.drive_to_point(-9,1,10,8);
  chassis.turn_to_angle(-20);
  PneumaticSide.set(true);
  chassis.swing_timeout = 200;
  chassis.right_swing_to_angle(-19);
  chassis.swing_timeout = 2000;
  Puncher12.spin(fwd,12,volt);
  Puncher5.spin(fwd,6,volt);
  wait(30, sec);
  PneumaticSide.set(false);
  puncherDown();
  chassis.drive_max_voltage=12;
  chassis.turn_to_angle(-135);
  chassis.drive_distance(-8);
  chassis.turn_to_angle(180);
  chassis.drive_to_point(3, 74);
  chassis.turn_to_angle(-45+180);
  chassis.drive_distance(-10);
  chassis.left_swing_to_angle(-90+180);
  chassis.drive_distance(-20, chassis.desired_heading, 12, 10, 1, 100, 1000);
  chassis.drive_distance(5);
  chassis.drive_distance(-20, chassis.desired_heading, 12, 10, 1, 100, 1000);
  chassis.drive_distance(5);
  
  //1st push
  chassis.turn_to_point(-20, 60);
  chassis.drive_to_point(-20, 60);
  chassis.turn_to_angle(90);
  PneumaticSide.set(true);
  chassis.drive_timeout = 2000;
  chassis.drive_distance(-40, 180,12,9);
  chassis.drive_distance(12);
  PneumaticSide.set(false);

  // 2nd push
  chassis.turn_to_angle(-90);
  chassis.drive_distance(17);
  PneumaticSide.set(true);
  chassis.drive_distance(-40, 180,12,8.5);
  chassis.drive_max_voltage=12;
  chassis.swing_max_voltage = 10;
  chassis.drive_distance(12);
  PneumaticSide.set(false);
  
  //3rd push
  chassis.turn_to_angle(-65);
  chassis.drive_distance(20);
  PneumaticWing.set(true);
  chassis.left_swing_to_angle(45);
  chassis.drive_distance(8);
  Intake.spin(fwd,-6,volt);
  PneumaticWing.set(false);
  chassis.turn_to_angle(90);
  chassis.drive_distance(30, chassis.desired_heading, 12, 10, 1, 100, 2000);
  chassis.drive_distance(-20);
```

first, Bot push both tribal into the goal, then Bot touch the matchload bar by expand back wing. start punch for 30 seconds.

then move to the other side of field goal and push twice from right side use back.

3rd push move to corner of two bars and use back wings to drive cursive to the goal to push all tribals.

4th push move back and move to the other corner and cursive drive to use back wings to push the tribal. 

5th push move to left side and make last push.

## 6 tribal farside autonomous
```c++
  chassis.drive_max_voltage=12;
  Intake.spin(fwd,6,volt);
  wait(1, sec);
  chassis.set_coordinates(1, 0, 0);
  chassis.heading_max_voltage=5;
  chassis.drive_to_point(8,-33);
  chassis.drive_settle_time = 50;
  
  PneumaticSide.set(true);
  chassis.swing_max_voltage=10;
  chassis.left_swing_to_angle(-70);
  PneumaticSide.set(false);

  chassis.drive_distance(-20, -90, 12, 2, 1,100,800);
  //chassis.set_heading(-90);
  chassis.drive_distance(7);
  chassis.turn_to_angle(110);

  Intake.spin(fwd,-6,volt);
  wait(.3,sec);
  chassis.drive_distance(20, chassis.desired_heading, 12,12, 1,100,800);
  chassis.drive_distance(-10);
    
  chassis.turn_to_point(35, -4);
  Intake.spin(fwd,6,volt);
  chassis.drive_timeout = 1300;
  chassis.drive_to_point(35, -4);
  chassis.drive_timeout = 5000;
  chassis.drive_distance(-5);
  chassis.turn_to_point(40, -40);
  Intake.spin(fwd,-6,volt);
  chassis.drive_distance(10);
  chassis.drive_distance(-3);
  chassis.turn_to_angle(50);
  Intake.spin(fwd,6,volt);
  chassis.drive_timeout = 1000;
  chassis.drive_to_point(43, -4);
  chassis.drive_timeout = 5000;
  chassis.turn_to_angle(190);
  Intake.spin(fwd,-6,volt);
  PneumaticWing.set(true);
  chassis.drive_distance(50, 190, 12, 10, 1, 100, 1000);
  PneumaticWing.set(false);
  chassis.drive_distance(-20);
```
start from side, place alliance tribal on the back.

intake spin for 1 second to release and intake the middle tribal

move backward and use the backwings to descore and push the tribal and alliance tribal into goal.

turn around and extake the tribal, then push it into goal

go to middle, intake the left tribal, turn and drop it near the goal.

go to the middle, intake, turn and extake while expand front wings to push both tribal into goal. 

move out the goal 


## nearside autonomous
```c++
  Intake.spin(fwd,6,volt);
  wait(1, sec);
  chassis.set_coordinates(0, 0, -90);
  chassis.drive_max_voltage =12;
  chassis.drive_to_point(-39, 9);  
  chassis.turn_max_voltage = 12;
  chassis.turn_to_angle(0);
  chassis.drive_distance(20);
  PneumaticSide.set(false);
  chassis.turn_max_voltage = 12;
  chassis.turn_to_point(-30, 20);
  chassis.drive_to_point(-30, 25);
  PneumaticWing.set(true);
  Intake.spin(fwd,-100,pct);
  chassis.turn_to_point(-30, 40);
  chassis.drive_distance(11, chassis.desired_heading, 12, 6, 1, 100, 600);
  chassis.drive_distance(-11);
  PneumaticWing.set(false);
  chassis.turn_to_point(10,0);
  chassis.drive_distance(20);
  chassis.turn_to_angle(90);
  chassis.drive_distance(20);
  chassis.turn_to_point(8, 20);
  chassis.drive_to_point(8, 32.5,10,9);
```
we spin the intake for 1 second to release it, meanwhile expand the front wings to push alliance tribal near the side of goal.
then mid rush to intake 1 tribal from middle

go back to side of goal, extake the tribal and push the alliance tribal into goal.

spin around and use back wings to descore and push the tribal to the other side, meanwhile use backwings to touch the bar.



# Bristol

## nearside autonomous change
during the match, the mid rush has a risk of cross the line or contact opponent's bot make it fail to descore and touch the bar. 

so at the end of first day, I remove the mid rush and leave only the decore and touch the bar.
```c++
  Intake.spin(fwd,6,volt);
  wait(1, sec);
  chassis.set_coordinates(0, 0, -90);
  chassis.drive_max_voltage =12;
  chassis.drive_to_point(-39, 9);  
  chassis.drive_distance(-5);
  chassis.turn_to_point(0, 0);
  chassis.drive_to_point(-8,-16);
  chassis.turn_to_angle(30);
  Intake.spin(fwd,-6,volt);
  wait(1, sec);
  chassis.turn_to_angle(-120);
  chassis.drive_distance(-6);
  PneumaticBackWing.set(true);
  chassis.left_swing_to_angle(-150);
  chassis.drive_to_point(6,25);
  chassis.right_swing_to_angle(-160);
```
 

# BHS Lonely Hearts
### Nearside Autonomous
autonomus 
## need to fix:
1. in last competition, we depend on our teammate to score the  tribal to get winpoint. However, some team don't have the code to score, so we need to make a near side autonomous to descore, score and touch the bar. 
   
2. We also change to use the backwing to touch the bar because it will stop the bot from moving forward because pid will auto correct the angle of bot, if the the back wing touch the bar, it will push and change the angle of the bot. The bot will stop moving because the angle is wrong. 

3. during the skill, the puncher has a small chance of not going down which block the bot when it tries to get to the other side of field. This bug happen might because my code has a line to detect the angle of puncher to check if the puncher is not at holding angle, the puncher motor will spin and hold it when it's not at this angle. However, if the puncher is releasing when the angle detect line execute, the code will think the puncher reach the hold line and hold the puncher at wrong angle. 

### New Near Side
```c++
  Intake.spin(fwd,6,volt);
  PneumaticWing.set(true);
  wait(.7, sec);
  PneumaticWing.set(false);
  chassis.set_coordinates(0, -6.5, -90);
  chassis.drive_max_voltage = 12;
  chassis.drive_to_point(-38, 0);
  chassis.right_swing_to_angle(-90);
  wait(.5, sec);  
  //chassis.drive_distance(-5);
  //chassis.turn_to_point(-6,-10);
  chassis.drive_to_point(-9.5,-10);
  chassis.right_swing_to_angle(45);
  Intake.spin(fwd,-6,volt);
  wait(1, sec);
  //
  chassis.drive_distance(-9,90,12,3);
  // or
  //chassis.drive_distance(-5);
  //chassis.right_swing_to_angle(90);
  //
  chassis.drive_distance(-15, chassis.desired_heading, 12, 10, 1, 100, 800);
  //chassis.drive_distance(5);
  chassis.drive_to_point(-15,-15);
  chassis.turn_to_angle(-90);
    PneumaticSide.set(true);

  chassis.left_swing_to_angle(-135);
  //chassis.drive_distance(-1);

  chassis.left_swing_to_angle(180);
  //chassis.turn_to_point(10,25);
  chassis.drive_to_point(10,25);
  chassis.left_swing_to_angle(-170);
```
in this code, we mid rush first because some team will get autonomous bonus if they score more tribal than us so I need to take one tribal as fast as possible. 
Then, we go back and score the alliance tribal from the side of goal. meanwhile drop the tribal from mid rush.
after score, we use the back wing to descore and push all the tibal to the otherside.
Last, use the back wing to touch the bar.

```c++
chassis.drive_distance(-9,90,12,3);
```
or 
```c++ 
chassis.drive_distance(-5);
chassis.right_swing_to_angle(90);
```
they move the same position, however, the 1st segment is cursive driving, it can save .5 seconds.

## change to skills

to solve the puncher not going down, and prevent bot moving to other side, I add more lines to detect puncher state after each move to make sure it will go down. 
```c++
  chassis.set_coordinates(-1, 7, 90);
  chassis.turn_to_angle(45);
  chassis.drive_distance(-12);
  chassis.right_swing_to_angle(90);
  chassis.drive_distance(3);
  chassis.drive_distance(-20, chassis.desired_heading, 12, 10, 1, 100, 1000);
  chassis.drive_to_point(-9,1,10,8);
  chassis.turn_to_angle(-20);
  PneumaticSide.set(true);
  chassis.swing_timeout = 200;
  chassis.right_swing_to_angle(-19);
  chassis.swing_timeout = 2000;
  Puncher12.spin(fwd,12,volt);
  Puncher5.spin(fwd,6,volt);
  wait(30, sec);
  PneumaticSide.set(false);
  puncherDown();
  chassis.drive_max_voltage=12;
  chassis.turn_to_angle(-135);
  puncherDown();
  chassis.drive_distance(-8);
  puncherDown();
  chassis.turn_to_angle(180);
  puncherDown();
  chassis.drive_to_point(3, 74);
  ```

I also add more cursive drive to replace the step of turn move, and then turn
for example:
```c++
chassis.drive_distance(30,90,12,3,1,200,2000);
```
move 30 inch while turn to 90 degree.

to replace

```c++
chassis.turn_to_angle(90);
chassis.drive_distance(30, chassis.desired_heading, 12, 10, 1, 100, 2000);
```
















