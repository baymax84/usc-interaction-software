// preprocessor directives
#include <assert.h>
#include <sparky/sparky_controller.h>
using namespace sparky;

SparkyController::SparkyController()
{
    //
}

SparkyController::SparkyController( YAML::Node const & node, bool const & auto_connect, bool const & home )
{
    initFromYaml( node );
    if( auto_connect ) connect( home );
}

//
SparkyController::~SparkyController()
{
    if ( isConnected() ) disconnect();
} // ~SparkyController()

//
bool SparkyController::connect( const bool home )
{
    if ( joint_angle_controller_.connect() && home )
    {
        for ( int i = 0, n = joint_angle_controller_.getNumDevices(); i < n; ++i )
            joint_angle_controller_.setServosHome( i );
    }
    return joint_angle_controller_.isConnected();
} // connect()

//
bool SparkyController::disconnect()
{
    return joint_angle_controller_.disconnect();
} // disconnect()

//
bool SparkyController::isConnected()
{
    return joint_angle_controller_.isConnected();
} // isConnected()

//
bool SparkyController::isJointMoving( const std::string name )
{
    return false;
} // isJointMoving(const std::string)

//
bool SparkyController::waitForJointDone( const std::string name ) const
{
    return false;
} // waitForJointDone(const std::string)

//
bool SparkyController::waitForJointsDone()
{
    return false;
} // waitForJointsDone(const std::string)

//
bool SparkyController::setPath( const std::string name )
{
    return joint_angle_controller_.setPath( name );
} // setPath(const std::string)

//
bool SparkyController::setJointLimits( const std::string name,
                                       const JointLimits limits )
{
    return false;
} // setJointLimits(const std::string, const JointLimits)

//
bool SparkyController::setJointLimits( const std::string name,
                                       const ServoJoint angle1,
                                       const ServoJoint angle2 )
{
    return false;
} // setJointLimits(const std::string, const ServoJoint, const ServoJoint)

//
bool SparkyController::setJointActive( const std::string name,
                                       const bool active )
{
    return false;
} // setJointActive(const std::string, const bool)

//
bool SparkyController::setJointAngle( const std::string name, double angle )
{
    if( !joint_angle_controller_.setJointAngleTarget( name, angle ) )
    {
        PRINT_WARN( "Failed to set joint angle %f for joint %s\n", angle, name.c_str() );
        return false;
    }
    return true;
} // setJointAngle(const std::string name, double)

//
bool SparkyController::setJointsHome()
{
    return false;
} // setJointsHome()

