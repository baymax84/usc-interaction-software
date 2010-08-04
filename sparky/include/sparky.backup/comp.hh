// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef COMP_HH
#define COMP_HH

#include "curve.hh"

void init_computing_env();
void reset_output_curves(double t);
double get_output_frac(double now);
void set_output_frac(double now, double frac);

void set_output_duration(double duration);
double get_output_duration();

CurveStatus get_output_data(double time, double vals[], const bool active[]);


#include <vector>
#include <gdkmm/types.h>

class Curve;

void get_plot_points (unsigned int i, std::vector<Gdk::Point> &points, int hei);
void get_deriv_points(unsigned int i, std::vector<Gdk::Point> &points, int hei);

Curve* get_curve(int i);

#endif  // COMP_HH
