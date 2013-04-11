// preprocessor directives
#ifndef POLOLU_MAESTRO_SERVO_CONTROLLER_H
#define POLOLU_MAESTRO_SERVO_CONTROLLER_H
#include <assert.h>  // for assert()
#include <errno.h>  // for error codes, errno, strerror(), etc.
#include <fcntl.h>  // for file control, open(), O_RDWR, etc
#include <fstream>  // for ifstream
#include <string.h> // for strerror() and memset()
#include <termios.h>    // for flow control, cfsetispeed(), etc.
#include <sparky/servo.h>
#include <stdint.h>
#include <string>
#include <vector>

//
namespace pololu
{


    //
    namespace maestro
    {


        // simple wrapper class for Pololu Maestro USB servo controllers
        class ServoController
        {
            public:


                // public constructors/destructors
                ServoController();
                ServoController( const uint8_t n_channels, const std::string path = "", const bool connect = false );
                ServoController( const uint8_t n_devices, const uint8_t n_channels_each, const std::string path = "", const bool connect = false );
                ServoController( const std::vector<uint8_t> n_device_channels, const std::string path = "", const bool connect = false );
                ServoController( const std::string filename, const std::string path = "", const bool connect = false );
                ServoController( std::ifstream &fin, const std::string path = "", const bool connect = false );
                ServoController( YAML::Parser &parser, const std::string path = "", const bool connect = false );
                ServoController( const YAML::Node &node );
                ServoController( const ServoController &servo_controller );

                // virtual public constructors/destructors
                virtual ~ServoController();

                // public utility functions
                bool connect( const bool home = false );
                bool disconnect();
                bool isConnected() const;
                bool isServoMoving( const uint8_t channel );
                bool isServoMoving( const uint8_t device, const uint8_t channel );
                bool waitForServoDone( const uint8_t channel );
                bool waitForServoDone( const uint8_t device, const uint8_t channel );
                bool waitForServosDone();
                bool isValidDevice( const uint8_t device ) const;
                bool isValidChannel( const uint8_t channel ) const;
                bool isValidChannel( const uint8_t device, const uint8_t channel ) const;
                bool isValidServoTarget( const uint8_t channel, const uint16_t target ) const;
                bool isValidServoTarget( const uint8_t device, const uint8_t channel, const uint16_t target ) const;
                double clipServoTargetValue( const uint8_t channel, const uint16_t target ) const;
                double clipServoTargetValue( const uint8_t device, const uint8_t channel, const uint16_t target ) const;

                // public mutator functions
                bool setPath( std::string path, bool connect = false );
                bool setServo( const uint8_t channel, Servo servo );
                bool setServo( const uint8_t device, const uint8_t channel, Servo servo );
                bool setServoLimits( const uint8_t channel, ServoLimits limits );
                bool setServoLimits( const uint8_t device, const uint8_t channel, ServoLimits limits );
                bool setServoLimits( const uint8_t channel, uint16_t limit1, uint16_t limit2 );
                bool setServoLimits( const uint8_t device, const uint8_t channel, uint16_t limit1, uint16_t limit2 );
                bool setServoEnabled( const uint8_t channel, bool enabled );
                bool setServoEnabled( const uint8_t device, const uint8_t channel, bool enabled );
                bool setServoTarget( const uint8_t channel, uint16_t target );
                bool setServoTarget( const uint8_t device, uint8_t channel, uint16_t target );
                bool setServoSpeed( const uint8_t channel, uint16_t speed );
                bool setServoSpeed( const uint8_t device, const uint8_t channel, uint16_t speed );
                bool setServoAcceleration( const uint8_t channel, uint16_t accel );
                bool setServoAcceleration( const uint8_t device, const uint8_t channel, uint16_t accel );
                bool setServosHome();
                bool setServosHome( const uint8_t device );

                // public accessor functions
                std::string getPath() const;
                uint8_t getNumDevices() const;
                uint8_t getNumChannels( const uint8_t device = 0 ) const;
                uint8_t getNumServosEnabled() const;
                uint8_t getNumServosDisabled() const;
                Servo getServo( const uint8_t channel ) const;
                Servo getServo( const uint8_t device, const uint8_t channel ) const;
                ServoLimits getServoLimits( const uint8_t channel ) const;
                ServoLimits getServoLimits( const uint8_t device, const uint8_t channel ) const;
                uint16_t getServoMinLimit( const uint8_t channel ) const;
                uint16_t getServoMinLimit( const uint8_t device, const uint8_t channel ) const;
                uint16_t getServoMaxLimit( const uint8_t channel ) const;
                uint16_t getServoMaxLimit( const uint8_t device, const uint8_t channel ) const;
                bool getServoEnabled( const uint8_t channel ) const;
                bool getServoEnabled( const uint8_t device, const uint8_t channel ) const;
                uint16_t getServoTarget( const uint8_t channel ) const;
                uint16_t getServoTarget( const uint8_t device, const uint8_t channel ) const;
                uint16_t getServoSpeed( const uint8_t channel ) const;
                uint16_t getServoSpeed( const uint8_t device, const uint8_t channel ) const;
                uint16_t getServoAcceleration( const uint8_t channel ) const;
                uint16_t getServoAcceleration( const uint8_t device, const uint8_t channel ) const;
                uint16_t getServoPosition( const uint8_t channel );
                uint16_t getServoPosition( const uint8_t device, const uint8_t channel );
                bool getServoMovingState( const uint8_t channel );
                bool getServoMovingState( const uint8_t device, const uint8_t channel );
                bool getServosMovingState();
                bool getServosMovingState( const uint8_t device );
                uint16_t getServosErrors();
                uint16_t getServosErrors( const uint8_t device );
                std::map<std::string, std::pair<uint8_t, uint8_t> > const & getServoNamesMap();

                // public overloaded operators
                ServoController& operator =( const ServoController &servo_controller );
                bool initFromYaml( const YAML::Node &node );

            protected:
                int fd_;
                std::string path_;
                std::vector<std::vector<Servo> > servos_;
                std::map<std::string, std::pair<uint8_t, uint8_t> > servo_names_map_;

            private:

                // private utility functions
                void flush() const;

                // private mutator functions
                bool setProperties();

                // private initializer functions
                bool init( const uint8_t n_devices, const uint8_t n_channels_each );
                bool init( const std::vector<uint8_t> n_device_channels );
                bool load( const std::string filename );
                bool load( std::ifstream &fin );
                bool load( YAML::Parser &parser );
        };// ServoController

    } // maestro

} // pololu

// overloaded operators
void operator >>( const YAML::Node &node, pololu::maestro::ServoController &servo_controller );
void operator >>( const YAML::Node &node, std::vector<uint8_t> &v );
void operator >>( const YAML::Node &node, uint8_t &i );

#endif // POLOLU_MAESTRO_SERVO_CONTROLLER_H
