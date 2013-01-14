// preprocessor directives
#include <assert.h>
#include <sparky/joint_angle_controller.h>
using namespace sparky;

//
JointAngleController::JointAngleController( const uint8_t n_channels,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( n_channels, path, connect )
{
    JointAngleController::init( 1, n_channels );
} // JointAngleController(const uint8_t, const std::string, const bool)

//
JointAngleController::JointAngleController( const uint8_t n_devices,
                                            const uint8_t n_channels_each,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( n_devices, n_channels_each, path, connect )
{
    JointAngleController::init( n_devices, n_channels_each );
} // JointAngleController(const uint8_t, const uint8_t, const std::string, const bool)

//
JointAngleController::JointAngleController(
                                            const std::vector<uint8_t> n_device_channels,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( n_device_channels, path, connect )
{
    JointAngleController::init( n_device_channels );
} // JointAngleController(const std::vector<uint8_t>, const std::string, const bool)

//
JointAngleController::JointAngleController( const std::string filename,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( filename, path, connect )
{
    JointAngleController::init();
} // JointAngleController(const std::string, const std::string, const bool)

//
JointAngleController::JointAngleController( std::ifstream &fin,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( fin, path, connect )
{
    JointAngleController::init();
} // JointAngleController(std::fstream &, const std::string, const bool)

//
JointAngleController::JointAngleController( YAML::Parser &parser,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( parser, path, connect )
{
    JointAngleController::init();
} // JointAngleController(YAML::Parser &, const std::string, const bool)

//
JointAngleController::JointAngleController( const YAML::Node &node,
                                            const std::string path,
                                            const bool connect ) :
    ServoAngleController( node, path, connect )
{
    JointAngleController::init();
} // JointAngleController(const YAML::Node &, const std::string, const bool)

//
JointAngleController::JointAngleController(
                                            const JointAngleController &joint_angle_controller ) :
    ServoAngleController( joint_angle_controller )
{
    *this = joint_angle_controller;
} // JointAngleController(const JointAngleController &)

// shuts down the servo controller, disconnecting if necessary
JointAngleController::~JointAngleController()
{
    if ( isConnected() ) disconnect();
} // ~JointAngleController()

//
bool JointAngleController::isValidJointAngleTarget( const uint8_t channel,
                                                    const double target ) const
{
    return isValidJointAngleTarget( 0, channel, target );
} // isValidJointAngleTarget(const uint8_t, const double)

//
bool JointAngleController::isValidJointAngleTarget( const uint8_t device,
                                                    const uint8_t channel,
                                                    const double target ) const
{
    if ( !isValidChannel( device, channel ) ) return false;
    return ( target >= getJointAngleMinLimit( device, channel ) ) && ( target
        <= getJointAngleMaxLimit( device, channel ) );
} // isValidJointAngleTarget(const uint8_t, const uint8_t, const double)

//
double JointAngleController::clipJointAngleTargetValue( const uint8_t channel,
                                                        const double target ) const
{
    return clipJointAngleTargetValue( 0, channel, target );
} // clipJointAngleTargetValue(const uint8_t, const double)

//
double JointAngleController::clipJointAngleTargetValue( const uint8_t device,
                                                        const uint8_t channel,
                                                        const double target ) const
{
    if ( !isValidChannel( device, channel ) ) return 0.0;

    double min_limit = getJointAngleMinLimit( device, channel );
    double max_limit = getJointAngleMaxLimit( device, channel );
    if ( target < min_limit ) return min_limit;
    else if ( target > max_limit ) return max_limit;
    return target;
} // clipJointAngleTargetValue(const uint8_t, const uint8_t, const double)

//
double JointAngleController::convertServoAngleToJointAngle(
                                                            const uint8_t channel,
                                                            double servo_angle )
{
    return convertServoAngleToJointAngle( 0, channel, servo_angle );
} // convertServoAngleToJointAngle(const uint8_t, uint16_t)

//
double JointAngleController::convertServoAngleToJointAngle(
                                                            const uint8_t device,
                                                            const uint8_t channel,
                                                            double servo_angle )
{
    if ( !isValidChannel( device, channel ) ) return 0.0;

    JointAnglePair min_limit = getJointAngleMinLimitPair( device, channel );
    JointAnglePair max_limit = getJointAngleMaxLimitPair( device, channel );
    double min_servo_angle = min_limit.first;
    double max_servo_angle = max_limit.first;
    double min_joint_angle = min_limit.second;
    double max_joint_angle = max_limit.second;

    return getTransferFunctionOutput( device, channel, servo_angle * M_PI / 180.0 );

//    return min_angle + ( max_angle - min_angle ) * double ( pos - min_pos ) / double ( max_pos - min_pos );
} // convertServoAngleToJointAngle(const uint8_t, const uint8_t, uint16_t)

//
double JointAngleController::convertJointAngleToServoAngle(
                                                              const uint8_t channel,
                                                              double joint_angle )
{
    return convertJointAngleToServoAngle( 0, channel, joint_angle );
} // convertJointAngleToServoAngle(const uint8_t, double)

//
double JointAngleController::convertJointAngleToServoAngle(
                                                              const uint8_t device,
                                                              const uint8_t channel,
                                                              double joint_angle )
{
    if ( !isValidChannel( device, channel ) ) return 0;

    JointAnglePair min_limit = getJointAngleMinLimitPair( device, channel );
    JointAnglePair max_limit = getJointAngleMaxLimitPair( device, channel );
    double min_servo_angle = min_limit.first;
    double max_servo_angle = max_limit.first;
    double min_joint_angle = min_limit.second;
    double max_joint_angle = max_limit.second;

    return 0;

//    return getInverseTransferFunctionOutput( device, channel, joint_angle * M_PI / 180 );

//    return min_pos + ( max_pos - min_pos ) * ( angle - min_angle ) / ( max_angle - min_angle );
} // convertJointAngleToServoAngle(const uint8_t, const uint8_t, double)

//
bool JointAngleController::setJointAngleLimits( const uint8_t channel,
                                                JointAngleLimits limits )
{
    return setJointAngleLimits( 0, channel, limits );
} // setJointAngleLimits(const uint8_t, JointAngleLimits)

//
bool JointAngleController::setJointAngleLimits( const uint8_t device,
                                                const uint8_t channel,
                                                JointAngleLimits limits )
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    joint_angle_limits_[device][channel] = limits;
    return true;
} // setJointAngleLimits(const uint8_t, const uint8_t, JointAngleLimits)

//
bool JointAngleController::setJointAngleLimits( const uint8_t channel,
                                                JointAnglePair limit1,
                                                JointAnglePair limit2 )
{
    return setJointAngleLimits( 0, channel, limit1, limit2 );
} // setJointAngleLimits(const uint8_t, JointAngle, JointAngle)

//
bool JointAngleController::setJointAngleLimits( const uint8_t device,
                                                const uint8_t channel,
                                                JointAnglePair limit1,
                                                JointAnglePair limit2 )
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    joint_angle_limits_[device][channel] = JointAngleLimits( limit1,
                                                                    limit2 );
    return true;
} // setJointAngleLimits(const uint8_t, const uint8_t, JointAngle, JointAngle)

// commands an individual servo motor to move to a target position
bool JointAngleController::setJointAngleTarget( const uint8_t channel,
                                                double target )
{
    //if ((!isConnected()) || (channel >= getNumChannels(0)) || (!isValidJointAngleTarget(channel, target)))
    if ( ( channel >= getNumChannels( 0 ) )
        || ( !isValidJointAngleTarget( channel, target ) ) ) return false;
    return setServoTarget( channel, convertJointAngleToServoAngle( channel,
                                                                   target ) );
} // setJointAngleTarget(const uint8_t, double)

// commands an individual servo motor to move to a target position
bool JointAngleController::setJointAngleTarget( const uint8_t device,
                                                const uint8_t channel,
                                                double target )
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)) || (!isValidJointAngleTarget(device, channel, target)))
    if ( ( !isValidChannel( device, channel ) )
        || ( !isValidJointAngleTarget( device, channel, target ) ) ) return false;
    return setServoTarget(
                           device,
                           channel,
                           convertJointAngleToServoAngle( device, channel, target ) );
} // setJointAngleTarget(const uint8_t, const uint8_t, double)

