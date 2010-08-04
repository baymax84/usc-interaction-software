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

#ifndef POLOLU_SERVO_CONTROLLER_HH
#define POLOLU_SERVO_CONTROLLER_HH

#include <string>

namespace pololu
{

class PololuServoController
{
public:
    // ctor / dtor
    PololuServoController();
    PololuServoController(const std::string deviceName);
    ~PololuServoController();

    const std::string&  device() const;
    void device(const std::string deviceName);

    bool open();
    void close();
    bool isValid() const;

    bool moveTo(unsigned char chan, float position);
    bool moveHome();

    bool turnOff(unsigned char chan);
    bool turnOffAll();

    void flush() const;

private:
    bool setProperties();

private:
    std::string _device;
    int         _fd;

private:
    // really just want to make certain these aren't used
    PololuServoController(const PololuServoController&);
    void operator= (const PololuServoController&);
};

};  // pololu


#endif // POLOLU_SERVO_CONTROLLER_HH
