// preprocessor directives
#include <assert.h>
#include <sparky/joint_angle_controller.h>
using namespace sparky;

Joint::Joint()
{
    //
}

Joint::Joint( std::string const & name, JointAngleLimits const & limits, double const & home )
:
    name_( name ), limits_( limits ), home_( home )
{
    //
}

// ############################################################################################################################################

JointAngleController::JointAngleController()
{
    //
}

//
JointAngleController::JointAngleController( size_t const num_joints, const std::string path, const bool connect ) :
    ServoAngleController( num_joints, path, connect )
{
    //
} // JointAngleController(const uint16_t, const std::string, const bool)

//
JointAngleController::JointAngleController( const std::string filename, const std::string path, const bool connect ) :
    ServoAngleController( filename, path, connect )
{
    //
} // JointAngleController(const std::string, const std::string, const bool)

//
JointAngleController::JointAngleController( std::ifstream &fin, const std::string path, const bool connect ) :
    ServoAngleController( fin, path, connect )
{
    //
} // JointAngleController(std::fstream &, const std::string, const bool)

//
JointAngleController::JointAngleController( YAML::Parser &parser, const std::string path, const bool connect ) :
    ServoAngleController( parser, path, connect )
{
    //
} // JointAngleController(YAML::Parser &, const std::string, const bool)

//
JointAngleController::JointAngleController( const YAML::Node &node )
{
    initFromYaml( node );
} // JointAngleController(const YAML::Node &, const std::string, const bool)

//
JointAngleController::JointAngleController( const JointAngleController &joint_angle_controller ) :
    ServoAngleController( joint_angle_controller )
{
    *this = joint_angle_controller;
} // JointAngleController(const JointAngleController &)

// shuts down the servo controller, disconnecting if necessary
JointAngleController::~JointAngleController()
{
    if ( isConnected() ) disconnect();
} // ~JointAngleController()

double JointAngleController::jointAngleToCableDisplacement( double const & joint_angle, double const & joint_radius, double const & joint_length )
{
    PRINT_INFO( "joint angle ( %f ) -> cable displacement\n", joint_angle );
    return sqrt( pow( joint_length, 2 ) + pow( joint_radius, 2 ) - 2 * joint_length * joint_radius * cos( joint_angle ) ) - sqrt( pow( joint_length, 2 ) + pow( joint_radius, 2 ) );
}

double JointAngleController::cableDisplacementToServoAngle( double const & cable_displacement, double const & servo_radius, double const & piston_length )
{
    PRINT_INFO( "cable displacement ( %f ) -> servo angle\n", cable_displacement );
    return acos( ( pow( ( sqrt( pow( piston_length, 2 ) - pow( servo_radius, 2 ) ) + cable_displacement ), 2 ) + pow( servo_radius, 2 ) - pow( piston_length, 2 ) ) / ( 2 * ( sqrt( pow( piston_length, 2 ) - pow( servo_radius, 2 ) ) + cable_displacement ) * servo_radius ) );
}

double JointAngleController::servoAngleToCableDisplacement( double const & servo_angle, double const & servo_radius, double const & piston_length )
{
    PRINT_INFO( "servo angle ( %f ) -> cable displacement\n", servo_angle );
    return ( sqrt( pow( piston_length, 2 ) - pow( ( servo_radius * cos( M_PI_2 - servo_angle ) ), 2 ) ) + servo_radius * sin( M_PI_2 - servo_angle ) ) - sqrt( pow( piston_length, 2 ) - pow( servo_radius, 2 ) );
}

double JointAngleController::cableDisplacementToJointAngle( double const & cable_displacement, double const & joint_radius, double const & joint_length )
{
    PRINT_INFO( "cable displacement ( %f ) -> joint angle\n", cable_displacement );
    return acos( ( pow( joint_length, 2 ) + pow( joint_radius, 2 ) - pow( ( sqrt( pow( joint_length, 2 ) + pow( joint_radius, 2 ) ) + cable_displacement ), 2 ) ) / ( 2 * joint_length * joint_radius ) );
}

