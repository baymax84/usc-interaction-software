// preprocessor directives
#include <assert.h>
#include <errno.h>	// for error codes, errno, strerror(), etc.
#include <fcntl.h>	// for file control, open(), O_RDWR, etc
#include <iostream>
#include <string.h>	// for strerror() and memset()
#include <termios.h>	// for flow control, cfsetispeed() etc.
#include <sparky/maestro_servo_controller.h>
using namespace pololu;
using namespace std;

// can't be used until the device has been set (use setDevicePath())
// and then opened (use connect())
MaestroServoController::MaestroServoController(const uint8_t n_channels, const std::string path, const bool connect) :
  fd_(-1), // indicate the file descriptor is not valid,
      path_(path) // (e.g., "/dev/ttyACM0", "/dev/ttyUSB0")

{
  servos_.resize(1);
  servos_[0].resize(n_channels);

  if (connect)
    MaestroServoController::connect();
} // MaestroServoController(const uint8_t, const std::string, const bool)

// can't be used until the device has been set (use setDevicePath())
// and then opened (use connect())
MaestroServoController::MaestroServoController(const uint8_t n_devices, const uint8_t n_channels_each,
                                               const std::string path, const bool connect) :
  fd_(-1), // indicate the file descriptor is not valid
      path_(path) // (e.g., "/dev/ttyACM0", "/dev/ttyUSB0")
{
  assert((n_devices > 0) && (n_channels_each > 0));
  servos_.resize(n_devices);
  for (int i = 0; i < n_devices; ++i)
    servos_[i].resize(n_channels_each);

  if (connect)
    MaestroServoController::connect();
} // MaestroServoController(const uint8_t, const uint8_t, const std::string, const bool)

// can't be used until the device has been set (use setDevicePath())
// and then opened (use connect())
MaestroServoController::MaestroServoController(const vector<uint8_t> n_device_channels, const std::string path,
                                               const bool connect) :
  fd_(-1), // indicate the file descriptor is not valid
      path_(path) // (e.g., "/dev/ttyACM0", "/dev/ttyUSB0")
{
  int n_devices = n_device_channels.size();
  assert(n_devices > 0);
  servos_.resize(n_devices);
  for (int i = 0; i < n_devices; ++i)
  {
    assert(n_device_channels[i] > 0);
    servos_[i].resize(n_device_channels[i]);
  }

  if (connect)
    MaestroServoController::connect();
} // MaestroServoController(const uint8_t, const uint8_t, const std::string, const bool)

// shuts down the servo controller, disconnecting if necessary
MaestroServoController::~MaestroServoController()
{
  if (isConnected())
    disconnect();
} // ~MaestroServoController()

// opens the file device, returning true if successful, false otherwise
bool MaestroServoController::connect(const bool home)
{
  // configuration options:
  // - 0_RDWR - we need read and write access
  // - 0_CTTY - prevent other input like keyboard from affecting what we read
  // - 0_NDELAY - we don't care if the other side is connected (some devices don't explicitly connect)
  //fd_ = ::open(getPath().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  fd_ = ::open(getPath().c_str(), O_RDWR | O_NOCTTY);

  // set speed and other properties
  // on failure setProperties() calls disconnect()
  if (fd_ != -1)
    setProperties();
  else
  {

    // unable to open()
    std::cerr << "Error opening '" << getPath() << "' : " << strerror(errno) << std::endl;
    disconnect();
  }

  // move to the home position
  if (home && isConnected())
    setServosHome();

  return isConnected();
} // connect(const bool)

