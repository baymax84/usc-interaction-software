// preprocessor directives
#ifndef SPARKY_CONTROLLER_H
#define SPARKY_CONTROLLER_H
#include <sparky/maestro_servo_controller.h>

// a namespace for the Sparky Disney Minimatronic robot platform
namespace sparky
{

// a class describing the controller for the
// Sparky Disney Minimatronic robot platform
class SparkyController
{
public:

  // public constructors/destructors
  SparkyController(const std::string device_path, const bool connect = false, const bool home = false);
  ~SparkyController();

  // public utility functions
  bool connect(const bool home = false);
  bool disconnect();
  bool isConnected();

  // public mutator functions
  bool setJointAngle(std::string name, double angle);
  bool setJointMinAngle(std::string name, double angle);
  bool setJointMaxAngle(std::string name, double angle);

  // public accessor functions
  double getJointAngle(std::string name) const;
  double getJointMinAngle(std::string name) const;
  double getJointMaxAngle(std::string name) const;
  size_t getNumJoints() const;

private:

  // private definitions
#define NUM_SERVOS (48)

  // private data members
  pololu::MaestroServoController servo_controller_;
};
// SparkyController
}
// sparky

#endif	// SPARKY_CONTROLLER_H
