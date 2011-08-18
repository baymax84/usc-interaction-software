// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  class Conrtoller
//
//  A wrapper class for the two Pololu USB 16-servo controllers
//  allowing the two to treated as one big controller
//


#ifndef CONTROLLER_HH
#define CONTROLLER_HH

//////////////////////////////////////////////////////////////////////

#include "sparky/pololu.hh"

//////////////////////////////////////////////////////////////////////

// number of channels to control (in minimatronic)
const int NChannels    = 24;

//////////////////////////////////////////////////////////////////////

enum Motors {
  Mouth,           // Motor  1
  HeadNod,         // Motor  2
  HeadTurn,        // Motor  3
  RtArmForward,    // Motor  4
  RtArmOut,        // Motor  5
  RtElbow,         // Motor  6
  LtArmForward,    // Motor  7
  LtArmOut,        // Motor  8
  LtElbow,         // Motor  9
  RtWrist,         // Motor 10
  LtWrist,         // Motor 11
  TorsoBend,       // Motor 12
  EyeBlink,        // Motor 13
  EyesLtRt,        // Motor 14
  RtFootUp,        // Motor 15
  RtFootForward,   // Motor 16
  LtFootUp,        // Motor 17
  LtFootForward    // Motor 18
};

////////////////////////////////////////////////////////////////////
class Controller
{
public:
    // ctor / dtor
    Controller();
    ~Controller();

    bool open();
    void close();
    bool isValid() const;

    bool moveTo(int channel, float position);
    bool moveHome();

    bool turnOffAll();

    void flush() const;

private:
    static const int NControllers = 1;   // number of servo control cards

    PololuServoController pololu[ NControllers ];

private:
    // really just want to make certain these aren't used
    Controller(const Controller&);
    void operator= (const Controller&);
};

//////////////////////////////////////////////////////////////////////

#endif // CONTROLLER_HH
