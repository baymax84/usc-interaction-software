// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  Handles talking to the R/C servo motors for the minimatronic
//

#include <iostream>
#include <stdio.h>

#include "sparky/servo.hh"
//#include "sparky/controller.hh"
#include <sparky/maestro_servo_controller.h>

#define N_JOINTS (18)
#define N_DEVICES (1)
#define N_CHANNELS_EACH (24)
#define PATH ("/dev/ttyACM0")
static pololu::maestro::ServoController controller(N_DEVICES, N_CHANNELS_EACH, PATH);
//static Controller controller;

//////////////////////////////////////////////////////////////////////
//
//  init_servo_controller()
//
//  connects to and initializes the Pololu controllers for the minimatronic
//  returns true on success, false on failure
//
//  no arguments
//

bool init_servo_controller()
{

  if (!controller.connect())
  {
    std::cout << "Unable to open controller, exiting" << std::endl;
    return false;
  }

  // initialize Sparky
  int device = -1;
  int channel = -1;
  pololu::maestro::ServoLimits servo_limits(1100, 2100);//(900, 2100);
  bool enabled = true;
  int speed = 0;
  int accel = 0;
  //int target = 0;

  bool success = true;
  for (int i = 0, n = N_JOINTS; i < n; ++i)
  {
    bool joint_success = true;
    device = getJointDevice(i);
    channel = getJointChannel(i);

    int sleep_time = 10000;
    joint_success = controller.setServoLimits(device, channel, servo_limits) && joint_success;
    usleep(sleep_time);
    joint_success = controller.setServoEnabled(device, channel, enabled) && joint_success;
    usleep(sleep_time);
    joint_success = controller.setServoSpeed(device, channel, speed) && joint_success;
    usleep(sleep_time);
    joint_success = controller.setServoAcceleration(device, channel, accel) && joint_success;
    usleep(sleep_time);
    //joint_success = controller.setServoTarget(device, channel, target) && joint_success;

    if (!joint_success)
      printf("Unable to initialize joint[%d] (servo[%d, %d])...\n", i, device, channel);  // ROS_ERROR
    else
      printf("Initialized joint[%d] (servo[%d, %d])!!!\n", i, device, channel);  // ROS_INFO

    success &= joint_success;
  }
  /*
   //success = true;
   success = jointMoveTo(0, 0.25f) && success;
   success = jointMoveTo(1, 0.50f) && success;
   success = jointMoveTo(2, 0.50f) && success;
   success = jointMoveTo(3, 0.25f) && success;
   success = jointMoveTo(4, 0.25f) && success;
   success = jointMoveTo(5, 0.25f) && success;
   success = jointMoveTo(6, 0.33f) && success;
   success = jointMoveTo(7, 0.25f) && success;
   success = jointMoveTo(8, 0.25f) && success;
   success = jointMoveTo(9, 0.25f) && success;
   success = jointMoveTo(10, 0.60f) && success;
   success = jointMoveTo(11, 0.58f) && success;
   success = jointMoveTo(12, 0.58f) && success;
   success = jointMoveTo(13, 0.57f) && success;
   success = jointMoveTo(14, 0.00f) && success;
   success = jointMoveTo(15, 0.37f) && success;
   success = jointMoveTo(16, 0.05f) && success; // standing upright
   success = jointMoveTo(17, 0.40f) && success;
   */
  // success
  return true;
}

//////////////////////////////////////////////////////////////////////
//
//  shutdown_servo_controller()
//
//  shuts down and disconnects the Pololu controllers for the minimatronic
//  returns no value (void)
//
//  no arguments
//

void shutdown_servo_controller()
{
  // move to home (mid stroke)
  controller.setServosHome();

  // allow some time to get there before we shutdown
  sleep(1);

  // turn off the servo controllers
  //controller . turnOffAll();

  // close (the close would be done automatically anyway by the dtor)
  controller.disconnect();
}

//////////////////////////////////////////////////////////////////////
//
//  outputPositions(double* vals, bool* active)
//
//  commands the positions of the mimiatronic's 18 motors
//  returns true on success, false on failure
//
//  arguments:
//      douible vals     - vertor of 18 positions
//      bool active      - vector of 18 bools (if true,  use the position val
//                                             if false, ignore the position val (hold current pos))
//

bool outputPositions(double* vals, bool* active)
{
  for (int chan = 0; chan < N_JOINTS; ++chan)
  {
    if (active == 0 || active[chan])
    {
      if (!jointMoveTo(chan, vals[chan]))
      {
        std::cout << "Error moving channel " << chan + 1 << std::endl;
        return false;
      }
    }
  }

  // success
  return true;
}

bool jointMoveTo(int id, double param) //double angle)
{
  if ((id < 0) || (id >= N_JOINTS))
  {
    printf("Joint ID %d out of range!\n", id);  // ROS_ERROR
    return false;
  }

  int device = getJointDevice(id);
  int channel = getJointChannel(id);
  int min_limit = controller.getServoMinLimit(device, channel);
  int max_limit = controller.getServoMaxLimit(device, channel);
  int target = min_limit + param * (max_limit - min_limit); // assumes param [0, 1]

  //printf("Moving joint[%d] (servo[%d, %d]) to %.2f (%d)\n", id, device, channel, param, target); // ROS_INFO
  if (!controller.setServoTarget(device, channel, target))
  {
    //printf("Error moving joint[%d] (servo[%d, %d] to %.2f (%d)!\n", id, device, channel, param, target); // ROS_ERROR
    return false;
  }

  return true;
} // jointMoveTo(int, double)

int getJointDevice(int id)
{
  return 0;
  return (id < (N_JOINTS / 2)) ? 0 : 1;
} // getJointDevice(int)

int getJointChannel(int id)
{
  return id;
  return 2 * ((getJointDevice(id) == 0) ? id : (id - (N_JOINTS / 2)));
} // getJointChannel(int)
