// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  Simple wrapper class for the Pololu USB 16-Servo Controller
//

#include <iostream>

#include <fcntl.h>         // for file control, open(), O_RDWR, etc
#include <termios.h>       // for flow control, cfsetispeed() etc.
#include <errno.h>         // for error codes, errno, strerror(), etc.
#include <string.h>        // for strerror() and memset()

#include "sparky/pololu.hh"

using namespace pololu;

//////////////////////////////////////////////////////////////////////
//
//  PololuServoController()
//
//  default ctor
//
//  Can't be used until the device has been set (use device(const string) )
//  and then opened (use open() )
//
//  no arguements
//

PololuServoController::
PololuServoController()
    : _device("Not Defined"), // put in a invalid name, must spec later
      _fd(-1)                 // indicate the file desciptor is not valid
{
}

//////////////////////////////////////////////////////////////////////
//
//  PololuServoController(const std::string deviceName)
//
//  ctor
//
//  Can't be used until the device has been opened (use open() )
//
//  arguements:
//     const std::string deviceName
//         name of the device, for example "/dev/ttyUSB0"
//

PololuServoController::
PololuServoController(const std::string deviceName)
    : _device(deviceName),   // name of the device (e.g., "/dev/ttyUSB0")
      _fd(-1)                 // indicate the file desciptor is not valid
{
}

//////////////////////////////////////////////////////////////////////
//
//  ~PololuServoController()
//
//  dtor
//
//  Shuts down the servo controller, closing if necessary
//
//  no arguements
//

PololuServoController::
~PololuServoController()
{
    if (isValid())
        close();
}

//////////////////////////////////////////////////////////////////////
//
//  const std::string& device()
//
//  returns the name of the device (e.g., "/dev/ttyUSB0")
//
//  no arguements
//

const std::string& PololuServoController::
device() const
{
    return _device;
}

//////////////////////////////////////////////////////////////////////
//
//  void device(const std::string&  deviceName)
//
//  sets the name of the device (e.g., "/dev/ttyUSB0")
//
//  arguements
//      const std::string deviceName  - name of the device (e.g., "/dev/ttyUSB0")
//

void PololuServoController::
device(const std::string deviceName)
{
    // close the device first if it's currently open
    if (isValid())
        close();

    _device = deviceName;
}

//////////////////////////////////////////////////////////////////////
//
//  bool open()
//
//  Opens the file device
//  returns true on success, false on failure
//
//  no arguements
//

bool PololuServoController::
open()
{
    _fd = ::open (device().c_str(), O_RDWR | O_NOCTTY);  // JUSTIN
    if (_fd == -1) {

        // unable to open()
        std::cerr << "Error opening "
                  << device()
                  << " : "
                  << strerror(errno)
                  << std::endl;
        close();
    }
    else {
        // set speed and other properties
        // on failure setProperties() calls close()
        setProperties();
    }

#if 0
    // move to the center position
    if (isValid()) {
        moveHome();
    }
#endif

    // success is 
    return isValid();
}

//////////////////////////////////////////////////////////////////////
//
//  void close()
//
//  Closes the file device
//  No value returned (void)
//
//  no arguements
//

void PololuServoController::
close()
{
    if (isValid()) {

        // close the port
        // try to be (sort of) thread safe
        int fd = _fd;
        _fd = -1;
        ::close(fd);
    }
}

//////////////////////////////////////////////////////////////////////
//
//  bool isValid() const
//
//  Returns true if the file device is open and ready for I/O, false otherwise
//
//  no arguements
//

bool PololuServoController::
isValid() const
{
    // a valid file descriptor, _fd != -1
    return (_fd != -1);
}

//////////////////////////////////////////////////////////////////////
//
//  void flush()
//
//  Flushes all data to the actual device (as opposed to being buffered in memory)
//
//  no arguements
//

void PololuServoController::
flush() const
{
    if (isValid()) {
        fdatasync(_fd);
    }
}

//////////////////////////////////////////////////////////////////////
//
//  bool setProperties()
//
//  Sets the speed and other properties of the tty device 
//  Used internally by open
//
//  no arguements
//

