// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef BEZ_CURVE_HH
#define BEZ_CURVE_HH

//////////////////////////////////////////////////////////////////////

#include <vector>
#include <list>
#include <cstdio>

#include "curve.hh"

namespace Gdk { class Point; }
class Anchor;

//////////////////////////////////////////////////////////////////////

class BezCurve : public Curve
{
public:
    BezCurve();
    virtual ~BezCurve();

    Anchor* find_near_anchor(int x_pixel, int y_pixel, double &drag_min, double& drag_max);
    bool find_anchor_near_x(int x_pixel) const;

    bool add_anchor_at( double t );
    void remove_anchor(Anchor* anchor);

    const Anchor* get_anchor(int index, bool &highlighted) const;
    const Anchor* get_handle(int index, bool &highlighted) const;

    bool get_handle_line(int index, double pt1[2], double pt2[2]) const;

    void get_plot_points(std::vector<Gdk::Point> &points, int hei);
    void get_deriv_points(std::vector<Gdk::Point> &points, int hei);
    void draw_anchors();

    CurveStatus get_value(double t, double& value) const;
    void calc_coef();

    bool read(FILE* fd);
    bool write(FILE* fd) const;

public:
    class BezData;

private:
    double get_value_internal(double t, double *frac) const;

private:
    std::vector<BezData>  _pt_data;

    Anchor* _active_anchor;

    struct Coef {
        double t0, dt;
        double a[4];
        double b[4];
    };
    std::vector<Coef> _coef;
};

//////////////////////////////////////////////////////////////////////

#endif // BEZ_CURVE_HH
