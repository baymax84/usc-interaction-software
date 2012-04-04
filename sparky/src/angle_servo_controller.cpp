// preprocessor directives
#include <assert.h>
#include <sparky/angle_servo_controller.h>
using namespace pololu;
using namespace std;

//
AngleServoController::AngleServoController(const uint8_t n_channels, const std::string path, const bool connect) :
  MaestroServoController(n_channels, path, connect)

{
  angle_servos_.resize(1);
  angle_servos_[0].resize(n_channels);
} // AngleServoController(const uint8_t, const std::string, const bool)

//
AngleServoController::AngleServoController(const uint8_t n_devices, const uint8_t n_channels_each,
                                           const std::string path, const bool connect) :
  MaestroServoController(n_devices, n_channels_each, path, connect)
{
  assert((n_devices > 0) && (n_channels_each > 0));
  angle_servos_.resize(n_devices);
  for (int i = 0; i < n_devices; ++i)
    angle_servos_[i].resize(n_channels_each);
} // AngleServoController(const uint8_t, const uint8_t, const std::string, const bool)

//
AngleServoController::AngleServoController(const vector<uint8_t> n_device_channels, const std::string path,
                                           const bool connect) :
  MaestroServoController(n_device_channels, path, connect)
{
  int n_devices = n_device_channels.size();
  assert(n_devices > 0);
  angle_servos_.resize(n_devices);
  for (int i = 0; i < n_devices; ++i)
  {
    assert(n_device_channels[i] > 0);
    angle_servos_[i].resize(n_device_channels[i]);
  }
} // AngleServoController(const uint8_t, const uint8_t, const std::string, const bool)

// shuts down the servo controller, disconnecting if necessary
AngleServoController::~AngleServoController()
{
  if (isConnected())
    disconnect();
} // ~AngleServoController()

//
bool AngleServoController::isValidAngleTarget(const uint8_t channel, const double target) const
{
  return isValidAngleTarget(0, channel, target);
} // isValidAngleTarget(const uint8_t, const double)

//
bool AngleServoController::isValidAngleTarget(const uint8_t device, const uint8_t channel, const double target) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return (target >= getAngleMinLimit(device, channel)) && (target <= getAngleMaxLimit(device, channel));
} // isValidAngleTarget(const uint8_t, const uint8_t, const double)

//
double AngleServoController::clipAngleTargetValue(const uint8_t channel, const double target) const
{
  return clipAngleTargetValue(0, channel, target);
} // clipAngleTargetValue(const uint8_t, const double)

//
double AngleServoController::clipAngleTargetValue(const uint8_t device, const uint8_t channel, const double target) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0.0;
  double min_limit = getAngleMinLimit(device, channel);
  double max_limit = getAngleMaxLimit(device, channel);
  if (target < min_limit)
    return min_limit;
  else if (target > max_limit)
    return max_limit;
  return target;
} // clipAngleTargetValue(const uint8_t, const uint8_t, const double)

//
double AngleServoController::servoToAngle(const uint8_t channel, uint16_t pos) const
{
  return servoToAngle(0, channel, pos);
} // servoToAngle(const uint8_t, uint16_t)

//
double AngleServoController::servoToAngle(const uint8_t device, const uint8_t channel, uint16_t pos) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0.0;

  AngleServoPair min_limit = getAngleMinLimitPair(device, channel);
  AngleServoPair max_limit = getAngleMaxLimitPair(device, channel);
  double min_angle = min_limit.first;
  double max_angle = max_limit.first;
  uint16_t min_pos = min_limit.second;
  uint16_t max_pos = max_limit.second;

  return min_angle + (max_angle - min_angle) * double(pos - min_pos) / double(max_pos - min_pos);
} // servoToAngle(const uint8_t, const uint8_t, uint16_t)

//
uint16_t AngleServoController::angleToServo(const uint8_t channel, double angle) const
{
  return angleToServo(0, channel, angle);
} // angleToServo(const uint8_t, double)

//
uint16_t AngleServoController::angleToServo(const uint8_t device, const uint8_t channel, double angle) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;

  AngleServoPair min_limit = getAngleMinLimitPair(device, channel);
  AngleServoPair max_limit = getAngleMaxLimitPair(device, channel);
  double min_angle = min_limit.first;
  double max_angle = max_limit.first;
  uint16_t min_pos = min_limit.second;
  uint16_t max_pos = max_limit.second;

  return min_pos + (max_pos - min_pos) * (angle - min_angle) / (max_angle - min_angle);
} // angleToServo(const uint8_t, const uint8_t, double)

//
bool AngleServoController::setAngleLimits(const uint8_t channel, AngleLimits limits)
{
  return setAngleLimits(0, channel, limits);
} // setAngleLimits(const uint8_t, AngleLimits)

//
bool AngleServoController::setAngleLimits(const uint8_t device, const uint8_t channel, AngleLimits limits)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  angle_servos_[device][channel] = limits;
  return true;
} // setAngleLimits(const uint8_t, const uint8_t, AngleLimits)

//
bool AngleServoController::setAngleLimits(const uint8_t channel, AngleServoPair limit1, AngleServoPair limit2)
{
  return setAngleLimits(0, channel, limit1, limit2);
} // setAngleLimits(const uint8_t, ServoAngle, ServoAngle)

//
bool AngleServoController::setAngleLimits(const uint8_t device, const uint8_t channel, AngleServoPair limit1,
                                          AngleServoPair limit2)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  angle_servos_[device][channel] = AngleLimits(limit1, limit2);
  return true;
} // setAngleLimits(const uint8_t, const uint8_t, ServoAngle, ServoAngle)

