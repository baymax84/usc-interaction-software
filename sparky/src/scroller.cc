// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "scroller.hh"
#include "curve_pane.hh"
#include "time_bar.hh"

#include <gtkmm/scrollbar.h>
#include <gtkmm/adjustment.h>

static Scroller* the_scroller = 0;

Scroller::
Scroller()
    : Gtk::VBox(false, 5),
      _bar        ( new Gtk::HScrollbar() ),
      _curve_pane ( new CurvePane( _bar->get_adjustment(), false ) ),
      _deriv_pane ( new CurvePane( _bar->get_adjustment(), true ) ),
      _time_bar   ( new TimeBar  ( _bar->get_adjustment() ) )
{
    pack_start( *_deriv_pane, true, true );
    pack_start( *_curve_pane, true, true );
    pack_start( *_time_bar, false, true );
    pack_start( *_bar, false, true );

    show_all();

    add_events(Gdk::KEY_PRESS_MASK);

    Gtk::Adjustment* adjust = _bar->get_adjustment();

    adjust->set_value(0.0);
    adjust->set_lower(0.0);
    adjust->set_upper(1.0);

    adjust->set_page_size(1.0);
    adjust->set_page_increment(1.0);
    adjust->set_step_increment(0.2);

    the_scroller = this;
}

Scroller::
~Scroller()
{
  the_scroller = 0;

    delete _curve_pane;
    delete _deriv_pane;
}


bool Scroller::
on_key_press_event(GdkEventKey* event)
{
    //printf("Scroller::on_key_press_event()\n");

    switch (event->keyval) {

        // zoom in
    case 'z':
        {
            Gtk::Adjustment* adjust = _bar->get_adjustment();

            double value =  adjust -> get_value();
            double page  =  adjust -> get_page_size();

            // zoom in
            page *= 0.5; // page size is half what it used to be
            // zoom around center (if not at the begining
            if (value != 0) {
                value += page * 0.5;
                if (value + page > 1.0)
                    value = 1.0 - page;
            }

            adjust->set_value(value);
            adjust->set_page_size(page);
            adjust->set_page_increment(page);
            adjust->set_step_increment(page / 8.);
        }
        break;

        // zoom out
    case 'Z':
        {
            Gtk::Adjustment* adjust = _bar->get_adjustment();

            double value =  adjust -> get_value();
            double page  =  adjust -> get_page_size();

            // zoom out
            page *= 2.0; // page size is half what it used to be
            if (page > 1.0)
                page = 1.0;
            // zoom around center (if not at the begining
            if (page == 1.0) {
                value = 0;
            }
            else if (value != 0) {
                value -= page * 0.25;
                if (value < 0.0)          value = 0.0;
                if (value + page > 1.0)   value = 1.0 - page;
            }

            adjust->set_value(value);
            adjust->set_page_size(page);
            adjust->set_page_increment(page);
            adjust->set_step_increment(page / 8.);
        }
        break;


    default:
        // pass it down to child
        break;
    }

    // if we get here, then we didn't handle it
    //return false;
    return Gtk::VBox::on_key_press_event(event);

}

void set_deriv_visible(bool visible)
{
  if (the_scroller)
    the_scroller -> set_deriv_visible(visible);
}

void Scroller::
set_deriv_visible(bool visible)
{
  if (visible)
    _deriv_pane -> show();
  else
    _deriv_pane -> hide();
}
