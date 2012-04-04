// preprocessor directives
#ifndef MAESTRO_SERVO_CONTROLLER_H
#define MAESTRO_SERVO_CONTROLLER_H
#include <stdint.h>
#include <string>
#include <vector>

//
namespace pololu
{

// simple wrapper class for Pololu Maestro USB servo controllers
class MaestroServoController
{
public:

  // public type redefinitions
  typedef std::pair<uint16_t, uint16_t> ServoLimits;

  // public constructors/destructors
  MaestroServoController(const uint8_t n_channels, const std::string path = "", const bool connect = false);
  MaestroServoController(const uint8_t n_devices, const uint8_t n_channels_each, const std::string path = "",
                         const bool connect = false);
  MaestroServoController(const std::vector<uint8_t> n_device_channels, const std::string path = "",
                         const bool connect = false);

  // virtual public constructors/destructors
  virtual ~MaestroServoController();

  // public utility functions
  bool connect(const bool home = false);
  bool disconnect();
  bool isConnected() const;
  bool isServoMoving(const uint8_t channel);
  bool isServoMoving(const uint8_t device, const uint8_t channel);
  bool waitForServoDone(const uint8_t channel);
  bool waitForServoDone(const uint8_t device, const uint8_t channel);
  bool waitForServosDone();
  bool isValidServoTarget(const uint8_t channel, const uint16_t target) const;
  bool isValidServoTarget(const uint8_t device, const uint8_t channel, const uint16_t target) const;
  double clipServoTargetValue(const uint8_t channel, const uint16_t target) const;
  double clipServoTargetValue(const uint8_t device, const uint8_t channel, const uint16_t target) const;

  // public mutator functions
  bool setPath(std::string path, bool connect = false);
  bool setServoLimits(const uint8_t channel, ServoLimits limits);
  bool setServoLimits(const uint8_t device, const uint8_t channel, ServoLimits limits);
  bool setServoLimits(const uint8_t channel, uint16_t limit1, uint16_t limit2);
  bool setServoLimits(const uint8_t device, const uint8_t channel, uint16_t limit1, uint16_t limit2);
  bool setServoEnabled(const uint8_t channel, bool enabled);
  bool setServoEnabled(const uint8_t device, const uint8_t channel, bool enabled);
  bool setServoTarget(const uint8_t channel, uint16_t target);
  bool setServoTarget(const uint8_t device, uint8_t channel, uint16_t target);
  bool setServoSpeed(const uint8_t channel, uint16_t speed);
  bool setServoSpeed(const uint8_t device, const uint8_t channel, uint16_t speed);
  bool setServoAcceleration(const uint8_t channel, uint16_t accel);
  bool setServoAcceleration(const uint8_t device, const uint8_t channel, uint16_t accel);
  bool setServosHome();
  bool setServosHome(const uint8_t device);

  // public accessor functions
  std::string getPath() const;
  uint8_t getNumDevices() const;
  uint8_t getNumChannels(const uint8_t device = 0) const;
  uint8_t getNumServosEnabled() const;
  uint8_t getNumServosDisabled() const;
  ServoLimits getServoLimits(const uint8_t channel) const;
  ServoLimits getServoLimits(const uint8_t device, const uint8_t channel) const;
  uint16_t getServoMinLimit(const uint8_t channel) const;
  uint16_t getServoMinLimit(const uint8_t device, const uint8_t channel) const;
  uint16_t getServoMaxLimit(const uint8_t channel) const;
  uint16_t getServoMaxLimit(const uint8_t device, const uint8_t channel) const;
  bool getServoEnabled(const uint8_t channel) const;
  bool getServoEnabled(const uint8_t device, const uint8_t channel) const;
  uint16_t getServoTarget(const uint8_t channel) const;
  uint16_t getServoTarget(const uint8_t device, const uint8_t channel) const;
  uint16_t getServoSpeed(const uint8_t channel) const;
  uint16_t getServoSpeed(const uint8_t device, const uint8_t channel) const;
  uint16_t getServoAcceleration(const uint8_t channel) const;
  uint16_t getServoAcceleration(const uint8_t device, const uint8_t channel) const;
  uint16_t getServoPosition(const uint8_t channel);
  uint16_t getServoPosition(const uint8_t device, const uint8_t channel);
  bool getServoMovingState(const uint8_t channel);
  bool getServoMovingState(const uint8_t device, const uint8_t channel);
  bool getServosMovingState();
  bool getServosMovingState(const uint8_t device);
  uint16_t getServosErrors();
  uint16_t getServosErrors(const uint8_t device);

private:

  // private data structures
  struct Servo
  {

    // data members
    ServoLimits limits_;
    bool enabled_;
    uint16_t target_;
    uint16_t speed_;
    uint16_t accel_;

    // constructors/destructors
    Servo(const ServoLimits limits = ServoLimits(992, 2304), const bool enabled = false, const uint16_t target = 0,
          const uint16_t speed = 0, const uint16_t accel = 0);
    Servo(const Servo &servo);

    // utility functions
    bool isValidTarget(const uint16_t target) const;
    uint16_t clipTargetValue(const uint16_t target) const;

    // mutator functions
    bool setTarget(uint16_t target, const bool clip_target = false);

    // accessor functions
    uint16_t getMinLimit() const;
    uint16_t getMaxLimit() const;
  }; // Servo

  // private data members
  int fd_;
  std::string path_;
  std::vector<std::vector<Servo> > servos_;

  // private utility functions
  void flush() const;

  // private mutator functions
  bool setProperties();
};
// MaestroServoController
}
// pololu

#endif // MAESTRO_SERVO_CONTROLLER_H
