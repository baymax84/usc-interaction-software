// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //


//
//  The main window for editing minimatronic motion curves
//

#include <stdlib.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gdk/gdkkeysyms.h>

#include "sparky/main_frame.hh"
#include "sparky/selector.hh"
#include "sparky/scroller.hh"

#include "sparky/comp.hh"

static MainFrame* the_main_frame = 0;
static Gtk::Label* the_status_label = 0;

extern void start_running();
extern void set_looping(bool);
extern void stop_running();
extern void save_show();
extern void load_show();


static void duration_changed(); // below

//////////////////////////////////////////////////////////////////////

MainFrame::
MainFrame()
{
    set_border_width(5);
    set_default_size(1500, 800);

    _selector    = Gtk::manage( new Selector );
    _curves      = Gtk::manage( new Scroller );
    _run         = Gtk::manage( new Gtk::Button("Run") );
    _stop        = Gtk::manage( new Gtk::Button("Stop") );
    _dur         = Gtk::manage( new Gtk::Entry );

    _filename    = Gtk::manage( new Gtk::Entry );

    _show_deriv  = Gtk::manage( new Gtk::CheckButton("Show Velocity") );
    _show_deriv -> set_active(true);

    _looping     = Gtk::manage( new Gtk::CheckButton("Loop") );

    Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox);
    Gtk::VBox* left = Gtk::manage(new Gtk::VBox);

    //add( *_curves );
    add(*hbox);

    hbox -> set_spacing(5);
    hbox -> pack_start(*left, false, true);
    hbox -> pack_start(*_curves, true, true);

    left -> pack_start(*_selector, false, true);

    ////////////////////////////////////////

    {
        Gtk::VBox* vbox  = Gtk::manage(new Gtk::VBox);
        vbox -> set_spacing(5);

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            vbox -> pack_start(*hbox2, true, false);
        }

        left -> pack_start(*vbox,  true, false);
    }

    ////////////////////////////////////////

    {
        Gtk::VBox* vbox  = Gtk::manage(new Gtk::VBox);
        vbox -> set_spacing(5);

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            hbox2 -> pack_start(*_run,  true, false);
            hbox2 -> pack_start(*_stop, true, false);

            hbox2 -> pack_start(*_looping, true, false);

            vbox -> pack_start(*hbox2, true, false);
        }
        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            Gtk::Label* label = Gtk::manage( new Gtk::Label("Duration:") );
            hbox2 -> pack_start(*label, false, true);
            hbox2 -> pack_start(*_dur, true, true);

            label = Gtk::manage( new Gtk::Label(" seconds") );
            hbox2 -> pack_start(*label, false, true);

            _dur -> set_text("10");


        
            vbox -> pack_start(*hbox2, true, false);
        }

        left -> pack_start(*vbox,  true, false);
    }


    ////////////////////////////////////////

    {
        Gtk::VBox* vbox  = Gtk::manage(new Gtk::VBox);
        vbox -> set_spacing(5);

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            vbox -> pack_start(*hbox2, true, false);
        }

        left -> pack_start(*vbox,  true, false);
    }

    ////////////////////////////////////////
    {
        Gtk::VBox* vbox  = Gtk::manage(new Gtk::VBox);
        vbox -> set_spacing(5);

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            Gtk::Label* label = Gtk::manage( new Gtk::Label("Show File:") );
            hbox2 -> pack_start(*label, false, true);

            _filename = Gtk::manage( new Gtk::Entry );
            _filename -> set_text("show.data");
            hbox2 -> pack_start(*_filename, true, true);
        
            vbox -> pack_start(*hbox2, true, false);
        }

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);


            _save   = Gtk::manage( new Gtk::Button("Save") );
            hbox2 -> pack_start(*_save, true, false);
            _load   = Gtk::manage( new Gtk::Button("Load") );
            hbox2 -> pack_start(*_load, true, false);

            _dur -> set_text("10");


        
            vbox -> pack_start(*hbox2, true, false);
        }

        left -> pack_start(*vbox,  true, false);
    }

    ////////////////////////////////////////
    {
        Gtk::VBox* vbox  = Gtk::manage(new Gtk::VBox);
        vbox -> set_spacing(5);

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            hbox2 -> pack_start(*_show_deriv, false, false);

            vbox -> pack_start(*hbox2, true, false);
        }

        {
            Gtk::HBox* hbox2 = Gtk::manage(new Gtk::HBox);
            hbox2 -> set_spacing(5);

            _status = Gtk::manage( new Gtk::Label("Mouse:") );
	    the_status_label = _status;
            hbox2 -> pack_start(*_status, true, false);

            vbox -> pack_start(*hbox2, true, false);
        }


        left -> pack_start(*vbox,  true, false);
    }

    ////////////////////////////////////////

    show_all();

    _run  -> signal_clicked().connect (sigc::ptr_fun(start_running));
    _stop -> signal_clicked().connect (sigc::ptr_fun( stop_running));

    _save -> signal_clicked().connect (sigc::mem_fun(*this, &MainFrame::save_show));
    _load -> signal_clicked().connect (sigc::mem_fun(*this, &MainFrame::load_show));

    _dur -> signal_activate().connect (sigc::ptr_fun(duration_changed));

    _show_deriv  -> signal_toggled() .connect (sigc::mem_fun(*this, &MainFrame::toggle_deriv));
    _looping     -> signal_toggled() .connect (sigc::mem_fun(*this, &MainFrame::toggle_looping));


    //add_events(Gdk::KEY_PRESS_MASK);

    the_main_frame = this;
}