//
bool JointAngleController::isValidJointAngleTarget( std::string const & joint_name, const double target ) const
{
    return ( target >= getJointAngleMinLimit( joint_name ) ) && ( target <= getJointAngleMaxLimit( joint_name ) );
} // isValidJointAngleTarget(const uint16_t, const uint16_t, const double)

//
double JointAngleController::clipJointAngleTargetValue( std::string const & joint_name, const double target ) const
{
    if ( !this->servo_names_map_.count( joint_name ) )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return 0.0;
    }

    double min_limit = getJointAngleMinLimit( joint_name );
    double max_limit = getJointAngleMaxLimit( joint_name );
    if ( target < min_limit ) return min_limit;
    else if ( target > max_limit ) return max_limit;
    return target;
} // clipJointAngleTargetValue(const uint16_t, const uint16_t, const double)

//
double JointAngleController::convertServoAngleToJointAngle( std::string const & joint_name, double servo_angle )
{
    PRINT_INFO( "servo angle %s ( %f ) -> joint angle\n", joint_name.c_str(), servo_angle );
    auto const & joint_it = joints_.find( joint_name );
    if( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return 0.0;
    }

    JointAnglePair const min_limit = getJointAngleMinLimitPair( joint_name );
    JointAnglePair const max_limit = getJointAngleMaxLimitPair( joint_name );
    double const min_joint_angle = min_limit.first;
    double const max_joint_angle = max_limit.first;
    double const min_servo_angle = min_limit.second;
    double const max_servo_angle = max_limit.second;

    PRINT_INFO( "max servo angle: %f\n", max_servo_angle );
    PRINT_INFO( "max joint angle: %f\n", max_joint_angle );

    auto const & actuator_parameters = joint_it->second.parameters_;
    double const max_linkage_angle = max_joint_angle + actuator_parameters.find("joint_offset")->second;
    double const direction = actuator_parameters.find("direction")->second;

    // cable displacement from max servo angle
    double const servo_cable_displacement = servoAngleToCableDisplacement( max_servo_angle * M_PI / 180.0, actuator_parameters.find("servo_radius")->second, actuator_parameters.find("piston_length")->second );
    // cable displacement from max joint angle
    double const joint_cable_displacement = jointAngleToCableDisplacement( ( 180.0 - max_linkage_angle ) * M_PI / 180.0, actuator_parameters.find("joint_radius")->second, actuator_parameters.find("joint_length")->second );
    // the cable offset is the difference between the joint cable displacement and the servo cable displacement; specifically, both
    // displacements should be 0 when both components are at 90 degrees
    double const cable_offset = servo_cable_displacement - joint_cable_displacement;
    double const cable_displacement = servoAngleToCableDisplacement( servo_angle * M_PI / 180.0, actuator_parameters.find("servo_radius")->second, actuator_parameters.find("piston_length")->second );
    double const joint_angle = cableDisplacementToJointAngle( cable_displacement + cable_offset, actuator_parameters.find("joint_radius")->second, actuator_parameters.find("joint_length")->second ) * 180.0 / M_PI;
    double const linkage_angle = joint_angle + actuator_parameters.find("joint_offset")->second;

    PRINT_INFO( "Cable offset is %f m\n", cable_offset );
    PRINT_INFO( "Cable displacement is %f m\n", cable_displacement );
    PRINT_INFO( "Total cable displacement is %f m\n", cable_displacement + cable_offset );
    PRINT_INFO( "Joint angle is %f ( %f )\n", joint_angle, 180.0 - joint_angle );
    PRINT_INFO( "---\n" );

    // normalize joint angle
    return 180.0 - linkage_angle;

//    return min_angle + ( max_angle - min_angle ) * double ( pos - min_pos ) / double ( max_pos - min_pos );
} // convertServoAngleToJointAngle(const uint16_t, const uint16_t, uint16_t)

