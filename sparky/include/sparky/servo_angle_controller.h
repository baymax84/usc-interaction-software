// preprocessor directives
#ifndef POLOLU_MAESTRO_SERVO_ANGLE_CONTROLLER_H
#define POLOLU_MAESTRO_SERVO_ANGLE_CONTROLLER_H
#include <iostream>
#include <sparky/servo_controller.h>
#include <stdint.h>

//
namespace pololu
{


    //
    namespace maestro
    {


        // public type redefinitions
        typedef std::pair<uint16_t, double> ServoAnglePair;
        typedef std::pair<ServoAnglePair, ServoAnglePair> ServoAngleLimits;


        //
        class ServoAngleController: public ServoController
        {
            public:


                // public constructors/destructors
                ServoAngleController();
                ServoAngleController( const uint8_t n_channels, const std::string path = "", const bool connect = false );
                ServoAngleController( const uint8_t n_devices, const uint8_t n_channels_each, const std::string path = "", const bool connect = false );
                ServoAngleController( const std::vector<uint8_t> n_device_channels, const std::string path = "", const bool connect = false );
                ServoAngleController( const std::string filename, const std::string path = "", const bool connect = false );
                ServoAngleController( std::ifstream &fin, const std::string path = "", const bool connect = false );
                ServoAngleController( YAML::Parser &parser, const std::string path = "", const bool connect = false );
                ServoAngleController( const YAML::Node &node );
                ServoAngleController( const ServoAngleController &servo_angle_controller );

                // virtual public constructors/destructors
                virtual ~ServoAngleController();

                // private utility functions
                bool isValidServoAngleTarget( const uint8_t channel, const double target ) const;
                bool isValidServoAngleTarget( const uint8_t device, const uint8_t channel, const double target ) const;
                double clipServoAngleTargetValue( const uint8_t channel, const double target ) const;
                double clipServoAngleTargetValue( const uint8_t device, const uint8_t channel, const double target ) const;
                double convertServoToAngle( const uint8_t channel, uint16_t pos ) const;
                double convertServoToAngle( const uint8_t device, const uint8_t channel, uint16_t pos ) const;
                uint16_t convertAngleToServo( const uint8_t channel, double angle ) const;
                uint16_t convertAngleToServo( const uint8_t device, const uint8_t channel, double angle ) const;

                // public mutator functions
                bool setServoAngleLimits( const uint8_t channel, ServoAngleLimits limits );
                bool setServoAngleLimits( const uint8_t device, const uint8_t channel, ServoAngleLimits limits );
                bool setServoAngleLimits( const uint8_t channel, ServoAnglePair limit1, ServoAnglePair limit2 );
                bool setServoAngleLimits( const uint8_t device, const uint8_t channel, ServoAnglePair limit1, ServoAnglePair limit2 );
                bool setServoAngleTarget( const uint8_t channel, double target );
                bool setServoAngleTarget( const uint8_t device, uint8_t channel, double target );
                bool setServoAngleSpeed( const uint8_t channel, double speed );
                bool setServoAngleSpeed( const uint8_t device, const uint8_t channel, double speed );
                bool setServoAngleAcceleration( const uint8_t channel, double accel );
                bool setServoAngleAcceleration( const uint8_t device, const uint8_t channel, double accel );

                // public accessor functions
                ServoAngleLimits getServoAngleLimits( const uint8_t channel ) const;
                ServoAngleLimits getServoAngleLimits( const uint8_t device, const uint8_t channel ) const;
                ServoAnglePair getServoAngleMinLimitPair( const uint8_t channel ) const;
                ServoAnglePair getServoAngleMinLimitPair( const uint8_t device, const uint8_t channel ) const;
                ServoAnglePair getServoAngleMaxLimitPair( const uint8_t channel ) const;
                ServoAnglePair getServoAngleMaxLimitPair( const uint8_t device, const uint8_t channel ) const;
                double getServoAngleMinLimit( const uint8_t channel ) const;
                double getServoAngleMinLimit( const uint8_t device, const uint8_t channel ) const;
                double getServoAngleMaxLimit( const uint8_t channel ) const;
                double getServoAngleMaxLimit( const uint8_t device, const uint8_t channel ) const;
                double getServoAngleTarget( const uint8_t channel );
                double getServoAngleTarget( const uint8_t device, const uint8_t channel );
                double getServoAngleSpeed( const uint8_t channel );
                double getServoAngleSpeed( const uint8_t device, const uint8_t channel );
                double getServoAngleAcceleration( const uint8_t channel );
                double getServoAngleAcceleration( const uint8_t device, const uint8_t channel );
                double getServoAnglePosition( const uint8_t channel );
                double getServoAnglePosition( const uint8_t device, const uint8_t channel );

                // public overloaded operators
                ServoAngleController
                & operator =( const ServoAngleController &servo_controller );
                void initFromYaml( YAML::Node const & node );

            protected:
                std::vector<std::vector<ServoAngleLimits> > servos_servo_angle_limits_;

            private:
                // private initializer functions
                bool init();
                bool init( const uint8_t n_devices, const uint8_t n_channels_each );
                bool init( const std::vector<uint8_t> n_device_channels );
        };// ServoAngleController

    } // maestro

} // pololu

// overloaded operators
void operator >>( const YAML::Node &node, pololu::maestro::ServoAngleController &servo_angle_controller );
void operator >>( const YAML::Node &node, pololu::maestro::ServoAngleLimits &servo_angle_limits );
void operator >>( const YAML::Node &node, pololu::maestro::ServoAnglePair &servo_angle_pair );

#endif // POLOLU_MAESTRO_SERVO_ANGLE_CONTROLLER_H