// closes the file device
bool MaestroServoController::disconnect()
{
  if (isConnected())
  {
    for (int device = 0, n_devices = getNumDevices(); device < n_devices; ++device)
      for (int channel = 0, n_channels = getNumChannels(device); channel < n_channels; ++channel)
        setServoEnabled(device, channel, false);

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

//
bool MaestroServoController::isServoMoving(const uint8_t channel)
{
  if ((!isConnected()) || (channel >= getNumChannels(0)))
    return false;
  return servos_[0][channel].enabled_ && (servos_[0][channel].target_ != getServoPosition(channel));
} // isServoMoving(const uint8_t)

//
bool MaestroServoController::isServoMoving(const uint8_t device, const uint8_t channel)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return servos_[device][channel].enabled_ && (servos_[device][channel].target_ != getServoPosition(device, channel));
} // isServoMoving(const uint8_t, const uint8_t)

//
bool MaestroServoController::waitForServoDone(const uint8_t channel) const
{
  return waitForServoDone(0, channel);
} // waitForServoDone(const uint8_t)

//
bool MaestroServoController::waitForServoDone(const uint8_t device, const uint8_t channel) const
{
  // NOTE: NOT IMPLEMENTED!!!
  return false;
} // waitForServoDone(const uint8_t, const uint8_t)

//
bool MaestroServoController::waitForServosDone()
{
  if (!isConnected())
    return false;
  bool servos_moving = true;
  while (servos_moving)
  {
    servos_moving = getServosMovingState();
    for (int i = 1, n_devices = getNumDevices(); (!servos_moving) && (i < n_devices); ++i)
      servos_moving = getServosMovingState(i);
  }
  return true;
} // waitForServosDone()

//  sets the path of the device (e.g., "/dev/ttyACM0", "/dev/ttyUSB0", etc.)
bool MaestroServoController::setPath(const std::string path, const bool connect)
{
  // disconnect from the device first if it's currently connected
  if (isConnected())
    disconnect();
  path_ = path;
  if (connect)
    return MaestroServoController::connect();
  return true;
} // setPath(const std::string, const bool)

//
bool MaestroServoController::setServoLimits(const uint8_t channel, ServoLimits limits)
{
  return setServoLimits(0, channel, limits);
} // setServoLimits(const uint8_t, ServoLimits)

//
bool MaestroServoController::setServoLimits(const uint8_t device, const uint8_t channel, ServoLimits limits)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  servos_[device][channel].limits_ = limits;
  return true;
} // setServoLimits(const uint8_t, const uint8_t, ServoLimits)

//
bool MaestroServoController::setServoLimits(const uint8_t channel, const uint16_t limit1, const uint16_t limit2)
{
  return setServoLimits(0, channel, limit1, limit2);
} // setServoLimits(const uint8_t, const uint16_t, const uint16_t)

//
bool MaestroServoController::setServoLimits(const uint8_t device, const uint8_t channel, const uint16_t limit1, const uint16_t limit2)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  servos_[device][channel].limits_ = ServoLimits(limit1, limit2);
  return true;
} // setServoLimits(const uint8_t, const uint8_t, const uint16_t, const uint16_t)

//
bool MaestroServoController::setServoEnabled(const uint8_t channel, bool enabled)
{
  return setServoEnabled(0, channel, enabled);
} // setServoLimits(const uint8_t, bool)

//
bool MaestroServoController::setServoEnabled(const uint8_t device, const uint8_t channel, bool enabled)
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  servos_[device][channel].enabled_ = enabled;

  if (enabled)
  {
    setServoAcceleration(device, channel, servos_[device][channel].accel_);
    setServoSpeed(device, channel, servos_[device][channel].speed_);
    if (servos_[device][channel].isValidTarget(servos_[device][channel].target_))
      setServoTarget(device, channel, servos_[device][channel].target_);
  }

  return true;
} // setServoLimits(const uint8_t, const uint8_t, bool)