// commands an individual servo motor to move at the parameterized speed
bool JointAngleController::setJointAngleSpeed( const uint8_t channel,
                                               double speed )
{
    //if (!isConnected() || (!isValidChannel(channel)))
    if ( !isValidChannel( channel ) ) return false;
    return setServoSpeed( channel, convertJointAngleToServoAngle( channel, speed ) );
} // setJointAngleSpeed(const uint8_t, double)

// commands an individual servo motor to move at the parameterized speed
bool JointAngleController::setJointAngleSpeed( const uint8_t device,
                                               const uint8_t channel,
                                               double speed )
{
    //if (!isConnected() || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    return setServoSpeed( device, channel,
                          convertJointAngleToServoAngle( device, channel, speed ) );
} // setJointAngleSpeed(const uint8_t, const uint8_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool JointAngleController::setJointAngleAcceleration( const uint8_t channel,
                                                      double accel )
{
    //if (!isConnected() || (!isValidChannel(channel)))
    if ( !isValidChannel( channel ) ) return false;
    return setServoAcceleration( channel, convertJointAngleToServoAngle( channel,
                                                                         accel ) );
} // setJointAngleAcceleration(const uint8_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool JointAngleController::setJointAngleAcceleration( const uint8_t device,
                                                      const uint8_t channel,
                                                      double accel )
{
    //if (!isConnected() || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    return setServoAcceleration( device, channel,
                                 convertJointAngleToServoAngle( device, channel,
                                                                accel ) );
} // setServoAcceleration(const uint8_t, const uint8_t, double)

