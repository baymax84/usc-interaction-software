// preprocessor directives
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sparky/joint_angle_controller.h>
#include <yaml-cpp/yaml.h>

using sparky::JointAnglePair;
using sparky::JointAngleLimits;

//
int main( int argc, char** argv )
{
    std::string yaml_doc_path = "params/params_prototype1.yaml";

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

    sparky::JointAngleController joint_angle_controller( nodes );
    PRINT_INFO( "servo_controller:\n" );
    PRINT_INFO( "  path: %s\n", joint_angle_controller.getPath().c_str() );
    PRINT_INFO( "  n_devices: %d\n", joint_angle_controller.getNumDevices() );
    PRINT_INFO( "  n_channels: %d\n", joint_angle_controller.getNumChannels( 0 ) );
    PRINT_INFO( "---\n" );
    //joint_angle_controller.connect();

    for ( int device = 0, n_devices = joint_angle_controller.getNumDevices(); device < n_devices; ++device )
    {
        for ( int channel = 0, n_channels = joint_angle_controller.getNumChannels( device ); channel < n_channels; ++channel )
        {
            if ( joint_angle_controller.getServoEnabled( device, channel ) )
            {
                PRINT_INFO( "- servo:\n" );
                PRINT_INFO( "    device: %d\n", device );
                PRINT_INFO( "    channel: %d\n", channel );
                PRINT_INFO( "    limits: [%d, %d]\n",
                            joint_angle_controller.getServoMinLimit( device, channel ),
                            joint_angle_controller.getServoMaxLimit( device, channel ) );
                PRINT_INFO( "    angle_limits: [[%d, %.1f], [%d, %.1f]]\n",
                            joint_angle_controller.getServoAngleLimits( device, channel ).first.first,
                            joint_angle_controller.getServoAngleLimits( device, channel ).first.second,
                            joint_angle_controller.getServoAngleLimits( device, channel ).second.first,
                            joint_angle_controller.getServoAngleLimits( device, channel ).second.second );
            }
        }
    }

    auto const & joints_map = joint_angle_controller.getJoints();
    for( auto joint_it = joints_map.cbegin(); joint_it != joints_map.cend(); ++joint_it )
    {
        auto const & joint_name = joint_it->first;
        auto const & joint = joint_it->second;

        PRINT_INFO( "- joint:\n" );
        PRINT_INFO( "    name: %s\n", joint.name_.c_str() );
        PRINT_INFO( "    servo_name: %s\n", joint.servo_name_.c_str() );

        PRINT_INFO( "    joint_limits: [[%.1f, %.1f], [%.1f, %.1f]]\n",
                    joint_angle_controller.getJointAngleLimits( joint_name ).first.first,
                    joint_angle_controller.getJointAngleLimits( joint_name ).first.second,
                    joint_angle_controller.getJointAngleLimits( joint_name ).second.first,
                    joint_angle_controller.getJointAngleLimits( joint_name ).second.second );
        PRINT_INFO( "    joint angle at min/max servo angle: [ %f ] -> [ %f ]/[ %f ] -> [ %f ]\n",
                    joint_angle_controller.getJointAngleLimits( joint_name ).first.second,
                    joint_angle_controller.convertServoAngleToJointAngle( joint_name, joint_angle_controller.getJointAngleLimits( joint_name ).first.second ),
                    joint_angle_controller.getJointAngleLimits( joint_name ).second.second,
                    joint_angle_controller.convertServoAngleToJointAngle( joint_name, joint_angle_controller.getJointAngleLimits( joint_name ).second.second ) );
        auto const & actuator_parameters = joint.parameters_;
        PRINT_INFO( "    servo angle at joint home [ %f ] ( %f ) : [ %f ]\n", joint.home_, sparky::JointAngleController::jointAngleToCableDisplacement( joint.home_ * M_PI / 180, actuator_parameters.find("joint_radius")->second, actuator_parameters.find("joint_length")->second ), joint_angle_controller.convertJointAngleToServoAngle( joint_name, joint.home_ ) );
        //joint_angle_controller.setServoAngleTarget(device, channel,
        //  ( joint_angle_controller.getServoAngleMinLimit( device, channel ) + joint_angle_controller.getServoAngleMaxLimit( device, channel ) ) / 2.0 );
    }

    //sleep(2);
    //joint_angle_controller.disconnect();

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
