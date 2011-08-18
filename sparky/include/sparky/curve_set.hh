// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef CURVE_SET_HH
#define CURVE_SET_HH

//////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "sparky/curve.hh"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class CurveSet
{
 public:
    CurveSet(unsigned int size);
    virtual ~CurveSet();

    void reset(double t);

    unsigned int size() const { return _size; }
    double duration() const { return _duration; }
    void set_duration(double d);

    double get_output_frac(double t) const;
    void set_output_frac(double now, double frac);

    Curve* curve(int i) { return _curves[i]; }
    const Curve* curve(int i) const { return _curves[i]; }
    void set_curve(int i, Curve* curve);

    CurveStatus get_values(double t, double values[], const bool active[] = 0) const ;

    virtual void get_plot_points (unsigned int i, std::vector<Gdk::Point> &points, int hei) = 0;
    virtual void get_deriv_points(unsigned int i, std::vector<Gdk::Point> &points, int hei) = 0;

    virtual bool read (FILE* fd)         = 0;
    virtual bool write(FILE* fd) const   = 0;


  bool read (const std::string& filename);
  bool write(const std::string& filename) const;

private:
    const unsigned int _size;
    Curve**   _curves;
    double    _duration;
    mutable double    _start_time;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class BezCurve;

////////////////////////////////////////

class BezSet : public CurveSet
{
 public:
    BezSet();
    virtual ~BezSet();

    virtual void get_plot_points (unsigned int i, std::vector<Gdk::Point> &points, int hei);
    virtual void get_deriv_points(unsigned int i, std::vector<Gdk::Point> &points, int hei);

    virtual bool read (FILE* fd);
    virtual bool write(FILE* fd) const;

private:
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // CURVE_SET_HH
