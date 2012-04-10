// preprocessor directives
#include <assert.h>
#include <sparky/servo_angle_controller.h>
using namespace pololu::maestro;

//
ServoAngleController::ServoAngleController(const uint8_t n_channels, const std::string path, const bool connect) :
  ServoController(n_channels, path, connect)

{
  servos_servo_angle_limits_.resize(1);
  servos_servo_angle_limits_[0].resize(n_channels);
} // ServoAngleController(const uint8_t, const std::string, const bool)

//
ServoAngleController::ServoAngleController(const uint8_t n_devices, const uint8_t n_channels_each,
                                           const std::string path, const bool connect) :
  ServoController(n_devices, n_channels_each, path, connect)
{
  assert((n_devices > 0) && (n_channels_each > 0));
  servos_servo_angle_limits_.resize(n_devices);
  for (int i = 0; i < n_devices; ++i)
    servos_servo_angle_limits_[i].resize(n_channels_each);
} // ServoAngleController(const uint8_t, const uint8_t, const std::string, const bool)

//
ServoAngleController::ServoAngleController(const std::vector<uint8_t> n_device_channels, const std::string path,
                                           const bool connect) :
  ServoController(n_device_channels, path, connect)
{
  int n_devices = n_device_channels.size();
  assert(n_devices > 0);
  servos_servo_angle_limits_.resize(n_devices);
  for (int i = 0; i < n_devices; ++i)
  {
    assert(n_device_channels[i] > 0);
    servos_servo_angle_limits_[i].resize(n_device_channels[i]);
  }
} // ServoAngleController(const std::vector<uint8_t>, const std::string, const bool)

// shuts down the servo controller, disconnecting if necessary
ServoAngleController::~ServoAngleController()
{
  if (isConnected())
    disconnect();
} // ~ServoAngleController()

//
bool ServoAngleController::isValidAngleTarget(const uint8_t channel, const double target) const
{
  return isValidAngleTarget(0, channel, target);
} // isValidAngleTarget(const uint8_t, const double)

//
bool ServoAngleController::isValidAngleTarget(const uint8_t device, const uint8_t channel, const double target) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return (target >= getAngleMinLimit(device, channel)) && (target <= getAngleMaxLimit(device, channel));
} // isValidAngleTarget(const uint8_t, const uint8_t, const double)

//
double ServoAngleController::clipAngleTargetValue(const uint8_t channel, const double target) const
{
  return clipAngleTargetValue(0, channel, target);
} // clipAngleTargetValue(const uint8_t, const double)

//
double ServoAngleController::clipAngleTargetValue(const uint8_t device, const uint8_t channel, const double target) const
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
double ServoAngleController::servoToAngle(const uint8_t channel, uint16_t pos) const
{
  return servoToAngle(0, channel, pos);
} // servoToAngle(const uint8_t, uint16_t)

//
double ServoAngleController::servoToAngle(const uint8_t device, const uint8_t channel, uint16_t pos) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0.0;

  ServoAnglePair min_limit = getServoAngleMinLimitPair(device, channel);
  ServoAnglePair max_limit = getServoAngleMaxLimitPair(device, channel);
  uint16_t min_pos = min_limit.first;
  uint16_t max_pos = max_limit.first;
  double min_angle = min_limit.second;
  double max_angle = max_limit.second;

  return min_angle + (max_angle - min_angle) * double(pos - min_pos) / double(max_pos - min_pos);
} // servoToAngle(const uint8_t, const uint8_t, uint16_t)

//
uint16_t ServoAngleController::angleToServo(const uint8_t channel, double angle) const
{
  return angleToServo(0, channel, angle);
} // angleToServo(const uint8_t, double)

//
uint16_t ServoAngleController::angleToServo(const uint8_t device, const uint8_t channel, double angle) const
{
  if ((device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;

  ServoAnglePair min_limit = getServoAngleMinLimitPair(device, channel);
  ServoAnglePair max_limit = getServoAngleMaxLimitPair(device, channel);
  uint16_t min_pos = min_limit.first;
  uint16_t max_pos = max_limit.first;
  double min_angle = min_limit.second;
  double max_angle = max_limit.second;

  return min_pos + (max_pos - min_pos) * (angle - min_angle) / (max_angle - min_angle);
} // angleToServo(const uint8_t, const uint8_t, double)

//
bool ServoAngleController::setServoAngleLimits(const uint8_t channel, ServoAngleLimits limits)
{
  return setServoAngleLimits(0, channel, limits);
} // setServoAngleLimits(const uint8_t, ServoAngleLimits)

//
bool ServoAngleController::setServoAngleLimits(const uint8_t device, const uint8_t channel, ServoAngleLimits limits)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  servos_servo_angle_limits_[device][channel] = limits;
  return true;
} // setServoAngleLimits(const uint8_t, const uint8_t, ServoAngleLimits)

//
bool ServoAngleController::setServoAngleLimits(const uint8_t channel, ServoAnglePair limit1, ServoAnglePair limit2)
{
  return setServoAngleLimits(0, channel, limit1, limit2);
} // setServoAngleLimits(const uint8_t, ServoAngle, ServoAngle)

//
bool ServoAngleController::setServoAngleLimits(const uint8_t device, const uint8_t channel, ServoAnglePair limit1,
                                               ServoAnglePair limit2)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  servos_servo_angle_limits_[device][channel] = ServoAngleLimits(limit1, limit2);
  return true;
} // setServoAngleLimits(const uint8_t, const uint8_t, ServoAngle, ServoAngle)

// commands an individual servo motor to move to a target position
bool ServoAngleController::setAngleTarget(const uint8_t channel, double target)
{
  if ((!isConnected()) || (channel >= getNumChannels(0)) || (target < getAngleMinLimit(channel)) || (target
      > getAngleMaxLimit(channel)))
    return false;
  return setServoTarget(channel, angleToServo(channel, target));
} // setAngleTarget(const uint8_t, double)