// commands an individual servo motor to move to a target position
bool MaestroServoController::setServoTarget(const uint8_t channel, uint16_t target)
{

  // check for errors
  if ((!isConnected()) || (channel >= getNumChannels()))
    return false;

  target *= 4;

  // form the output packet
  static const int N_BYTES = 4;
  unsigned char cmd[N_BYTES] = {0x84, // Command byte: Set Target.
                                channel, // First data byte holds channel number
                                target & 0x7F, // Second byte holds the lower 7 bits of target.
                                (target >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoTarget(const uint8_t, uint16_t)

// commands an individual servo motor to move to a target position
bool MaestroServoController::setServoTarget(const uint8_t device, const uint8_t channel, uint16_t target)
{
  /*
   if (device == 0)
   return setServoTarget(channel, target);
   */
  // check for errors
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels()))
    return false;

  target *= 4;

  // form the output packet
  static const int N_BYTES = 6;
  unsigned char cmd[N_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                device, // Device number.
                                0x04, // Command byte: Set Target.
                                channel, // First data byte holds channel number
                                target & 0x7F, // Second byte holds the lower 7 bits of target.
                                (target >> 7) & 0x7F // Third data byte holds the bits 7-13 of target.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServoTarget(const uint8_t, const uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized speed
bool MaestroServoController::setServoSpeed(const uint8_t channel, uint16_t speed)
{

  // check for errors
  if ((!isConnected()) || (channel >= getNumChannels()))
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
} // setServoSpeed(const uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized speed
bool MaestroServoController::setServoSpeed(const uint8_t device, const uint8_t channel, uint16_t speed)
{
  /*
   if (device == 0)
   return setServoSpeed(channel, speed);
   */
  // check for errors
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels()))
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
} // setServoSpeed(const uint8_t, const uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized acceleration
bool MaestroServoController::setServoAcceleration(const uint8_t channel, uint16_t accel)
{

  // check for errors
  if ((!isConnected()) || (channel >= getNumChannels()))
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
} // setServoAcceleration(const uint8_t, uint16_t)

// commands an individual servo motor to move at the parameterized acceleration
bool MaestroServoController::setServoAcceleration(const uint8_t device, const uint8_t channel, uint16_t accel)
{
  /*
   if (device == 0)
   return setServoAcceleration(channel, accel);
   */
  // check for errors
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels()))
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
} // setServoAcceleration(const uint8_t, const uint8_t, uint16_t)

