// preprocessor directives
#include <assert.h>
#include <sparky/servo_angle_controller.h>
using namespace pololu::maestro;

ServoAngleController::ServoAngleController()
{
    //
}

//
ServoAngleController::ServoAngleController( const uint16_t n_channels, const std::string path, const bool connect ) :
    ServoController( n_channels, path, connect )
{
    ServoAngleController::init( 1, n_channels );
} // ServoAngleController(const uint16_t, const std::string, const bool)

//
ServoAngleController::ServoAngleController( const uint16_t n_devices, const uint16_t n_channels_each, const std::string path, const bool connect ) :
    ServoController( n_devices, n_channels_each, path, connect )
{
    ServoAngleController::init( n_devices, n_channels_each );
} // ServoAngleController(const uint16_t, const uint16_t, const std::string, const bool)

//
ServoAngleController::ServoAngleController( const std::vector<uint16_t> n_device_channels, const std::string path, const bool connect ) :
    ServoController( n_device_channels, path, connect )
{
    ServoAngleController::init( n_device_channels );
} // ServoAngleController(const std::vector<uint16_t>, const std::string, const bool)

//
ServoAngleController::ServoAngleController( const std::string filename, const std::string path, const bool connect ) :
    ServoController( filename, path, connect )
{
    ServoAngleController::init();
} // ServoAngleController(const std::string, const std::string, const bool)

//
ServoAngleController::ServoAngleController( std::ifstream &fin, const std::string path, const bool connect ) :
    ServoController( fin, path, connect )
{
    ServoAngleController::init();
} // ServoAngleController(std::fstream &, const std::string, const bool)

//
ServoAngleController::ServoAngleController( YAML::Parser &parser, const std::string path, const bool connect ) :
    ServoController( parser, path, connect )
{
    ServoAngleController::init();
} // ServoAngleController(YAML::Parser &, const std::string, const bool)

//
ServoAngleController::ServoAngleController( const YAML::Node &node )
{
    initFromYaml( node );
} // ServoAngleController(const YAML::Node &, const std::string, const bool)

//
ServoAngleController::ServoAngleController( const ServoAngleController &servo_angle_controller ) :
    ServoController( servo_angle_controller )
{
    *this = servo_angle_controller;
} // ServoAngleController(const ServoAngleController &)

// shuts down the servo controller, disconnecting if necessary
ServoAngleController::~ServoAngleController()
{
    if ( isConnected() ) disconnect();
} // ~ServoAngleController()

//
bool ServoAngleController::isValidServoAngleTarget( const uint16_t channel, const double target ) const
{
    return isValidServoAngleTarget( 0, channel, target );
} // isValidServoAngleTarget(const uint16_t, const double)

//
bool ServoAngleController::isValidServoAngleTarget( const uint16_t device, const uint16_t channel, const double target ) const
{
    if ( !isValidChannel( device, channel ) ) return false;
    return ( target >= getServoAngleMinLimit( device, channel ) ) && ( target
        <= getServoAngleMaxLimit( device, channel ) );
} // isValidServoAngleTarget(const uint16_t, const uint16_t, const double)

//
double ServoAngleController::clipServoAngleTargetValue( const uint16_t channel, const double target ) const
{
    return clipServoAngleTargetValue( 0, channel, target );
} // clipServoAngleTargetValue(const uint16_t, const double)

//
double ServoAngleController::clipServoAngleTargetValue( const uint16_t device, const uint16_t channel, const double target ) const
{
    if ( !isValidChannel( device, channel ) ) return 0.0;

    double min_limit = getServoAngleMinLimit( device, channel );
    double max_limit = getServoAngleMaxLimit( device, channel );
    if ( target < min_limit ) return min_limit;
    else if ( target > max_limit ) return max_limit;
    return target;
} // clipServoAngleTargetValue(const uint16_t, const uint16_t, const double)

//
double ServoAngleController::convertServoToAngle( const uint16_t channel, uint16_t pos ) const
{
    return convertServoToAngle( 0, channel, pos );
} // convertServoToAngle(const uint16_t, uint16_t)

//
double ServoAngleController::convertServoToAngle( const uint16_t device, const uint16_t channel, uint16_t pos ) const
{
    if ( !isValidChannel( device, channel ) ) return 0.0;

    ServoAnglePair min_limit = getServoAngleMinLimitPair( device, channel );
    ServoAnglePair max_limit = getServoAngleMaxLimitPair( device, channel );
    uint16_t min_pos = min_limit.first;
    uint16_t max_pos = max_limit.first;
    double min_angle = min_limit.second;
    double max_angle = max_limit.second;

    return min_angle + ( max_angle - min_angle ) * double ( pos - min_pos )
        / double ( max_pos - min_pos );
} // convertServoToAngle(const uint16_t, const uint16_t, uint16_t)