// commands an individual servo motor to move to a target position
bool ServoAngleController::setAngleTarget(const uint8_t device, const uint8_t channel, double target)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)) || (target
      < getAngleMinLimit(device, channel)) || (target > getAngleMaxLimit(device, channel)))
    return false;
  return setServoTarget(device, channel, angleToServo(device, channel, target));
} // setAngleTarget(const uint8_t, const uint8_t, double)

// commands an individual servo motor to move at the parameterized speed
bool ServoAngleController::setAngleSpeed(const uint8_t channel, double speed)
{
  if (!isConnected() || (channel >= getNumChannels(0)))
    return false;
  return setServoSpeed(channel, angleToServo(channel, speed));
} // setAngleSpeed(const uint8_t, double)

// commands an individual servo motor to move at the parameterized speed
bool ServoAngleController::setAngleSpeed(const uint8_t device, const uint8_t channel, double speed)
{
  if (!isConnected() || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return setServoSpeed(device, channel, angleToServo(device, channel, speed));
} // setAngleSpeed(const uint8_t, const uint8_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool ServoAngleController::setAngleAcceleration(const uint8_t channel, double accel)
{
  if (!isConnected() || (channel >= getNumChannels(0)))
    return false;
  return setServoAcceleration(channel, angleToServo(channel, accel));
} // setAngleAcceleration(const uint8_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool ServoAngleController::setAngleAcceleration(const uint8_t device, const uint8_t channel, double accel)
{
  if (!isConnected() || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return setServoAcceleration(device, channel, angleToServo(device, channel, accel));
} // setServoAcceleration(const uint8_t, const uint8_t, double)

//
ServoAngleLimits ServoAngleController::getServoAngleLimits(const uint8_t channel) const
{
  return getServoAngleLimits(0, channel);
} // getServoAngleLimits(const uint8_t)

//
ServoAngleLimits ServoAngleController::getServoAngleLimits(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return ServoAngleLimits(ServoAnglePair(0, 0.0), ServoAnglePair(0, 0.0));
  return servos_servo_angle_limits_[device][channel];
} // getServoAngleLimits(const uint8_t, const uint8_t)

//
ServoAnglePair ServoAngleController::getServoAngleMinLimitPair(const uint8_t channel) const
{
  return getServoAngleMinLimitPair(0, channel);
} // getServoAngleMinLimitPair(const uint8_t)

//
ServoAnglePair ServoAngleController::getServoAngleMinLimitPair(const uint8_t device, const uint8_t channel) const
{
  ServoAngleLimits limits = getServoAngleLimits(device, channel);
  return (limits.first.second < limits.second.second) ? limits.first : limits.second;
} // getServoAngleMinLimitPair(const uint8_t, const uint8_t)

//
ServoAnglePair ServoAngleController::getServoAngleMaxLimitPair(const uint8_t channel) const
{
  return getServoAngleMaxLimitPair(0, channel);
} // getServoAngleMaxLimitPair(const uint8_t)

//
ServoAnglePair ServoAngleController::getServoAngleMaxLimitPair(const uint8_t device, const uint8_t channel) const
{
  ServoAngleLimits limits = getServoAngleLimits(device, channel);
  return (limits.first.second > limits.second.second) ? limits.first : limits.second;
} // getServoAngleMaxLimitPair(const uint8_t, const uint8_t)

//
double ServoAngleController::getAngleMinLimit(const uint8_t channel) const
{
  return getAngleMinLimit(0, channel);
} // getAngleMinLimit(const uint8_t)

//
double ServoAngleController::getAngleMinLimit(const uint8_t device, const uint8_t channel) const
{
  return getServoAngleMinLimitPair(device, channel).second;
} // getAngleMinLimit(const uint8_t, const uint8_t)

//
double ServoAngleController::getAngleMaxLimit(const uint8_t channel) const
{
  return getAngleMinLimit(0, channel);
} // getAngleMaxLimit(const uint8_t)

//
double ServoAngleController::getAngleMaxLimit(const uint8_t device, const uint8_t channel) const
{
  return getServoAngleMaxLimitPair(device, channel).second;
} // getAngleMaxLimit(const uint8_t, const uint8_t)

//
double ServoAngleController::getAngleTarget(const uint8_t channel)
{
  return getAngleTarget(0, channel);
} // getAngleTarget(const uint8_t)

//
double ServoAngleController::getAngleTarget(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoTarget(device, channel));
} // getAngleTarget(const uint8_t, const uint8_t)

//
double ServoAngleController::getAngleSpeed(const uint8_t channel)
{
  return getAngleSpeed(0, channel);
} // getAngleSpeed(const uint8_t)

//
double ServoAngleController::getAngleSpeed(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoSpeed(device, channel));
} // getAngleSpeed(const uint8_t, const uint8_t)

//
double ServoAngleController::getAngleAcceleration(const uint8_t channel)
{
  return getAngleAcceleration(0, channel);
} // getAngleAcceleration(const uint8_t)

//
double ServoAngleController::getAngleAcceleration(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoAcceleration(device, channel));
} // getAngleAcceleration(const uint8_t, const uint8_t)

//
double ServoAngleController::getAnglePosition(const uint8_t channel)
{
  return servoToAngle(channel, getServoPosition(channel));
} // getAnglePosition(const uint8_t)

//
double ServoAngleController::getAnglePosition(const uint8_t device, const uint8_t channel)
{
  return servoToAngle(device, channel, getServoPosition(device, channel));
} // getAnglePosition(const uint8_t, const uint8_t)