//
double JointAngleController::convertJointAngleToServoAngle( std::string const & joint_name, double joint_angle )
{
    PRINT_INFO( "joint angle %s ( %f ) -> servo angle\n", joint_name.c_str(), joint_angle );
    auto const & joint_it = joints_.find( joint_name );
    if( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return 0.0;
    }

    JointAnglePair const min_limit = getJointAngleMinLimitPair( joint_name );
    JointAnglePair const max_limit = getJointAngleMaxLimitPair( joint_name );
    double const min_joint_angle = min_limit.first;
    double const max_joint_angle = max_limit.first;
    double const min_servo_angle = min_limit.second;
    double const max_servo_angle = max_limit.second;

    auto const & actuator_parameters = joint_it->second.parameters_;
    double const max_linkage_angle = max_joint_angle + actuator_parameters.find("joint_offset")->second;
    double const direction = actuator_parameters.find("direction")->second;
    double const linkage_angle = joint_angle + actuator_parameters.find("joint_offset")->second;

    // cable displacement from max servo angle
    double const servo_cable_displacement = servoAngleToCableDisplacement( max_servo_angle * M_PI / 180.0, actuator_parameters.find("servo_radius")->second, actuator_parameters.find("piston_length")->second );
    // cable displacement from max joint angle
    double const joint_cable_displacement = jointAngleToCableDisplacement( ( 180.0 - max_linkage_angle ) * M_PI / 180.0, actuator_parameters.find("joint_radius")->second, actuator_parameters.find("joint_length")->second );
    // the cable offset is the difference between the joint cable displacement and the servo cable displacement; specifically, both
    // displacements should be 0 when both components are at 90 degrees
    double const cable_offset = servo_cable_displacement - joint_cable_displacement;
    double const cable_displacement = jointAngleToCableDisplacement( ( 180.0 - linkage_angle ) * M_PI / 180.0, actuator_parameters.find("joint_radius")->second, actuator_parameters.find("joint_length")->second );
    double const servo_angle = cableDisplacementToServoAngle( cable_displacement + cable_offset, actuator_parameters.find("servo_radius")->second, actuator_parameters.find("piston_length")->second ) * 180.0 / M_PI;

    PRINT_INFO( "Cable offset is %f m\n", cable_offset );
    PRINT_INFO( "Cable displacement is %f m\n", cable_displacement );
    PRINT_INFO( "Total cable displacement is %f m\n", cable_displacement + cable_offset );
    PRINT_INFO( "Servo angle is %f\n", servo_angle );
    PRINT_INFO( "---\n" );

    return servo_angle;

//    return getInverseTransferFunctionOutput( device, channel, joint_angle * M_PI / 180.0 );

//    return min_pos + ( max_pos - min_pos ) * ( angle - min_angle ) / ( max_angle - min_angle );
} // convertJointAngleToServoAngle(const uint16_t, const uint16_t, double)

//
bool JointAngleController::setJointAngleLimits( std::string const & joint_name, JointAnglePair limit1, JointAnglePair limit2 )
{
    return setJointAngleLimits( joint_name, JointAngleLimits( limit1, limit2 ) );
}

//
bool JointAngleController::setJointAngleLimits( std::string const & joint_name, JointAngleLimits limits )
{
    if ( !this->servo_names_map_.count( joint_name ) ) return false;

    joints_[joint_name].limits_ = limits;
    return true;
} // setJointAngleLimits(const uint16_t, const uint16_t, JointAngleLimits)

