// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "curve_set.hh"
#include "global.hh"

//////////////////////////////////////////////////////////////////////

static CurveSet* the_curve_set = 0;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void init_computing_env()
{
    the_curve_set = new BezSet;
}

//////////////////////////////////////////////////////////////////////

CurveStatus get_output_data(double time, double vals[], const bool active[])
{
    return (the_curve_set
            ?  the_curve_set -> get_values(time, vals, active)
            :  ErrorStatus);
}

//////////////////////////////////////////////////////////////////////

void get_plot_points(unsigned int i, std::vector<Gdk::Point> &points, int hei)
{
    if (the_curve_set) {
        the_curve_set -> get_plot_points(i, points, hei);
    }
}

//////////////////////////////////////////////////////////////////////

void get_deriv_points(unsigned int i, std::vector<Gdk::Point> &points, int hei)
{
    if (the_curve_set) {
        the_curve_set -> get_deriv_points(i, points, hei);
    }
}

//////////////////////////////////////////////////////////////////////

Curve* get_curve(int i)
{
    return (the_curve_set != 0
            ?  the_curve_set -> curve(i)
            :  0);
}

//////////////////////////////////////////////////////////////////////

void reset_output_curves(double t)
{
    if (the_curve_set) {
        the_curve_set -> reset(t);
    }
}

//////////////////////////////////////////////////////////////////////

double get_output_frac(double now)
{
    return (the_curve_set != 0
            ?  the_curve_set -> get_output_frac(now)
            :  0);
}

//////////////////////////////////////////////////////////////////////

void set_output_frac(double now, double frac)
{
    if (the_curve_set != 0)
        the_curve_set -> set_output_frac(now, frac);
}

//////////////////////////////////////////////////////////////////////

void set_output_duration(double duration)
{
    if (the_curve_set != 0)
        the_curve_set -> set_duration(duration);
}

//////////////////////////////////////////////////////////////////////

double get_output_duration()
{
    return (the_curve_set != 0
            ?  the_curve_set -> duration()
            :  10);
}

//////////////////////////////////////////////////////////////////////

bool save_show(const std::string& filename)
{
    if (! the_curve_set)
        return false;

    printf("save_show (%s)\n", filename.c_str());

    return the_curve_set -> write(filename);
}

//////////////////////////////////////////////////////////////////////

bool load_show(const std::string& filename)
{
    if (! the_curve_set)
        return false;

    printf("load_show (%s)\n", filename.c_str());

    return the_curve_set -> read(filename);
}