//////////////////////////////////////////////////////////////////////

MainFrame::
~MainFrame()
{
    the_main_frame = 0;
}

//////////////////////////////////////////////////////////////////////

bool MainFrame::
on_key_press_event(GdkEventKey* event)
{
    if(event->keyval == GDK_Escape) {

        hide_all();

        extern void endme(int rtn);
        endme(0);
        //exit(0);
    }

    // this is needed to have children get a look
    //Gtk::Window::on_key_press_event(event);
    _curves -> on_key_press_event(event);

    // if we get here, then we didn't handle it
    return Gtk::Window::on_key_press_event(event);
}

//////////////////////////////////////////////////////////////////////

double MainFrame::
get_duration() const
{
    double dur = atof(_dur -> get_text() . c_str());
    //printf("Dur = %g\n", dur);

    return dur;
}

//////////////////////////////////////////////////////////////////////

void MainFrame::
set_duration(double d)
{
    char buffer[64];
    sprintf(buffer, "%g", d);
    _dur -> set_text(buffer);
}

//////////////////////////////////////////////////////////////////////

void MainFrame::
save_show()
{
    const std::string filename = _filename->get_text();

    extern bool save_show(const std::string& filename); // in comp.cc
    bool rtn = save_show(filename);

    if (! rtn) {
        fprintf(stderr, "Save show to '%s' failed\n", filename.c_str());
    }
}

//////////////////////////////////////////////////////////////////////

void MainFrame::
load_show()
{
    const std::string filename = _filename->get_text();

    extern bool load_show(const std::string& filename); // in comp.cc
    bool rtn = load_show(filename);

    if (! rtn) {
        fprintf(stderr, "Load show to '%s' failed\n", filename.c_str());
    }

    extern void redraw_curve_pane(); // in curve_pane.cc
    redraw_curve_pane();
}

//////////////////////////////////////////////////////////////////////

void MainFrame::
toggle_deriv()
{
    bool have_deriv = _show_deriv->get_active();

    printf("Turn deriv %s\n", have_deriv ? "on" : "off");


    void set_deriv_visible(bool visible); // in scroller.cc
    set_deriv_visible(have_deriv);
}

//////////////////////////////////////////////////////////////////////

void MainFrame::
toggle_looping()
{
    bool is_looping = _looping->get_active();

    printf("Turn looping %s\n", is_looping ? "on" : "off");

    set_looping( is_looping );
}

//////////////////////////////////////////////////////////////////////

bool MainFrame::
on_delete_event(GdkEventAny* /* ignore_event */ )
{
    fprintf(stderr, "In TopWindow::on_delete_event()\n");

    extern void endme(int rtn); // in main.cc
    endme(0);

    return false; // let someone else handle actually shutting down
}

//////////////////////////////////////////////////////////////////////

static double get_duration()
{
    const double Default_Duration = 10;

    return (the_main_frame
            ?  the_main_frame -> get_duration()
            :  Default_Duration);
}

//////////////////////////////////////////////////////////////////////

void set_displayed_duration(double dur)
{
    if (the_main_frame) {
        the_main_frame -> set_duration(dur);
        the_main_frame -> queue_draw();
    }
}

//////////////////////////////////////////////////////////////////////

void duration_changed()
{
    double duration = get_duration();
    set_output_duration(duration);
    set_displayed_duration(duration);

    /*
    extern void redraw_time_bar();
    redraw_time_bar();
    */
    if (the_main_frame)
        the_main_frame -> queue_draw();
}

//////////////////////////////////////////////////////////////////////

void set_status_label(const char* str)
{
  if (the_status_label)
    the_status_label -> set_text(str);
}


//////////////////////////////////////////////////////////////////////
