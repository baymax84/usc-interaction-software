// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
// Simple test program of using the Pololu USB 16-servo controller
//
// This is intended as a sample program, rather than field ready code.
//
// By Alexis Wieland, Aug 2006
//

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/soundcard.h>

//////////////////////////////////////////////////////////////////////
//
// forward decl

static int set_properties(int servo);

//////////////////////////////////////////////////////////////////////
//
// open_servo()
//
// Opens the Palolu USB device and sets it up for communication
//
// arguements:
//    const char*  servoDev     name of the servo device (e.g., "/dev/ttyUSB0")
//
// returns:
//    -1 on failure, integer address (other than -1) on success
//

int
open_servo(const char* servoDev)
{
    int servo = open (servoDev, O_RDWR | O_NOCTTY);
    if (servo == -1  ||
        set_properties(servo) != 1) {
        // create a message
        char msg[64];
        sprintf(msg, "Unable to open %s", servoDev);
        perror (msg);
    }
    else
        printf("Servo opened successfully \n");

    return servo;
}

//////////////////////////////////////////////////////////////////////
//
// set_properties()
//
// Sets the speed and other properties of the tty USB device
// Used internally by open_servo() above, shouild not be call directly
//
// arguements:
//    const char*  servoDev     name of the servo device (e.g., "/dev/ttyUSB0")
//
// returns:
//    1 on sucess, 0 on failure
//

static int
set_properties(int servo)
{
    struct termios control;

    // sanity check that servo is valid
    if (servo == -1) // error value
        return 0;    // fail silently

    // overkill on the initialization
    memset(&control, 0, sizeof(control));
    cfmakeraw(&control);

    // set some control characters
    control.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    control.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

    // set speed
    cfsetispeed(&control, B38400);
    cfsetospeed(&control, B38400);

    tcflush(servo, TCIOFLUSH);

    // actually set the properties
    if (tcsetattr(servo, TCSANOW, &control) != 0) {
        // unable to set servo's attributes
        perror("Unable to set attr of show_controller");
        close(servo);
        servo = -1;
        return 0;
    }

    // success
    return 1;
}

//////////////////////////////////////////////////////////////////////
//
// move_servo_to()
//
// commands a channel of the servo controller to output a new position
//
// arguements:
//    int             servo     servo id returned from open_servo()
//    unsigned char   id        channel address, value in [0, 15]
//    double          position  target position, value in [0.0, 1.0]
//
// returns:
//    1 on sucess, 0 on failure
//

int
move_servo_to(int           servo,
              unsigned char id,
              double        position)
{
    if (servo == -1) // error value
        return 0;    // fail silently

    if (id >= 16)    // id out of range
        return 0;    // fail silently

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
        id,      // number of servo on card
        pos/128, // data 1 (upper bits)
        pos%128  // data 2 (lower bits)
    };

    // success is being able to write the packet
    // (that's all the feedback we get)
    return (write(servo, cmd, 6) == 6);
}

//////////////////////////////////////////////////////////////////////
//
// close_servo()
//
// arguements:
//    int             servo     servo id returned from open_servo()
//
// returns:
//    no value returned (i.e., void)
//

void
close_servo(int servo)
{
    if (servo != -1) {
        if (close(servo) == 0)
            printf("Servo closed successfully \n");
        else {
            perror("closing servo");
        }
    }
}

//////////////////////////////////////////////////////////////////////
//
//  main()
//
//  A simple test program
//  Commands first five motors to follow the same sine wave
//

int main()
{
    static const int N_Loop = 2000; // Number of iterations, how many positions to output
    int cnt, chan;

    //
    // open a servo (just assume it's at /dev/ttyUSB0 for now)
    //
    int servo1 = open_servo("/dev/ttyUSB0");

    if (servo1 == -1) {  // check it opened successfully
        fprintf(stderr, "Failed to open servo, exiting\n");
        exit(-1);
    }

    //
    // simple loop to test outputting motion
    //
    for (cnt=0; cnt<N_Loop; ++cnt) {

        //
        // this really should always be time based, not loop counter based
        //
        double position = 0.5 * (1.0 + sin(cnt * 0.02) );


        // just output to a few channels
        static int N_Motors = 1;
        for (chan = 0; chan < N_Motors; ++chan) {

            // send the move command
            if (! move_servo_to(servo1, chan, position) ) {
                perror ("write");
                break;
            }
        }
    }

    // cleanly close the device
    close_servo(servo1);

    // success
    return 0;
}