bool PololuServoController::
setProperties()  // JUSTIN
{
    // sanity check that servo is valid
    if (_fd == -1) // error value
        return false;    // fail silently

    // overkill on the initialization
    struct termios control;
    memset(&control, 0, sizeof(control));
    cfmakeraw(&control);

    // set some control characters
    control.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    control.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

    // set speed
    cfsetispeed(&control, B38400);
    cfsetospeed(&control, B38400);

    tcflush(_fd, TCIOFLUSH);

    // actually set the properties
    if (tcsetattr(_fd, TCSANOW, &control) != 0) {
        // unable to set servo's attributes
        std::cerr << "Unable to set "
                  << device()
                  << " attributes : "
                  << strerror(errno)
                  << std::endl;
        close();
        return false;
    }

    // success
    return true;
}

//////////////////////////////////////////////////////////////////////
//
//  bool moveTo(unsigned char chan, float position)
//
//  Commands an individual motor to move to a position
//  Positions are specified as a float in [0, 1]
//  values less than 0 or more than 1 are silently clipped to that range
//
//  arguements:
//      unsigned char chan      - number of the motor channel, in [0,15]
//      float         position  - target position, in [0,1]
//
//  returns true on success, false on failure
//

bool PololuServoController::
moveTo(unsigned char chan, float position)  // JUSTIN
{
    //
    // check for errors
    if (_fd == -1)       // error value
        return false;    // fail silently

    if (chan >= 16)      // channel out of range
        return false;    // fail silently

    // clip position just to be certain
    if (position < 0.0)  position = 0.0;
    if (position > 1.0)  position = 1.0;

    // scale value and convert to an integer
    // - the pololu outputs between 0.25 ms (=> 500) to 2.75 ms (=> 5500)
    //    with the center position of 1.5 ms (=> 3000)
    // - a comfortable range seems to be 0.5 ms -> 2.5 ms (1000 -> 5000)
    static const int PosMin = 1000; // 0.5 msec pulse
    static const int PosMax = 5000; // 2.5 msec pulse
    int pos = (int)(PosMin + (PosMax - PosMin) * position);

    // form the output packet
    unsigned char cmd[6]
        = {
        0x80,    // start byte (always 0x80)
        0x01,    // device id  (always 0x01)
        0x04,    // command (4 = set absolute position)
        chan,    // servo channel, value in [0,15]
        pos/128, // data 1 (upper bits)
        pos%128  // data 2 (lower bits)
    };

    // success is being able to write the packet
    // (that's all the feedback we get)
    return (6 == write(_fd, cmd, 6));
}

//////////////////////////////////////////////////////////////////////
//
//  bool moveHome()
//
//  Moves all 16 motors to their home (mid stroke) position
//
//  no arguements
//  returns true on success, false on failure
//

bool PololuServoController::
moveHome()
{
    // successful if all of the moveTo() return true
    bool success = true;

    // move all of the motors to the middle position (0.5)
    for (int id=0; id<16; ++id) {
        // and in the success of each move
        // but keep trying to move regardless
        success &= moveTo(id, 0.5);
    }

    return success;
}

//////////////////////////////////////////////////////////////////////
//
//  bool turnOff(unsigned char chan)
//
//  turns off servoing (stop applying poser) of a motor channel
//  (servos *start* the first time a position is commanded)
//
//  arguements:
//      unsigned char chan      - number of the motor channel, in [0,15]
//  returns true on success, false on failure
//

bool PololuServoController::
turnOff(unsigned char chan)  // JUSTIN
{
    //
    // check for errors
    if (_fd == -1)       // error value
        return false;    // fail silently

    if (chan >= 16)      // channel out of range
        return false;    // fail silently

    // form the output packet
    unsigned char cmd[5]
        = {
        0x80,    // start byte (always 0x80)
        0x01,    // device id  (always 0x01)
        0x00,    // command (0 = set parameters)
        chan,    // servo channel, value in [0,15]
        0        // data 1
    };

    // success is being able to write the packet
    // (that's all the feedback we get)
    return (5 == write(_fd, cmd, 5));
}

//////////////////////////////////////////////////////////////////////
//
//  bool turnOff(unsigned char chan)
//
//  turns off servoing (stop applying poser) of all motors
//  (servos *start* the first time a position is commanded)
//
//  no arguements:
//  returns true on success, false on failure
//

bool PololuServoController::
turnOffAll()
{
    // successful if all of the turnOff() return true
    bool success = true;

    // move all of the motors to the middle position (0.5)
    for (int id=0; id<16; ++id) {
        // and in the success of each move
        // but keep trying to move regardless
        success &= turnOff(id);
    }

    return success;
}