//
JointAngleLimits JointAngleController::getJointAngleLimits(
                                                            const uint8_t channel ) const
{
    return getJointAngleLimits( 0, channel );
} // getJointAngleLimits(const uint8_t)

//
JointAngleLimits JointAngleController::getJointAngleLimits(
                                                            const uint8_t device,
                                                            const uint8_t channel ) const
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return JointAngleLimits(
                                                                       JointAnglePair(
                                                                                       0,
                                                                                       0.0 ),
                                                                       JointAnglePair(
                                                                                       0,
                                                                                       0.0 ) );
    return joint_angle_limits_[device][channel];
} // getJointAngleLimits(const uint8_t, const uint8_t)

//
JointAnglePair JointAngleController::getJointAngleMinLimitPair(
                                                                const uint8_t channel ) const
{
    return getJointAngleMinLimitPair( 0, channel );
} // getJointAngleMinLimitPair(const uint8_t)

//
JointAnglePair JointAngleController::getJointAngleMinLimitPair(
                                                                const uint8_t device,
                                                                const uint8_t channel ) const
{
    JointAngleLimits limits = getJointAngleLimits( device, channel );
    return ( limits.first.second < limits.second.second ) ? limits.first
                                                          : limits.second;
} // getJointAngleMinLimitPair(const uint8_t, const uint8_t)

//
JointAnglePair JointAngleController::getJointAngleMaxLimitPair(
                                                                const uint8_t channel ) const
{
    return getJointAngleMaxLimitPair( 0, channel );
} // getJointAngleMaxLimitPair(const uint8_t)

//
JointAnglePair JointAngleController::getJointAngleMaxLimitPair(
                                                                const uint8_t device,
                                                                const uint8_t channel ) const
{
    JointAngleLimits limits = getJointAngleLimits( device, channel );
    return ( limits.first.second > limits.second.second ) ? limits.first
                                                          : limits.second;
} // getJointAngleMaxLimitPair(const uint8_t, const uint8_t)

//
double JointAngleController::getJointAngleMinLimit( const uint8_t channel ) const
{
    return getJointAngleMinLimit( 0, channel );
} // getJointAngleMinLimit(const uint8_t)

//
double JointAngleController::getJointAngleMinLimit( const uint8_t device,
                                                    const uint8_t channel ) const
{
    return getJointAngleMinLimitPair( device, channel ).second;
} // getJointAngleMinLimit(const uint8_t, const uint8_t)

//
double JointAngleController::getJointAngleMaxLimit( const uint8_t channel ) const
{
    return getJointAngleMinLimit( 0, channel );
} // getJointAngleMaxLimit(const uint8_t)