// commands an individual servo motor to move to a target position
bool JointAngleController::setJointAngleTarget( std::string const & joint_name, double target )
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)) || (!isValidJointAngleTarget(device, channel, target)))
    auto const & joint_it = joints_.find( joint_name );
    if ( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return false;
    }

    auto const & servo_name_it = this->servo_names_map_.find( joint_it->second.servo_name_ );
    if( servo_name_it == this->servo_names_map_.cend() )
    {
        PRINT_WARN( "Servo %s of joint %s not found\n", joint_it->second.servo_name_.c_str(), joint_it->first.c_str() );
        return false;
    }

    if( !isValidJointAngleTarget( joint_name, target ) )
    {
        PRINT_WARN( "Joint angle target %.1f out of angle limits of joint %s\n", target, joint_name.c_str() );
        return false;
    }

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    if( !ServoAngleController::setServoAngleTarget( device, channel, convertJointAngleToServoAngle( joint_name, target ) ) )
    {
        PRINT_WARN( "Failed to set servo angle %f for joint %s\n", target, joint_name.c_str() );
        return false;
    }
    return true;
} // setJointAngleTarget(const uint16_t, const uint16_t, double)

// commands an individual servo motor to move at the parameterized speed
bool JointAngleController::setJointAngleSpeed( std::string const & joint_name, double speed )
{
    auto const & joint_it = joints_.find( joint_name );
    if ( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return false;
    }

    auto const & servo_name_it = this->servo_names_map_.find( joint_it->second.servo_name_ );

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    return ServoController::setServoSpeed( device, channel, convertJointAngleToServoAngle( joint_name, speed ) );
} // setJointAngleSpeed(const uint16_t, const uint16_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool JointAngleController::setJointAngleAcceleration( std::string const & joint_name, double accel )
{
    auto const & joint_it = joints_.find( joint_name );
    if ( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return false;
    }

    auto const & servo_name_it = this->servo_names_map_.find( joint_it->second.servo_name_ );

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    return ServoController::setServoAcceleration( device, channel, convertJointAngleToServoAngle( joint_name, accel ) );
} // setServoAcceleration(const uint16_t, const uint16_t, double)

void JointAngleController::setJointsHome()
{
    for( auto joint_it = joints_.cbegin(); joint_it != joints_.cend(); ++joint_it )
    {
        setJointHome( joint_it->first );
    }
}

void JointAngleController::setJointHome( std::string const & joint_name )
{
    auto const & joint_it = joints_.find( joint_name );
    if( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return;
    }

    setJointAngleTarget( joint_name, joint_it->second.home_ );
}

//
JointAngleLimits JointAngleController::getJointAngleLimits( std::string const & joint_name ) const
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    auto const & joint_it = joints_.find( joint_name );
    if ( joint_it == joints_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return JointAngleLimits( JointAnglePair( 0, 0.0 ), JointAnglePair( 0, 0.0 ) );
    }
//    PRINT_INFO( "joint angle limits [[%f %f][%f %f]]\n", joint_it->second.limits_.first.first, joint_it->second.limits_.first.second, joint_it->second.limits_.second.first, joint_it->second.limits_.second.second );
    return joint_it->second.limits_;
} // getJointAngleLimits(const uint16_t, const uint16_t)

//
JointAnglePair JointAngleController::getJointAngleMinLimitPair( std::string const & joint_name ) const
{
    JointAngleLimits const & limits = getJointAngleLimits( joint_name );
//    PRINT_INFO( "(min) joint angle limits [[%f %f][%f %f]]\n", limits.first.first, limits.first.second, limits.second.first, limits.second.second );
    if( limits.first.first < limits.second.first )
    {
        return limits.first;
    }
    return limits.second;
} // getJointAngleMinLimitPair(const uint16_t, const uint16_t)

//
JointAnglePair JointAngleController::getJointAngleMaxLimitPair( std::string const & joint_name ) const
{
    JointAngleLimits limits = getJointAngleLimits( joint_name );
//    PRINT_INFO( "(max) joint angle limits [[%f %f][%f %f]]\n", limits.first.first, limits.first.second, limits.second.first, limits.second.second );
    if( limits.first.first > limits.second.first )
    {
        return limits.first;
    }
    return limits.second;
} // getJointAngleMaxLimitPair(const uint16_t, const uint16_t)