//
uint16_t ServoAngleController::convertAngleToServo( const uint16_t channel, double angle ) const
{
    return convertAngleToServo( 0, channel, angle );
} // convertAngleToServo(const uint16_t, double)

//
uint16_t ServoAngleController::convertAngleToServo( const uint16_t device, const uint16_t channel, double angle ) const
{
    if ( !isValidChannel( device, channel ) ) return 0;

    ServoAnglePair min_limit = getServoAngleMinLimitPair( device, channel );
    ServoAnglePair max_limit = getServoAngleMaxLimitPair( device, channel );
    uint16_t min_pos = min_limit.first;
    uint16_t max_pos = max_limit.first;
    double min_angle = min_limit.second;
    double max_angle = max_limit.second;

    return min_pos + ( max_pos - min_pos ) * ( angle - min_angle ) / ( max_angle
        - min_angle );
} // convertAngleToServo(const uint16_t, const uint16_t, double)

//
bool ServoAngleController::setServoAngleLimits( const uint16_t channel, ServoAngleLimits limits )
{
    return setServoAngleLimits( 0, channel, limits );
} // setServoAngleLimits(const uint16_t, ServoAngleLimits)

//
bool ServoAngleController::setServoAngleLimits( const uint16_t device, const uint16_t channel, ServoAngleLimits limits )
{
    PRINT_DEBUG( "ServoAngleController::setServoAngleLimits( %u, %u, ServoAngleLimits )\n", device, channel );
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) )
    {
        PRINT_WARN( "setServoAngleLimits failed because channel %u for device %u is invalid\n", channel, device );
        return false;
    }
    servos_servo_angle_limits_[device][channel] = limits;
    return true;
} // setServoAngleLimits(const uint16_t, const uint16_t, ServoAngleLimits)

//
bool ServoAngleController::setServoAngleLimits( const uint16_t channel, ServoAnglePair limit1, ServoAnglePair limit2 )
{
    return setServoAngleLimits( 0, channel, limit1, limit2 );
} // setServoAngleLimits(const uint16_t, ServoAngle, ServoAngle)

//
bool ServoAngleController::setServoAngleLimits( const uint16_t device, const uint16_t channel, ServoAnglePair limit1, ServoAnglePair limit2 )
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    servos_servo_angle_limits_[device][channel] = ServoAngleLimits( limit1,
                                                                    limit2 );
    return true;
} // setServoAngleLimits(const uint16_t, const uint16_t, ServoAngle, ServoAngle)

// commands an individual servo motor to move to a target position
bool ServoAngleController::setServoAngleTarget( const uint16_t channel, double target )
{
    //if ((!isConnected()) || (channel >= getNumChannels(0)) || (!isValidServoAngleTarget(channel, target)))
    if ( ( channel >= getNumChannels( 0 ) ) || ( !isValidServoAngleTarget( channel, target ) ) ) return false;
    return setServoTarget( channel, convertAngleToServo( channel, target ) );
} // setServoAngleTarget(const uint16_t, double)

// commands an individual servo motor to move to a target position
bool ServoAngleController::setServoAngleTarget( const uint16_t device, const uint16_t channel, double target )
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)) || (!isValidServoAngleTarget(device, channel, target)))
    if ( !isValidChannel( device, channel ) )
    {
        PRINT_WARN( "Invalid device/channel pair %u %u\n", device, channel );
        return false;
    }

    if( !isValidServoAngleTarget( device, channel, target ) )
    {
        PRINT_WARN( "Invalid servo angle target %f for servo %u %u\n", target, device, channel );
        return false;
    }
    if( !setServoTarget( device, channel, convertAngleToServo( device, channel, target ) ) )
    {
        PRINT_WARN( "Failed to set servo target %f for servo %u %u\n", target, device, channel );
        return false;
    }
    return true;
} // setServoAngleTarget(const uint16_t, const uint16_t, double)

// commands an individual servo motor to move at the parameterized speed
bool ServoAngleController::setServoAngleSpeed( const uint16_t channel, double speed )
{
    //if (!isConnected() || (!isValidChannel(channel)))
    if ( !isValidChannel( channel ) ) return false;
    return setServoSpeed( channel, convertAngleToServo( channel, speed ) );
} // setServoAngleSpeed(const uint16_t, double)

// commands an individual servo motor to move at the parameterized speed
bool ServoAngleController::setServoAngleSpeed( const uint16_t device, const uint16_t channel, double speed )
{
    //if (!isConnected() || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    return setServoSpeed( device, channel, convertAngleToServo( device, channel, speed ) );
} // setServoAngleSpeed(const uint16_t, const uint16_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool ServoAngleController::setServoAngleAcceleration( const uint16_t channel, double accel )
{
    //if (!isConnected() || (!isValidChannel(channel)))
    if ( !isValidChannel( channel ) ) return false;
    return setServoAcceleration( channel, convertAngleToServo( channel, accel ) );
} // setServoAngleAcceleration(const uint16_t, double)

