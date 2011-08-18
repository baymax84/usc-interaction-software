// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include <time.h>          // for time stuff: clock_gettime(), etc.

#include "sparky/utility.hh"

//////////////////////////////////////////////////////////////////////
//
//  double getTime()
//
//  returns the amount of time, in seconds
//  the first call sets the start time and always returns zero
//
//  no arguements
//

double getTime()
{
    // use first call to set time zero
    static bool firstTime = true;
    static struct timespec startTp;
    if (firstTime) {
        // clear the firstTime flag 
        firstTime = false;

        // get this first time's time as the starting time
        clock_gettime(CLOCK_REALTIME, &startTp);

        // just return zero this first time
        return 0.0;
    }
    else {

        // get current time and subtract off the starting time
        // to get the number of seconds
        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);
        return (tp.tv_sec - startTp.tv_sec) + 1.0e-9*(tp.tv_nsec - startTp.tv_nsec);
    }
}
