// preprocessor directives
#include <sparky/sparky_controller.h>
using namespace sparky;

//
SparkyController::
SparkyController(const std::string device_path, const bool connect)
	: servo_controller_(device_path, connect)
{
	if (connect) SparkyController::connect();
}	// SparkyController()



//
SparkyController::
~SparkyController()
{
	if (isConnected()) disconnect();
}	// ~SparkyController()



//
bool SparkyController::
connect(const bool home)
{
	if (servo_controller_.connect() && home)
		servo_controller_.setServoPositionHome(0);  // IMPLEMENT FOR ALL!!!
	return servo_controller_.isConnected();
}	// connect()



//
bool SparkyController::
disconnect()
{
	return servo_controller_.disconnect();
}	// disconnect()



//
bool SparkyController::
isConnected()
{
	return servo_controller_.isConnected();
}	// isConnected()



//
bool SparkyController::
setJointAngle(std::string name, double angle)
{
	return false;
}	// setJointAngle(std::string, double)



//
bool SparkyController::
setJointMinAngle(std::string name, double angle)
{
	return false;
}	// setJointMinAngle(std::string, double)



//
bool SparkyController::
setJointMaxAngle(std::string name, double angle)
{
	return false;
}	// setJointMaxAngle(std::string, double)



//
double SparkyController::
getJointAngle(std::string name) const
{
	return 0.0l;
}	// getJointAngle(std::string)



//
double SparkyController::
getJointMinAngle(std::string name) const
{
	return 0.0l;
}	// getJointMinAngle(std::string)



//
double SparkyController::
getJointMaxAngle(std::string name) const
{
	return 0.0l;
}	// getJointMaxAngle(std::string)



//
size_t SparkyController::
getNumJoints() const
{
	return size_t(0);
}	// getNumJoints()
