// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
//  Background loop that plays out the curve data,
//  possibly at the same time as the curve is being edited
//  by the GUI editor tool
//

#include <cstdio>
#include <unistd.h>
#include <time.h>

#include <pthread.h>
#include <errno.h>

#include "sparky/global.hh"
#include "sparky/comp.hh"
#include "sparky/servo.hh"
#include "sparky/utility.hh"

static bool time_going     = false;
static double last_time    = 0;
static double player_time  = 0;


static bool looping        = false;
static bool thread_running = false;
static bool keepGoing      = true;

extern void set_curve_pane_output_x(double x); // curve_pane.cc

extern void get_curve_outputting(bool outputting[]); // in selector.cc

//////////////////////////////////////////////////////////////////////

static pthread_t player_thread;
static void* 
player_function(void*)
{
    thread_running = true;

    init_servo_controller();

    // variables used to get data 
    double   vals[N_Curves];
    bool   active[N_Curves];

    player_time = 0;
    reset_output_curves(player_time);

    while (keepGoing) {
        double raw_now_time = getTime();
        const double dt = (time_going
                           ?  raw_now_time - last_time
                           :  0.0);

        player_time += dt;
        last_time = raw_now_time;

        get_curve_outputting(active); // which curves are outputting

        CurveStatus rtn = get_output_data(player_time, vals, active); // get data from curves
        if (rtn != OkayStatus) {
            if (rtn == ErrorStatus) {
                time_going = false;
                printf("Stopped time on an error\n");
            }
            else { // Done status
                if (! looping) {
                    time_going = false;
                    printf("Stopped time on done\n");
                }
                reset_output_curves(player_time);
            }
        }

        // output the data
        outputPositions(vals, active);

        // move display's time to output time
        double frac = get_output_frac(player_time);
        set_curve_pane_output_x(frac); // just the fractional part

        // wait about 1/50th second
        usleep(20 * 1000); // 0.02 sec
    }
    
    printf("player thread ending\n");
    shutdown_servo_controller();
    thread_running = false;

    printf("player thread done\n");

    return 0;
}

//////////////////////////////////////////////////////////////////////

static void start_thread()
{
    //
    // make the thread detached (we're never going to "join" the thread)
    pthread_attr_t detached_attr;
    pthread_attr_init(&detached_attr);
    pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);

    //
    // create the thread
    int create_rtn = pthread_create(&player_thread,
				    &detached_attr,
				    player_function,
				    (void*)0);

    //
    // handle thread create errors
    //
    if (create_rtn != 0) {
	fprintf(stderr, "Error: pthread_create, ");
	if (create_rtn == EAGAIN)
	    fprintf(stderr, "Insufficient resources\n");
	else if (create_rtn == EINVAL)
	    fprintf(stderr, "Invalid arguments\n");
	else {
	    fprintf(stderr, "Unrecognized code %d\n", create_rtn);
	    perror("create");
	}
	return; // failure
    }

    //
    // otherwise the thread created successfully
}

//////////////////////////////////////////////////////////////////////


void start_running()
{
    time_going = true;
}

//////////////////////////////////////////////////////////////////////

void set_looping(bool loop_value)
{
    looping = loop_value;
}

//////////////////////////////////////////////////////////////////////

void stop_running()
{
    time_going = false;
}

//////////////////////////////////////////////////////////////////////

void set_output_frac(double frac)
{
    if (frac < 0.0) frac = 0.0;
    if (frac > 1.0) frac = 1.0;

    set_output_frac(player_time, frac);
}

//////////////////////////////////////////////////////////////////////

double get_player_time()
{
    return player_time;
}

//////////////////////////////////////////////////////////////////////

void init_player()
{
    if (! thread_running) {
        start_thread();
        usleep(10*1000);
    }
}

bool playerRunning()
{
    return thread_running;
}

void stopPlayer()
{
    keepGoing = false;
}
