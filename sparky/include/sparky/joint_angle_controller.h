// preprocessor directives
#ifndef SPARKY_JOINT_ANGLE_CONTROLLER_H
#define SPARKY_JOINT_ANGLE_CONTROLLER_H
#include <iostream>
#include <sparky/servo_angle_controller.h>
#include <stdint.h>
#include <muParser/muParser.h>
//
namespace sparky
{


    // public type redefinitions
    typedef std::pair<double, double> JointAnglePair;
    typedef std::pair<JointAnglePair, JointAnglePair> JointAngleLimits;
    typedef std::pair<double, mu::Parser> _TransferFunction;

    //
    class JointAngleController : public pololu::maestro::ServoAngleController
    {
        public:


            // public constructors/destructors
            JointAngleController( const uint8_t n_channels, const std::string path =
                "", const bool connect = false );
            JointAngleController( const uint8_t n_devices,
                                  const uint8_t n_channels_each,
                                  const std::string path = "", const bool connect =
                                      false );
            JointAngleController( const std::vector<uint8_t> n_device_channels,
                                  const std::string path = "", const bool connect =
                                      false );
            JointAngleController( const std::string filename, const std::string path =
                "", const bool connect = false );
            JointAngleController( std::ifstream &fin, const std::string path = "",
                                  const bool connect = false );
            JointAngleController( YAML::Parser &parser, const std::string path = "",
                                  const bool connect = false );
            JointAngleController( const YAML::Node &node,
                                  const std::string path = "", const bool connect =
                                      false );
            JointAngleController( const JointAngleController &joint_angle_controller );

            // virtual public constructors/destructors
            virtual ~JointAngleController();

            // private utility functions
            bool
                isValidJointAngleTarget( const uint8_t channel, const double target ) const;
            bool
            isValidJointAngleTarget( const uint8_t device, const uint8_t channel,
                                     const double target ) const;
            double
                clipJointAngleTargetValue( const uint8_t channel, const double target ) const;
            double
            clipJointAngleTargetValue( const uint8_t device, const uint8_t channel,
                                       const double target ) const;
            double
            convertServoAngleToJointAngle( const uint8_t channel, double servo_angle );
            double
            convertServoAngleToJointAngle( const uint8_t device,
                                           const uint8_t channel, double servo_angle );
            double convertJointAngleToServoAngle( const uint8_t channel,
                                                    double joint_angle );
            double convertJointAngleToServoAngle( const uint8_t device,
                                                    const uint8_t channel,
                                                    double joint_angle );

            // public mutator functions
            bool setJointAngleLimits( const uint8_t channel, JointAngleLimits limits );
            bool setJointAngleLimits( const uint8_t device, const uint8_t channel,
                                      JointAngleLimits limits );
            bool setJointAngleLimits( const uint8_t channel, JointAnglePair limit1,
                                      JointAnglePair limit2 );
            bool setJointAngleLimits( const uint8_t device, const uint8_t channel,
                                      JointAnglePair limit1, JointAnglePair limit2 );
            bool setJointAngleTarget( const uint8_t channel, double target );
            bool setJointAngleTarget( const uint8_t device, uint8_t channel,
                                      double target );
            bool setJointAngleSpeed( const uint8_t channel, double speed );
            bool setJointAngleSpeed( const uint8_t device, const uint8_t channel,
                                     double speed );
            bool setJointAngleAcceleration( const uint8_t channel, double accel );
            bool setJointAngleAcceleration( const uint8_t device,
                                            const uint8_t channel, double accel );

            // public accessor functions
            JointAngleLimits getJointAngleLimits( const uint8_t channel ) const;
            JointAngleLimits getJointAngleLimits( const uint8_t device,
                                                  const uint8_t channel ) const;
            JointAnglePair getJointAngleMinLimitPair( const uint8_t channel ) const;
            JointAnglePair getJointAngleMinLimitPair( const uint8_t device,
                                                      const uint8_t channel ) const;
            JointAnglePair getJointAngleMaxLimitPair( const uint8_t channel ) const;
            JointAnglePair getJointAngleMaxLimitPair( const uint8_t device,
                                                      const uint8_t channel ) const;
            double getJointAngleMinLimit( const uint8_t channel ) const;
            double
            getJointAngleMinLimit( const uint8_t device, const uint8_t channel ) const;
            double getJointAngleMaxLimit( const uint8_t channel ) const;
            double
            getJointAngleMaxLimit( const uint8_t device, const uint8_t channel ) const;
            double getJointAngleTarget( const uint8_t channel );
            double
            getJointAngleTarget( const uint8_t device, const uint8_t channel );
            double getJointAngleSpeed( const uint8_t channel );
            double getJointAngleSpeed( const uint8_t device, const uint8_t channel );
            double getJointAngleAcceleration( const uint8_t channel );
            double getJointAngleAcceleration( const uint8_t device,
                                              const uint8_t channel );
            double getJointAnglePosition( const uint8_t channel );
            double
            getJointAnglePosition( const uint8_t device, const uint8_t channel );

            // public overloaded operators
            JointAngleController
            & operator =( const JointAngleController &servo_controller );

            _TransferFunction & getTransferFunction( uint8_t const & device, uint8_t const & channel );
            _TransferFunction const & getTransferFunction( uint8_t const & device, uint8_t const & channel ) const;
            void setTransferFunctionInput( uint8_t const & device, uint8_t const & channel, double const & value );
            double & getTransferFunctionInput( uint8_t const & device, uint8_t const & channel );
            double const & getTransferFunctionInput( uint8_t const & device, uint8_t const & channel ) const;
            mu::Parser & getTransferFunctionParser(  uint8_t const & device, uint8_t const & channel );
            mu::Parser const & getTransferFunctionParser(  uint8_t const & device, uint8_t const & channel ) const;
            double getTransferFunctionOutput( uint8_t const & device, uint8_t const & channel ) const;
            double getTransferFunctionOutput( uint8_t const & device, uint8_t const & channel, double const & input );

        private:


            // private data members
            std::vector<std::vector<JointAngleLimits> > joint_angle_limits_;
            std::vector<std::vector<_TransferFunction> > transfer_functions_;

            // private initializer functions
            bool init();
            bool init( const uint8_t n_devices, const uint8_t n_channels_each );
            bool init( const std::vector<uint8_t> n_device_channels );
    };// JointAngleController

} // sparky

// overloaded operators
void operator >>( const YAML::Node &node, sparky::JointAngleController &joint_angle_controller );
void operator >>( const YAML::Node &node, sparky::JointAngleLimits &joint_angle_limits );
void operator >>( const YAML::Node &node, sparky::JointAnglePair &joint_angle_pair );

#endif // SPARKY_JOINT_ANGLE_CONTROLLER_H