//
double JointAngleController::getJointAngleMaxLimit( const uint8_t device,
                                                    const uint8_t channel ) const
{
    return getJointAngleMaxLimitPair( device, channel ).second;
} // getJointAngleMaxLimit(const uint8_t, const uint8_t)

//
double JointAngleController::getJointAngleTarget( const uint8_t channel )
{
    return getJointAngleTarget( 0, channel );
} // getJointAngleTarget(const uint8_t)

//
double JointAngleController::getJointAngleTarget( const uint8_t device,
                                                  const uint8_t channel )
{
    return convertServoAngleToJointAngle( device, channel,
                                          getServoTarget( device, channel ) );
} // getJointAngleTarget(const uint8_t, const uint8_t)

//
double JointAngleController::getJointAngleSpeed( const uint8_t channel )
{
    return getJointAngleSpeed( 0, channel );
} // getJointAngleSpeed(const uint8_t)

//
double JointAngleController::getJointAngleSpeed( const uint8_t device,
                                                 const uint8_t channel )
{
    return convertServoAngleToJointAngle( device, channel,
                                          getServoSpeed( device, channel ) );
} // getJointAngleSpeed(const uint8_t, const uint8_t)

//
double JointAngleController::getJointAngleAcceleration( const uint8_t channel )
{
    return getJointAngleAcceleration( 0, channel );
} // getJointAngleAcceleration(const uint8_t)

//
double JointAngleController::getJointAngleAcceleration( const uint8_t device,
                                                        const uint8_t channel )
{
    return convertServoAngleToJointAngle( device, channel,
                                          getServoAcceleration( device, channel ) );
} // getJointAngleAcceleration(const uint8_t, const uint8_t)

//
double JointAngleController::getJointAnglePosition( const uint8_t channel )
{
    return convertServoAngleToJointAngle( channel, getServoPosition( channel ) );
} // getJointAnglePosition(const uint8_t)

//
double JointAngleController::getJointAnglePosition( const uint8_t device,
                                                    const uint8_t channel )
{
    return convertServoAngleToJointAngle( device, channel,
                                          getServoPosition( device, channel ) );
} // getJointAnglePosition(const uint8_t, const uint8_t)

//
JointAngleController& JointAngleController::operator =(
                                                        const JointAngleController &joint_angle_controller )
{
    if ( &joint_angle_controller != this )
    {
        //*this = ServoAngleController(joint_angle_controller);
        joint_angle_limits_
            = joint_angle_controller.joint_angle_limits_;
    }
} // =(const JointAngleController &)

//
bool JointAngleController::init()
{
    uint8_t n_devices = getNumDevices();
    if ( n_devices == 0 ) return false;
    std::vector<uint8_t> n_device_channels( n_devices );
    for ( int i = 0; i < n_devices; ++i )
        n_device_channels[i] = getNumChannels( i );
    return init( n_device_channels );
} // init()

//
bool JointAngleController::init( const uint8_t n_devices,
                                 const uint8_t n_channels_each )
{
    if ( ( n_devices == 0 ) || ( n_channels_each == 0 ) ) return false;
    joint_angle_limits_.resize( n_devices );
    transfer_functions_.resize( n_devices );
    for ( int i = 0; i < n_devices; ++i )
    {
        joint_angle_limits_[i].resize( n_channels_each );
        transfer_functions_[i].resize( n_channels_each );
    }
    return true;
} // init(const uint8_t, const uint8_t)

//
bool JointAngleController::init( const std::vector<uint8_t> n_device_channels )
{
    unsigned int n_devices = n_device_channels.size();
    if ( n_devices == 0 ) return false;
    joint_angle_limits_.resize( n_devices );
    transfer_functions_.resize( n_devices );
    for ( int i = 0; i < n_devices; ++i )
    {
        if ( n_device_channels[i] == 0 ) return false;
        joint_angle_limits_[i].resize( n_device_channels[i] );
        transfer_functions_[i].resize( n_device_channels[i] );
    }
    return true;
} // init(const std::vector<uint8_t>)

_TransferFunction & JointAngleController::getTransferFunction( uint8_t const & device, uint8_t const & channel )
{
    return transfer_functions_[device][channel];
}

