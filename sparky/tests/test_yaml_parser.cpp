// preprocessor directives
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sparky/servo_angle_controller.h>
#include <yaml-cpp/yaml.h>

typedef std::pair<double, double> JointAnglePair;
typedef std::pair<JointAnglePair, JointAnglePair> JointLimits;

void operator >>( const YAML::Node &node, JointLimits &joint_limits );

//
int main( int argc, char** argv )
{
    std::string yaml_doc_path = "params/params.yaml";

    if( argc <= 1 ) PRINT_DEBUG( "Using default path [ %s ] for yaml document.\n", yaml_doc_path.c_str() );
    else
    {
        yaml_doc_path = argv[1];
        PRINT_DEBUG( "Using path [ %s ] for yaml document.\n", yaml_doc_path.c_str() );
    }

    std::ifstream fin( yaml_doc_path.c_str() );
    YAML::Parser parser( fin );
    YAML::Node nodes;
    parser.GetNextDocument( nodes );

    PRINT_DEBUG( "Document has %zu nodes\n", nodes.size() );

    pololu::maestro::ServoAngleController servo_angle_controller( nodes["servo_controller"] );
    PRINT_INFO( "servo_controller:\n" );
    PRINT_INFO( "  path: %s\n", servo_angle_controller.getPath().c_str() );
    PRINT_INFO( "  n_devices: %d\n", servo_angle_controller.getNumDevices() );
    PRINT_INFO( "  n_channels: %d\n", servo_angle_controller.getNumChannels( 0 ) );
    PRINT_INFO( "---\n" );
    //servo_angle_controller.connect();

    parser.GetNextDocument( nodes );
    nodes >> servo_angle_controller;

    for ( int device = 0, n_devices = servo_angle_controller.getNumDevices(); device < n_devices; ++device )
    {
        for ( int channel = 0, n_channels = servo_angle_controller.getNumChannels( device ); channel < n_channels; ++channel )
        {
            if ( servo_angle_controller.getServoEnabled( device, channel ) )
            {
                PRINT_INFO( "- servo:\n" );
                PRINT_INFO( "    device: %d\n", device );
                PRINT_INFO( "    channel: %d\n", channel );
                PRINT_INFO( "    limits: [%d, %d]\n",
                            servo_angle_controller.getServoMinLimit( device, channel ),
                            servo_angle_controller.getServoMaxLimit( device, channel ) );
                PRINT_INFO( "    angle_limits: [[%d, %.1f], [%d, %.1f]]\n",
                            servo_angle_controller.getServoAngleLimits( device, channel ).first.first,
                            servo_angle_controller.getServoAngleLimits( device, channel ).first.second,
                            servo_angle_controller.getServoAngleLimits( device, channel ).second.first,
                            servo_angle_controller.getServoAngleLimits( device, channel ).second.second );
                //servo_angle_controller.setServoAngleTarget(device, channel,
                //  ( servo_angle_controller.getServoAngleMinLimit( device, channel ) + servo_angle_controller.getServoAngleMaxLimit( device, channel ) ) / 2.0 );
            }
        }
    }

    //sleep(2);
    //servo_angle_controller.disconnect();

    return 0;
} // main(int, char**)
/*
 //
 int main( int argc, char** argv )
 {
 std::string name;
 int device;
 int channel;
 pololu::maestro::ServoLimits servo_limits;
 pololu::maestro::ServoAngleLimits angle_limits;
 std::pair<std::pair<double, double>, std::pair<double, double> > joint_limits;
 double home;
 double servo_radius;
 double joint_radius;

 std::ifstream fin( "params/params.yaml" );
 YAML::Parser parser( fin );
 YAML::Node nodes;
 parser.GetNextDocument( nodes );

 for ( unsigned int i = 0, n = nodes.size(); i < n; ++i )
 {
 nodes[i]["name"] >> name;

 nodes[i]["servo"]["device"] >> device;
 nodes[i]["servo"]["channel"] >> channel;
 nodes[i]["servo"]["limits"] >> servo_limits;
 //nodes[i]["servo"]["servo_limits"] >> servo_limits;
 nodes[i]["servo"]["angle_limits"] >> angle_limits;

 nodes[i]["joint"]["joint_servo_mappings"] >> joint_limits;
 nodes[i]["joint"]["home"] >> home;
 nodes[i]["joint"]["servo_radius"] >> servo_radius;
 nodes[i]["joint"]["joint_radius"] >> joint_radius;

 printf( "- name: %s\n", name.c_str() );
 printf( "  servo:\n" );
 printf( "    device: %d\n", device );
 printf( "    channel: %d\n", channel );
 printf( "    servo_limits: [%d, %d]\n", servo_limits.first,
 servo_limits.second );
 printf( "    angle_limits: [[%d, %.1f], [%d, %.1f]]\n",
 angle_limits.first.first, angle_limits.first.second,
 angle_limits.second.first, angle_limits.second.second );
 printf( "  joint:\n" );
 printf( "    joint_servo_mappings: [[%.1f, %.1f], [%.1f, %.1f]]\n",
 joint_limits.first.first, joint_limits.first.second,
 joint_limits.second.first, joint_limits.second.second );
 printf( "    home: %.1f\n", home );
 printf( "    servo_radius: %.1f\n", servo_radius );
 printf( "    joint_radius: %.1f\n", joint_radius );
 }

 return 0;
 } // main(int, char**)
 */
void operator >>( const YAML::Node &node, JointLimits &joint_limits )
{
    node[0][0] >> joint_limits.first.first;
    node[0][1] >> joint_limits.first.second;
    node[1][0] >> joint_limits.second.first;
    node[1][1] >> joint_limits.second.second;
} // >>(const YAML::Node &, JointLimits &)