// moves all motors to their home (mid stroke) position
bool MaestroServoController::setServosHome()
{

  // check for errors
  if (!isConnected())
    return false;

  // form the output packet
  static const int N_BYTES = 1;
  unsigned char cmd[N_BYTES] = {0xA2}; // Command byte: Go Home.

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServosHome()

// moves all motors to their home (mid stroke) position
bool MaestroServoController::setServosHome(const uint8_t device)
{
  /*
   if (device == 0)
   return setServosHome();
   */
  // check for errors
  if ((!isConnected()) || (device >= getNumDevices()))
    return false;

  // form the output packet
  static const int N_BYTES = 3;
  unsigned char cmd[N_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                device, // Device number.
                                0x22, // Command byte: Go Home.
      };

  // success is being able to write the packet
  // (that's all the feedback we get)
  return (write(fd_, cmd, N_BYTES) == N_BYTES);
} // setServosHome(const uint8_t)

//  returns the path of the device (e.g., "/dev/ttyACM0", "/dev/ttyUSB0", etc.)
std::string MaestroServoController::getPath() const
{
  return path_;
} // getPath()

//
uint8_t MaestroServoController::getNumDevices() const
{
  return servos_.size();
} // getNumDevices()

//
uint8_t MaestroServoController::getNumChannels(const uint8_t device) const
{
  if (device >= getNumDevices())
    return 0;
  return servos_[device].size();
} // getNumChannels(const uint8_t)

//
uint8_t MaestroServoController::getNumServosEnabled() const
{
  uint8_t n_enabled = 0;
  for (int device = 0, n_devices = getNumDevices(); device < n_devices; ++device)
    for (int channel = 0, n_channels = getNumChannels(); channel < n_channels; ++channel)
      if (servos_[device][channel].enabled_)
        ++n_enabled;
  return n_enabled;
} // getNumServosEnabled()

//
uint8_t MaestroServoController::getNumServosDisabled() const
{
  uint8_t n_disabled = 0;
  for (int device = 0, n_devices = getNumDevices(); device < n_devices; ++device)
    for (int channel = 0, n_channels = getNumChannels(); channel < n_channels; ++channel)
      if (servos_[device][channel].enabled_)
        ++n_disabled;
  return n_disabled;
} // getNumServosDisabled()

//
MaestroServoController::ServoLimits MaestroServoController::getServoLimits(const uint8_t channel) const
{
  return getServoLimits(0, channel);
} // getServoLimits(const uint8_t)

//
MaestroServoController::ServoLimits MaestroServoController::getServoLimits(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return ServoLimits(0, 0);
  return servos_[device][channel].limits_;
} // getServoLimits(const uint8_t)

//
uint16_t MaestroServoController::getServoMinLimit(const uint8_t channel) const
{
  return getServoMinLimit(0, channel);
} // getServoMinLimit(const uint8_t)

//
uint16_t MaestroServoController::getServoMinLimit(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;
  return servos_[device][channel].getMinLimit();
} // getServoMinLimit(const uint8_t)

//
uint16_t MaestroServoController::getServoMaxLimit(const uint8_t channel) const
{
  return getServoMinLimit(0, channel);
} // getServoMaxLimit(const uint8_t)

//
uint16_t MaestroServoController::getServoMaxLimit(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;
  return servos_[device][channel].getMaxLimit();
} // getServoMaxLimit(const uint8_t)

//
bool MaestroServoController::getServoEnabled(const uint8_t channel) const
{
  return getServoEnabled(0, channel);
} // getServoEnabled(const uint8_t)

//
bool MaestroServoController::getServoEnabled(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return false;
  return servos_[device][channel].enabled_;
} // getServoEnabled(const uint8_t)

//
uint16_t MaestroServoController::getServoTarget(const uint8_t channel) const
{
  return getServoTarget(0, channel);
} // getServoTarget(const uint8_t)

//
uint16_t MaestroServoController::getServoTarget(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;
  return servos_[device][channel].target_;
} // getServoTarget(const uint8_t)

//
uint16_t MaestroServoController::getServoSpeed(const uint8_t channel) const
{
  return getServoSpeed(0, channel);
} // getServoSpeed(const uint8_t)

//
uint16_t MaestroServoController::getServoSpeed(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;
  return servos_[device][channel].speed_;
} // getServoSpeed(const uint8_t)

//
uint16_t MaestroServoController::getServoAcceleration(const uint8_t channel) const
{
  return getServoTarget(0, channel);
} // getServoAcceleration(const uint8_t)

//
uint16_t MaestroServoController::getServoAcceleration(const uint8_t device, const uint8_t channel) const
{
  if ((!isConnected()) || (device >= getNumDevices()) || (channel >= getNumChannels(device)))
    return 0;
  return servos_[device][channel].accel_;
} // getServoAcceleration(const uint8_t)

//
uint16_t MaestroServoController::getServoPosition(const uint8_t channel)
{

  // check for errors
  if (isConnected() && (channel < getNumChannels()))
  {

    // form the output packet
    static const int N_REQ_BYTES = 2;
    unsigned char req[N_REQ_BYTES] = {0x90, // Command byte: Get Position.
                                      channel, // First data byte holds channel number
        };

    // success is being able to write the packet
    // (that's all the feedback we get)
    static const int N_RES_BYTES = 2;
    unsigned char res[N_RES_BYTES];
    if ((write(fd_, req, N_REQ_BYTES) == N_REQ_BYTES) && (read(fd_, res, N_RES_BYTES) == N_RES_BYTES))
      return ((uint16_t(res[1]) << 8) + uint16_t(res[0])) / 4;
  }
  return 0;
} // getServoPosition(const uint8_t)

//
uint16_t MaestroServoController::getServoPosition(const uint8_t device, const uint8_t channel)
{
  /*
   if (device == 0)
   return getServoPosition(channel);
   */
  // check for errors
  if (isConnected() && (device < getNumDevices()) && (channel < getNumChannels()))
  {

    // form the output packet
    static const int N_REQ_BYTES = 4;
    unsigned char req[N_REQ_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                      device, // Device number.
                                      0x10, // Command byte: Get Position.
                                      channel, // First data byte holds channel number
        };

    // success is being able to write the packet
    // (that's all the feedback we get)
    static const int N_RES_BYTES = 2;
    unsigned char res[N_RES_BYTES];
    if ((write(fd_, req, N_REQ_BYTES) == N_REQ_BYTES) && (read(fd_, res, N_RES_BYTES) == N_RES_BYTES))
      return ((uint16_t(res[1]) << 8) + uint16_t(res[0])) / 4;
  }
  return 0;
} // getServoPosition(const uint8_t, const uint8_t)

//
bool MaestroServoController::getServosMovingState()
{

  // check for errors
  if (isConnected())
  {

    // form the output packet
    static const int N_REQ_BYTES = 1;
    unsigned char req[N_REQ_BYTES] = {0x93}; // Command byte: Get Moving State.

    // success is being able to write the packet
    // (that's all the feedback we get)
    static const int N_RES_BYTES = 1;
    unsigned char res[N_RES_BYTES];
    if ((write(fd_, req, N_REQ_BYTES) == N_REQ_BYTES) && (read(fd_, res, N_RES_BYTES) == N_RES_BYTES))
      return (res[0] == 0x01);
  }
  return false;
} // getServosMovingState()

//
bool MaestroServoController::getServosMovingState(const uint8_t device)
{
  /*
   if (device == 0)
   return getServosMovingState();
   */
  // check for errors
  if (isConnected() && (device < getNumDevices()))
  {

    // form the output packet
    static const int N_REQ_BYTES = 3;
    unsigned char req[N_REQ_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                      device, // Device number.
                                      0x13 // Command byte: Get Moving State.
        };

    // success is being able to write the packet
    // (that's all the feedback we get)
    static const int N_RES_BYTES = 1;
    unsigned char res[N_RES_BYTES];
    if ((write(fd_, req, N_REQ_BYTES) == N_REQ_BYTES) && (read(fd_, res, N_RES_BYTES) == N_RES_BYTES))
      return (res[0] == 0x01);
  }
  return false;
} // getServosMovingState(const uint8_t)

//
uint16_t MaestroServoController::getServosErrors()
{

  // check for errors
  if (isConnected())
  {

    // form the output packet
    static const int N_REQ_BYTES = 1;
    unsigned char req[N_REQ_BYTES] = {0xA1}; // Command byte: Get Errors.

    // success is being able to write the packet
    // (that's all the feedback we get)
    static const int N_RES_BYTES = 2;
    unsigned char res[N_RES_BYTES];
    if ((write(fd_, req, N_REQ_BYTES) == N_REQ_BYTES) && (read(fd_, res, N_RES_BYTES) == N_RES_BYTES))
      return (uint16_t(res[0]) << 8) + uint16_t(res[1]); // note: untested!!!
  }
  return 0x0000; // note: should probably change...
} // getServosErrors()

//
uint16_t MaestroServoController::getServosErrors(const uint8_t device)
{
  /*
   if (device == 0)
   return getServosErrors();
   */
  // check for errors
  if (isConnected() && (device < getNumDevices()))
  {

    // form the output packet
    static const int N_REQ_BYTES = 3;
    unsigned char req[N_REQ_BYTES] = {0xAA, // Start byte: Pololu protocol.
                                      device, // Device number.
                                      0x21 // Command byte: Get Errors.
        };

    // success is being able to write the packet
    // (that's all the feedback we get)
    static const int N_RES_BYTES = 2;
    unsigned char res[N_RES_BYTES];
    if ((write(fd_, req, N_REQ_BYTES) == N_REQ_BYTES) && (read(fd_, res, N_RES_BYTES) == N_RES_BYTES))
      return (uint16_t(res[0]) << 8) + uint16_t(res[1]); // note: untested!!!
  }
  return 0x0000; // note: should probably change...
} // getServosErrors(const uint8_t)

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
    std::cerr << "Unable to set '" << getPath() << "' properties : " << strerror(errno) << std::endl;
    disconnect();
    return false;
  }

  /*
   struct termios options;
   fcntl(fd_, F_SETFL, FNDELAY); // Configure port reading

   // Get the current options for the port
   tcgetattr(fd_, &options);
   cfsetispeed(&options, B9600); // Set the baud rates to 9600
   cfsetospeed(&options, B9600);

   // Enable the receiver and set local mode
   options.c_cflag |= (CLOCAL | CREAD);
   options.c_cflag &= ~PARENB; // Mask the character size to 8 bits, no parity
   options.c_cflag &= ~CSTOPB;
   options.c_cflag &= ~CSIZE;
   options.c_cflag |= CS8; // Select 8 data bits
   options.c_cflag &= ~CRTSCTS; // Disable hardware flow control

   // Enable data to be processed as raw input
   options.c_lflag &= ~(ICANON | ECHO | ISIG);

   // Set the new options for the port
   tcsetattr(fd_, TCSANOW, &options);
   */

  /*
   * Configuring the port is harder in Linux over that of other OS's.
   * There is more bit masking involved in order to change a single option.
   * While there are convenient functions that can be used to set the speed of the port, other options,like
   * parity and number of stop bits, are set using the c-cflag member of the termios struct, and require bitwise
   * operations to set the various settings.
   * Linux is also capable of setting the read time-out values. This is set using the c-cc member of the termios
   * struct which is actually an array indexed by defined values.
   */
  /*
   // Create the struct
   struct termios options;

   // Get the current settings of the serial port.
   tcgetattr(fd_, &options);

   // Set the read and write speed to 19200 BAUD.
   // All speeds can be prefixed with B as a settings.
   cfsetispeed(&options, B9600);
   cfsetospeed(&options, B9600);

   // Now to set the other settings. Here we use the no parity example. Both will assume 8-bit words.
   // PARENB is enabled parity bit. This disables the parity bit.
   options.c_cflag &= ~PARENB;

   // CSTOPB means 2 stop bits, otherwise (in this case) only one stop bit.
   options.c_cflag &= ~CSTOPB;

   // CSIZE is a mask for all the data size bits, so anding with the negation clears out the current data size setting.
   options.c_cflag &= ~CSIZE;

   // CS8 means 8-bits per work
   options.c_cflag |= CS8;
   */
  /*
   * Configuring the port is harder in Linux over that of other OS's.
   * There is more bit masking involved in order to change a single option.
   * While there are convenient functions that can be used to set the speed of the port, other options,like
   * parity and number of stop bits, are set using the c-cflag member of the termios struct, and require bitwise
   * operations to set the various settings.
   * Linux is also capable of setting the read time-out values. This is set using the c-cc member of the termios
   * struct which is actually an array indexed by defined values.
   */
  /*
   // Create the struct
   struct termios options;
   fcntl(fd_, F_SETFL, FNDELAY); // Configure port reading

   // Get the current settings of the serial port.
   tcgetattr(fd_, &options);

   // Set the read and write speed to 19200 BAUD.
   // All speeds can be prefixed with B as a settings.
   cfsetispeed(&options, B9600);
   cfsetospeed(&options, B9600);

   // Now to set the other settings. Here we use the no parity example. Both will assume 8-bit words.
   // PARENB is enabled parity bit. This disables the parity bit.
   options.c_cflag &= ~PARENB;

   // CSTOPB means 2 stop bits, otherwise (in this case) only one stop bit.
   options.c_cflag &= ~CSTOPB;

   // CSIZE is a mask for all the data size bits, so anding with the negation clears out the current data size setting.
   options.c_cflag &= ~CSIZE;

   // CS8 means 8-bits per work
   options.c_cflag |= CS8;

   options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

   // what's this?
   tcflush(fd_, TCIOFLUSH);

   // actually set the properties
   if (tcsetattr(fd_, TCSANOW, &options) != 0)
   {

   // unable to set servo's properties
   std::cerr << "Unable to set '" << getPath() << "' properties : " << strerror(errno) << std::endl;
   disconnect();
   return false;
   }
   */
  return true;
} // setProperties()

