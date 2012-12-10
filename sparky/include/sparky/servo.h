// preprocessor directives
#ifndef POLOLU_MAESTRO_SERVO_H
#define POLOLU_MAESTRO_SERVO_H
#include <algorithm>  // for std::max, std::min, and std::pair
#include <stdint.h>   // for uint16_t
#include <yaml-cpp/yaml.h>

//
namespace pololu
{


	//
	namespace maestro
	{


		// public type redefinitions
		typedef std::pair<uint16_t, uint16_t> ServoLimits;


		// simple wrapper data structure for servos used by Pololu Maestro USB servo controllers
		struct Servo
		{


				// data members
				ServoLimits limits_;
				bool enabled_;
				uint16_t target_;
				uint16_t speed_;
				uint16_t accel_;

				// constructors/destructors
				Servo( const ServoLimits limits = ServoLimits( 0, 4095 ),
				       const bool enabled = false, const uint16_t target = 0,
				       const uint16_t speed = 0, const uint16_t accel = 0 );
				Servo( const Servo &servo );

				// utility functions
				bool isValid() const;
				bool isValidTarget( const uint16_t target ) const;
				uint16_t clipTargetValue( const uint16_t target ) const;

				// mutator functions
				bool setTarget( uint16_t target, const bool clip_target = false );

				// accessor functions
				uint16_t getMinLimit() const;
				uint16_t getMaxLimit() const;

				// overloaded operators
				Servo& operator =( const Servo &servo );
		};// Servo

	} // maestro

} // pololu

// overloaded operators
void operator >>( const YAML::Node &node, pololu::maestro::Servo &servo );
void operator >>( const YAML::Node &node,
                  pololu::maestro::ServoLimits &servo_limits );

#endif // POLOLU_MAESTRO_SERVO_H
