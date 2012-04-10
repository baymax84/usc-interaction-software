// preprocessor directives
#ifndef POLOLU_MAESTRO_SERVO_ANGLE_CONTROLLER_H
#define POLOLU_MAESTRO_SERVO_ANGLE_CONTROLLER_H
#include <iostream>
#include <sparky/servo_controller.h>
#include <stdint.h>

//
namespace pololu
{

namespace maestro
{

// public type redefinitions
typedef std::pair<uint16_t, double> ServoAnglePair;
typedef std::pair<ServoAnglePair, ServoAnglePair> ServoAngleLimits;

//
class ServoAngleController : public ServoController
{
public:

  // public constructors/destructors
  ServoAngleController(const uint8_t n_channels, const std::string path = "", const bool connect = false);
  ServoAngleController(const uint8_t n_devices, const uint8_t n_channels_each, const std::string path = "",
                       const bool connect = false);
  ServoAngleController(const std::vector<uint8_t> n_device_channels, const std::string path = "", const bool connect =
                           false);

  // virtual public constructors/destructors
  virtual ~ServoAngleController();

  // private utility functions
  bool isValidAngleTarget(const uint8_t channel, const double target) const;
  bool isValidAngleTarget(const uint8_t device, const uint8_t channel, const double target) const;
  double clipAngleTargetValue(const uint8_t channel, const double target) const;
  double clipAngleTargetValue(const uint8_t device, const uint8_t channel, const double target) const;
  double servoToAngle(const uint8_t channel, uint16_t pos) const;
  double servoToAngle(const uint8_t device, const uint8_t channel, uint16_t pos) const;
  uint16_t angleToServo(const uint8_t channel, double angle) const;
  uint16_t angleToServo(const uint8_t device, const uint8_t channel, double angle) const;

  // public mutator functions
  bool setServoAngleLimits(const uint8_t channel, ServoAngleLimits limits);
  bool setServoAngleLimits(const uint8_t device, const uint8_t channel, ServoAngleLimits limits);
  bool setServoAngleLimits(const uint8_t channel, ServoAnglePair limit1, ServoAnglePair limit2);
  bool setServoAngleLimits(const uint8_t device, const uint8_t channel, ServoAnglePair limit1, ServoAnglePair limit2);
  bool setAngleTarget(const uint8_t channel, double target);
  bool setAngleTarget(const uint8_t device, uint8_t channel, double target);
  bool setAngleSpeed(const uint8_t channel, double speed);
  bool setAngleSpeed(const uint8_t device, const uint8_t channel, double speed);
  bool setAngleAcceleration(const uint8_t channel, double accel);
  bool setAngleAcceleration(const uint8_t device, const uint8_t channel, double accel);

  // public accessor functions
  ServoAngleLimits getServoAngleLimits(const uint8_t channel) const;
  ServoAngleLimits getServoAngleLimits(const uint8_t device, const uint8_t channel) const;
  ServoAnglePair getServoAngleMinLimitPair(const uint8_t channel) const;
  ServoAnglePair getServoAngleMinLimitPair(const uint8_t device, const uint8_t channel) const;
  ServoAnglePair getServoAngleMaxLimitPair(const uint8_t channel) const;
  ServoAnglePair getServoAngleMaxLimitPair(const uint8_t device, const uint8_t channel) const;
  double getAngleMinLimit(const uint8_t channel) const;
  double getAngleMinLimit(const uint8_t device, const uint8_t channel) const;
  double getAngleMaxLimit(const uint8_t channel) const;
  double getAngleMaxLimit(const uint8_t device, const uint8_t channel) const;
  double getAngleTarget(const uint8_t channel);
  double getAngleTarget(const uint8_t device, const uint8_t channel);
  double getAngleSpeed(const uint8_t channel);
  double getAngleSpeed(const uint8_t device, const uint8_t channel);
  double getAngleAcceleration(const uint8_t channel);
  double getAngleAcceleration(const uint8_t device, const uint8_t channel);
  double getAnglePosition(const uint8_t channel);
  double getAnglePosition(const uint8_t device, const uint8_t channel);

private:

  // private data members
  std::vector<std::vector<ServoAngleLimits> > servos_servo_angle_limits_;
};// ServoAngleController

} // maestro

} // pololu

#endif // POLOLU_MAESTRO_SERVO_ANGLE_CONTROLLER_H
