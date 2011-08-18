// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "sparky/global.hh"
#include "sparky/curve_pane.hh"
#include "sparky/curve_set.hh"
#include "sparky/comp.hh"

#include <list>
#include <cmath>

#include <gdk/gdkkeysyms.h>
#include <gtkmm/adjustment.h>
#include <pangomm.h>

extern int  get_active_curve();
extern void set_active_curve(int i);
extern bool get_curve_visible(int index);

static bool monitor_progress_fun(); // defined below

static CurvePane* the_curve_pane = 0;

static CurvePane* the_pane = 0;
static CurvePane* the_deriv_pane = 0;

//////////////////////////////////////////////////////////////////////

void redraw_curve_pane()
{
    if (the_curve_pane != 0)   the_curve_pane->queue_draw();
    if (the_deriv_pane != 0)   the_deriv_pane->queue_draw();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

int get_x2pix(double x)
{
    return (the_pane
            ?  the_pane -> x2pix(x)
            :  0);
}
int get_y2pix(double y)
{
    return (the_pane
            ?  the_pane -> y2pix(y)
            :  0);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

static Anchor *active_anchor = 0;
static Anchor *drag_anchor   = 0;

static int drag_x;
static int drag_y; //, drag_min_y, drag_max_y;
static double  drag_min_x, drag_max_x;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

static double output_x = 0;  // in [0,1]
double get_curve_pane_output_x() {
    return output_x;
}
void set_curve_pane_output_x(double x) {
    output_x = x;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CurvePane::
CurvePane(Gtk::Adjustment* adjust, bool deriv)
  : _adjust(adjust),
    _deriv(deriv),

    _x_min( 0.0 ),
    _x_max( 1.0 ),
    _y_min( _deriv ? -10.0 : 0.0 ),
    _y_max( _deriv ?  10.0 : 1.0 )
{

    Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap();\

    _blue    = Gdk::Color("blue");        colormap->alloc_color(_blue);
    _red     = Gdk::Color("red");         colormap->alloc_color(_red);
    _green   = Gdk::Color("green");       colormap->alloc_color(_green);
    _black   = Gdk::Color("black");       colormap->alloc_color(_black);
    _white   = Gdk::Color("white");       colormap->alloc_color(_white);
    _grey    = Gdk::Color("grey");        colormap->alloc_color(_grey);
    _ltgrey  = Gdk::Color("grey90");      colormap->alloc_color(_ltgrey);
    _dkgrey  = Gdk::Color("grey50");      colormap->alloc_color(_dkgrey);
    _yellow  = Gdk::Color("yellow");      colormap->alloc_color(_yellow);

    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::ENTER_NOTIFY_MASK);
    add_events(Gdk::LEAVE_NOTIFY_MASK);
    add_events(Gdk::POINTER_MOTION_MASK);
    add_events(Gdk::POINTER_MOTION_HINT_MASK);

    add_events(Gdk::KEY_PRESS_MASK);

    //_adjust->signal_changed().connect( sigc::mem_fun(*this, &CurvePane::queue_draw));
    _adjust->signal_changed().connect( sigc::ptr_fun(redraw_curve_pane));
    //_adjust->signal_value_changed().connect( sigc::mem_fun(*this, &CurvePane::queue_draw));
    _adjust->signal_value_changed().connect( sigc::ptr_fun(redraw_curve_pane));

    //Glib::signal_idle().connect( sigc::ptr_fun(monitor_progress_fun), Glib::PRIORITY_LOW );
    static const int TimeoutTime = 20; // 20 = 50 Hz
    Glib::signal_timeout().connect( sigc::ptr_fun(monitor_progress_fun),
                                    TimeoutTime,
                                    Glib::PRIORITY_LOW );

    if (_deriv)
      the_deriv_pane = this;
    else
      the_curve_pane = this;
}


CurvePane::
~CurvePane()
{
  if (this == the_curve_pane)
    the_curve_pane = 0;
  else
    the_deriv_pane = 0;
}



bool CurvePane::
on_configure_event(GdkEventConfigure* event)
{
    // We need to call the base on_realize()
    Gtk::DrawingArea::on_configure_event(event);

    _wid = get_width();
    _hei = get_height();

    _x_scalar = double(_wid) / (_x_max - _x_min);
    _y_scalar = double(_hei) / (_y_max - _y_min);

   // We've handled the configure event, no need for further processing.
    return true;
}

//////////////////////////////////////////////////////////////////////

void CurvePane::
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

bool CurvePane::
on_expose_event(GdkEventExpose*  /* event */)
{
    the_pane = this;

    Glib::RefPtr<Gdk::Window> window = get_window();


    // draw text ("Position" or "Velocity") in upper right corner
    {
        Glib::RefPtr<Pango::Layout> lay = create_pango_layout(_deriv
                                                              ?  "Velocity"
                                                              :  "Position");
        Pango::FontDescription fd;
        fd.set_size(24        // font size in points
                    * 1024);  // "pango units" (1024 per font point)
        lay->set_font_description(fd);
        _gc -> set_foreground(_dkgrey);
        window->draw_layout(_gc, 10, 5, lay);
    }


    _x_min = _adjust->get_value();
    if (_x_min < 0.0) _x_min = 0.0;
    _x_max   = _x_min + _adjust->get_page_size();
    if (_x_max > 1.0) _x_max = 1.0;

    _x_scalar = double(_wid) / (_x_max - _x_min);
    _y_scalar = double(_hei) / (_y_max - _y_min);

    // draw grid
    {
        // set up line properties
        _gc -> set_foreground(_grey);
        _gc -> set_line_attributes(1,                  // line_width
                                   Gdk::LINE_SOLID,    // LineStyle
                                   Gdk::CAP_NOT_LAST,  // CapStyle
                                   Gdk::JOIN_MITER);   // JoinStyle

        extern double compute_timeline_dt(double duration);
        const double duration = get_output_duration();
        double dt = compute_timeline_dt(duration);

        for (double tx=0; tx<duration; tx += dt) {
            int x = x2pix(tx / duration);
            window->draw_line(_gc, //get_style()->get_black_gc(),
                              x, 0, x, _hei-1);
        }

	if (_deriv) {
            for (int i=-10; i<=10; i += 2) {
                int y = y2pix(i);
                window->draw_line(_gc, //get_style()->get_black_gc(),
                                  0, y, _wid-1, y);
            }

            // zero line
            int y = y2pix(0);
            _gc -> set_foreground(_dkgrey);
            _gc -> set_line_attributes(3,                  // line_width
                                       Gdk::LINE_SOLID,    // LineStyle
                                       Gdk::CAP_NOT_LAST,  // CapStyle
                                       Gdk::JOIN_MITER);   // JoinStyle
            window->draw_line(_gc, //get_style()->get_black_gc(),
                              0, y, _wid-1, y);
        }
	else {
            for (int i=1; i<=10; i++) {
                int y = y2pix(i / 10.0);
                window->draw_line(_gc, //get_style()->get_black_gc(),
                                  0, y, _wid-1, y);
            }
	}
    }

    // draw line showing what's currently being played
    if (output_x >= _x_min   &&  output_x <= _x_max) {

        // set up line properties
        _gc -> set_foreground(_red);
        _gc -> set_line_attributes(3,                  // line_width
                                   Gdk::LINE_SOLID,    // LineStyle
                                   Gdk::CAP_NOT_LAST,  // CapStyle
                                   Gdk::JOIN_MITER);   // JoinStyle

        window->draw_line(_gc,
                          x2pix(output_x), 0,
                          x2pix(output_x), _hei-1);
    }


    int active_curve = get_active_curve();


    // the curve
    {
        if (_deriv)
            _gc -> set_foreground(_green);
        else
            _gc -> set_foreground(_blue);

        // draw the curves
        for (int i=0; i<N_Curves; ++i) {
            if (get_curve_visible(i)) {

                std::vector<Gdk::Point> points;
		if (_deriv)
                    get_deriv_points(i, points, _hei);
		else
                    get_plot_points (i, points, _hei);

                if (i == active_curve)
                    _gc -> set_line_attributes(4,                  // line_width
                                               Gdk::LINE_SOLID,    // LineStyle
                                               Gdk::CAP_NOT_LAST,  // CapStyle
                                               Gdk::JOIN_BEVEL);   // JoinStyle
                else
                    _gc -> set_line_attributes(2,                  // line_width
                                               Gdk::LINE_SOLID,    // LineStyle
                                               Gdk::CAP_NOT_LAST,  // CapStyle
                                               Gdk::JOIN_BEVEL);   // JoinStyle

                window->draw_lines(_gc, points);
            }
        }


        // draw the anchors
        if (get_curve_visible(active_curve)  &&  !_deriv) {
            Curve* curve = get_curve(active_curve);

            // draw any handle lines
            double pt1[2], pt2[2];
            for (int cnt=0; curve->get_handle_line(cnt, pt1, pt2); ++cnt)
                window->draw_line(get_style()->get_black_gc(),
                                  x2pix(pt1[0]), y2pix(pt1[1]),
                                  x2pix(pt2[0]), y2pix(pt2[1]));

            const Anchor* anchor = 0;
            bool highlighted = false;
            for (int cnt=0;  (anchor = curve->get_anchor(cnt, highlighted)) != 0; ++cnt)
                draw_anchor_1(x2pix(anchor->x()), y2pix(anchor->y()), highlighted);
            for (int cnt=0;  (anchor = curve->get_handle(cnt, highlighted)) != 0; ++cnt)
                draw_anchor_2(x2pix(anchor->x()), y2pix(anchor->y()), highlighted);

        }
    }


    // black border around entire pane
    window->draw_rectangle(get_style()->get_black_gc(),
			   false,
			   0, 0, _wid-1, _hei-1);


    // We've handled it, no need for further processing.
    return true;
}

//////////////////////////////////////////////////////////////////////

bool CurvePane::
on_button_press_event(GdkEventButton* event)
{
    // ignore mouse clicks in derivative window
    if (_deriv)
      return false; // pass on to parent


    // left mouse button (button 1)
    if (event->button == 1) {

        //
        // shift left mouse means add an anchor
        //
        if (event->state & GDK_SHIFT_MASK) {

            Curve* curve = get_curve( get_active_curve() );
            int x = int(event->x + 0.5);
            int y = int(event->y + 0.5);

            if ( ! curve -> find_anchor_near_x(x) ) {
                curve -> add_anchor_at( pix2x( x ) );
                curve -> calc_coef();

                active_anchor = curve -> find_near_anchor(x, y,
                                                          drag_min_x,
                                                          drag_max_x);
                redraw_curve_pane();
            }
        }

        //
        // control left mouse means delete an anchor
        //
        else if (event->state & GDK_CONTROL_MASK) {
            if (active_anchor != 0) {
                Curve* curve = get_curve( get_active_curve() );
                curve -> remove_anchor( active_anchor );

                active_anchor = 0;
                curve -> calc_coef();
                redraw_curve_pane();
            }
        }

        // active_anchor is set if the mouse is near an anchor
        // select it for dragging
        else {

            // if an anchor is active, now it's being dragged
            //
            if (active_anchor != 0) {
                // remember this as the starting point of the drag
                drag_x = int(event->x + 0.5);
                drag_y = int(event->y + 0.5);
            }
            // we didn't click on an anchor, did we click on a curve?
            else {
                double x = pix2x( int(event->x + 0.5) );
                int nearest = -1, nearest_dist = 0;
                for (int i=0; i<N_Curves; ++i) {
                    if (get_curve_visible(i)) {
                        double curve_y;
                        if (OkayStatus == get_curve(i) -> get_value(x, curve_y)) {
                            int dist = abs(int(event->y) - y2pix(curve_y));
                            printf("for x=%g, curve[%d].y=%g => %d pix diff\n",
                                   x, i, curve_y, dist);
                            if (nearest < 0  ||  dist < nearest_dist) {
                                nearest = i;
                                nearest_dist = dist;
                            }
                        }
                    }
                }
                int MinDist = 10;
                if (nearest >= 0  &&  nearest_dist <= MinDist ) {
                    set_active_curve(nearest);
                }
            }

            // store this as the anchor being dragged
            drag_anchor = active_anchor;
        }

        // redraw the screen
	redraw_curve_pane();

    } // end if button == 1


    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool CurvePane::
on_button_release_event(GdkEventButton* event)
{
    if (event->button == 1) {
        if (drag_anchor != 0) {
            drag_anchor = 0;
            Curve* curve = get_curve( get_active_curve() );
            active_anchor = curve -> find_near_anchor(int(event->x), int(event->y),
						      drag_min_x, drag_max_x);
            redraw_curve_pane();
        }
    }

    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool CurvePane::
on_enter_notify_event(GdkEventCrossing* /* event */ )
{
    the_pane = this;
    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool CurvePane::
on_leave_notify_event(GdkEventCrossing* /* event */ )
{
    the_pane = 0;

    void set_status_label(const char* str); // in main_frame.cc
    set_status_label("Mouse:");

    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool CurvePane::
on_key_press_event(GdkEventKey* event)
{
    switch (event->keyval) {
    case GDK_Delete:
    case GDK_BackSpace:

        if (active_anchor != 0) {
            Curve* curve = get_curve( get_active_curve() );
            curve -> remove_anchor( active_anchor );

            active_anchor = 0;
            curve -> calc_coef();
            redraw_curve_pane();
        }

        break;

    default:
        // fall through
        break;
    }

    //return false; // pass on to parent
    return true; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

bool CurvePane::
on_motion_notify_event(GdkEventMotion* event)
{
  extern double get_current_duration(); // in curve_set.cc
  bool set_status_string = false;

    int x, y;
    Gdk::ModifierType state;
    bool hint = (event->is_hint);

    //
    // get the x, y, and state
    //
    if (hint) {
        Glib::RefPtr<Gdk::Window> window = get_window();
        window -> get_pointer(x, y, state);
    }
    else
    {
        x = (int)event->x;
        y = (int)event->y;
        state = Gdk::ModifierType(event->state);
    }

    //
    // if we're dragging an anchor around
    //
    if (drag_anchor != 0) {

        int dx = x - drag_x;
        int dy = y - drag_y;

        if (dx != 0  ||  dy != 0) {

            drag_anchor -> move(pix2dx(dx),
                                pix2dy(dy));


            // clip motion at edge of screen
            if (drag_anchor->y() < 0.0) {
                y += y2pix(-drag_anchor->y());
                drag_anchor->y(0);
            }
            if (drag_anchor->y() > 1.0) {
                y += _hei - y2pix(drag_anchor->y());
                drag_anchor->y(1.0);
            }
            if (drag_anchor->x() < drag_min_x) {
                x += int(0.5 - (drag_anchor->x()-drag_min_x) * _x_scalar);
                drag_anchor->x(drag_min_x);
            }
            if (drag_anchor->x() > drag_max_x) {
                x += x2pix(drag_max_x) - int(0.5 + (drag_anchor->x()-_x_min) * _x_scalar);
                drag_anchor->x(drag_max_x);
            }

            drag_x = x;
            drag_y = y;

            get_curve( get_active_curve() ) -> calc_coef();
            redraw_curve_pane();

	    char status[64];
            if (_deriv) {
                sprintf(status, "Anchor:  t = %5.2f sec,  vel = %4.1f deg/sec",
                        drag_anchor->x() * get_current_duration(),
                        -180. *  drag_anchor->y() / get_current_duration());
            }
            else {
                sprintf(status, "Anchor:  t = %5.2f sec,  pos = %4.1f deg",
                        drag_anchor->x() * get_current_duration(),
                        180.0 *  (0.5 - drag_anchor->y()));
            }

	    void set_status_label(const char* str); // in main_frame.cc
	    set_status_label(status);
	    set_status_string = true;
        }        
    }

    // so we're not dragging an anchor
    else {
        Anchor *last_active_anchor = active_anchor;

        Curve* curve = get_curve( get_active_curve() );
        active_anchor = curve -> find_near_anchor(x, y,
						  drag_min_x,
						  drag_max_x);

        bool anchor_changed = (active_anchor != last_active_anchor);
        if (anchor_changed)
	  redraw_curve_pane();
    }

    //printf("motion @ %s %d,%d %d\n", hint?"Hint":"", x, y, int(state));
    if (!set_status_string) {

      char status[64];
      if (_deriv) {
          sprintf(status, "Mouse:  t = %5.2f, vel = %5.1f deg/sec",
                  pix2x(x) * get_current_duration(),
                  -180. * pix2y(y) / get_current_duration());
      }
      else {
          sprintf(status, "Mouse:  t = %5.2f sec,  pos = %4.1f deg",
                  pix2x(x) * get_current_duration(),
                  180.0 * (0.5 - pix2y(y)));
      }

      void set_status_label(const char* str); // in main_frame.cc
      set_status_label(status);
    }

    return false; // pass on to parent
}

//////////////////////////////////////////////////////////////////////

void CurvePane::
draw_anchor_1(int x, int y, bool highlight)
{
    const int HalfSize = 3;
    const int Size = 2*HalfSize + 1;

    if (highlight) {
        _gc -> set_foreground(_red);
        get_window()->draw_rectangle(_gc,
                                     true, // filled
                                     x-HalfSize, y-HalfSize, Size, Size);
    }

    _gc -> set_foreground(_black);
    _gc -> set_line_attributes(2,                  // line_width
                               Gdk::LINE_SOLID,    // LineStyle
                               Gdk::CAP_NOT_LAST,  // CapStyle
                               Gdk::JOIN_MITER);   // JoinStyle
    get_window()->draw_rectangle(_gc,
                                 false, // filled
                                 x-HalfSize, y-HalfSize, Size, Size);
}

//////////////////////////////////////////////////////////////////////

void CurvePane::
draw_anchor_2(int x, int y, bool highlight)
{
    const int HalfSize = 3;

    // define a diamond
    std::vector<Gdk::Point> data(5);
    data[0] = Gdk::Point(x-HalfSize, y);
    data[1] = Gdk::Point(x, y+HalfSize);
    data[2] = Gdk::Point(x+HalfSize, y);
    data[3] = Gdk::Point(x, y-HalfSize);
    data[4] = Gdk::Point(x-HalfSize, y);

    if (highlight) {
        _gc -> set_foreground(_red);
        get_window()->draw_polygon(_gc,
                                   true, // filled
                                   data);
    }

    _gc -> set_foreground(_black);
    _gc -> set_line_attributes(1,                  // line_width
                               Gdk::LINE_SOLID,    // LineStyle
                               Gdk::CAP_NOT_LAST,  // CapStyle
                               Gdk::JOIN_MITER);   // JoinStyle
    get_window()->draw_polygon(_gc,
                               false, // filled
                               data);
}

void CurvePane::
adjust_changed()
{
  printf("adjust_changed:  ");
  printf("%g in [%g,%g],  (%g, %g, %g)\n",
	 _adjust->get_value(), _adjust->get_lower(), _adjust->get_upper(),
	 _adjust->get_page_increment(), _adjust->get_page_size(), _adjust->get_step_increment());
}

void CurvePane::
adjust_value_changed()
{
  printf("adjust_value_changed:  ");
  printf("%g in [%g,%g],  (%g, %g, %g)\n",
	 _adjust->get_value(), _adjust->get_lower(), _adjust->get_upper(),
	 _adjust->get_page_increment(), _adjust->get_page_size(), _adjust->get_step_increment());
}


static bool monitor_progress_fun()
{
    static double last_output_x = -1;
    if (output_x != last_output_x) {
        redraw_curve_pane();

        extern void redraw_time_bar(); // int time_bar.cc
        redraw_time_bar();

        last_output_x = output_x;
    }

    return true; // infinite loop
}


