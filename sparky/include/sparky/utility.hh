// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  Some useful utility functions.
//
//  Seem to have stripped out everything except the getTime() function !?
//

#ifndef UTILITY_HH
#define UTILITY_HH

namespace utility
{

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

};  // utility

#endif // UTILITY_HH
