// preprocessor directives
#include <sparky/sparky_controller.h>
using namespace sparky;

//
SparkyController::SparkyController(const std::string device_path, const bool connect, const bool home) :
  servo_controller_(N_DEVICES, N_CHANNELS_EACH, device_path)
{
  if (connect)
    SparkyController::connect(home);
} // SparkyController(const std::string, const bool, const bool)

//
SparkyController::~SparkyController()
{
  if (isConnected())
    disconnect();
} // ~SparkyController()

//
bool SparkyController::connect(const bool home)
{
  if (servo_controller_.connect() && home)
  {
    for (int i = 0, n = servo_controller_.getNumDevices(); i < n; ++i)
      servo_controller_.setServosHome(i);
  }
  return servo_controller_.isConnected();
} // connect()

//
bool SparkyController::disconnect()
{
  return servo_controller_.disconnect();
} // disconnect()

//
bool SparkyController::isConnected()
{
  return servo_controller_.isConnected();
} // isConnected()

//
bool SparkyController::isJointMoving(const std::string name)
{
  return false;
} // isJointMoving(const std::string)

//
bool SparkyController::waitForJointDone(const std::string name) const
{
  return false;
} // waitForJointDone(const std::string)

//
bool SparkyController::waitForJointsDone()
{
  return false;
} // waitForJointsDone(const std::string)

//
bool SparkyController::setPath(const std::string name)
{
  return servo_controller_.setPath(name);
} // setPath(const std::string)
/*
//
bool SparkyController::setJointAngleLimits(const std::string name, const JointLimits limits)
{
  return false;
} // setJointAngleLimits(const std::string, const JointLimits)
*/
//
bool SparkyController::setJointAngleLimits(const std::string name, const double angle1, const double angle2)
{
  return false;
} // setJointAngleLimits(const std::string, const double, const double)

//
bool SparkyController::setJointMinAngle(const std::string name, const double angle)
{
  return false;
} // setJointMinAngle(const std::string, const double)

//
bool SparkyController::setJointMaxAngle(const std::string name, const double angle)
{
  return false;
} // setJointMaxAngle(const std::string, const double)

//
bool SparkyController::setJointActive(const std::string name, const bool active)
{
  return false;
} // setJointActive(const std::string, const bool)

//
bool SparkyController::setJointAngle(const std::string name, double angle)
{
  return false;
} // setJointAngle(const std::string name, double)

//
bool SparkyController::setJointSpeed(const std::string name, double speed)
{
  return false;
} // setJointSpeed(const std::string, double)

//
bool SparkyController::setJointAcceleration(const std::string name, double accel)
{
  return false;
} // setJointAcceleration(const std::string, double)

//
bool SparkyController::setJointsHome()
{
  return false;
} // setJointsHome()

//
std::string SparkyController::getPath() const
{
  return servo_controller_.getPath();
} // getPath()

//
size_t SparkyController::getNumJoints() const
{
  return 0;
} // getNumJoints()

//
size_t SparkyController::getNumJointsActive() const
{
  return 0;
} // getNumJointsActive()

//
size_t SparkyController::getNumJointsInactive() const
{
  return 0;
} // getNumJointsInactive()
/*
//
JointLimits SparkyController::getJointLimits(const std::string name) const
{
  return JointLimits(0.0, 0.0);
} // getJointLimits(const std::string)
*/
//
double SparkyController::getJointMinAngle(const std::string name) const
{
  return 0.0;
} // getJointMinAngle(const std::string)

//
double SparkyController::getJointMaxAngle(const std::string name) const
{
  return 0.0;
} // getJointMaxAngle(const std::string)

//
bool SparkyController::getJointActive(const std::string name) const
{
  return false;
} // getJointActive(const std::string)

//
double SparkyController::getJointAngle(const std::string name) const
{
  return 0.0;
} // getJointAngle(const std::string)

//
double SparkyController::getJointSpeed(const std::string name) const
{
  return 0.0;
} // getJointSpeed(const std::string)

//
double SparkyController::getJointAccel(const std::string name) const
{
  return 0.0;
} // getJointAccel(const std::string)

//
bool SparkyController::getJointMoving(const std::string name)
{
  return false;
} // getJointMoving(const std::string)

//
bool SparkyController::getJointsMoving()
{
  return false;
} // getJointsMoving()

//
uint16_t SparkyController::getErrors()
{
  return 0;
} // getErrors()
