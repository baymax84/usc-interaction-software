// preprocessor directives
#include <sparky/maestro_servo_controller.h>

// executes main program code
int main(int argc, char** argv)
{
  pololu::MaestroServoController servo_controller(2, 24);
  servo_controller.setPath("/dev/ttyACM0");
  servo_controller.connect();
  servo_controller.isConnected();
  servo_controller.setServoAcceleration(0, 0);
  servo_controller.setServoSpeed(0, 0);
  servo_controller.setServoTarget(0, 0, 2000);
  servo_controller.disconnect();
  return 0;
} // main(int, char**)
