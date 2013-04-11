// preprocessor directives
#ifndef SPARKY_CONTROLLER_H
#define SPARKY_CONTROLLER_H
#include <map>
#include <stdint.h>
#include <string>
#include <sparky/joint_angle_controller.h>

// a namespace for the Sparky Disney Minimatronic robot platform
namespace sparky
{


    // a class describing the controller for the
    // Sparky Disney Minimatronic robot platform
    class SparkyController
    {
    public:
        // public type redefinitions
        typedef std::pair<uint16_t, double> ServoJoint;
        typedef std::pair<ServoJoint, ServoJoint> JointLimits;

        // public constructors/destructors
        SparkyController();
        SparkyController( YAML::Node const & node, bool const & auto_connect = false, bool const & home = false );
        ~SparkyController();

        // public utility functions
        bool connect( const bool home = false );
        bool disconnect();
        bool isConnected();
        bool isJointMoving( const std::string name );
        bool waitForJointDone( const std::string name ) const;
        bool waitForJointsDone();

        // public mutator functions
        bool setPath( const std::string name );
        bool setJointLimits( const std::string name, const JointLimits limits );
        bool setJointLimits( const std::string name, const ServoJoint limit1, const ServoJoint limit2 );
        bool setJointActive( const std::string name, const bool active );
        bool setJointAngle( const std::string name, double angle );
        bool setJointsHome();

        // public accessor functions
        std::string getPath() const;
        size_t getNumJoints() const;
        size_t getNumJointsActive() const;
        size_t getNumJointsInactive() const;
        JointLimits getJointLimits( const std::string name ) const;
        double getJointMinAngle( const std::string name ) const;
        double getJointMaxAngle( const std::string name ) const;
        bool getJointActive( const std::string name ) const;
        double getJointAngle( const std::string name ) const;
        bool getJointMoving( const std::string name );
        bool getJointsMoving();
        uint16_t getErrors();
        void initFromYaml( YAML::Node const & node );

        JointAngleController joint_angle_controller_;

    private:
        // private data structures
        struct Joint
        {
            // data members
            uint16_t servo_device_;
            uint16_t servo_channel_;
            JointLimits limits_;
            bool active_;
            double angle_;

            // constructors/destructors
            Joint( const uint16_t servo_device, const uint16_t servo_channel,
                   const JointLimits limits, const bool active = false );
            Joint( const Joint &joint );

            // utility functions
            bool isValidAngle( const double angle ) const;
            double clipAngleValue( const double angle ) const;

            // mutator functions
            bool setAngle( double angle, const bool clip_angle = false );

            // accessor functions
            double getMinAngle() const;
            double getMaxAngle() const;
        }; // Joint

        // private data members
        std::map<std::string, Joint> joints_;

        // private utility functions
        bool initJoints();
    };
// SparkyController
}
// sparky

void operator>>( YAML::Node const & node, sparky::SparkyController & sparky_controller );
#endif  // SPARKY_CONTROLLER_H
