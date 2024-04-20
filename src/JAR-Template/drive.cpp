#include "vex.h"
#include <cmath>
Drive::Drive(enum::drive_setup drive_setup, motor_group DriveL, motor_group DriveR, int gyro_port, float wheel_diameter, float wheel_ratio, float gyro_scale, int DriveLF_port, int DriveRF_port, int DriveLB_port, int DriveRB_port, int ForwardTracker_port, float ForwardTracker_diameter, float ForwardTracker_center_distance, int SidewaysTracker_port, float SidewaysTracker_diameter, float SidewaysTracker_center_distance) :
  wheel_diameter(wheel_diameter),
  wheel_ratio(wheel_ratio),
  gyro_scale(gyro_scale),
  drive_in_to_deg_ratio(wheel_ratio/360.0*M_PI*wheel_diameter),
  ForwardTracker_center_distance(ForwardTracker_center_distance),
  ForwardTracker_diameter(ForwardTracker_diameter),
  ForwardTracker_in_to_deg_ratio(M_PI*ForwardTracker_diameter/360.0),
  SidewaysTracker_center_distance(SidewaysTracker_center_distance),
  SidewaysTracker_diameter(SidewaysTracker_diameter),
  SidewaysTracker_in_to_deg_ratio(M_PI*SidewaysTracker_diameter/360.0),
  drive_setup(drive_setup),
  DriveL(DriveL),
  DriveR(DriveR),
  Gyro(inertial(gyro_port)),
  DriveLF(DriveLF_port, is_reversed(DriveLF_port)),
  DriveRF(DriveRF_port, is_reversed(DriveRF_port)),
  DriveLB(DriveLB_port, is_reversed(DriveLB_port)),
  DriveRB(DriveRB_port, is_reversed(DriveRB_port)),
  R_ForwardTracker(ForwardTracker_port),
  R_SidewaysTracker(SidewaysTracker_port),
  E_ForwardTracker(ThreeWire.Port[to_port(ForwardTracker_port)]),
  E_SidewaysTracker(ThreeWire.Port[to_port(SidewaysTracker_port)])
{
  if (drive_setup != ZERO_TRACKER_NO_ODOM){
    if (drive_setup == TANK_ONE_ENCODER || drive_setup == TANK_ONE_ROTATION || drive_setup == ZERO_TRACKER_ODOM){
      odom.set_physical_distances(ForwardTracker_center_distance, 0);
    } else {
      odom.set_physical_distances(ForwardTracker_center_distance, SidewaysTracker_center_distance);
    }
  }
}

void Drive::drive_with_voltage(float leftVoltage, float rightVoltage){
  DriveL.spin(fwd, leftVoltage, volt);
  DriveR.spin(fwd, rightVoltage,volt);
}

void Drive::set_turn_constants(float turn_max_voltage, float turn_kp, float turn_ki, float turn_kd, float turn_starti){
  this->turn_max_voltage = turn_max_voltage;
  this->turn_kp = turn_kp;
  this->turn_ki = turn_ki;
  this->turn_kd = turn_kd;
  this->turn_starti = turn_starti;
} 

void Drive::set_drive_constants(float drive_max_voltage, float drive_kp, float drive_ki, float drive_kd, float drive_starti){
  this->drive_max_voltage = drive_max_voltage;
  this->drive_kp = drive_kp;
  this->drive_ki = drive_ki;
  this->drive_kd = drive_kd;
  this->drive_starti = drive_starti;
} 

void Drive::set_heading_constants(float heading_max_voltage, float heading_kp, float heading_ki, float heading_kd, float heading_starti){
  this->heading_max_voltage = heading_max_voltage;
  this->heading_kp = heading_kp;
  this->heading_ki = heading_ki;
  this->heading_kd = heading_kd;
  this->heading_starti = heading_starti;
}

void Drive::set_swing_constants(float swing_max_voltage, float swing_kp, float swing_ki, float swing_kd, float swing_starti){
  this->swing_max_voltage = swing_max_voltage;
  this->swing_kp = swing_kp;
  this->swing_ki = swing_ki;
  this->swing_kd = swing_kd;
  this->swing_starti = swing_starti;
} 

