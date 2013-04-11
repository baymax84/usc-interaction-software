// preprocessor directives
#include <sparky/servo.h>
using namespace pololu::maestro;

//
Servo::Servo( ServoLimits const & limits, bool const & enabled, uint16_t const & device, uint16_t const & channel, uint16_t const & target, uint16_t const & speed, uint16_t const & accel, std::string const & name ) :
    limits_( limits ), enabled_( enabled ), device_( device ), channel_( channel ), target_( target ), speed_( speed ), accel_( accel ), name_( name )
{
} // Servo(const ServoLimits, const uint16_t, const uint16_t, const uint16_t)

//
Servo::Servo( const Servo &servo )
{
    *this = servo;
} // Servo(const Servo &)

//
bool Servo::isValid() const
{
    return ( !enabled_ ) || isValidTarget( target_ );
} // isValid()

//
bool Servo::isValidTarget( const uint16_t target ) const
{
    return ( target >= getMinLimit() ) && ( target <= getMaxLimit() );
} // isValidTarget(const uint16_t)

//
uint16_t Servo::clipTargetValue( const uint16_t target ) const
{
    uint16_t min_limit = getMinLimit();
    uint16_t max_limit = getMaxLimit();
    if ( target < min_limit ) return min_limit;
    else if ( target > max_limit ) return max_limit;
    return target;
} // clipTargetValue(const uint16_t)

//
bool Servo::setTarget( uint16_t target, const bool clip_target )
{
    if ( clip_target ) target = clipTargetValue( target );
    else if ( !isValidTarget( target ) ) return false;
    target_ = target;
    return true;
} // setTarget(uint16_t, const bool)

//
uint16_t Servo::getMinLimit() const
{
    return std::min( limits_.first, limits_.second );
} // getMinLimit()

//
uint16_t Servo::getMaxLimit() const
{
    return std::max( limits_.first, limits_.second );
} // getMaxLimit()

//
Servo& Servo::operator =( const Servo &servo )
{
    if ( &servo != this )
    {
        name_ = servo.name_;
        limits_ = servo.limits_;
        enabled_ = servo.enabled_;
        device_ = servo.device_;
        channel_ = servo.channel_;
        target_ = servo.target_;
        speed_ = servo.speed_;
        accel_ = servo.accel_;
    }
} // =(const Servo &)

void Servo::initFromYaml( YAML::Node const & node )
{
    try
    {
        node["name"] >> name_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["limits"] >> limits_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["enabled"] >> enabled_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["device"] >> device_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["channel"] >> channel_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["target"] >> target_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["speed"] >> speed_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    try
    {
        node["accel"] >> accel_;
    }
    catch ( YAML::Exception const & e )
    {
        PRINT_WARN( "%s\n", e.what() );
    }

    //assert(servo.isValid());
}

//
void operator >>( const YAML::Node &node, Servo &servo )
{
    servo.initFromYaml( node );
} // >>(const YAML::Node &, Servo &)

//
void operator >>( const YAML::Node &node, ServoLimits &servo_limits )
{
    assert((node.Type() == YAML::NodeType::Sequence) && (node.size() == 2));
    node[0] >> servo_limits.first;
    node[1] >> servo_limits.second;
} // >>(const YAML::Node &, ServoLimits &)