// commands an individual servo motor to move at the parameterized acceleration
bool ServoAngleController::setServoAngleAcceleration( const uint16_t device, const uint16_t channel, double accel )
{
    //if (!isConnected() || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return false;
    return setServoAcceleration( device, channel, convertAngleToServo( device, channel, accel ) );
} // setServoAcceleration(const uint16_t, const uint16_t, double)

//
ServoAngleLimits ServoAngleController::getServoAngleLimits( const uint16_t channel ) const
{
    return getServoAngleLimits( 0, channel );
} // getServoAngleLimits(const uint16_t)

//
ServoAngleLimits ServoAngleController::getServoAngleLimits( const uint16_t device, const uint16_t channel ) const
{
    //if ((!isConnected()) || (!isValidChannel(device, channel)))
    if ( !isValidChannel( device, channel ) ) return ServoAngleLimits( ServoAnglePair( 0, 0.0 ), ServoAnglePair( 0, 0.0 ) );
    return servos_servo_angle_limits_[device][channel];
} // getServoAngleLimits(const uint16_t, const uint16_t)

//
ServoAnglePair ServoAngleController::getServoAngleMinLimitPair( const uint16_t channel ) const
{
    return getServoAngleMinLimitPair( 0, channel );
} // getServoAngleMinLimitPair(const uint16_t)

//
ServoAnglePair ServoAngleController::getServoAngleMinLimitPair( const uint16_t device, const uint16_t channel ) const
{
    ServoAngleLimits limits = getServoAngleLimits( device, channel );
    return ( limits.first.second < limits.second.second ) ? limits.first
                                                          : limits.second;
} // getServoAngleMinLimitPair(const uint16_t, const uint16_t)

//
ServoAnglePair ServoAngleController::getServoAngleMaxLimitPair( const uint16_t channel ) const
{
    return getServoAngleMaxLimitPair( 0, channel );
} // getServoAngleMaxLimitPair(const uint16_t)

//
ServoAnglePair ServoAngleController::getServoAngleMaxLimitPair( const uint16_t device, const uint16_t channel ) const
{
    ServoAngleLimits limits = getServoAngleLimits( device, channel );
    return ( limits.first.second > limits.second.second ) ? limits.first
                                                          : limits.second;
} // getServoAngleMaxLimitPair(const uint16_t, const uint16_t)

//
double ServoAngleController::getServoAngleMinLimit( const uint16_t channel ) const
{
    return getServoAngleMinLimit( 0, channel );
} // getServoAngleMinLimit(const uint16_t)

//
double ServoAngleController::getServoAngleMinLimit( const uint16_t device, const uint16_t channel ) const
{
    return getServoAngleMinLimitPair( device, channel ).second;
} // getServoAngleMinLimit(const uint16_t, const uint16_t)

//
double ServoAngleController::getServoAngleMaxLimit( const uint16_t channel ) const
{
    return getServoAngleMinLimit( 0, channel );
} // getServoAngleMaxLimit(const uint16_t)

//
double ServoAngleController::getServoAngleMaxLimit( const uint16_t device, const uint16_t channel ) const
{
    return getServoAngleMaxLimitPair( device, channel ).second;
} // getServoAngleMaxLimit(const uint16_t, const uint16_t)

//
double ServoAngleController::getServoAngleTarget( const uint16_t channel )
{
    return getServoAngleTarget( 0, channel );
} // getServoAngleTarget(const uint16_t)

//
double ServoAngleController::getServoAngleTarget( const uint16_t device, const uint16_t channel )
{
    return convertServoToAngle( device, channel, getServoTarget( device, channel ) );
} // getServoAngleTarget(const uint16_t, const uint16_t)

//
double ServoAngleController::getServoAngleSpeed( const uint16_t channel )
{
    return getServoAngleSpeed( 0, channel );
} // getServoAngleSpeed(const uint16_t)

//
double ServoAngleController::getServoAngleSpeed( const uint16_t device, const uint16_t channel )
{
    return convertServoToAngle( device, channel, getServoSpeed( device, channel ) );
} // getServoAngleSpeed(const uint16_t, const uint16_t)

//
double ServoAngleController::getServoAngleAcceleration( const uint16_t channel )
{
    return getServoAngleAcceleration( 0, channel );
} // getServoAngleAcceleration(const uint16_t)

//
double ServoAngleController::getServoAngleAcceleration( const uint16_t device, const uint16_t channel )
{
    return convertServoToAngle( device, channel, getServoAcceleration( device, channel ) );
} // getServoAngleAcceleration(const uint16_t, const uint16_t)