// commands an individual servo motor to move to a target position
bool AngleServoController::setAngleTarget(const uint8_t channel, double target)
{
  if ((!isConnected()) || (channel >= getNumChannels(0)) || (target < getAngleMinLimit(channel)) || (target
      > getAngleMaxLimit(channel)))
    return false;
  return setServoTarget(channel, angleToServo(channel, target));
} // setAngleTarget(const uint8_t, double)

// commands an individual servo motor to move to a target position
bool AngleServoController::setAngleTarget(const uint8_t device, const uint8_t channel, double target)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)) || (target
      < getAngleMinLimit(device, channel)) || (target > getAngleMaxLimit(device, channel)))
    return false;
  return setServoTarget(device, channel, angleToServo(device, channel, target));
} // setAngleTarget(const uint8_t, const uint8_t, double)

// commands an individual servo motor to move at the parameterized speed
bool AngleServoController::setAngleSpeed(const uint8_t channel, double speed)
{
  if (!isConnected() || (channel >= getNumChannels(0)))
    return false;
  return setServoSpeed(channel, angleToServo(channel, speed));
} // setAngleSpeed(const uint8_t, double)

// commands an individual servo motor to move at the parameterized speed
bool AngleServoController::setAngleSpeed(const uint8_t device, const uint8_t channel, double speed)
{
  if (!isConnected() || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return setServoSpeed(device, channel, angleToServo(device, channel, speed));
} // setAngleSpeed(const uint8_t, const uint8_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool AngleServoController::setAngleAcceleration(const uint8_t channel, double accel)
{
  if (!isConnected() || (channel >= getNumChannels(0)))
    return false;
  return setServoAcceleration(channel, angleToServo(channel, accel));
} // setAngleAcceleration(const uint8_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool AngleServoController::setAngleAcceleration(const uint8_t device, const uint8_t channel, double accel)
{
  if (!isConnected() || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return setServoSpeed(device, channel, angleToServo(device, channel, accel));
} // setServoAcceleration(const uint8_t, const uint8_t, double)

//
AngleServoController::AngleLimits AngleServoController::getAngleLimits(const uint8_t channel) const
{
  return getAngleLimits(0, channel);
} // getAngleLimits(const uint8_t)

//
AngleServoController::AngleLimits AngleServoController::getAngleLimits(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return AngleLimits(AngleServoPair(0.0, 0), AngleServoPair(0.0, 0));
  return angle_servos_[device][channel];
} // getAngleLimits(const uint8_t, const uint8_t)

//
AngleServoController::AngleServoPair AngleServoController::getAngleMinLimitPair(const uint8_t channel) const
{
  return getAngleMinLimitPair(0, channel);
} // getAngleMinLimitPair(const uint8_t)

//
AngleServoController::AngleServoPair AngleServoController::getAngleMinLimitPair(const uint8_t device,
                                                                                const uint8_t channel) const
{
  AngleLimits limits = getAngleLimits(device, channel);
  return (limits.first.first < limits.second.first) ? limits.first : limits.second;
} // getAngleMinLimitPair(const uint8_t, const uint8_t)

//
AngleServoController::AngleServoPair AngleServoController::getAngleMaxLimitPair(const uint8_t channel) const
{
  return getAngleMaxLimitPair(0, channel);
} // getAngleMaxLimitPair(const uint8_t)

//
AngleServoController::AngleServoPair AngleServoController::getAngleMaxLimitPair(const uint8_t device,
                                                                                const uint8_t channel) const
{
  AngleLimits limits = getAngleLimits(device, channel);
  return (limits.first.first > limits.second.first) ? limits.first : limits.second;
} // getAngleMaxLimitPair(const uint8_t, const uint8_t)

//
double AngleServoController::getAngleMinLimit(const uint8_t channel) const
{
  return getAngleMinLimit(0, channel);
} // getAngleMinLimit(const uint8_t)

//
double AngleServoController::getAngleMinLimit(const uint8_t device, const uint8_t channel) const
{
  return getAngleMinLimitPair(device, channel).first;
} // getAngleMinLimit(const uint8_t, const uint8_t)

//
double AngleServoController::getAngleMaxLimit(const uint8_t channel) const
{
  return getAngleMinLimit(0, channel);
} // getAngleMaxLimit(const uint8_t)

//
double AngleServoController::getAngleMaxLimit(const uint8_t device, const uint8_t channel) const
{
  return getAngleMaxLimitPair(device, channel).first;
} // getAngleMaxLimit(const uint8_t, const uint8_t)

//
double AngleServoController::getAngleTarget(const uint8_t channel)
{
  return getAngleTarget(0, channel);
} // getAngleTarget(const uint8_t)

//
double AngleServoController::getAngleTarget(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoTarget(device, channel));
} // getAngleTarget(const uint8_t, const uint8_t)

//
double AngleServoController::getAngleSpeed(const uint8_t channel)
{
  return getAngleSpeed(0, channel);
} // getAngleSpeed(const uint8_t)

//
double AngleServoController::getAngleSpeed(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoSpeed(device, channel));
} // getAngleSpeed(const uint8_t, const uint8_t)

//
double AngleServoController::getAngleAcceleration(const uint8_t channel)
{
  return getAngleAcceleration(0, channel);
} // getAngleAcceleration(const uint8_t)

//
double AngleServoController::getAngleAcceleration(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoAcceleration(device, channel));
} // getAngleAcceleration(const uint8_t, const uint8_t)

//
double AngleServoController::getAnglePosition(const uint8_t channel)
{
  return servoToAngle(channel, getServoPosition(channel));
} // getAnglePosition(const uint8_t)

//
double AngleServoController::getAnglePosition(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoPosition(device, channel));
} // getAnglePosition(const uint8_t, const uint8_t)
