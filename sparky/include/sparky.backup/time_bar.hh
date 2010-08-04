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

#ifndef TIME_BAR_HH
#define TIME_BAR_HH

#include <gtkmm/drawingarea.h>


class TimeBar
    : public Gtk::DrawingArea
{
public:
    TimeBar(Gtk::Adjustment* adjust);
    virtual ~TimeBar();

private:
    virtual void on_realize();
    virtual bool on_configure_event(GdkEventConfigure* event);
    virtual bool on_expose_event(GdkEventExpose* event);

    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_leave_notify_event(GdkEventCrossing* event);
    virtual bool on_motion_notify_event(GdkEventMotion* event);


private:
    int  _wid, _hei;

    Glib::RefPtr<Gdk::GC> _gc;
    Gdk::Color _red, _grey, _ltgrey, _yellow;

    Gtk::Adjustment* _adjust;
};

#endif  // TIME_BAR_HH