void Drive::set_diff_constants(float diff_max_voltage, float diff_kp, float diff_ki, float diff_kd, float diff_starti){
  this->swing_max_voltage = diff_max_voltage;
  this->swing_kp = diff_kp;
  this->swing_ki = diff_ki;
  this->swing_kd = diff_kd;
  this->swing_starti = diff_starti;
} 

void Drive::set_turn_exit_conditions(float turn_settle_error, float turn_settle_time, float turn_timeout){
  this->turn_settle_error = turn_settle_error;
  this->turn_settle_time = turn_settle_time;
  this->turn_timeout = turn_timeout;
}

void Drive::set_drive_exit_conditions(float drive_settle_error, float drive_settle_time, float drive_timeout){
  this->drive_settle_error = drive_settle_error;
  this->drive_settle_time = drive_settle_time;
  this->drive_timeout = drive_timeout;
}

void Drive::set_swing_exit_conditions(float swing_settle_error, float swing_settle_time, float swing_timeout){
  this->swing_settle_error = swing_settle_error;
  this->swing_settle_time = swing_settle_time;
  this->swing_timeout = swing_timeout;
}

void Drive::set_diff_exit_conditions(float diff_settle_error, float diff_settle_time, float diff_timeout){
  this->diff_settle_error = diff_settle_error;
  this->diff_settle_time = diff_settle_time;
  this->diff_timeout = diff_timeout;
}

float Drive::get_absolute_heading(){ 
  return( reduce_0_to_360( Gyro.rotation()*360.0/gyro_scale ) ); 
}

float Drive::get_left_position_in(){
  return( DriveL.position(deg)*drive_in_to_deg_ratio );
}

float Drive::get_right_position_in(){
  return( DriveR.position(deg)*drive_in_to_deg_ratio );
}

