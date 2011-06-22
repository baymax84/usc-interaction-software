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

#include <iostream>

#include "sparky/controller.hh"

using namespace controller;

//////////////////////////////////////////////////////////////////////

Controller::
Controller()
{
    // set the devices for the two control cards
    pololu[0] . device( "/dev/ttyACM0" );
    //pololu[1] . device( "/dev/ttyACM1" );
}

//////////////////////////////////////////////////////////////////////

Controller::
~Controller()
{
}

//////////////////////////////////////////////////////////////////////

bool Controller::
open()
{
    // open each controller in turn
    for (int i=0; i<NControllers; ++i) {
        if ( ! pololu[i] . open() ) {

            // failure

            // print out a message
            std::cout << "Unable to open pololu board "
                      << i + 1      // make index 1 based
                      << " of "
                      << NControllers
                      << " : "
                      << pololu[i].device()
                      << std::endl;

            // return failure
            return false;
        }
    }

    // success
    return true;
}

//////////////////////////////////////////////////////////////////////

void Controller::
close()
{
    pololu[0].close();
    //pololu[1].close();
}

//////////////////////////////////////////////////////////////////////

bool Controller::
isValid() const
{
/*
    return (pololu[0].isValid()  &&
            pololu[1].isValid());
*/
    return pololu[0].isValid();
}

//////////////////////////////////////////////////////////////////////

bool Controller::
moveTo(int channel, float position)
{
    // check that it's a legal value (in range)
/*
    if (channel < Mouth  ||
        channel > LtFootForward)
        return false; // fail quietly

    if (channel < 9) {
        channel = channel * 2;
        if (channel >= 16)
            channel = 15;
        return pololu[0].moveTo(channel,
                              position);
    }
    else {
        channel = (channel-9) * 2;
        if (channel >= 16)
            channel = 15;
        return pololu[1].moveTo(channel,
                              position);
    }
*/
    if ((channel < 0) || (channel >= NChannels)) return false;
    return pololu[0].moveTo(channel, position);
}

//////////////////////////////////////////////////////////////////////

bool Controller::
moveHome()
{
    bool rtn = true;

    // move each controller in turn
    for (int i=0; i<NControllers; ++i) {

        // remember if it failed, but keep going regardless
        rtn &= pololu[i] . moveHome();

    }

    // return true if both succeeded
    return rtn;
}

//////////////////////////////////////////////////////////////////////

bool Controller::
turnOffAll()
{
    bool rtn = true;

    // turn each controller off turn
    for (int i=0; i<NControllers; ++i) {

        // remember if it failed, but keep going regardless
        rtn &= pololu[i] . turnOffAll();

    }

    // return true if both succeeded
    return rtn;
}

//////////////////////////////////////////////////////////////////////

void Controller::
flush() const
{
    // flush both
    pololu[0] . flush();
    pololu[1] . flush();
}

//////////////////////////////////////////////////////////////////////
