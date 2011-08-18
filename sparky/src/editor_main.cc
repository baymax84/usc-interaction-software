// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include <signal.h>        // for signal handling, signal(), etc
#include <gtkmm/main.h>
#include "sparky/main_frame.hh"

#include "sparky/init.hh"

//////////////////////////////////////////////////////////////////////

void endme(int rtn);

//////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[])
{
    //
    // on interupt (i.e. ^C) call endme()
    //
    if (signal(SIGINT,  SIG_IGN) != SIG_IGN) 
	signal(SIGINT,  endme);
    signal(SIGPIPE,  SIG_IGN);


    //
    // try to initialize 
    //
    if ( ! init() ) {
        fprintf(stderr, "Initialization failed\n");
        exit(-1);
    }

    //
    // startup and run the GUI
    //
    Gtk::Main kit(argc, argv);
    MainFrame editor;
    Gtk::Main::run(editor);
    
    return 0;
}

//////////////////////////////////////////////////////////////////////


void endme(int rtn)
{
    //  ignore interupts since we are exiting
    signal(SIGINT,  SIG_IGN);

    extern void stopPlayer();  // in player.cc
    stopPlayer();

    for (int i=0; i<20; ++i) {
        extern bool playerRunning();
        if ( ! playerRunning() ) {
            break;
        }
        usleep(100 * 1000);
    }

    signal(SIGINT, SIG_DFL);
    exit(rtn);
}

