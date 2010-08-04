// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "global.hh"
#include "selector.hh"

#include <gtkmm/checkbutton.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>

extern void redraw_curve_pane();

int get_active_curve(); // below
void set_active_curve(int i); // below


static const int N_Rows = N_Curves + 1;
static const int N_Cols =  4;

static const char* Names[N_Curves]
= { "Mouth",              //  1
    "Head Nod",           //  2
    "Head Turn",          //  3
    "Right Arm Forw",     //  4
    "Right Arm Out",      //  5
    "Right Elbow",        //  6
    "Left Arm Forw",      //  7
    "Left Arm Out",       //  8
    "Left Elbow",         //  9
    "Right Wrist",        // 10
    "Left Wrist",         // 11
    "Body Forw",          // 12
    "Eyelids",            // 13
    "Eye Left/Right",     // 14
    "Right Foot Up",      // 15
    "Right Foot Forward", // 16
    "Left Foot Up",       // 17
    "Left Foot Forward",  // 18
 };

//////////////////////////////////////////////////////////////////////

class ClickLabel;

static struct {
    ClickLabel        *label;
    bool               visible;
    Gtk::CheckButton  *visible_button;
    bool               outputting;
    Gtk::CheckButton  *outputting_button;
} CurveData[N_Curves];

//////////////////////////////////////////////////////////////////////

bool get_curve_visible(int index)
{
    return CurveData[index].visible;
}
bool get_curve_outputting(int index)
{
    return CurveData[index].outputting;
}
void get_curve_outputting(bool outputting[])
{
    for (int i=0; i<N_Curves; ++i)
        outputting[i] = CurveData[i].outputting;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class ClickLabel
    : public Gtk::EventBox
{
public:
    ClickLabel(int id);
    virtual ~ClickLabel();

    void set_bold();
    void set_normal();

private:
    const int _id;
    Gtk::Label _label;
    bool on_button_press_event(GdkEventButton* event);
};

ClickLabel::
ClickLabel(int id)
    : _id(id),
      _label(Names[id])
{
    add(_label);
    show_all();

    add_events(Gdk::BUTTON_PRESS_MASK);
}


ClickLabel::
~ClickLabel()
{
}

bool ClickLabel::
on_button_press_event(GdkEventButton* /* unused-event */)
{
    if (_id != get_active_curve()) {
        set_active_curve(_id);
    }
    return false; // pass on to parent
}

void ClickLabel::
set_bold()
{
    _label . modify_font(Pango::FontDescription("Bold 14"));
}

void ClickLabel::
set_normal()
{
    _label . modify_font(Pango::FontDescription(""));
}

//////////////////////////////////////////////////////////////////////

static void toggle_visible(int i)
{
    CurveData[i].visible = CurveData[i].visible_button->get_active();
    redraw_curve_pane();
}

static void toggle_outputting(int i)
{
    CurveData[i].outputting = CurveData[i].outputting_button->get_active();
}

Selector::
Selector()
    : Gtk::Table(N_Rows, N_Cols)
{

    set_col_spacings(25);
    set_row_spacings( 5);

    //
    // labels at the top of columns
    {
        Gtk::Label* label = Gtk::manage(new Gtk::Label("Visible") );
        attach(*label,  2,3,  0,1, Gtk::SHRINK);

        label = Gtk::manage(new Gtk::Label("Follow") );
        attach(*label,  3,4,  0,1, Gtk::SHRINK);
    }

    for (int i=0; i<N_Curves; i++) {

        char lineNumber[8];
        sprintf(lineNumber, "%d", i+1);

        attach(*Gtk::manage(new Gtk::Label(lineNumber) ),
               0,1,  i+1,i+2, Gtk::SHRINK);

        ClickLabel *label = Gtk::manage( new ClickLabel(i) );
        attach(*label,  1,2,  i+1,i+2, Gtk::EXPAND);
        CurveData[i].label = label;

        //
        // check button for visibility
        {
            Gtk::CheckButton* butt = Gtk::manage(new Gtk::CheckButton(""));
            attach(*butt,  2,3,  i+1,i+2, Gtk::SHRINK);
            CurveData[i].visible_button = butt;

            // only the first curve is visible initially
            bool visible = i==0;
            butt -> set_active(visible);
            CurveData[i].visible = visible;
            butt -> signal_toggled().connect (sigc::bind(sigc::ptr_fun(toggle_visible), i));
        }

        //
        // check button for outputting (when run)
        {
            Gtk::CheckButton* butt = Gtk::manage(new Gtk::CheckButton(""));
            attach(*butt,  3,4,  i+1,i+2, Gtk::SHRINK);
            CurveData[i].outputting_button = butt;

            butt -> set_active(false);
            CurveData[i].outputting = false;
            butt -> signal_toggled().connect (sigc::bind(sigc::ptr_fun(toggle_outputting), i));
        }
    }

    CurveData[0].label -> set_bold();
}

//////////////////////////////////////////////////////////////////////

Selector::
~Selector()
{
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

static int ActiveCurve = 0;

int get_active_curve()
{
    return ActiveCurve;
}
void set_active_curve(int i)
{
    if (ActiveCurve != i) {

        CurveData[ActiveCurve].label -> set_normal();

        // if you select a curve, make visible if it's not currently
        if (CurveData[i].visible == false)
            CurveData[i].visible_button->set_active(true);

        CurveData[i].label -> set_bold();

        ActiveCurve = i;

        redraw_curve_pane();
    }
}

