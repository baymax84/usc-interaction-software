// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef CURVE_PANE_HH
#define CURVE_PANE_HH

#include "sparky/anchor.hh"
#include <gtkmm/drawingarea.h>

class Gtk::Adjustment;

class CurvePane
    : public Gtk::DrawingArea
{
public:
    CurvePane(Gtk::Adjustment* adjust, bool deriv=false);
    virtual ~CurvePane();

    void draw_anchor_1(int x, int y, bool highlight);
    void draw_anchor_2(int x, int y, bool highlight);

private:
    virtual void on_realize();
    virtual bool on_configure_event(GdkEventConfigure* event);
    virtual bool on_expose_event(GdkEventExpose* event);

    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_enter_notify_event(GdkEventCrossing* event);
    virtual bool on_leave_notify_event(GdkEventCrossing* event);
    virtual bool on_motion_notify_event(GdkEventMotion* event);

    virtual bool on_key_press_event(GdkEventKey* event);

    void adjust_changed();
    void adjust_value_changed();

private:
    int              _wid, _hei;

    Glib::RefPtr<Gdk::GC> _gc;
    Gdk::Color _blue, _red, _green, _black, _white, _grey, _ltgrey, _dkgrey, _yellow;

    Gtk::Adjustment* _adjust;
    const bool _deriv;

    double _x_min;
    double _x_max;
    double _x_scalar;

    double _y_min;
    double _y_max;
    double _y_scalar;

public:
    int x2pix(double x) {
        return int((x-_x_min) * _x_scalar + 0.5);
    }
    int y2pix(double y) {
        return int((y-_y_min) * _y_scalar + 0.5);
    }
    double pix2x(int x) {
        return x / _x_scalar + _x_min;
    }
    double pix2y(int y) {
        return y / _y_scalar + _y_min;
    }
    double pix2dx(int x) {
        return x / _x_scalar;
    }
    double pix2dy(int y) {
        return y / _y_scalar;
    }

};

#endif // CURVE_PANE_HH
