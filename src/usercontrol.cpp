#include "vex.h"
#include <cmath>

int UC_Intake(){
  while(1){
    if(Controller1.ButtonL1.pressing() && Controller1.ButtonR1.pressing()){
      back_wings.set(true);
      back_wings2.set(true);
      intakeMotor.spin(fwd,0,pct);
    }
    else if(Controller1.ButtonR1.pressing()) intakeMotor.spin(forward,(-100)*120,voltageUnits::mV);
    else if(Controller1.ButtonL1.pressing()) intakeMotor.spin(forward,(100)*120,voltageUnits::mV);
    // else if(Controller1.ButtonLeft.pressing()){
    //   intakeMotor.spin(forward,-(100)*120,voltageUnits::mV);
    //   wait(260,msec);
    //   intakeMotor.spin(forward,(100)*120,voltageUnits::mV);
    //   wait(300,msec);
    //   intakeMotor.stop(hold);
    // }
    else{
      intakeMotor.stop(hold);
      back_wings.set(false);
      back_wings2.set(false);
    }
    wait(20,msec);
  }
}

int UC_Slapper(){
  bool kick_io = false;
  while(1){
    if (Controller1.ButtonRight.pressing()){
      waitUntil(Controller1.ButtonRight.pressing() == false);
      kick_io=!kick_io;
    }
    if(kick_io == true){
      kicker.spin(forward,67.5,percent);
      back_wings2.set(true);
    }
    else{
      kicker.stop(coast);
      // back_wings2.set(false);
    }
    wait(10,msec);
  }
}

int UC_frontwings(){
  while(1){
    if(Controller1.ButtonL2.pressing() && Controller1.ButtonR2.pressing()){
      front_wings.set(true);
      front_wings2.set(true);
    }
    if(Controller1.ButtonL2.pressing()) front_wings.set(true);
    else if(Controller1.ButtonR2.pressing()) front_wings2.set(true);
    else{
      front_wings.set(false);
      front_wings2.set(false);
    }
  }
}

int horvert(){
  double hangpos;
  bool ver_is_pressed = false;
  bool hor_is_pressed = false;
  // double startTimeA;
  // float break_condition1 = 10000;
  // float break_condition2 = 15000;
  Brain.Timer.clear();
  // startTimeA = Brain.timer(sec);
  hangrot.resetPosition();
  while(1){
    hangpos = std::abs(hangrot.position(rotationUnits::rev));
    // startTimeA = Brain.timer(msec);
    if(Controller1.ButtonA.pressing() && hor_is_pressed == false){ //Click once, set ratchet to false, lift up
      while(hangpos <= 1.15){
        hangpos = std::abs(hangrot.position(rotationUnits::rev));
        cataMotor.spin(forward,-100,pct);
        wait(20,msec);
        // if(startTimeA >= break_condition1) break;
      }
      cataMotor.stop(hold);
      hor_is_pressed = true;
    }
    else if(Controller1.ButtonA.pressing() && hor_is_pressed == true){ //Click another time, set ratchet to true, lift down
      while(hangpos >= 0.01){
        hangpos = std::abs(hangrot.position(rotationUnits::rev));
        cataMotor.spin(forward,100,pct);
        wait(20,msec);
        // if(startTimeA >= break_condition2) break;
      }
      cataMotor.stop(hold);
    }

    if(Controller1.ButtonB.pressing() && ver_is_pressed == false){ //Click once, set ratchet to false, lift up
      while(hangpos <= 3.45){
        cataMotor.spin(forward,-100,pct);
        hangpos = std::abs(hangrot.position(rotationUnits::rev));
        wait(20,msec);
        // if(startTimeA >= break_condition1) break;
      }
      cataMotor.stop(hold);
      ver_is_pressed = true;
    }
    else if(Controller1.ButtonB.pressing() && ver_is_pressed == true){ //Click another time, set ratchet to true, lift down
      while(hangpos >= 0.125){
        cataMotor.spin(forward,100,pct);
        hangpos = std::abs(hangrot.position(rotationUnits::rev));
        wait(20,msec);
        // if(startTimeA >= break_condition2) break;
      }
      cataMotor.stop(hold);
    }

    if(Controller1.ButtonDown.pressing()){ //Manual Lift function
      cataMotor.spin(forward,100,percent);
    }
    else if (Controller1.ButtonUp.pressing()){
      cataMotor.spin(forward,-100,percent);
    }
    else cataMotor.stop(hold);
  }
  wait(20,msec);
}

int pistonratchett(){
  bool ratchet = false;
  while(1){
    if(Controller1.ButtonX.pressing()){ // Piston Ratchet function
      waitUntil(Controller1.ButtonX.pressing() == false);
      ratchet =!ratchet;
      if(ratchet == true){
        Controller1.Screen.clearScreen();
        Controller1.Screen.setCursor(1,1);
        Controller1.Screen.print("Ratchet Engaged");
      }
      else if(ratchet == false){
        Controller1.Screen.clearScreen();
        Controller1.Screen.setCursor(1,1);
        Controller1.Screen.print("Ratchet NOT Engaged");
      }
    }
    pistonratchet.set(ratchet);
    wait(20,msec);
  }
}