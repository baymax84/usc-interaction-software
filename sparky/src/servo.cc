// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  Handles talking to the R/C servo motors for the minimatronic
//

#include <iostream>

#include "sparky/servo.hh"
#include "sparky/controller.hh"

static Controller controller;

//////////////////////////////////////////////////////////////////////
//
//  init_servo_controller()
//
//  connects to and initializes the Pololu controllers for the minimatronic
//  returns true on success, false on failure
//
//  no arguments
//

bool init_servo_controller()
{

    if ( ! controller.open() ) {
        std::cout << "Unable to open controller, exiting" <<  std::endl;
        return false;
    }

    // success
    return true;
}

//////////////////////////////////////////////////////////////////////
//
//  shutdown_servo_controller()
//
//  shuts down and disconnects the Pololu controllers for the minimatronic
//  returns no value (void)
//
//  no arguments
//

void shutdown_servo_controller()
{
    // move to home (mid stroke)
    controller . moveHome();

    // allow some time to get there before we shutdown
    sleep(1);

    // turn off the servo controllers
    controller . turnOffAll();

    // close (the close would be done automatically anyway by the dtor)
    controller.close();
}


//////////////////////////////////////////////////////////////////////
//
//  outputPositions(double* vals, bool* active)
//
//  commands the positions of the mimiatronic's 18 motors
//  returns true on success, false on failure
//
//  arguments:
//      douible vals     - vertor of 18 positions
//      bool active      - vector of 18 bools (if true,  use the position val
//                                             if false, ignore the position val (hold current pos))
//

bool outputPositions(double* vals, bool* active)
{
    for (int chan=0; chan<NChannels; ++chan) {
        if (active == 0  || active[chan]) {
            if ( ! controller . moveTo(chan, vals[chan]) ) {
                std::cout << "Error moving channel " << chan+1 <<  std::endl;
                return false;
            }
        }
    }

    // success
    return true;
}