void Drive::turn_to_angle(float angle){
  turn_to_angle(angle, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
}

void Drive::turn_to_angle(float angle, float turn_max_voltage){
  turn_to_angle(angle, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
}

void Drive::turn_to_angle(float angle, float turn_max_voltage, float turn_settle_error, float turn_settle_time, float turn_timeout){
  turn_to_angle(angle, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
}

double errang = 0;

void Drive::turn_to_angle(float angle, float turn_max_voltage, float turn_settle_error, float turn_settle_time, float turn_timeout, float turn_kp, float turn_ki, float turn_kd, float turn_starti){
  desired_heading = angle;
  printf("[start] Left_swing_to_angle: %f\n",get_absolute_heading());
  PID turnPID(reduce_negative_180_to_180(angle - get_absolute_heading()), turn_kp, turn_ki, turn_kd, turn_starti, turn_settle_error, turn_settle_time, turn_timeout);
  while(turnPID.is_settled() == false){
    float error = reduce_negative_180_to_180(angle - get_absolute_heading());
    float output = turnPID.compute(error);
    output = clamp(output, -turn_max_voltage, turn_max_voltage);
    drive_with_voltage(output, -output);
    // printf("error:%f\n",error);
    task::sleep(10);
  }
  double errang = get_absolute_heading();
  printf("[end] Left_swing_to_angle: %f\n",get_absolute_heading());
  DriveL.stop(hold);
  DriveR.stop(hold);
}

// void Drive::turn_to_angle_chain(float angle){
//   turn_to_angle(angle, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
// }

// void Drive::turn_to_angle_chain(float angle, float turn_max_voltage){
//   turn_to_angle(angle, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
// }

// void Drive::turn_to_angle_chain(float angle, float turn_max_voltage, float turn_settle_error, float turn_settle_time, float turn_timeout){
//   turn_to_angle(angle, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
// }

// void Drive::turn_to_angle_chain(float angle, float turn_max_voltage, float turn_settle_error, float turn_settle_time, float turn_timeout, float turn_kp, float turn_ki, float turn_kd, float turn_starti){
//   desired_heading = angle;
//   printf("[start] Left_swing_to_angle: %f",Inertial100.heading());
//   PID turnPID(reduce_negative_180_to_180(angle - get_absolute_heading()), turn_kp, turn_ki, turn_kd, turn_starti, turn_settle_error, turn_settle_time, turn_timeout);
//   while(turnPID.is_settled() == false){
//     float error = reduce_negative_180_to_180(angle - get_absolute_heading());
//     float output = turnPID.compute(error);
//     output = clamp(output, -turn_max_voltage, turn_max_voltage);
//     drive_with_voltage(output, -output);
//     task::sleep(10);
//   }
//   printf("[end] Left_swing_to_angle: %f",Inertial100.heading());
//   DriveL.spin(fwd,0,percent);
//   DriveR.spin(fwd,0,percent);
// }

// void Drive::turn_to_angle_custom(float angle, bool direction){
//   turn_to_angle_custom(angle, direction, turn_max_voltage, turn_timeout);
// }

// void Drive::turn_to_angle_custom(float angle, bool direction, float turn_max_voltage, float turn_timeout){
//   printf("[start] Left_swing_to_angle: %f\n",get_absolute_heading());
//   float error = std::abs(angle - Inertial100.heading());
//   while(std::abs(error) > 1.8){
//     if(direction == true){
//       DriveL.spin(fwd,turn_max_voltage,percent);
//       DriveR.spin(fwd,-(turn_max_voltage),percent);
//     }
//     else if(direction == false){
//       DriveL.spin(fwd,-(turn_max_voltage),percent);
//       DriveR.spin(fwd,turn_max_voltage,percent);
//     }
//     else{
//       DriveL.stop(hold);
//       DriveR.stop(hold);
//     }
//     wait(10,msec);
//   }
//   printf("[end] Left_swing_to_angle: %f\n",get_absolute_heading());
//   DriveL.spin(fwd,0,percent);
//   DriveR.spin(fwd,0,percent);
// }

void Drive::drive_distance(float distance){
  drive_distance(distance, desired_heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::drive_distance(float distance, float heading){
  drive_distance(distance, heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::drive_distance(float distance, float heading, float drive_max_voltage, float heading_max_voltage){
  drive_distance(distance, heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::drive_distance(float distance, float heading, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout){
  drive_distance(distance, heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

double errlin = 0;

void Drive::drive_distance(float distance, float heading, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout, float drive_kp, float drive_ki, float drive_kd, float drive_starti, float heading_kp, float heading_ki, float heading_kd, float heading_starti){
  desired_heading = heading;
  printf("[start] distance:");
  PID drivePID(distance, drive_kp, drive_ki, drive_kd, drive_starti, drive_settle_error, drive_settle_time, drive_timeout);
  PID headingPID(reduce_negative_180_to_180(heading - get_absolute_heading()), heading_kp, heading_ki, heading_kd, heading_starti);
  float start_average_position = (get_left_position_in()+get_right_position_in())/2.0;
  float average_position = start_average_position;
  
  while(drivePID.is_settled() == false){
    average_position = (get_left_position_in()+get_right_position_in())/2.0;
    float drive_error = distance+start_average_position-average_position;
    float heading_error = reduce_negative_180_to_180(heading - get_absolute_heading());
    float drive_output = drivePID.compute(drive_error);
    float heading_output = headingPID.compute(heading_error);

    drive_output = clamp(drive_output, -drive_max_voltage, drive_max_voltage);
    heading_output = clamp(heading_output, -heading_max_voltage, heading_max_voltage);

    drive_with_voltage(drive_output+heading_output, drive_output-heading_output);
    task::sleep(10);
  }
  errlin = average_position;
  printf("[end] distance: %f",average_position);
  DriveL.stop(hold);
  DriveR.stop(hold);
}


// void Drive::drive_distance_chain(float distance, float distexit){
//   drive_distance(distance, desired_heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
// }

// void Drive::drive_distance_chain(float distance, float distexit, float heading){
//   drive_distance(distance, heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
// }

// void Drive::drive_distance_chain(float distance, float distexit, float heading, float drive_max_voltage, float heading_max_voltage){
//   drive_distance(distance, heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
// }

// void Drive::drive_distance_chain(float distance, float distexit, float heading, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout){
//   drive_distance(distance, heading, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
// }

// void Drive::drive_distance_chain(float distance, float distexit, float heading, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout, float drive_kp, float drive_ki, float drive_kd, float drive_starti, float heading_kp, float heading_ki, float heading_kd, float heading_starti){
//   desired_heading = heading;
//   printf("[start] distance_chain: ");
//   PID drivePID(distance, drive_kp, drive_ki, drive_kd, drive_starti, drive_settle_error, drive_settle_time, drive_timeout);
//   PID headingPID(reduce_negative_180_to_180(heading - get_absolute_heading()), heading_kp, heading_ki, heading_kd, heading_starti);
//   float start_average_position = (get_left_position_in()+get_right_position_in())/2.0;
//   float average_position = start_average_position;
//   while(drivePID.is_settled() == false){
//     average_position = (get_left_position_in()+get_right_position_in())/2.0;
//     float drive_error = distance+start_average_position-average_position;
//     float heading_error = reduce_negative_180_to_180(heading - get_absolute_heading());
//     float drive_output = drivePID.compute(drive_error);
//     float heading_output = headingPID.compute(heading_error);

//     drive_output = clamp(drive_output, -drive_max_voltage, drive_max_voltage);
//     heading_output = clamp(heading_output, -heading_max_voltage, heading_max_voltage);

//     drive_with_voltage(drive_output+heading_output, drive_output-heading_output);
//     task::sleep(10);
//   }
//   printf("[end] distance_chain: %f",average_position);
//   DriveL.spin(fwd,0,percent);
//   DriveR.spin(fwd,0,percent);
// }

void Drive::left_swing_to_angle(float angle){
  left_swing_to_angle(angle, swing_max_voltage, swing_settle_error, swing_settle_time, swing_timeout, swing_kp, swing_ki, swing_kd, swing_starti);
}

void Drive::left_swing_to_angle(float angle, float swing_max_voltage, float swing_settle_error, float swing_settle_time, float swing_timeout, float swing_kp, float swing_ki, float swing_kd, float swing_starti){
  printf("[start] Left_swing_to_angle: %f",Inertial100.heading());
  desired_heading = angle;
  PID swingPID(reduce_negative_180_to_180(angle - get_absolute_heading()), swing_kp, swing_ki, swing_kd, swing_starti, swing_settle_error, swing_settle_time, swing_timeout);
  while(swingPID.is_settled() == false){
    float error = reduce_negative_180_to_180(angle - get_absolute_heading());
    float output = swingPID.compute(error);
    output = clamp(output, -turn_max_voltage, turn_max_voltage);
    DriveL.spin(fwd, output, volt);
    DriveR.stop(hold);
    task::sleep(10);
  }
  printf("[end] Left_swing_to_angle: %f\n",get_absolute_heading());
  DriveL.stop(hold);
  DriveR.stop(hold);
}

void Drive::right_swing_to_angle(float angle){
  right_swing_to_angle(angle, swing_max_voltage, swing_settle_error, swing_settle_time, swing_timeout, swing_kp, swing_ki, swing_kd, swing_starti);
  // printf("right_swing_to_angle");
}

void Drive::right_swing_to_angle(float angle, float swing_max_voltage, float swing_settle_error, float swing_settle_time, float swing_timeout, float swing_kp, float swing_ki, float swing_kd, float swing_starti){
  printf("[start] Right_swing_to_angle: %f\n",get_absolute_heading());
  desired_heading = angle;
  PID swingPID(reduce_negative_180_to_180(angle - get_absolute_heading()), swing_kp, swing_ki, swing_kd, swing_starti, swing_settle_error, swing_settle_time, swing_timeout);
  while(swingPID.is_settled() == false){
    float error = reduce_negative_180_to_180(angle - get_absolute_heading());
    float output = swingPID.compute(error);
    output = clamp(output, -turn_max_voltage, turn_max_voltage);
    DriveR.spin(reverse, output, volt);
    DriveL.stop(hold);
    task::sleep(10);
  }
  printf("[end] Right_swing_to_angle: %f\n",get_absolute_heading());
  DriveL.stop(hold);
  DriveR.stop(hold);
}

void Drive::diff_to_angle(float angle, float left_max_volt, float right_max_volt){
  diff_to_angle(angle, left_max_volt, right_max_volt, diff_settle_error, diff_settle_time, diff_timeout, diff_kp, diff_ki, diff_kd, diff_starti);
}

void Drive::diff_to_angle(float angle, float left_max_volt, float right_max_volt, float diff_settle_error, float diff_settle_time, float diff_timeout, float diff_kp, float diff_ki, float diff_kd, float diff_starti){
  desired_heading = angle;
  PID swing2PID(reduce_0_to_360(angle - get_absolute_heading()), diff_kp, diff_ki, diff_kd, diff_starti, diff_settle_error, diff_settle_time, diff_timeout);
  while(swing2PID.is_settled() == false){
    float error = reduce_0_to_360(angle - get_absolute_heading());
    float outputL = swing2PID.compute(error);
    float outputR = swing2PID.compute(error);
    outputL = clamp(outputL, -left_max_volt, left_max_volt);
    outputR = clamp(outputR, -right_max_volt, right_max_volt);
    DriveL.spin(fwd, outputL, volt);
    DriveR.spin(fwd, outputR, volt);
    task::sleep(10);
  }
  DriveL.stop(hold);
  DriveR.stop(hold);
}

void Drive::diff(double veloleft, double veloright, double timeout = 4000, double wingtimeout = 300){
  chassis.DriveL.spin(fwd, to_volt(veloleft), volt);
  chassis.DriveR.spin(fwd, to_volt(veloright), volt);
  wait(wingtimeout,msec);
  back_wings.set(false);
  wait(timeout-wingtimeout,msec);
  chassis.DriveL.spin(fwd,0,volt);
  chassis.DriveR.spin(fwd,0,volt);
}
void Drive::diff2(double veloleft, double veloright, double timeout = 4000, double wingtimeout = 300){
  chassis.DriveL.spin(fwd, to_volt(veloleft), volt);
  chassis.DriveR.spin(fwd, to_volt(veloright), volt);
  wait(wingtimeout,msec);
  back_wings2.set(false);
  wait(timeout-wingtimeout,msec);
  chassis.DriveL.spin(fwd,0,volt);
  chassis.DriveR.spin(fwd,0,volt);
}
void Drive::reramF(){
  // float angle = Inertial100.heading();
  chassis.DriveL.spin(fwd, -6, volt);
  chassis.DriveR.spin(fwd, -6, volt);
  wait(450,msec);
  // chassis.turn_to_angle(175);
  chassis.DriveL.spin(fwd, 9, volt);
  chassis.DriveR.spin(fwd, 9, volt);
  wait(800,msec);
  chassis.DriveL.spin(fwd,0,volt);
  chassis.DriveR.spin(fwd,0,volt);
}
void Drive::reramB(){
  chassis.DriveL.spin(fwd, 6, volt);
  chassis.DriveR.spin(fwd, 6, volt);
  wait(750,msec);
  chassis.DriveL.spin(fwd, -9, volt);
  chassis.DriveR.spin(fwd, -9, volt);
  wait(1000,msec);
  chassis.DriveL.spin(fwd,0,volt);
  chassis.DriveR.spin(fwd,0,volt);
}
void Drive::reramB2(int timeout1, int timeout2){
  chassis.DriveL.spin(fwd, 6, volt);
  chassis.DriveR.spin(fwd, 6, volt);
  wait(timeout1,msec);
  chassis.DriveL.spin(fwd, -9, volt);
  chassis.DriveR.spin(fwd, -9, volt);
  wait(timeout2,msec);
  chassis.DriveL.spin(fwd,0,volt);
  chassis.DriveR.spin(fwd,0,volt);
}
void Drive::diffram(double veloleft, double veloright, double timeout = 4000, double wingtimeout = 300){
  chassis.DriveL.spin(fwd, to_volt(veloleft), volt);
  chassis.DriveR.spin(fwd, to_volt(veloright), volt);
  wait(wingtimeout,msec);
  back_wings.set(false);
  back_wings2.set(true);
  wait(timeout-wingtimeout,msec);
  chassis.DriveL.spin(fwd,0,volt);
  chassis.DriveR.spin(fwd,0,volt);
  back_wings2.set(false);
}

// void Drive::moveToPose(float x, float y, float theta, double dlead, int timeout, bool direction = true){
//   desired_heading = theta;
//   float Xpos = chassis.get_X_position();
//   float Ypos = chassis.get_Y_position();

//   float Xtheta = Xpos-x;
//   float Ytheta = Ypos-y;

//   float combo = std::sqrt((Xtheta)*(Xtheta)+(Ytheta)*(Ytheta));

//   float carrotX = x - (combo*(std::sin(theta)*dlead));
//   float carrotY = y - (combo*(std::cos(theta)*dlead));

//   // PID lateralPID(reduce_negative_180_to_180(theta - get_absolute_heading()), heading_kp, heading_ki, heading_kd);
//   // PID angularPID(reduce_negative_180_to_180(theta - get_absolute_heading()), turn_kp, turn_ki, turn_kd);



// }

void Drive::cross(double timeout = 5000){
  // int timer = vex::timer();
  chassis.DriveL.spin(fwd, 70, volt);
  chassis.DriveR.spin(fwd, 70, volt);
  waitUntil(Inertial100.pitch(vex::degrees) > 5);
  waitUntil(Inertial100.pitch(vex::degrees) < -5);
  waitUntil(Inertial100.pitch(vex::degrees) > -1 && Inertial100.pitch(vex::degrees) < 1 );
  wait(250,msec);
  chassis.DriveL.stop(brake);
  chassis.DriveR.stop(brake);
}

void Drive::ram(double timeout = 1000, double leftvolt = -9, double rightvolt = -9){
  chassis.DriveL.spin(fwd, leftvolt, volt);
  chassis.DriveR.spin(fwd, rightvolt, volt);
  wait(timeout,msec);
}

void Drive::pre_outtake(float desired_angle, float errorval=10){
  float error = std::abs(desired_angle-Inertial100.heading());
  chassis.turn_to_angle(desired_angle);
  waitUntil(error <= errorval);
  intakeMotor.spin(reverse,100,percent);
}

void Drive::endgame(float timeout = 1000){
  pistonratchet.set(true);
  chassis.DriveL.spin(fwd, 8.2, volt);
  chassis.DriveR.spin(fwd, 8.2, volt);
  wait(timeout,msec);
  // waitUntil(Inertial100.pitch(vex::degrees) > 1 || Inertial100.pitch(vex::degrees) < -1);
  pistonratchet.set(false);
}
// void Drive::diffdist(double distleft, double distright, double veloleft, double veloright, double timeout){
//   double wheelcalc = 4*M_PI;
//   double leftdistcalc = distleft/wheelcalc*1.75*360;
//   double rightdistcalc = distright/wheelcalc*1.75*360;
//   chassis.DriveL.spinFor()
//   // chassis.DriveL.spinToPosition(leftdistcalc,degrees,false,veloleft);
//   // chassis.DriveR.spinToPosition(rightdistcalc,degrees,false,veloright);
//   wait(timeout,msec);
//   chassis.DriveL.stop(coast);
//   chassis.DriveR.stop(coast);
// }

float Drive::get_ForwardTracker_position(){
  if (drive_setup==ZERO_TRACKER_ODOM){
    return(get_right_position_in());
  }
  if (drive_setup==TANK_ONE_ENCODER || drive_setup == TANK_TWO_ENCODER || drive_setup == HOLONOMIC_TWO_ENCODER){
    return(E_ForwardTracker.position(deg)*ForwardTracker_in_to_deg_ratio);
  }else{
    return(R_ForwardTracker.position(deg)*ForwardTracker_in_to_deg_ratio);
  }
}

float Drive::get_SidewaysTracker_position(){
  if (drive_setup==TANK_ONE_ENCODER || drive_setup == TANK_ONE_ROTATION || drive_setup == ZERO_TRACKER_ODOM){
    return(0);
  }else if (drive_setup == TANK_TWO_ENCODER || drive_setup == HOLONOMIC_TWO_ENCODER){
    return(E_SidewaysTracker.position(deg)*SidewaysTracker_in_to_deg_ratio);
  }else{
    return(R_SidewaysTracker.position(deg)*SidewaysTracker_in_to_deg_ratio);
  }
}

void Drive::position_track(){
  while(1){
    odom.update_position(get_ForwardTracker_position(), get_SidewaysTracker_position(), get_absolute_heading());
    task::sleep(5);
  }
}

void Drive::set_heading(float orientation_deg){
  Gyro.setRotation(orientation_deg*gyro_scale/360.0, deg);
}

void Drive::set_coordinates(float X_position, float Y_position, float orientation_deg){
  odom.set_position(X_position, Y_position, orientation_deg, get_ForwardTracker_position(), get_SidewaysTracker_position());
  set_heading(orientation_deg);
  odom_task = task(position_track_task);
}

float Drive::get_X_position(){
  return(odom.X_position);
}

float Drive::get_Y_position(){
  return(odom.Y_position);
}

void Drive::drive_to_point(float X_position, float Y_position){
  drive_to_point(X_position, Y_position, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::drive_to_point(float X_position, float Y_position, float drive_max_voltage, float heading_max_voltage){
  drive_to_point(X_position, Y_position, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::drive_to_point(float X_position, float Y_position, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout){
  drive_to_point(X_position, Y_position, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::drive_to_point(float X_position, float Y_position, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout, float drive_kp, float drive_ki, float drive_kd, float drive_starti, float heading_kp, float heading_ki, float heading_kd, float heading_starti){
  PID drivePID(hypot(X_position-get_X_position(),Y_position-get_Y_position()), drive_kp, drive_ki, drive_kd, drive_starti, drive_settle_error, drive_settle_time, drive_timeout);
  PID headingPID(reduce_negative_180_to_180(to_deg(atan2(X_position-get_X_position(),Y_position-get_Y_position()))-get_absolute_heading()), heading_kp, heading_ki, heading_kd, heading_starti);
  while(drivePID.is_settled() == false){
    float drive_error = hypot(X_position-get_X_position(),Y_position-get_Y_position());
    float heading_error = reduce_negative_180_to_180(to_deg(atan2(X_position-get_X_position(),Y_position-get_Y_position()))-get_absolute_heading());
    float drive_output = drivePID.compute(drive_error);

    float heading_scale_factor = cos(to_rad(heading_error));
    drive_output*=heading_scale_factor;
    heading_error = reduce_negative_90_to_90(heading_error);
    float heading_output = headingPID.compute(heading_error);
    
    if (drive_error<drive_settle_error) { heading_output = 0; }

    drive_output = clamp(drive_output, -fabs(heading_scale_factor)*drive_max_voltage, fabs(heading_scale_factor)*drive_max_voltage);
    heading_output = clamp(heading_output, -heading_max_voltage, heading_max_voltage);

    drive_with_voltage(drive_output+heading_output, drive_output-heading_output);
    task::sleep(10);
  }
  desired_heading = get_absolute_heading();
  DriveL.stop(hold);
  DriveR.stop(hold);
}

void Drive::turn_to_point(float X_position, float Y_position){
  turn_to_point(X_position, Y_position, 0, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
}

void Drive::turn_to_point(float X_position, float Y_position, float extra_angle_deg){
  turn_to_point(X_position, Y_position, extra_angle_deg, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
}

void Drive::turn_to_point(float X_position, float Y_position, float extra_angle_deg, float turn_max_voltage, float turn_settle_error, float turn_settle_time, float turn_timeout){
  turn_to_point(X_position, Y_position, extra_angle_deg, turn_max_voltage, turn_settle_error, turn_settle_time, turn_timeout, turn_kp, turn_ki, turn_kd, turn_starti);
}

void Drive::turn_to_point(float X_position, float Y_position, float extra_angle_deg, float turn_max_voltage, float turn_settle_error, float turn_settle_time, float turn_timeout, float turn_kp, float turn_ki, float turn_kd, float turn_starti){
  PID turnPID(reduce_negative_180_to_180(to_deg(atan2(X_position-get_X_position(),Y_position-get_Y_position())) - get_absolute_heading()), turn_kp, turn_ki, turn_kd, turn_starti, turn_settle_error, turn_settle_time, turn_timeout);
  while(turnPID.is_settled() == false){
    float error = reduce_negative_180_to_180(to_deg(atan2(X_position-get_X_position(),Y_position-get_Y_position())) - get_absolute_heading() + extra_angle_deg);
    float output = turnPID.compute(error);
    output = clamp(output, -turn_max_voltage, turn_max_voltage);
    drive_with_voltage(output, -output);
    task::sleep(10);
  }
  desired_heading = get_absolute_heading();
  DriveL.stop(hold);
  DriveR.stop(hold);
}

void Drive::holonomic_drive_to_point(float X_position, float Y_position){
  holonomic_drive_to_point(X_position, Y_position, get_absolute_heading(), drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::holonomic_drive_to_point(float X_position, float Y_position, float angle){
  holonomic_drive_to_point(X_position, Y_position, angle, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::holonomic_drive_to_point(float X_position, float Y_position, float angle, float drive_max_voltage, float heading_max_voltage){
  holonomic_drive_to_point(X_position, Y_position, angle, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::holonomic_drive_to_point(float X_position, float Y_position, float angle, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout){
  holonomic_drive_to_point(X_position, Y_position, angle, drive_max_voltage, heading_max_voltage, drive_settle_error, drive_settle_time, drive_timeout, drive_kp, drive_ki, drive_kd, drive_starti, heading_kp, heading_ki, heading_kd, heading_starti);
}

void Drive::holonomic_drive_to_point(float X_position, float Y_position, float angle, float drive_max_voltage, float heading_max_voltage, float drive_settle_error, float drive_settle_time, float drive_timeout, float drive_kp, float drive_ki, float drive_kd, float drive_starti, float heading_kp, float heading_ki, float heading_kd, float heading_starti){
  desired_heading = angle;
  PID drivePID(hypot(X_position-get_X_position(),Y_position-get_Y_position()), drive_kp, drive_ki, drive_kd, drive_starti, drive_settle_error, drive_settle_time, drive_timeout);
  PID turnPID(reduce_negative_180_to_180(to_deg(atan2(X_position-get_X_position(),Y_position-get_Y_position()))-get_absolute_heading()), heading_kp, heading_ki, heading_kd, heading_starti);
  while(!(drivePID.is_settled() && turnPID.is_settled() ) ){
    float drive_error = hypot(X_position-get_X_position(),Y_position-get_Y_position());
    float turn_error = reduce_negative_180_to_180(to_deg(atan2(X_position-get_X_position(),Y_position-get_Y_position()))-get_absolute_heading());

    float drive_output = drivePID.compute(drive_error);
    float turn_output = turnPID.compute(turn_error);

    drive_output = clamp(drive_output, drive_max_voltage, drive_max_voltage);
    turn_output = clamp(turn_output, -heading_max_voltage, heading_max_voltage);

    float heading_error = atan2(Y_position-get_Y_position(), X_position-get_X_position());

    DriveLF.spin(fwd, drive_output*cos(to_rad(get_absolute_heading()) + heading_error - M_PI/4) + turn_output, volt);
    DriveLB.spin(fwd, drive_output*cos(-to_rad(get_absolute_heading()) - heading_error + 3*M_PI/4) + turn_output, volt);
    DriveRB.spin(fwd, drive_output*cos(to_rad(get_absolute_heading()) + heading_error - M_PI/4) - turn_output, volt);
    DriveRF.spin(fwd, drive_output*cos(-to_rad(get_absolute_heading()) - heading_error + 3*M_PI/4) - turn_output, volt);
    task::sleep(10);
  }
  DriveLF.stop(hold);
  DriveLB.stop(hold);
  DriveRB.stop(hold);
  DriveRF.stop(hold);
}

void Drive::control_arcade(){
  float throttle = deadband(controller(primary).Axis3.value(), 5);
  float turn = deadband(controller(primary).Axis1.value(), 5);
  DriveL.spin(fwd, to_volt(throttle+turn), volt);
  DriveR.spin(fwd, to_volt(throttle-turn), volt);
}

void Drive::control_holonomic(){
  float throttle = deadband(controller(primary).Axis3.value(), 5);
  float turn = deadband(controller(primary).Axis1.value(), 5);
  float strafe = deadband(controller(primary).Axis4.value(), 5);
  DriveLF.spin(fwd, to_volt(throttle+turn+strafe), volt);
  DriveRF.spin(fwd, to_volt(throttle-turn-strafe), volt);
  DriveLB.spin(fwd, to_volt(throttle+turn-strafe), volt);
  DriveRB.spin(fwd, to_volt(throttle-turn+strafe), volt);
}

void Drive::control_tank(){
  float leftthrottle = deadband(controller(primary).Axis3.value(), 5);
  float rightthrottle = deadband(controller(primary).Axis2.value(), 5);
  DriveL.spin(fwd, to_volt(leftthrottle), volt);
  DriveR.spin(fwd, to_volt(rightthrottle), volt);
}

int Drive::position_track_task(){
  chassis.position_track();
  return(0);
}