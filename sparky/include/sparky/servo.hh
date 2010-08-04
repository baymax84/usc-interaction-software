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

#ifndef SERVO_HH
#define SERVO_HH

bool init_servo_controller();
void shutdown_servo_controller();
bool outputPositions(double* vals, bool* active = 0);

#endif  // SERVO_HH

