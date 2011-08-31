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
  bool isJointMoving(const std::string name);
  bool waitForJointDone(const std::string name) const;
  bool waitForJointsDone();

  // public mutator functions
  bool setPath(const std::string name);
  //bool setJointAngleLimits(const std::string name, const JointLimits limits);
  bool setJointAngleLimits(const std::string name, const double angle1, const double angle2);
  bool setJointMinAngle(const std::string name, const double angle);
  bool setJointMaxAngle(const std::string name, const double angle);
  bool setJointActive(const std::string name, const bool active);
  bool setJointAngle(const std::string name, double angle);
  bool setJointSpeed(const std::string name, double speed);
  bool setJointAcceleration(const std::string name, double accel);
  bool setJointsHome();

  // public accessor functions
  std::string getPath() const;
  size_t getNumJoints() const;
  size_t getNumJointsActive() const;
  size_t getNumJointsInactive() const;
  //JointLimits getJointLimits(const std::string name) const;
  double getJointMinAngle(const std::string name) const;
  double getJointMaxAngle(const std::string name) const;
  bool getJointActive(const std::string name) const;
  double getJointAngle(const std::string name) const;
  double getJointSpeed(const std::string name) const;
  double getJointAccel(const std::string name) const;
  bool getJointMoving(const std::string name);
  bool getJointsMoving();
  uint16_t getErrors();

private:

  // private definitions
#define N_DEVICES (2)
#define N_CHANNELS_EACH (24)

  // private data members
  pololu::MaestroServoController servo_controller_;
};
// SparkyController
}
// sparky

#endif	// SPARKY_CONTROLLER_H
