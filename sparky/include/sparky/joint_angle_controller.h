// preprocessor directives
#ifndef SPARKY_JOINT_ANGLE_CONTROLLER_H
#define SPARKY_JOINT_ANGLE_CONTROLLER_H
#include <iostream>
#include <sparky/servo_angle_controller.h>
#include <stdint.h>
#include <muParser/muParser.h>
#include <sparky/parsed_function.h>
#include <sparky/transfer_function.h>
//
namespace sparky
{


    // public type redefinitions
    typedef std::pair<double, double> JointAnglePair;
    typedef std::pair<JointAnglePair, JointAnglePair> JointAngleLimits;
    typedef sparky::TransferFunction _TransferFunction;

    struct Joint
    {
        JointAngleLimits limits_;
        std::string name_;
        double home_;
        std::map<std::string, double> parameters_;
        std::string servo_name_;

        Joint();

        Joint( std::string const & name, JointAngleLimits const & limits, double const & home );
    };

    //
    class JointAngleController : public pololu::maestro::ServoAngleController
    {
        public:


            // public constructors/destructors
            JointAngleController();
            JointAngleController( size_t const num_joints, const std::string path = "", const bool connect = false );
            JointAngleController( const std::string filename, const std::string path = "", const bool connect = false );
            JointAngleController( std::ifstream &fin, const std::string path = "", const bool connect = false );
            JointAngleController( YAML::Parser &parser, const std::string path = "", const bool connect = false );
            JointAngleController( const YAML::Node &node );
            JointAngleController( const JointAngleController &joint_angle_controller );

            // virtual public constructors/destructors
            virtual ~JointAngleController();

            static double jointAngleToCableDisplacement( double const & joint_angle, double const & joint_radius, double const & joint_length );
            static double cableDisplacementToServoAngle( double const & cable_displacement, double const & servo_radius, double const & piston_length );
            static double servoAngleToCableDisplacement( double const & servo_angle, double const & servo_radius, double const & piston_length );
            static double cableDisplacementToJointAngle( double const & cable_displacement, double const & joint_radius, double const & joint_length );

            // private utility functions
            bool isValidJointAngleTarget( std::string const & joint_name, const double target ) const;
            double clipJointAngleTargetValue( std::string const & joint_name, const double target ) const;
            double convertServoAngleToJointAngle( std::string const & joint_name, double servo_angle );
            double convertJointAngleToServoAngle( std::string const & joint_name, double joint_angle );

            // public mutator functions
            bool setJointAngleLimits( std::string const & joint_name, JointAnglePair limit1, JointAnglePair limit2 );
            bool setJointAngleLimits( std::string const & joint_name, JointAngleLimits limits );
            bool setJointAngleTarget( std::string const & joint_name, double target );
            bool setJointAngleSpeed( std::string const & joint_name, double speed );
            bool setJointAngleAcceleration( std::string const & joint_name, double accel );
            void setJointsHome();
            void setJointHome( std::string const & joint_name );

            // public accessor functions
            JointAngleLimits getJointAngleLimits( std::string const & joint_name ) const;
            JointAnglePair getJointAngleMinLimitPair( std::string const & joint_name ) const;
            JointAnglePair getJointAngleMaxLimitPair( std::string const & joint_name ) const;
            double getJointAngleMinLimit( std::string const & joint_name ) const;
            double getJointAngleMaxLimit( std::string const & joint_name ) const;
            double const & getJointAngleTarget( std::string const & joint_name );
            double const & getJointAngleSpeed( std::string const & joint_name );
            double const & getJointAngleAcceleration( std::string const & joint_name );
            double const & getJointAnglePosition( std::string const & joint_name );
            std::map<std::string, Joint> const & getJoints();

            // public overloaded operators
            JointAngleController & operator =( const JointAngleController &servo_controller );
            bool initFromYaml( YAML::Node const & node );

//            _TransferFunction & getTransferFunction( uint16_t const & device, uint16_t const & channel );
//            _TransferFunction const & getTransferFunction( uint16_t const & device, uint16_t const & channel ) const;
//            void setTransferFunctionInput( uint16_t const & device, uint16_t const & channel, double const & value );
//            double & getTransferFunctionInput( uint16_t const & device, uint16_t const & channel );
//            double const & getTransferFunctionInput( uint16_t const & device, uint16_t const & channel ) const;
//            mu::Parser & getTransferFunctionParser(  uint16_t const & device, uint16_t const & channel );
//            mu::Parser const & getTransferFunctionParser(  uint16_t const & device, uint16_t const & channel ) const;
//            double getTransferFunctionOutput( uint16_t const & device, uint16_t const & channel ) const;
//            double getTransferFunctionOutput( uint16_t const & device, uint16_t const & channel, double const & input );

        protected:
            std::map<std::string, Joint> joints_;

        private:
            // private initializer functions
            bool init( size_t const & num_joints );
    };// JointAngleController

} // sparky

// overloaded operators
void operator >>( const YAML::Node &node, sparky::JointAngleController &joint_angle_controller );
void operator >>( YAML::Node const & node, sparky::Joint & joint );
void operator >>( const YAML::Node &node, sparky::JointAngleLimits &joint_angle_limits );
void operator >>( const YAML::Node &node, sparky::JointAnglePair &joint_angle_pair );

#endif // SPARKY_JOINT_ANGLE_CONTROLLER_H