//
double JointAngleController::getJointAngleMinLimit( std::string const & joint_name ) const
{
    return getJointAngleMinLimitPair( joint_name ).first;
} // getJointAngleMinLimit(const uint16_t, const uint16_t)

//
double JointAngleController::getJointAngleMaxLimit( std::string const & joint_name ) const
{
    return getJointAngleMaxLimitPair( joint_name ).first;
} // getJointAngleMaxLimit(const uint16_t, const uint16_t)

//
double const & JointAngleController::getJointAngleTarget( std::string const & joint_name )
{
    auto const & servo_name_it = this->servo_names_map_.find( joint_name );
    if( servo_name_it == this->servo_names_map_.cend() )
    {
        PRINT_WARN( "Joint %s not found\n", joint_name.c_str() );
        return 0.0;
    }

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    return convertServoAngleToJointAngle( joint_name, ServoController::getServoTarget( device, channel ) );
} // getJointAngleTarget(const uint16_t, const uint16_t)

//
double const & JointAngleController::getJointAngleSpeed( std::string const & joint_name )
{
    auto const & servo_name_it = this->servo_names_map_.find( joint_name );
    if( servo_name_it == this->servo_names_map_.cend() ) return false;

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    return convertServoAngleToJointAngle( joint_name, ServoController::getServoSpeed( device, channel ) );
} // getJointAngleSpeed(const uint16_t, const uint16_t)

//
double const & JointAngleController::getJointAngleAcceleration( std::string const & joint_name )
{
    auto const & servo_name_it = this->servo_names_map_.find( joint_name );
    if( servo_name_it == this->servo_names_map_.cend() ) return false;

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    return convertServoAngleToJointAngle( joint_name, ServoController::getServoAcceleration( device, channel ) );
} // getJointAngleAcceleration(const uint16_t, const uint16_t)

//
double const & JointAngleController::getJointAnglePosition( std::string const & joint_name )
{
    auto const & servo_name_it = this->servo_names_map_.find( joint_name );
    if( servo_name_it == this->servo_names_map_.cend() ) return false;

    uint16_t const & device = servo_name_it->second.first;
    uint16_t const & channel = servo_name_it->second.second;

    return convertServoAngleToJointAngle( joint_name, ServoController::getServoPosition( device, channel ) );
} // getJointAnglePosition(const uint16_t, const uint16_t)

std::map<std::string, Joint> const & JointAngleController::getJoints()
{
    return joints_;
}

//
JointAngleController& JointAngleController::operator =( const JointAngleController &joint_angle_controller )
{
    if ( &joint_angle_controller != this )
    {
        //*this = ServoAngleController(joint_angle_controller);
        joints_ = joint_angle_controller.joints_;
    }
} // =(const JointAngleController &)

//_TransferFunction & JointAngleController::getTransferFunction( uint16_t const & device, uint16_t const & channel )
//{
//    return transfer_functions_[device][channel];
//}
//
//_TransferFunction const & JointAngleController::getTransferFunction( uint16_t const & device, uint16_t const & channel ) const
//{
//    return transfer_functions_[device][channel];
//}
//
//void JointAngleController::setTransferFunctionInput( uint16_t const & device, uint16_t const & channel, double const & value )
//{
//    getTransferFunction( device, channel ).first.variable_value_ = value;
//}
//
//double & JointAngleController::getTransferFunctionInput( uint16_t const & device, uint16_t const & channel )
//{
//    return getTransferFunction( device, channel ).first;
//}
//
//double const & JointAngleController::getTransferFunctionInput( uint16_t const & device, uint16_t const & channel ) const
//{
//    return getTransferFunction( device, channel ).first;
//}
//
//mu::Parser & JointAngleController::getTransferFunctionParser(  uint16_t const & device, uint16_t const & channel )
//{
//    return getTransferFunction( device, channel ).second;
//}
//
//mu::Parser const & JointAngleController::getTransferFunctionParser(  uint16_t const & device, uint16_t const & channel ) const
//{
//    return getTransferFunction( device, channel ).second;
//}
//
//double JointAngleController::getTransferFunctionOutput( uint16_t const & device, uint16_t const & channel ) const
//{
//    try
//    {
//        return getTransferFunctionParser( device, channel ).Eval();
//    }
//    catch( mu::Parser::exception_type const & e )
//    {
//        PRINT_WARN( "%s\n", e.GetMsg().c_str() );
//    }
//    return 0;
//}
//
//double JointAngleController::getTransferFunctionOutput( uint16_t const & device, uint16_t const & channel, double const & input )
//{
//    setTransferFunctionInput( device, channel, input );
//    return getTransferFunctionOutput( device, channel );
//}

