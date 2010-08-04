// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "global.hh"
#include "comp.hh"
#include "curve.hh"
#include "bez_curve.hh"
#include "curve_set.hh"

static CurveSet* the_curve = 0;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CurveSet::
CurveSet(unsigned int size)
    : _size(size),
      _curves(new Curve* [size]),
      _duration(10),
      _start_time(0)
{
    the_curve = this;
}

//////////////////////////////////////////////////////////////////////

CurveSet::
~CurveSet()
{
    delete [] _curves;
}

//////////////////////////////////////////////////////////////////////

void CurveSet::
set_curve(int i, Curve* curve)
{
    _curves[i] = curve;
}

//////////////////////////////////////////////////////////////////////

void CurveSet::
reset(double t)
{
    _start_time = t;
}

//////////////////////////////////////////////////////////////////////

CurveStatus CurveSet::
get_values(double t, double values[], const bool active[]) const
{
    // offset and scale time
    t -= _start_time;

    double frac = (_duration > 0
                   ?  t / _duration
                   :  t);

    CurveStatus value = (frac < 1.0
                         ?  OkayStatus
                         :  DoneStatus);

    unsigned int N = size();
    for (unsigned int i=0; i<N; ++i)
        if (active==0  ||  active[i]) {
            CurveStatus rtn = curve(i) -> get_value(frac, values[i]);
            // if you get an error, stop right there
            if (rtn == ErrorStatus)  return ErrorStatus; // fail
            // if any component is finished, get the rest of the values and then finish
            if (rtn ==  DoneStatus)  value = DoneStatus;
        }

    return value;
}

//////////////////////////////////////////////////////////////////////

void CurveSet::
set_duration(double d)
{

  //
  // make it so we stay at the same position in the curve when duration changes
    extern double get_player_time();
    double player_time = get_player_time();

    double frac = get_output_frac( player_time );
    _duration = d;
    set_output_frac(player_time, frac);

    extern void set_displayed_duration(double dur); // in main_frame.cc
    set_displayed_duration(d);

}

//////////////////////////////////////////////////////////////////////

double CurveSet::
get_output_frac(double t) const
{
    return (_duration>0
            ?  (t-_start_time)/_duration 
            :  0);
}

//////////////////////////////////////////////////////////////////////

void CurveSet::
set_output_frac(double now, double frac)
{
    _start_time = now - frac*_duration;
}

//////////////////////////////////////////////////////////////////////

bool CurveSet::
read (const std::string& filename)
{

    std::string full_name = filename;
    FILE* fd = fopen(full_name.c_str(), "r");
    if (fd == NULL) {
        perror(filename.c_str());
        return false;
    }

    bool rtn = read(fd);

    fclose(fd);

    return rtn;
}

bool CurveSet::
write(const std::string& filename) const
{
    std::string full_name = filename;
    FILE* fd = fopen(full_name.c_str(), "w");
    if (fd == NULL) {
        perror(filename.c_str());
        return false;
    }

    bool rtn = write(fd);

    fclose(fd);

    return rtn;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

BezSet::
BezSet()
    : CurveSet(N_Curves)
{
    for (unsigned int i=0; i<size(); ++i)
        set_curve (i, new BezCurve);
}

//////////////////////////////////////////////////////////////////////

BezSet::
~BezSet()
{
}

//////////////////////////////////////////////////////////////////////

void BezSet::
get_plot_points(unsigned int i, std::vector<Gdk::Point> &points, int hei)
{
    if (i < size()  &&  curve(i)!=0)
        curve(i) -> get_plot_points(points, hei);
}

//////////////////////////////////////////////////////////////////////

void BezSet::
get_deriv_points(unsigned int i, std::vector<Gdk::Point> &points, int hei)
{
    if (i < size()  &&  curve(i)!=0)
        curve(i) -> get_deriv_points(points, hei);
}

//////////////////////////////////////////////////////////////////////

bool BezSet::
read (FILE* fd)
{
    fscanf(fd, "Curve {\n");

    float duration;
    fscanf(fd, "    Duration = %g\n", &duration);

    set_duration(duration);

    // N_Curves is defined in global.hh
    int n_input_curves;
    fscanf(fd, "    N Curves = %d\n", &n_input_curves);
    if (N_Curves != n_input_curves) {
        fprintf(stderr, "Wrong number of curves: %d instead of %d\n", n_input_curves, N_Curves);
        return false;
    }

    for (unsigned int idx=0; idx<size(); ++idx) {
        if (! curve(idx) -> read(fd)) {
            fprintf(stderr, "Reading curve %d of %d failed\n", idx+1, N_Curves);
            return false;
        }
    }

    fscanf(fd, "}  // end Curve\n");

    return true;
}

//////////////////////////////////////////////////////////////////////

bool BezSet::
write(FILE* fd) const
{
    fprintf(fd, "Curve {\n");

    fprintf(fd, "    Duration = %g\n", duration());

    // N_Curves is defined in global.hh
    fprintf(fd, "    N Curves = %d\n", N_Curves);

    for (unsigned int idx=0; idx<size(); ++idx) {
        curve(idx) -> write(fd);
    }

    fprintf(fd, "}  // end Curve\n");

    return true;
}

//////////////////////////////////////////////////////////////////////

double get_current_duration()
{
  const double Default_Duration = 10;

  return (the_curve!=0
	  ?  the_curve->duration()
	  :  Default_Duration);
}
