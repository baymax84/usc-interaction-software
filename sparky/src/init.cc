// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "init.hh"
#include "comp.hh"
#include "servo.hh"

extern void init_player();// in player.cc

bool init()
{
    if ( ! init_servo_controller() ) {
        return false;
    }

    init_computing_env();
    init_player();

    return true;
}
