// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  Main() entry point for the simple sine wave test program
//

#include <iostream>

#include <signal.h>        // for signal handling, signal(), etc
#include <math.h>          // for sin(), used in test motion

#include "sparky/controller.hh"
#include "sparky/utility.hh"

using namespace controller;

//////////////////////////////////////////////////////////////////////

// static variable
static bool keepGoing = true;   // flag for shutting down gracefully

// forward decl
static float computePosition(float t);
static void endme(int rtn);

//////////////////////////////////////////////////////////////////////

int
main()
{
    //
    // on interupt (i.e. ^C) call endme()
    //
    if (signal(SIGINT,  SIG_IGN) != SIG_IGN) 
	signal(SIGINT,  endme);
    signal(SIGPIPE,  SIG_IGN);


    //
    // create and initialize the servo controller
    //
    Controller controller;
    if ( ! controller.open() ) {
        std::cout << "Unable to open controller, exiting" <<  std::endl;
        return -1;
    }

    //
    // simple loop to test outputting motion
    //
    static const int N = 1000;
    for (int cnt=0; cnt<N && keepGoing; ++cnt) {

        //
        // get time (in seconds)
        //
        float now = utility::getTime(); 

        //
        // get the target position (in this test it's the same for all the motors)
        //
        float position = computePosition(now);

        //
        // output the same position to all the motors
        //
        for (int chan = 0; chan<18 && keepGoing; ++chan) {

            //
            // command each motor in turn (to the same position)
            //
            if ( ! controller.moveTo(chan, position) ) {

                //
                // if there's a problem, print a message and exit the loop
                //
                std::cout << "Error moving motor "
                          << chan + 1         // make index 1 based to match motor numbering
                          << " at time "
                          << now
                          << " sec"
                          << std::endl;
                keepGoing = false;
                break;
            }

        } // for chan

        // flush any buffers (before we sleep)
        controller . flush();

        // allow others threads time to work
        // has minimal effect (unless the CPU is swamped)
        usleep(1000);

    }  // for cnt

    //
    // done with the test loop
    //


    // move to home (mid stroke)
    controller . moveHome();

    // allow some time to get there before we shutdown
    sleep(1);

    // turn off the controllers
    controller . turnOffAll();

    // close (the close would be done automatically anyway by the dtor)
    controller.close();
}


//////////////////////////////////////////////////////////////////////
//
//  computePosition()
//
//  compute the target position for the motors
//  position is in [0,1], but typically is much smaller due to clipping
//
//  really just a place holder function
//  here we just output the same sine wave for all the motors
//

float computePosition(float t)
{
    //
    // compute a target position
    //

    // sine wave based on time
    static const double freq       = 1.5; // Hz
    static const double amplitude  = 0.2; // keep small so no motor clips

    // output in [0.1]
    return 0.5  +  amplitude * sin(t * (freq*M_PI));
}

//////////////////////////////////////////////////////////////////////
//
//  endme()
//
//  signals the loop to end,
//  but then hands control back to the main loop so it can end cleanly
//

void endme(int /* ignore_rtn */)
{
    //  ignore interupts since we are exiting
    signal(SIGINT,  SIG_IGN);

    keepGoing = false;

    signal(SIGINT, SIG_DFL);
}