//
MaestroServoController::Servo::Servo(const ServoLimits limits, const bool enabled, const uint16_t target,
                                     const uint16_t speed, const uint16_t accel) :
  limits_(limits), enabled_(enabled), target_(target), speed_(speed), accel_(accel)
{
  //target_ = scaleTargetValue(target);
} // Servo(const ServoLimits, const uint16_t, const uint16_t, const uint16_t)

//
MaestroServoController::Servo::Servo(const Servo &servo)
{
  limits_ = servo.limits_;
  enabled_ = servo.enabled_;
  target_ = servo.target_;
  speed_ = servo.speed_;
  accel_ = servo.accel_;
} // Servo(const Servo &)

//
bool MaestroServoController::Servo::isValidTarget(const uint16_t target) const
{
  return (target >= getMinLimit()) && (target <= getMaxLimit());
} // isValidTarget(const uint16_t)

//
uint16_t MaestroServoController::Servo::clipTargetValue(const uint16_t target) const
{
  uint16_t min_limit = getMinLimit();
  uint16_t max_limit = getMaxLimit();
  if (target < min_limit)
    return min_limit;
  else if (target > max_limit)
    return max_limit;
  return target;
} // clipTargetValue(const uint16_t)

//
bool MaestroServoController::Servo::setTarget(uint16_t target, const bool clip_target)
{
  if (clip_target)
    target = clipTargetValue(target);
  else if (!isValidTarget(target))
    return false;
  target_ = target;
  return true;
} // setTarget(const uint16_t, const bool)

//
uint16_t MaestroServoController::Servo::getMinLimit() const
{
  return std::min(limits_.first, limits_.second);
} // getMinLimit()

//
uint16_t MaestroServoController::Servo::getMaxLimit() const
{
  return std::max(limits_.first, limits_.second);
}
