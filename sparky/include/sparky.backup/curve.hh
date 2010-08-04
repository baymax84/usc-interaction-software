// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef CURVE_HH
#define CURVE_HH

//////////////////////////////////////////////////////////////////////

#include <cstdio>

#include <vector>
#include <gdkmm/types.h>

class Anchor;

//////////////////////////////////////////////////////////////////////

enum CurveStatus {
    ErrorStatus,
    OkayStatus,
    DoneStatus
};

//////////////////////////////////////////////////////////////////////

class Curve
{
 public:
    Curve() {}
    virtual ~Curve() {}

    virtual CurveStatus get_value(double t, double& value) const = 0;

    virtual void calc_coef() = 0;

    virtual bool read (FILE* fd)        = 0;
    virtual bool write(FILE* fd) const  = 0;

    virtual void get_plot_points (std::vector<Gdk::Point> &points, int hei) = 0;
    virtual void get_deriv_points(std::vector<Gdk::Point> &points, int hei) = 0;

    virtual bool add_anchor_at( double t ) = 0;
    virtual void remove_anchor(Anchor* anchor) = 0;

    virtual Anchor* find_near_anchor(int x_pixel, int y_pixel, double &drag_min, double& drag_max) = 0;
    virtual bool    find_anchor_near_x(int x_pixel) const = 0;

    virtual const Anchor* get_anchor(int index, bool &highlighted) const = 0;
    virtual const Anchor* get_handle(int index, bool &highlighted) const = 0;

    virtual bool get_handle_line(int index, double pt1[2], double pt2[2]) const = 0;

 private:
    
};

//////////////////////////////////////////////////////////////////////

#endif // CURVE_HH
