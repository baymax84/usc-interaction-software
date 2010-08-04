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

#ifndef MAIN_FRAME_HH
#define MAIN_FRAME_HH

#include <gtkmm/window.h>

class Selector;
class Scroller;
namespace Gtk { class Button; }
namespace Gtk { class Entry; }
namespace Gtk { class CheckButton; }
namespace Gtk { class Label; }

class MainFrame
    : public Gtk::Window
{
public:
    MainFrame();
    virtual ~MainFrame();

    double get_duration() const;
    void set_duration(double d);

    void set_sound_info(const char* filename, double delay, bool play);

private:
  virtual bool on_key_press_event(GdkEventKey* event);

    void save_show();
    void load_show();

    void toggle_deriv();
    void toggle_looping();

protected:
    virtual bool on_delete_event(GdkEventAny* event);

private:
    Selector*  _selector;
    Scroller*  _curves;

    Gtk::Button* _run;
    Gtk::Button* _stop;
    Gtk::CheckButton* _looping;

    Gtk::Entry*  _dur;

    Gtk::Button* _save;
    Gtk::Button* _load;
    Gtk::Entry*  _filename;

    Gtk::CheckButton* _show_deriv;

    Gtk::Label* _status;
};

#endif // MAIN_FRAME_HH