_TransferFunction const & JointAngleController::getTransferFunction( uint8_t const & device, uint8_t const & channel ) const
{
    return transfer_functions_[device][channel];
}

void JointAngleController::setTransferFunctionInput( uint8_t const & device, uint8_t const & channel, double const & value )
{
    getTransferFunction( device, channel ).first = value;
}

double & JointAngleController::getTransferFunctionInput( uint8_t const & device, uint8_t const & channel )
{
    return getTransferFunction( device, channel ).first;
}

double const & JointAngleController::getTransferFunctionInput( uint8_t const & device, uint8_t const & channel ) const
{
    return getTransferFunction( device, channel ).first;
}

mu::Parser & JointAngleController::getTransferFunctionParser(  uint8_t const & device, uint8_t const & channel )
{
    return getTransferFunction( device, channel ).second;
}

mu::Parser const & JointAngleController::getTransferFunctionParser(  uint8_t const & device, uint8_t const & channel ) const
{
    return getTransferFunction( device, channel ).second;
}

double JointAngleController::getTransferFunctionOutput( uint8_t const & device, uint8_t const & channel ) const
{
    try
    {
        return getTransferFunctionParser( device, channel ).Eval();
    }
    catch( mu::Parser::exception_type const & e )
    {
        PRINT_WARN( "%s\n", e.GetMsg().c_str() );
    }
    return 0;
}

double JointAngleController::getTransferFunctionOutput( uint8_t const & device, uint8_t const & channel, double const & input )
{
    setTransferFunctionInput( device, channel, input );
    return getTransferFunctionOutput( device, channel );
}

//
void operator >>( const YAML::Node &node,
                  JointAngleController &joint_angle_controller )
{
    for ( int i = 0, n = node.size(); i < n; ++i )
    {
        uint8_t device = 0;
        uint8_t channel = 0;

        try
        {
            node[i]["servo"]["device"] >> device;
        }
        catch ( YAML::Exception const & e )
        {
            PRINT_WARN( "%s\n", e.what() );
        }

        try
        {
            node[i]["servo"]["channel"] >> channel;
            if ( joint_angle_controller.isValidChannel( device, channel ) )
            {
                pololu::maestro::Servo servo;
                try
                {
                    node[i]["servo"] >> servo;
                    joint_angle_controller.setServo( device, channel, servo );
                }
                catch ( YAML::Exception const & e )
                {
                    PRINT_WARN( "%s\n", e.what() );
                }

                pololu::maestro::ServoAngleLimits servo_angle_limits;
                try
                {
                    node[i]["servo"]["angle_limits"] >> servo_angle_limits;
                    joint_angle_controller.setServoAngleLimits( device, channel,
                                                                servo_angle_limits );
                }
                catch ( YAML::Exception const & e )
                {
                    PRINT_WARN( "%s\n", e.what() );
                }

                sparky::JointAngleLimits joint_angle_limits;
                try
                {
                    node[i]["joint"]["joint_limits"] >> joint_angle_limits;
                    joint_angle_controller.setJointAngleLimits( device, channel,
                                                                joint_angle_limits );
                }
                catch ( YAML::Exception const & e )
                {
                    PRINT_WARN( "%s\n", e.what() );
                }

                sparky::_TransferFunction & joint_transfer_function = joint_angle_controller.getTransferFunction( device, channel );
                sparky::_TransferFunction::first_type & joint_tf_input = joint_transfer_function.first;
                sparky::_TransferFunction::second_type & joint_tf_parser = joint_transfer_function.second;
                std::string joint_tf_equation;

                try
                {
                    node[i]["joint"]["joint_transfer_function"] >> joint_tf_equation;
                    joint_tf_parser.DefineVar( "x", &joint_tf_input );
                    joint_tf_parser.SetExpr( joint_tf_equation );
                }
                catch( YAML::Exception const & e )
                {
                    PRINT_WARN( "%s\n", e.what() );
                }
                catch( mu::Parser::exception_type const & e )
                {
                    PRINT_WARN( "%s\n", e.GetMsg().c_str() );
                }
            }
        }
        catch ( YAML::Exception const & e )
        {
            PRINT_WARN( "%s\n", e.what() );
        }
    }
} // >>(const YAML::Node &, JointAngleController &)

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