bool JointAngleController::initFromYaml( YAML::Node const & node )
{
    PRINT_DEBUG( "Loading JointAngleController values from yaml node.\n" );
    try
    {
        YAML::Node const & servo_controllers_node = node["servo_controllers"];
        ServoAngleController::initFromYaml( servo_controllers_node );

        YAML::Node const & joints_node = node["joints"];

        for( size_t i = 0; i < joints_node.size(); ++i )
        {
            Joint joint;
            joints_node[i] >> joint;

            PRINT_DEBUG( "Adding joint with name %s, servo %s, and home %f\n", joint.name_.c_str(), joint.servo_name_.c_str(), joint.home_ );

            joints_[joint.name_] = joint;
        }
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
        return false;
    }
    return true;
}

//
void operator >>( const YAML::Node &node, JointAngleController &joint_angle_controller )
{
    joint_angle_controller.initFromYaml( node );
} // >>(const YAML::Node &, JointAngleController &)

void operator >>( YAML::Node const & node, sparky::Joint & joint )
{
    try
    {
        node["name"] >> joint.name_;
        node["joint_limits"] >> joint.limits_;
        node["home"] >> joint.home_;

        YAML::Node const * servo_name_node_ptr = node.FindValue( "servo_name" );
        if( servo_name_node_ptr ) *servo_name_node_ptr >> joint.servo_name_;
        else joint.servo_name_ = joint.name_;

        PRINT_DEBUG( "Parsed joint with name %s, servo %s, and home %f\n", joint.name_.c_str(), joint.servo_name_.c_str(), joint.home_ );

        static std::vector<std::string> const target_parameters{ "servo_radius", "joint_radius", "joint_length", "piston_length", "joint_offset", "direction" };
        static std::vector<double> const parameter_scales{ 0.01, 0.01, 0.01, 0.01, 1.0, 1.0 };
        static std::vector<double> const default_values{ 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
        for( size_t i = 0; i < target_parameters.size(); ++i )
        {
            try
            {
                double input_value;
                node[target_parameters[i]] >> input_value;
                joint.parameters_[target_parameters[i]] = input_value * parameter_scales[i];
            }
            catch ( YAML::Exception const & e )
            {
                PRINT_DEBUG( "%s\n", e.what() );
                joint.parameters_[target_parameters[i]] = default_values[i];
            }
        }
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }
}
//
void operator >>( const YAML::Node &node, JointAngleLimits &joint_angle_limits )
{
    assert((node.Type() == YAML::NodeType::Sequence) && (node.size() == 2));
    node[0] >> joint_angle_limits.first;
    node[1] >> joint_angle_limits.second;
} // >>(const YAML::Node &, JointAngleLimits &)

//
void operator >>( const YAML::Node &node, JointAnglePair &joint_angle_pair )
{
    assert((node.Type() == YAML::NodeType::Sequence) && (node.size() == 2));
    node[0] >> joint_angle_pair.first;
    node[1] >> joint_angle_pair.second;
} // >>(const YAML::Node &, JointAnglePair &)
