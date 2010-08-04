// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

//
// class TimeBar
//
// Draws the bar below plots that shows the time in seconds
//

#include "global.hh"
#include "time_bar.hh"

#include "comp.hh"

#include <gtkmm/adjustment.h>
#include <vector>

extern double get_output_time(); // in curve_pane.cc
extern void set_output_frac(double t); // in player.cc
extern double get_duration(); // in main_frame.cc
extern void set_status_label(const char* str); // in main_frame.cc

//////////////////////////////////////////////////////////////////////

static TimeBar* the_time_bar = 0;

//////////////////////////////////////////////////////////////////////

static double x_min    = 0.0;
static double x_max    = 1.0;
static double x_scalar = 800;

static double y_scalar = 800;

//////////////////////////////////////////////////////////////////////

static inline int x2pix(double x) {
    return int((x-x_min) * x_scalar + 0.5);
}

static inline int y2pix(double y) {
    return int(y * y_scalar + 0.5);
}

//////////////////////////////////////////////////////////////////////

static inline double pix2x(int x) {
    return x / x_scalar + x_min;
}

static inline double pix2y(int y) {
    return y / y_scalar;
}


double compute_timeline_dt(double duration)
{
    double dt = 1.0;

    const int Spacing = 150;
    while ((x2pix(dt/duration) - x2pix(0)) > Spacing) {
        if ((x2pix(dt/duration) - x2pix(0)) > Spacing)  dt /= 2.0;
        if ((x2pix(dt/duration) - x2pix(0)) > Spacing)  dt /= 2.5;
        if ((x2pix(dt/duration) - x2pix(0)) > Spacing)  dt /= 2.0;
    }

    return dt;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TimeBar::
TimeBar(Gtk::Adjustment* adjust)
  : _adjust(adjust)
{
    Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap();

    _red     = Gdk::Color("red");         colormap->alloc_color(_red);
    _grey    = Gdk::Color("grey");        colormap->alloc_color(_grey);
    _ltgrey  = Gdk::Color("grey90");      colormap->alloc_color(_ltgrey);
    _yellow  = Gdk::Color("yellow");      colormap->alloc_color(_yellow);

    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::POINTER_MOTION_MASK);
    add_events(Gdk::LEAVE_NOTIFY_MASK);
    add_events(Gdk::POINTER_MOTION_HINT_MASK);

    set_size_request(-1, 20);

    _adjust->signal_changed().connect( sigc::mem_fun(*this, &TimeBar::queue_draw));
    _adjust->signal_value_changed().connect( sigc::mem_fun(*this, &TimeBar::queue_draw));

    the_time_bar = this;
}


TimeBar::
~TimeBar()
{
    the_time_bar = 0;
}



bool TimeBar::
on_configure_event(GdkEventConfigure* event)
{
    // We need to call the base on_realize()
    Gtk::DrawingArea::on_configure_event(event);

    _wid = get_width();
    _hei = get_height();

    x_scalar = double(_wid) / (x_max - x_min);
    y_scalar = double(_hei);

   // We've handled the configure event, no need for further processing.
    return true;
}

//////////////////////////////////////////////////////////////////////

void TimeBar::
on_realize()
{
    // We need to call the base on_realize()
    Gtk::DrawingArea::on_realize();

    // Now we can allocate any additional resources we need
    Glib::RefPtr<Gdk::Window> window = get_window();

    _gc = Gdk::GC::create(window);

    window->set_background(_ltgrey);
    window->clear();
}

//////////////////////////////////////////////////////////////////////

bool TimeBar::
on_expose_event(GdkEventExpose*  /* event */)
{
    x_min = _adjust->get_value();
    if (x_min < 0.0) x_min = 0.0;
    x_max   = x_min + _adjust->get_page_size();
    if (x_max > 1.0) x_max = 1.0;

    x_scalar = double(_wid) / (x_max - x_min);
    y_scalar = double(_hei);

    Glib::RefPtr<Gdk::Window> window = get_window();

    {
        // draw line showing what's currently being played
        extern double get_curve_pane_output_x();
        double output_x = get_curve_pane_output_x();

        // define a diamond
        const int HalfWid = 10;
        const int xpix  = x2pix(output_x);
        const int left  = xpix + HalfWid;
        const int right = xpix - HalfWid;

        if (left > 0 || right < _wid) {

            std::vector<Gdk::Point> data(4);
            data[0] = Gdk::Point(xpix,  0);
            data[1] = Gdk::Point(left,  _hei-1);
            data[2] = Gdk::Point(right, _hei-1);
            data[3] = Gdk::Point(xpix,  0);

            _gc -> set_foreground(_red);
            get_window()->draw_polygon(_gc,
                                       true, // filled
                                       data);
        }
    }

    // numbers
    const double duration = get_output_duration();
    double dt = compute_timeline_dt(duration);

    int start_t = int(x_min * duration / dt);
    int end_t   = int(x_max * duration / dt + 0.5);

    _gc -> set_foreground(_red);

    for (int i=start_t; i<=end_t; ++i) {
        double t = i * dt;
        int x = x2pix(t / duration);
        window->draw_line(get_style()->get_black_gc(),
                          x, 0, x, 2);

        char str[32];
        sprintf(str, "%g", t);
        Glib::RefPtr<Pango::Layout> layout = create_pango_layout(str);
        int wid, hei;
        layout -> get_pixel_size(wid, hei);
        window->draw_layout(get_style()->get_black_gc(),
                            x - wid/2, 2, layout);
    }

    // black border around entire bar
    window->draw_rectangle(get_style()->get_black_gc(),
			   false,
			   0, 0, _wid-1, _hei-1);


    // We've handled it, no need for further processing.
    return true;
}

//////////////////////////////////////////////////////////////////////

bool TimeBar::
on_button_press_event(GdkEventButton* event)
{
    int x = int(event->x);
    double t = pix2x(x);

    set_output_frac(t);
    queue_draw();

    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool TimeBar::
on_button_release_event(GdkEventButton* /* unused-event */ )
{
    //int x = int(event->x);
    //double duration = get_output_duration();
    //double t = pix2x(x);
    //set_output_time(t);
    queue_draw();

    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool TimeBar::
on_leave_notify_event(GdkEventCrossing* /* event */ )
{
    void set_status_label(const char* str); // in main_frame.cc
    set_status_label("Mouse:");

    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool TimeBar::
on_motion_notify_event(GdkEventMotion* event)
{

    int x, y;
    Gdk::ModifierType state;
    bool hint;

    //
    // get the x, y, and state
    //
    if (event->is_hint) {
        Glib::RefPtr<Gdk::Window> window = get_window();
        window -> get_pointer(x, y, state);
        hint = true;
    }
    else
        {
            x = (int)event->x;
            y = (int)event->y;
            state = Gdk::ModifierType(event->state);
            hint = false;
        }

    double t = pix2x(x);
    if (event->state & GDK_BUTTON1_MASK) { // only interested if button is pressed
        set_output_frac(t);
        queue_draw();
    }

    extern double get_current_duration(); // in curve_set.cc
    char status[64];
    sprintf(status, "Mouse:  t = %5.2f sec", t * get_current_duration());
    set_status_label( status );


    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

void redraw_time_bar()
{
    if (the_time_bar)
        the_time_bar -> queue_draw();
}

