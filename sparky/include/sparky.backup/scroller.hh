// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef SCROLLER_HH
#define SCROLLER_HH

//#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>

class CurvePane;
class TimeBar;
namespace Gtk { class HScrollbar; }
namespace Gtk { class Adjustment; }


class Scroller
    : public Gtk::VBox
{
public:
    Scroller();
    virtual ~Scroller();

    virtual bool on_key_press_event(GdkEventKey* event);

    void set_deriv_visible(bool visible);

private:
    Gtk::HScrollbar* _bar;
    CurvePane* _curve_pane;
    CurvePane* _deriv_pane;
    TimeBar*   _time_bar;
};

#endif  // SCROLLER_HH
