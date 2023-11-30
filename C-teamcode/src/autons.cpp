#include "vex.h"

void default_constants(){
  chassis.set_drive_constants(10, 1.5, 0, 10, 0);
  chassis.set_heading_constants(6, .4, 0, 1, 0);
  chassis.set_turn_constants(12, .4, .03, 3, 15);
  chassis.set_swing_constants(12, .3, .01, 2, 15);
  chassis.set_drive_exit_conditions(1.5, 50, 5000);
  chassis.set_turn_exit_conditions(1, 50, 3000);
  chassis.set_swing_exit_conditions(3, 50, 1000);
}

void odom_constants(){
  default_constants();
  chassis.drive_max_voltage = 8;
  chassis.drive_settle_error = 3;
}

void drive_test(){
  
  

  chassis.drive_distance(24);
  chassis.turn_to_angle(90);
  chassis.drive_distance(24);
  chassis.turn_to_angle(180);
  //
  chassis.drive_distance(24);
  chassis.turn_to_angle(270);
  chassis.drive_distance(24);
  chassis.turn_to_angle(0);
  //
  chassis.turn_to_angle(45);
  chassis.drive_distance(34);
  chassis.turn_to_angle(225);
  chassis.drive_distance(34);
  chassis.turn_to_angle(0);


}

void auton_drive(){
  chassis.drive_distance(24);
  chassis.turn_to_angle(90);
  chassis.drive_distance(24);
  chassis.turn_to_angle(180);
  //
  chassis.drive_distance(24);
  chassis.turn_to_angle(270);
  chassis.drive_distance(24);
  chassis.turn_to_angle(0);
  //
  chassis.turn_to_angle(45);
  chassis.drive_distance(34);
  chassis.turn_to_angle(225);
  chassis.drive_distance(34);
  chassis.turn_to_angle(0);
}


void full_test(){
  chassis.drive_distance(24);
  chassis.turn_to_angle(-45);
  chassis.drive_distance(-36);
  chassis.right_swing_to_angle(-90);
  chassis.drive_distance(24);
  chassis.turn_to_angle(0);
}

void odom_test(){
  chassis.set_coordinates(0, 0, 0);
  while(1){
    Brain.Screen.clearScreen();
    Brain.Screen.printAt(0,50, "X: %f", chassis.get_X_position());
    Brain.Screen.printAt(0,70, "Y: %f", chassis.get_Y_position());
    Brain.Screen.printAt(0,90, "Heading: %f", chassis.get_absolute_heading());
    Brain.Screen.printAt(0,110, "ForwardTracker: %f", chassis.get_ForwardTracker_position());
    Brain.Screen.printAt(0,130, "SidewaysTracker: %f", chassis.get_SidewaysTracker_position());
    task::sleep(20);
  }
}