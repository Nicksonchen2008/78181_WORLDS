#include "vex.h"
#include "usercontrol.hpp"
#include "vexfield.h"
#include "logo.h"
#include "rlogo.h"
#include "autofunctions.hpp"
#include <cmath>
#include <iomanip>
using namespace vex;
competition Competition;

Drive chassis(

//ZERO_TRACKER_NO_ODOM, ZERO_TRACKER_ODOM, TANK_ONE_ENCODER, TANK_ONE_ROTATION, TANK_TWO_ENCODER, TANK_TWO_ROTATION, HOLONOMIC_TWO_ENCODER, and HOLONOMIC_TWO_ROTATION
ZERO_TRACKER_NO_ODOM,

//Left Motors:
motor_group(LF,LM,LB),

//Right Motors:
motor_group(RF,RM,RB),

//Inertial Port
PORT7,

//Wheel Diameter
3.25,

//Gear ratio
0.75,

//Gyro scale
352,

/*---------------------------------------------------------------------------*/
/*                                  PAUSE!                                   */
/*---------------------------------------------------------------------------*/

//HOLO DT

//LF:      //RF:    
PORT1,     -PORT2,

//LB:      //RB: 
PORT3,     -PORT4,

//If you are using position tracking, this is the Forward Tracker port (the tracker which runs parallel to the direction of the chassis).
//Rotation sensor: "PORT1"
//Encoder: Triport A will be "1", 
3,

//Forward Tracker diameter
2.75,

//Forward Tracker center distance
-2,

//Sideways Tracker Port
1,

//Sideways tracker diameter
-2.75,

//Sideways tracker center distance
5.5

);

void debug_menu(int selection){
  bool debug_is_pressed = false;
  while(1){
    if(debug_is_pressed == false){
      Brain.Screen.drawImageFromBuffer(vexfield, 50, 0, sizeof(vexfield));
      Brain.Screen.drawImageFromBuffer(logo, 10, 0, sizeof(logo));
      Brain.Screen.setPenWidth(4); 
      Brain.Screen.setFont(monoL); 
      Brain.Screen.drawLine(310,0,310,240); 
      Brain.Screen.drawLine(310,70,480,70); 
      Brain.Screen.setFillColor(black); 
      if(selection == 0) Brain.Screen.printAt(350, 43, "AWP");
      else if(selection == 1) Brain.Screen.printAt(350, 43, "Safe Six");
      else if(selection == 2) Brain.Screen.printAt(350, 43, "Steal AWP");
      else if(selection == 3) Brain.Screen.printAt(350, 43, "Sixball");
      else if(selection == 4) Brain.Screen.printAt(350, 43, "Safe Five");
      else if(selection == 5) Brain.Screen.printAt(350, 43, "Skills");
      Brain.Screen.setFont(mono40);
      Brain.Screen.setFillColor(blue);
      Brain.Screen.drawRectangle(310,70,170,210);
      Brain.Screen.printAt(345,160,"Debug");
    }
    if(Brain.Screen.pressing()){
      Brain.Screen.clearScreen();
      waitUntil(Brain.Screen.pressing() == false);
        debug_is_pressed =!debug_is_pressed;
    }
    if(debug_is_pressed == true){
      Brain.Screen.setFillColor(clearerr); 
      Brain.Screen.setFont(mono30);
      Brain.Screen.setPenColor(blue);
      Brain.Screen.printAt(155, 40, "Debug Menu:");
      Brain.Screen.setFont(mono20);
      Brain.Screen.setPenColor(white);
      Brain.Screen.printAt(20, 90, "Heading:%f",std::floor(Inertial100.heading()));
      Brain.Screen.printAt(20, 140, "Battery:%f",std::floor(Brain.Battery.capacity()));
      Brain.Screen.printAt(20, 190, "DT Avg Temp:%f",std::floor((LF.temperature(percent)+LM.temperature(percent)+LB.temperature(percent)+RF.temperature(percent)+RM.temperature(percent)+RB.temperature(percent))/6));
      Brain.Screen.printAt(270, 90, "Hang Temp:%f",std::floor(cataMotor.temperature(percent)));
      Brain.Screen.printAt(270, 140, "Intake Temp:%f",std::floor(intakeMotor.temperature(percent)));
      Brain.Screen.printAt(270, 190, "Kicker Temp:%f",std::floor(kicker.temperature(percent)));
    }
    wait(20,msec);
  }
}

int current_auton_selection = 0;
bool auto_started = false;
int autonState = 0; 

void pre_auton(void) {
  vexcodeInit();
  default_constants();
  printf("Battery: %f \n",std::round(Brain.Battery.capacity()));
  intakeMotor.setStopping(hold);
  back_wings.set(false);
  back_wings2.set(false);
  hangrot.resetPosition(); 
  hangrot.setPosition(0, rotationUnits::rev); 
  Brain.Screen.clearScreen();
  while(1) {
    switch(autonState) {
      case 0:
        debug_menu(0);
        break; 
      case 1:
        debug_menu(1);
        break; 
      case 2:
        debug_menu(2);
        break; 
      case 3:
        debug_menu(3);
        break; 
      case 4:
        debug_menu(4);
        break; 
      case 5:
        debug_menu(5);
        break;
      default:
        debug_menu(0);
        break;
    }
    if(limitselect.pressing()){
      while(limitselect.pressing()) {}
      Brain.Screen.clearScreen();
      autonState++; 
    }
    else if(autonState>4) autonState = 0; 
  }
}

void autonomous(void) {
  switch(autonState){  
    case 0:
      // testing();
      // ramAWP2();
      noramAWP2();
      // sixball();
      // Safesix();
      // RushNoRamAWP();
      // Worlds_Skills();
      break;
    case 1:
      Safesix();
      break;
    case 2:
      RushNoRamAWP();
      break;  
    case 3:
      sixball();
      break;
    case 4:
      fiveballtouch();
      break;
    case 5:
      Worlds_Skills();
      break;
    default:
      noramAWP2();
      break;
  }
}

void usercontrol(void) {
  task Suck1task(UC_Intake);
  task Launchtask(UC_Slapper);
  task Releasetask(UC_frontwings);
  task Ratchettask(pistonratchett);
  task verhormac(horvert);

  while(1){
    chassis.control_arcade();
    if(Controller1.ButtonY.pressing() && autonState == 5){
      chassis.drive_max_voltage = 11.2;
      chassis.set_drive_exit_conditions(0.5, 20, 1000);
      chassis.set_turn_exit_conditions(0.4, 20, 900);
      back_wings.set(true);
      wait(120,msec);
      back_wings.set(false);
      chassis.diff(-45, -80, 1700, 300);
      chassis.set_heading(180);
      chassis.drive_distance(10);
      chassis.turn_to_angle(69.5);
      chassis.drive_distance(-4);
      back_wings2.set(true);
    }
    wait(20, msec);
  }
}

int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();
  while (true) {
    wait(100, msec);
  }
} 