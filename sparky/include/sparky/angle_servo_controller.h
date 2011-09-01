// preprocessor directives
#ifndef SERVO_ANGLE_CONTROLLER_H
#define SERVO_ANGLE_CONTROLLER_H
#include <iostream>
#include <stdint.h>
#include <sparky/maestro_servo_controller.h>

//
namespace pololu
{

//
class AngleServoController : public MaestroServoController
{
public:

  // public type redefinitions
  typedef std::pair<double, uint16_t> AngleServoPair;
  typedef std::pair<AngleServoPair, AngleServoPair> AngleLimits;

  // public constructors/destructors
  AngleServoController(const uint8_t n_channels, const std::string path = "", const bool connect = false);
  AngleServoController(const uint8_t n_devices, const uint8_t n_channels_each, const std::string path = "",
                       const bool connect = false);
  AngleServoController(const std::vector<uint8_t> n_device_channels, const std::string path = "", const bool connect =
                           false);

  // virtual public constructors/destructors
  virtual ~AngleServoController();

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
  bool setAngleLimits(const uint8_t channel, AngleLimits limits);
  bool setAngleLimits(const uint8_t device, const uint8_t channel, AngleLimits limits);
  bool setAngleLimits(const uint8_t channel, AngleServoPair limit1, AngleServoPair limit2);
  bool setAngleLimits(const uint8_t device, const uint8_t channel, AngleServoPair limit1, AngleServoPair limit2);
  bool setAngleTarget(const uint8_t channel, double target);
  bool setAngleTarget(const uint8_t device, uint8_t channel, double target);
  bool setAngleSpeed(const uint8_t channel, double speed);
  bool setAngleSpeed(const uint8_t device, const uint8_t channel, double speed);
  bool setAngleAcceleration(const uint8_t channel, double accel);
  bool setAngleAcceleration(const uint8_t device, const uint8_t channel, double accel);

  // public accessor functions
  AngleLimits getAngleLimits(const uint8_t channel) const;
  AngleLimits getAngleLimits(const uint8_t device, const uint8_t channel) const;
  AngleServoPair getAngleMinLimitPair(const uint8_t channel) const;
  AngleServoPair getAngleMinLimitPair(const uint8_t device, const uint8_t channel) const;
  AngleServoPair getAngleMaxLimitPair(const uint8_t channel) const;
  AngleServoPair getAngleMaxLimitPair(const uint8_t device, const uint8_t channel) const;
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
  std::vector<std::vector<AngleLimits> > angle_servos_;
};
// AngleServoController
}// pololu

#endif // SERVO_ANGLE_CONTROLLER_H
