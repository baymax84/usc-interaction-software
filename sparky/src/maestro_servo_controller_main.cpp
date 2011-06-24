// preprocessor directives
#include <sparky/maestro_servo_controller.h>

// executes main program code
int main(int argc, char** argv)
{
  pololu::MaestroServoController servo_controller;
  servo_controller.setDevicePath("/dev/ttyACM0");
  servo_controller.setNumServos(48);
  servo_controller.connect();
  servo_controller.isConnected();
  servo_controller.disconnect();
  return 0;
} // main(int, char**)