//
std::string SparkyController::getPath() const
{
    return joint_angle_controller_.getPath();
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

//
SparkyController::JointLimits SparkyController::getJointLimits( const std::string name ) const
{
    return JointLimits( ServoJoint( 0, 0.0 ), ServoJoint( 0, 0.0 ) );
} // getJointLimits(const std::string)

//
double SparkyController::getJointMinAngle( const std::string name ) const
{
    return 0.0;
} // getJointMinAngle(const std::string)

//
double SparkyController::getJointMaxAngle( const std::string name ) const
{
    return 0.0;
} // getJointMaxAngle(const std::string)

//
bool SparkyController::getJointActive( const std::string name ) const
{
    return false;
} // getJointActive(const std::string)

//
double SparkyController::getJointAngle( const std::string name ) const
{
    return 0.0;
} // getJointAngle(const std::string)

//
bool SparkyController::getJointMoving( const std::string name )
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

//
bool SparkyController::initJoints()
{
    joints_.clear();

    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "mouth",
                                                   Joint(
                                                          0,
                                                          0,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1580,
                                                                                   -40.0 ),
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   0.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "head_nod",
                                                   Joint(
                                                          0,
                                                          2,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1120,
                                                                                   -29.0 ),
                                                                       ServoJoint(
                                                                                   2060,
                                                                                   45.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "head_turn",
                                                   Joint(
                                                          0,
                                                          4,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1340,
                                                                                   -30.0 ),
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   30.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "rt_arm_forward",
                                                   Joint(
                                                          0,
                                                          6,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1000,
                                                                                   -0.0 ),
                                                                       ServoJoint(
                                                                                   2000,
                                                                                   80.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "rt_arm_out",
                                                   Joint(
                                                          0,
                                                          8,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   2000,
                                                                                   -0.0 ),
                                                                       ServoJoint(
                                                                                   1100,
                                                                                   75.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "rt_elbow",
                                                   Joint(
                                                          0,
                                                          10,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1060,
                                                                                   -0.0 ),
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   90.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "lt_arm_forward",
                                                   Joint(
                                                          0,
                                                          12,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1200,
                                                                                   -0.0 ),
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   80.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "lt_arm_out",
                                                   Joint(
                                                          0,
                                                          14,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   -0.0 ),
                                                                       ServoJoint(
                                                                                   1000,
                                                                                   72.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "lt_elbow",
                                                   Joint(
                                                          0,
                                                          16,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1030,
                                                                                   -0.0 ),
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   90.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "rt_wrist",
                                                   Joint(
                                                          1,
                                                          0,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1910,
                                                                                   -45.0 ),
                                                                       ServoJoint(
                                                                                   1181,
                                                                                   60.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "lt_wrist",
                                                   Joint(
                                                          1,
                                                          2,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   2050,
                                                                                   -35.0 ),
                                                                       ServoJoint(
                                                                                   1535,
                                                                                   58.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "torso_forebend",
                                                   Joint(
                                                          1,
                                                          4,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   2100,
                                                                                   -73.0 ),
                                                                       ServoJoint(
                                                                                   1100,
                                                                                   0.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "eye_blink",
                                                   Joint(
                                                          1,
                                                          6,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1460,
                                                                                   -151.0 ),
                                                                       ServoJoint(
                                                                                   2000,
                                                                                   0.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "eye_lt_rt",
                                                   Joint(
                                                          1,
                                                          8,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1200,
                                                                                   -30.0 ),
                                                                       ServoJoint(
                                                                                   1800,
                                                                                   22.0 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "rt_foot_up",
                                                   Joint(
                                                          1,
                                                          10,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   900,
                                                                                   2.6 ),
                                                                       ServoJoint(
                                                                                   1675,
                                                                                   8.4 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "rt_foot_forward",
                                                   Joint(
                                                          1,
                                                          12,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   1800,
                                                                                   -4.5 ),
                                                                       ServoJoint(
                                                                                   900,
                                                                                   4.8 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "lt_foot_up",
                                                   Joint(
                                                          1,
                                                          14,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   900,
                                                                                   2.6 ),
                                                                       ServoJoint(
                                                                                   1675,
                                                                                   8.4 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "lt_foot_forward",
                                                   Joint(
                                                          1,
                                                          16,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   925,
                                                                                   -4.5 ),
                                                                       ServoJoint(
                                                                                   1900,
                                                                                   5.7 ) ),
                                                          true ) ) );
    joints_.insert(
                    std::pair<std::string, Joint>(
                                                   "base_turn",
                                                   Joint(
                                                          1,
                                                          18,
                                                          JointLimits(
                                                                       ServoJoint(
                                                                                   754,
                                                                                   -335.0 ),
                                                                       ServoJoint(
                                                                                   2254,
                                                                                   340.0 ) ),
                                                          true ) ) );
    /*
     joints_["mouth"] = Joint(0, 0, JointLimits(ServoJoint(1580, -40.0), ServoJoint(2100, 0.0)), true);
     joints_["head_nod"] = Joint(0, 2, JointLimits(ServoJoint(1120, -29.0), ServoJoint(2060, 45.0)), true);
     joints_["head_turn"] = Joint(0, 4, JointLimits(ServoJoint(1340, -30.0), ServoJoint(2100, 30.0)), true);
     joints_["rt_arm_forward"] = Joint(0, 6, JointLimits(ServoJoint(1000, -0.0), ServoJoint(2000, 80.0)), true);
     joints_["rt_arm_out"] = Joint(0, 8, JointLimits(ServoJoint(2000, -0.0), ServoJoint(1100, 75.0)), true);
     joints_["rt_elbow"] = Joint(0, 10, JointLimits(ServoJoint(1060, -0.0), ServoJoint(2100, 90.0)), true);
     joints_["lt_arm_forward"] = Joint(0, 12, JointLimits(ServoJoint(1200, -0.0), ServoJoint(2100, 80.0)), true);
     joints_["lt_arm_out"] = Joint(0, 14, JointLimits(ServoJoint(2100, -0.0), ServoJoint(1000, 72.0)), true);
     joints_["lt_elbow"] = Joint(0, 16, JointLimits(ServoJoint(1030, -0.0), ServoJoint(2100, 90.0)), true);
     joints_["rt_wrist"] = Joint(1, 0, JointLimits(ServoJoint(1910, -45.0), ServoJoint(1181, 60.0)), true);
     joints_["lt_wrist"] = Joint(1, 2, JointLimits(ServoJoint(2050, -35.0), ServoJoint(1535, 58.0)), true);
     joints_["torso_forebend"] = Joint(1, 4, JointLimits(ServoJoint(2100, -73.0), ServoJoint(1100, 0.0)), true);
     joints_["eye_blink"] = Joint(1, 6, JointLimits(ServoJoint(1460, -151.0), ServoJoint(2000, 0.0)), true);
     joints_["eye_lt_rt"] = Joint(1, 8, JointLimits(ServoJoint(1200, -30.0), ServoJoint(1800, 22.0)), true);
     joints_["rt_foot_up"] = Joint(1, 10, JointLimits(ServoJoint(900, 2.6), ServoJoint(1675, 8.4)), true);
     joints_["rt_foot_forward"] = Joint(1, 12, JointLimits(ServoJoint(1800, -4.5), ServoJoint(900, 4.8)), true);
     joints_["lt_foot_up"] = Joint(1, 14, JointLimits(ServoJoint(900, 2.6), ServoJoint(1675, 8.4)), true);
     joints_["lt_foot_forward"] = Joint(1, 16, JointLimits(ServoJoint(925, -4.5), ServoJoint(1900, 5.7)), true);
     joints_["base_turn"] = Joint(1, 18, JointLimits(ServoJoint(754, -335.0), ServoJoint(2254, 340.0)), true);
     */
    return true;
} // initJoints()

//
SparkyController::Joint::Joint( const uint16_t servo_device,
                                const uint16_t servo_channel,
                                const JointLimits limits, const bool active ) :
    servo_device_( servo_device ), servo_channel_( servo_channel ),
        limits_( limits ), active_( active )
{
} // Joint(const uint16_t, const uint16_t, const JointLimits, const bool)

//
SparkyController::Joint::Joint( const Joint &joint )
{
    servo_device_ = joint.servo_device_;
    servo_channel_ = joint.servo_channel_;
    limits_ = joint.limits_;
    active_ = joint.active_;
    angle_ = joint.angle_;
} // Joint(const Joint &)

//
bool SparkyController::Joint::isValidAngle( const double angle ) const
{
    return ( angle >= getMinAngle() ) && ( angle <= getMaxAngle() );
} // isValidAngle(const double)

//
double SparkyController::Joint::clipAngleValue( const double angle ) const
{
    double min_angle = getMinAngle();
    double max_angle = getMaxAngle();
    if ( angle < min_angle ) return min_angle;
    else if ( angle > max_angle ) return max_angle;
    return angle;
} // clipAngleValue(const double)

//
bool SparkyController::Joint::setAngle( double angle, const bool clip_angle )
{
    if ( clip_angle ) angle = clipAngleValue( angle );
    else if ( !isValidAngle( angle ) ) return false;
    angle_ = angle;
    return true;
} // setAngle(double, const bool)

//
double SparkyController::Joint::getMinAngle() const
{
    return std::min( limits_.first.second, limits_.second.second );
} // getMinAngle()

//
double SparkyController::Joint::getMaxAngle() const
{
    return std::max( limits_.first.second, limits_.second.second );
} // getMaxAngle()

void SparkyController::initFromYaml( YAML::Node const & node )
{
    joint_angle_controller_.initFromYaml( node );
}

void operator>>( YAML::Node const & node, sparky::SparkyController & sparky_controller )
{
    sparky_controller.initFromYaml( node );
}
