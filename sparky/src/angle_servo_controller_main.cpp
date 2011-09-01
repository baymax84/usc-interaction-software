// preprocessor directives
#include <iostream>
#include <sparky/angle_servo_controller.h>
#include <stdio.h>
#include <string>
using namespace std;

// executes main program code
int main(int argc, char** argv)
{
  int n_devices = 2;
  int n_channels_each = 24;
  pololu::AngleServoController angle_controller(n_devices, n_channels_each);

  string path = "/dev/ttyACM0";
  int device = 1;
  int channel = 4;
  pololu::AngleServoController::ServoLimits limits(900, 2100);
  bool enabled = true;
  int accel = 0;
  int speed = 0;
  int target = 1800;

  printf("n_devices = %d\n", angle_controller.getNumDevices());
  for (int i = 0, n = angle_controller.getNumDevices(); i < n; ++i)
    printf("- n_channels[%d] = %d\n", i, angle_controller.getNumChannels(i));

  printf("setPath(%s) = ", path.c_str());
  if (angle_controller.setPath(path)) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("connect() = ");
  if (angle_controller.connect()) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("isConnected() = ");
  if (angle_controller.isConnected()) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("setServoLimits(%d, %d, <%d, %d>) = ", device, channel, limits.first, limits.second);
  if (angle_controller.setServoLimits(device, channel, limits)) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("setServoEnabled(%d, %d, %d) = ", device, channel, enabled);
  if (angle_controller.setServoEnabled(device, channel, enabled)) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("setServoAcceleration(%d, %d, %d) = ", device, channel, accel);
  if (angle_controller.setServoAcceleration(device, channel)) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("setServoSpeed(%d, %d, %d) = ", device, channel, speed);
  if (angle_controller.setServoSpeed(device, channel, speed)) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  printf("setServoTarget(%d, %d, %d) = ", device, channel, target);
  if (angle_controller.setServoTarget(device, channel, target)) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  sleep(1);

  printf("disconnected() = ");
  if (angle_controller.disconnect()) printf("SUCCESS!!!\n");
  else printf("FAILURE...\n");

  return 0;
} // main(int, char**)