//
double ServoAngleController::getServoAnglePosition( const uint16_t channel )
{
    return convertServoToAngle( channel, getServoPosition( channel ) );
} // getServoAnglePosition(const uint16_t)

//
double ServoAngleController::getServoAnglePosition( const uint16_t device, const uint16_t channel )
{
    return convertServoToAngle( device, channel, getServoPosition( device, channel ) );
} // getServoAnglePosition(const uint16_t, const uint16_t)

//
ServoAngleController& ServoAngleController::operator =( const ServoAngleController &servo_angle_controller )
{
    if ( &servo_angle_controller != this )
    {
        //*this = ServoController(servo_angle_controller);
        servos_servo_angle_limits_ = servo_angle_controller.servos_servo_angle_limits_;
    }
} // =(const ServoAngleController &)

//
bool ServoAngleController::init()
{
    PRINT_DEBUG( "ServoAngleController::init()\n" );
    uint16_t n_devices = getNumDevices();
    if ( n_devices == 0 )
    {
        PRINT_WARN( "Init failed because n_devices = 0\n" );
        return false;
    }
    std::vector<uint16_t> n_device_channels( n_devices );
    for ( int i = 0; i < n_devices; ++i )
        n_device_channels[i] = getNumChannels( i );
    return init( n_device_channels );
} // init()

//
bool ServoAngleController::init( const uint16_t n_devices, const uint16_t n_channels_each )
{
    PRINT_DEBUG( "ServoAngleController::init( %u %u )\n", n_devices, n_channels_each );
    if ( ( n_devices == 0 ) || ( n_channels_each == 0 ) )
    {
        PRINT_WARN( "Init failed because n_devices = 0 or n_channels_each = 0\n" );
        return false;
    }
    servos_servo_angle_limits_.resize( n_devices );
    for ( int i = 0; i < n_devices; ++i )
        servos_servo_angle_limits_[i].resize( n_channels_each );
    return true;
} // init(const uint16_t, const uint16_t)

//
bool ServoAngleController::init( const std::vector<uint16_t> n_device_channels )
{
    PRINT_DEBUG( "ServoAngleController::init( std::vector( %zu ) )\n", n_device_channels.size() );
    unsigned int n_devices = n_device_channels.size();
    if ( n_devices == 0 )
    {
        PRINT_WARN( "Init failed because n_devices = 0\n" );
        return false;
    }
    servos_servo_angle_limits_.resize( n_devices );
    for ( int i = 0; i < n_devices; ++i )
    {
        if ( n_device_channels[i] == 0 ) return false;
        servos_servo_angle_limits_[i].resize( n_device_channels[i] );
    }
    return true;
} // init(const std::vector<uint16_t>)

void ServoAngleController::initFromYaml( YAML::Node const & node )
{
    PRINT_DEBUG( "Loading ServoAngleController values from yaml node.\n" );
    ServoController::initFromYaml( node );

    servos_servo_angle_limits_.resize( this->servos_.size() );
    for( size_t i = 0; i < this->servos_.size(); ++i )
    {
        servos_servo_angle_limits_[i].resize( this->servos_[i].size() );
    }

    try
    {
        YAML::Node const & first_node = node[0];

        YAML::Node const & servos_node = first_node["servos"];
        for ( int i = 0; i < servos_node.size(); ++i )
        {
            uint16_t const & device = this->servos_[0][i].device_;
            uint16_t const & channel = this->servos_[0][i].channel_;

            ServoAngleLimits servo_angle_limits;
            servos_node[i]["angle_limits"] >> servo_angle_limits;
            setServoAngleLimits( device, channel, servo_angle_limits );
        }
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }
}

//
void operator >>( const YAML::Node &node, ServoAngleController &servo_angle_controller )
{
    servo_angle_controller.initFromYaml( node );
} // >>(const YAML::Node &, ServoAngleController &)

//
void operator >>( const YAML::Node &node, ServoAngleLimits &servo_angle_limits )
{
    PRINT_DEBUG( "Creating ServoAngleLimits from yaml node\n" );
    assert((node.Type() == YAML::NodeType::Sequence) && (node.size() == 2));
    node[0] >> servo_angle_limits.first;
    node[1] >> servo_angle_limits.second;
} // >>(const YAML::Node &, ServoAngleLimits &)

//
void operator >>( const YAML::Node &node, ServoAnglePair &servo_angle_pair )
{
    PRINT_DEBUG( "Creating ServoAnglePair from yaml node\n" );
    assert((node.Type() == YAML::NodeType::Sequence) && (node.size() == 2));
    node[0] >> servo_angle_pair.first;
    node[1] >> servo_angle_pair.second;
} // >>(const YAML::Node &, ServoAnglePair &)
