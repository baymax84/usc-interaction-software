// preprocessor directives
#include <errno.h>		// for error codes, errno, strerror(), etc.
#include <fcntl.h>		// for file control, open(), O_RDWR, etc
#include <iostream>
#include <string.h>		// for strerror() and memset()
#include <termios.h>	// for flow control, cfsetispeed() etc.
#include <sparky/maestro_servo_controller.h>
using namespace pololu;

// can't be used until the device has been set (use setDevicePath())
// and then opened (use connect())
MaestroServoController::MaestroServoController(const std::string device_path, const bool connect) :
  device_path_(device_path), // (e.g., "/dev/ttyACM0", "/dev/ttyUSB0")
      fd_(-1) // indicate the file desciptor is not valid
{
  if (connect)
    MaestroServoController::connect();
} // MaestroServoController(const std::string, const bool)

// shuts down the servo controller, disconnecting if necessary
MaestroServoController::~MaestroServoController()
{
  if (isConnected())
    disconnect();
} // ~MaestroServoController()

// opens the file device, returning true if successful, false otherwise
bool MaestroServoController::connect(const bool home)
{
  fd_ = ::open(getDevicePath().c_str(), O_RDWR | O_NOCTTY);

  // set speed and other properties
  // on failure setProperties() calls disconnect()
  if (fd_ != -1)
    setProperties();
  else
  {

    // unable to open()
    std::cerr << "Error opening '" << getDevicePath() << "' : " << strerror(errno) << std::endl;
    disconnect();
  }

  // move to the home position
  if (home && isConnected())
    setServoPositionHome();

  return isConnected();
} // connect(const bool)

// closes the file device
bool MaestroServoController::disconnect()
{
  if (isConnected())
  {

    // close the port
    // try to be (sort of) thread safe
    int fd = fd_;
    fd_ = -1;
    ::close(fd);
  }
  return true;
} // disconnect()

// returns true if the file device is open and ready for I/O, false otherwise
bool MaestroServoController::isConnected() const
{
  return (fd_ != -1); // a valid file descriptor, fd_ != -1
} // isConnected()

//  sets the path of the device (e.g., "/dev/ttyACM0", "/dev/ttyUSB0", etc.)
bool MaestroServoController::setDevicePath(const std::string device_path, const bool connect)
{
  // disconnect from the device first if it's currently connected
  if (isConnected())
    disconnect();
  device_path_ = device_path;
  if (connect)
    return MaestroServoController::connect();
  return true;
} // setDevicePath(const std::string, const bool)

// commands an individual servo motor to move to a target position
bool MaestroServoController::setServoPosition(uint8_t channel, uint16_t pos)
{

  // check for errors
  if (!isConnected())
    return false;

  pos *= 4;

  // form the output packet
  static const int N_BYTES = 4;
  unsigned char cmd[N_BYTES] = {0x84, // Command byte: Set Target.
                                channel, // First data byte holds channel number
                                pos & 0x7F, // Second byte holds the lower 7 bits of target.
                                (pos >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoPosition(uint8_t, uint16_t)

// commands an individual servo motor to move to a target position
bool MaestroServoController::setServoPosition(uint8_t device, uint8_t channel, uint16_t pos)
{

  // check for errors
  if (!isConnected())
    return false;

  pos *= 4;

  // form the output packet
  static const int N_BYTES = 6;
  unsigned char cmd[N_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                device, // Device number.
                                0x04, // Command byte: Set Target.
                                channel, // First data byte holds channel number
                                pos & 0x7F, // Second byte holds the lower 7 bits of target.
                                (pos >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoPosition(uint8_t, uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized speed
bool MaestroServoController::setServoSpeed(uint8_t channel, uint16_t speed)
{

  // check for errors
  if (!isConnected())
    return false;

  //speed *= 4;

  // form the output packet
  static const int N_BYTES = 4;
  unsigned char cmd[N_BYTES] = {0x87, // Command byte: Set Speed.
                                channel, // First data byte holds channel number
                                speed & 0x7F, // Second byte holds the lower 7 bits of target.
                                (speed >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoSpeed(uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized speed
bool MaestroServoController::setServoSpeed(uint8_t device, uint8_t channel, uint16_t speed)
{

  // check for errors
  if (!isConnected())
    return false;

  //speed *= 4;

  // form the output packet
  static const int N_BYTES = 6;
  unsigned char cmd[N_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                device, // Device number.
                                0x07, // Command byte: Set Speed.
                                channel, // First data byte holds channel number
                                speed & 0x7F, // Second byte holds the lower 7 bits of target.
                                (speed >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoSpeed(uint8_t, uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized acceleration
bool MaestroServoController::setServoAcceleration(uint8_t channel, uint16_t accel)
{

  // check for errors
  if (!isConnected())
    return false;

  //accel *= 4;

  // form the output packet
  static const int N_BYTES = 4;
  unsigned char cmd[N_BYTES] = {0x89, // Command byte: Set Acceleration.
                                channel, // First data byte holds channel number
                                accel & 0x7F, // Second byte holds the lower 7 bits of target.
                                (accel >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoAcceleration(uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized acceleration
bool MaestroServoController::setServoAcceleration(uint8_t device, uint8_t channel, uint16_t accel)
{

  // check for errors
  if (!isConnected())
    return false;

  //accel *= 4;

  // form the output packet
  static const int N_BYTES = 6;
  unsigned char cmd[N_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                device, // Device number.
                                0x09, // Command byte: Set Acceleration.
                                channel, // First data byte holds channel number
                                accel & 0x7F, // Second byte holds the lower 7 bits of target.
                                (accel >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoAcceleration(uint8_t, uint8_t, uint16_t)

// moves all 16 motors to their home (mid stroke) position
bool MaestroServoController::setServoPositionHome()
{
  return false;
} // setServoPositionHome()

//  returns the path of the device (e.g., "/dev/ttyACM0", "/dev/ttyUSB0", etc.)
std::string MaestroServoController::getDevicePath() const
{
  return device_path_;
} // getDevicePath()

// flushes all data to the actual device
// (as opposed to being buffered in memory)
void MaestroServoController::flush() const
{
  if (isConnected())
    fdatasync(fd_);
} // flush()

//  sets the speed and other properties of the tty device
//  (note: used internally by connect)
bool MaestroServoController::setProperties()
{
  if (!isConnected())
    return false;

  // overkill on the initialization
  struct termios options;
  tcgetattr(fd_, &options);
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

  // what's this?
  tcflush(fd_, TCIOFLUSH);

  // actually set the properties
  if (tcsetattr(fd_, TCSANOW, &options) != 0)
  {

    // unable to set servo's properties
    std::cerr << "Unable to set '" << getDevicePath() << "' properties : " << strerror(errno) << std::endl;
    disconnect();
    return false;
  }

  return true;
} // setProperties()
