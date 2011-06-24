// preprocessor directives
#ifndef MAESTRO_SERVO_CONTROLLER_H
#define MAESTRO_SERVO_CONTROLLER_H
#include <stdint.h>
#include <string>

//
namespace pololu
{

// simple wrapper class for the Pololu Maestro USB servo controller
class MaestroServoController
{
public:

  // public constructors/destructors
  MaestroServoController(const std::string device_path = "", const size_t num_servos = 24, const bool connect = false);
  ~MaestroServoController();

  // public utility functions
  bool connect(const bool home = false);
  bool disconnect();
  bool isConnected() const;

  // public mutator functions
  bool setDevicePath(const std::string device_path, const bool connect = false);
  bool setNumServos(const size_t num_servos);
  bool setServoPosition(uint8_t channel, uint16_t pos);
  bool setServoPosition(uint8_t device, uint8_t channel, uint16_t pos);
  bool setServoPositionHome();
  bool setServoPositionHome(uint8_t device);
  bool setServoSpeed(uint8_t channel, uint16_t speed);
  bool setServoSpeed(uint8_t device, uint8_t channel, uint16_t speed);
  bool setServoAcceleration(uint8_t channel, uint16_t accel);
  bool setServoAcceleration(uint8_t device, uint8_t channel, uint16_t accel);

  // public accessor functions
  std::string getDevicePath() const;
  size_t getNumServos() const;
  uint16_t getServoPosition(uint8_t channel);
  uint16_t getServoPosition(uint8_t device, uint8_t channel);
  bool getServoMovingState(uint8_t channel);
  bool getServoMovingState(uint8_t device, uint8_t channel);
  uint16_t getServoErrors(uint8_t channel);
  uint16_t getServoErrors(uint8_t device, uint8_t channel);

private:

  // private data members
  std::string device_path_;int fd_;
  size_t num_servos_;

  // private utility functions
  void flush() const;

  // private mutator functions
  bool setProperties();
};
// MaestroServoController
}
// pololu

#endif // MAESTRO_SERVO_CONTROLLER_H
