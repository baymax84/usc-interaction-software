// preprocessor directives
#include <fstream>
#include <iostream>
#include <sparky/angle_servo_controller.h>
#include <yaml-cpp/yaml.h>

typedef std::pair<double, double> JointAnglePair;
typedef std::pair<JointAnglePair, JointAnglePair> JointLimits;

void operator >>(const YAML::Node &node, pololu::MaestroServoController::ServoLimits &servo_limits);
void operator >>(const YAML::Node &node, pololu::AngleServoController::AngleLimits &angle_limits);
void operator >>(const YAML::Node &node, JointLimits &joint_limits);

//
int main(int argc, char** argv)
{
  std::string name;
  int device;
  int channel;
  pololu::MaestroServoController::ServoLimits servo_limits;
  pololu::AngleServoController::AngleLimits angle_limits;
  std::pair< std::pair<double, double>, std::pair<double, double> > joint_limits;
  double home;
  double servo_radius;
  double joint_radius;
/*
  std::ifstream fin("params/params.yaml");
  YAML::Parser parser(fin);
  YAML::Node nodes;
  parser.GetNextDocument(nodes);
  for (unsigned int i = 0, n = nodes.size(); i < n; ++i)
  {
	  nodes[i]["name"] >> name;

	  nodes[i]["servo"]["device"] >> device;
	  nodes[i]["servo"]["channel"] >> channel;
	  nodes[i]["servo"]["servo_limits"] >> servo_limits;
	  nodes[i]["servo"]["angle_limits"] >> angle_limits;

	  nodes[i]["joint"]["joint_servo_mappings"] >> joint_limits;
	  nodes[i]["joint"]["home"] >> home;
	  nodes[i]["joint"]["servo_radius"] >> servo_radius;
	  nodes[i]["joint"]["joint_radius"] >> joint_radius;

	  printf("- name: %s\n", name.c_str());
	  printf("  servo:\n");
	  printf("    device: %d\n", device);
	  printf("    channel: %d\n", channel);
	  printf("    servo_limits: [%d, %d]\n", servo_limits.first, servo_limits.second);
	  printf("    angle_limits: [[%.1f, %d], [%.1f, %d]]\n",
			 angle_limits.first.first, angle_limits.first.second,
			 angle_limits.second.first, angle_limits.second.second);
	  printf("  joint:\n");
	  printf("    joint_servo_mappings: [[%.1f, %.1f], [%.1f, %.1f]]\n",
			 joint_limits.first.first, joint_limits.first.second,
			 joint_limits.second.first, joint_limits.second.second);
	  printf("    home: %.1f\n", home);
	  printf("    servo_radius: %.1f\n", servo_radius);
	  printf("    joint_radius: %.1f\n", joint_radius);
  }
*/
  return 0;
} // main(int, char**)

void operator >>(const YAML::Node &node, pololu::MaestroServoController::ServoLimits &servo_limits)
{
  std::pair<int, int> temp;
  node[0] >> temp.first;
  node[1] >> temp.second;
  servo_limits.first = uint16_t(temp.first);
  servo_limits.second = uint16_t(temp.second);
} // >>(const YAML::Node &, pololu::MaestroServoController::ServoLimits &)

void operator >>(const YAML::Node &node, pololu::AngleServoController::AngleLimits &angle_limits)
{
  std::pair< std::pair<double, int>, std::pair<double, int> > temp;
  node[0][0] >> temp.first.first;
  node[0][1] >> temp.first.second;
  node[1][0] >> temp.second.first;
  node[1][1] >> temp.second.second;
  angle_limits.first.first = temp.first.first;
  angle_limits.first.second = uint16_t(temp.first.second);
  angle_limits.second.first = temp.second.first;
  angle_limits.second.second = uint16_t(temp.second.second);
} // >>(const YAML::Node &, pololu::AngleServoController::AngleLimits &)

void operator >>(const YAML::Node &node, JointLimits &joint_limits)
{
  node[0][0] >> joint_limits.first.first;
  node[0][1] >> joint_limits.first.second;
  node[1][0] >> joint_limits.second.first;
  node[1][1] >> joint_limits.second.second;
} // >>(const YAML::Node &, JointLimits &)
