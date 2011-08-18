// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#include "sparky/global.hh"
#include "sparky/curve.hh"
#include "sparky/bez_curve.hh"
#include "sparky/anchor.hh"

#include <cmath>
#include <assert.h>

#include <gdkmm/types.h>

extern int get_x2pix(double x); // in curve_pane.cc
extern int get_y2pix(double y); // in curve_pane.cc
extern void set_drag_limits(double min, double max); // in curve_pane.cc
extern void redraw_curve_pane(); // in curve_pane.cc

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PosAnchor : public Anchor
{
public:
    PosAnchor(double x, double y) : _x(x), _y(y) {}
    PosAnchor(const PosAnchor& p) : Anchor(), _x(p._x), _y(p._y) {}
    virtual ~PosAnchor() {}

    // the required methods
    virtual double x() const { return _x; }
    virtual double y() const { return _y; }

    virtual void x(double val) { _x = val; }
    virtual void y(double val) { _y = val; }

    virtual void set (double  x, double  y) { _x  =  x; _y  =  y; }
    virtual void move(double dx, double dy) { _x += dx; _y += dy; }

private:
    double  _x,  _y;  // anchor point
};


class BezAnchor : public Anchor
{
public:
    BezAnchor(BezCurve::BezData* parent) : _parent(parent) {}
    virtual ~BezAnchor() {}

    // the required methods
    virtual double x() const;
    virtual double y() const;

    virtual void x(double val);
    virtual void y(double val);
    virtual void set (double  x, double  y);
    virtual void move(double dx, double dy);

private:
    BezCurve::BezData* const _parent;
};

//////////////////////////////////////////////////////////////////////

class BezCurve::BezData
{
public:
    BezData()
        : _pos(0,0),
          _handle1(this),
          _handle2(this),
          _r1(0),
          _r2(0),
          _theta(0)
    {}
    BezData(double x, double y, double r1, double r2, double theta);
    BezData(const BezData& b)
        : _pos(b._pos),
          _handle1(this),
          _handle2(this),
          _r1(b._r1),
          _r2(b._r2),
          _theta(b._theta)
    {}
    ~BezData();

    void operator= (const BezData& b) {
        _pos = b._pos;
        _r1  = b._r1;
        _r2  = b._r2;
        _theta  = b._theta;
    }

    double x0() const { return _pos.x(); }
    double y0() const { return _pos.y(); }

    double r1() const { return _r1; }
    double r2() const { return _r2; }
    double theta() const { return _theta; }


    void r1(double val)  { _r1 = val; }
    void r2(double val)  { _r2 = val; }
    void theta(double val)  { _theta = val; }

    double x1() const { return _pos.x() + _r1 * cos(_theta); }
    double y1() const { return _pos.y() + _r1 * sin(_theta); }
    double x2() const { return _pos.x() - _r2 * cos(_theta); }
    double y2() const { return _pos.y() - _r2 * sin(_theta); }

    Anchor* pos() { return &_pos; }
    const Anchor* pos() const { return &_pos; }

    Anchor* handle1() { return &_handle1; }
    const Anchor* handle1() const { return &_handle1; }
    Anchor* handle2() { return &_handle2; }
    const Anchor* handle2() const { return &_handle2; }

#if 0
    double dx() const { return _dx; }
    double dy() const { return _dy; }

    void dx(double val) { _dx = val; }
    void dy(double val) { _dy = val; }

    void move(double dx, double dy) {
        // _dx += dx; _dy += dy;
    }
#endif

    bool left(const BezAnchor* handle) const {
        return (handle == &_handle1);
    }

private:
    PosAnchor _pos;
    BezAnchor _handle1;
    BezAnchor _handle2;

    // the bezier handle data
    //double _dx, _dy;
    friend class BezAnchor;
    friend class BezCurve;
    double _r1;
    double _r2;
    double _theta;
};

BezCurve::BezData::
BezData(double x, double y, double r1, double r2, double theta)
    :  _pos(x,y),
       _handle1(this),
       _handle2(this),
       _r1(r1),
       _r2(r2),
       _theta(theta)
{
}

BezCurve::BezData::
~BezData()
{
}



double BezAnchor::
x() const {
    return (_parent->left(this)
            ? _parent->x1()
            : _parent->x2());
}
double BezAnchor::
y() const {
    return (_parent->left(this)
            ? _parent->y1()
            : _parent->y2());
}

void BezAnchor::
x(double val)
{

    /*
    if (_parent->left(this))
        _parent->dx(val - _parent->x0());
    else
        _parent->dx(_parent->x0() - val);
    */
    if (_parent->left(this)) {
        double old_theta = _parent->_theta;
        double old_r     = _parent->_r1;

        double x = val - _parent->x0();
        double y = old_r * sin(old_theta);

        double r = hypot(x, y);  // sqrt(x*x + y*y);
        double theta = atan2(y, x);

        _parent->_r1 = r;
        _parent->_theta = theta;
    }
    else {
        double old_theta = _parent->_theta;
        double old_r     = _parent->_r2;

        double x = val - _parent->x0();
        double y = -old_r * sin(old_theta);

        double r = hypot(x, y);  // sqrt(x*x + y*y);
        double theta = atan2(-y, -x);

        _parent->_r2 = r;
        _parent->_theta = theta;
    }
}
void BezAnchor::
y(double val)
{
    if (_parent->left(this)) {
        double old_theta = _parent->_theta;
        double old_r     = _parent->_r1;

        double x = old_r * cos(old_theta);
        double y = val - _parent->y0();

        double r = hypot(x, y);  // sqrt(x*x + y*y);
        double theta = atan2(y, x);

        _parent->_r1 = r;
        _parent->_theta = theta;
    }
    else {
        double old_theta = _parent->_theta;
        double old_r     = _parent->_r2;

        double x = -old_r * cos(old_theta);
        double y = val - _parent->y0();

        double r = hypot(x, y);  // sqrt(x*x + y*y);
        double theta = atan2(-y, -x);

        _parent->_r2 = r;
        _parent->_theta = theta;
    }
}
void BezAnchor::
set (double  x_val, double  y_val)
{
    x(x_val);
    y(y_val);
}

void BezAnchor::
move(double dx, double dy)
{
    if (_parent->left(this)) {
        double old_theta = _parent->_theta;
        double old_r     = _parent->_r1;

        double x = old_r * cos(old_theta) + dx;
        double y = old_r * sin(old_theta) + dy;

        double r = hypot(x, y);  // sqrt(x*x + y*y);
        double theta = atan2(y, x);

        _parent->_r1 = r;
        _parent->_theta = theta;
    }
    else {
        double old_theta = _parent->_theta;
        double old_r     = _parent->_r2;

        double x = -old_r * cos(old_theta) + dx;
        double y = -old_r * sin(old_theta) + dy;

        double r = hypot(x, y);  // sqrt(x*x + y*y);
        double theta = atan2(-y, -x);

        _parent->_r2 = r;
        _parent->_theta = theta;
    }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

BezCurve::
BezCurve()
    : _active_anchor(0)
{
    // add some initial data
    _pt_data . push_back( BezData(0.0, 0.5, 0.1, 0.1, 0.0) );
    _pt_data . push_back( BezData(1.0, 0.5, 0.1, 0.1, 0.0) );
    
    calc_coef();
}

BezCurve::
~BezCurve()
{
}

CurveStatus BezCurve::
get_value(double t, double& value) const
{
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    value = get_value_internal(t,
                               0); // don't bother getting the curve frac
    return (t < 1.0
            ?  OkayStatus
            :  DoneStatus);
}

double BezCurve::
get_value_internal(double t, double* frac) const
{
    double value = 0; // default value

    if (t <= 0.0) {
        value = _pt_data.front().y0();
        if (frac != 0) *frac = 0;
    }
    else if (t >= 1.0) {
        value = _pt_data.back().y0();
        if (frac != 0) *frac = 1;
    }
    else {
        // binary search for segment
        int k = 0;
        {
            int k_hi = _coef.size();
            while (k_hi - k > 1) {
                int mid = (k + k_hi) / 2;
                if (_coef[mid].t0 > t)
                    k_hi = mid;
                else
                    k = mid;
            }
        }

        if (_coef[k].dt > 0) {
            double f0 = 0;
            double t0 = _coef[k].t0;
            double f1 = 1;
            double t1 = t0 + _coef[k].dt;
            assert(t0 <=  t);
            assert( t <= t1);

            static const double Dt = 1.0e-6;
            while (f1 - f0 > Dt) {
                double f = (f0 + f1) * 0.5;
                double val = _coef[k].a[0] + f*(_coef[k].a[1] + f*(_coef[k].a[2] + f*_coef[k].a[3]));
                //printf("%g < %g < %g   vs  %g < %g < %g\n", f0, f, f1, t0, t, t1);
                if (val > t) {
                    f1 = f;
                    t1 = val;
                }
                else {
                    f0 = f;
                    t0 = val;
                }
            }

            double f = f0 + (f1-f0)*(t0-t)/(t0-t1);
            if (frac != 0) *frac = f;

            value = _coef[k].b[0] + f*(_coef[k].b[1] + f*(_coef[k].b[2] + f*_coef[k].b[3]));
        }
        else {
            value = _coef[k].b[0];
        }

    }

    return value;
}

Anchor* BezCurve::
find_near_anchor(int x_pixel, int y_pixel, double &drag_min, double& drag_max)
{
    static const int dist = 5; // pixels

    drag_min = 0;
    drag_max = 1;

    const int N = int(_pt_data.size());
    for (int i=0; i<N; i++) {
        if (fabs(get_x2pix(_pt_data[i].x0()) - x_pixel) <= dist  &&
            fabs(get_y2pix(_pt_data[i].y0()) - y_pixel) <= dist) {
	  if (i<=0) {
	    drag_max = 0;
	  }
            else if (i>=N-1)
	      drag_min = 1;
	  else {
	    drag_min = _coef[i-1].t0;
	    drag_max = _coef[i+1].t0;
	  }

            return (_active_anchor = _pt_data[i].pos());
        }

        // make distance for handle larger so we can get them out from under
        // the main anchor when necessary
        static const int handleDist = (3*dist)/2;

        if (fabs(get_x2pix(_pt_data[i].x1()) - x_pixel) <= handleDist  &&
            fabs(get_y2pix(_pt_data[i].y1()) - y_pixel) <= handleDist) {
            return (_active_anchor = _pt_data[i].handle1());
        }

        if (fabs(get_x2pix(_pt_data[i].x2()) - x_pixel) <= handleDist  &&
            fabs(get_y2pix(_pt_data[i].y2()) - y_pixel) <= handleDist) {
            return (_active_anchor = _pt_data[i].handle2());
        }
    }

    return  (_active_anchor = 0);
}


//////////////////////////////////////////////////////////////////////

bool BezCurve::
find_anchor_near_x(int x_pixel) const
{
    static const int dist = 5; // pixels

    const int N = int(_pt_data.size());
    for (int i=0; i<N; i++) {
        if (fabs(get_x2pix(_pt_data[i].x0()) - x_pixel) <= dist) {
            
            return true;
        }
    }

    // no anchor found
    return false;
}

//////////////////////////////////////////////////////////////////////

bool BezCurve::
add_anchor_at( double t )
{
    if (t > 0.0  &&  t < 1.0) {

        double frac = 0;
        double y = get_value_internal(t, &frac);

        assert(frac >= 0.0);
        assert(frac <= 1.0);

        if (y < 0)  y = 0;
        if (y > 1)  y = 1;

        // find the position
        const int N = int(_pt_data.size());
        int cnt = 0;
        for (std::vector<BezData>::iterator pos = _pt_data.begin(); pos != _pt_data.end(); ++pos) {
            if (pos->x0() > t) {

                assert(cnt > 0);
                assert(cnt < N);

                // get the four bezier points
                const double p0[2] = {_pt_data[cnt-1].x0(), _pt_data[cnt-1].y0()};
                const double p1[2] = {_pt_data[cnt-1].x1(), _pt_data[cnt-1].y1()};
                const double p2[2] = {_pt_data[cnt].x2(), _pt_data[cnt].y2()};
                const double p3[2] = {_pt_data[cnt].x0(), _pt_data[cnt].y0()};

                // find the corresponding points the appropriate fraction along
#define MIX(f,a,b)    a[0] + f * (b[0] - a[0]), a[1] + f * (b[1] - a[1])
                const double p01[2]   = { MIX(frac, p0,   p1)   };
                const double p12[2]   = { MIX(frac, p1,   p2)   };
                const double p23[2]   = { MIX(frac, p2,   p3)   };
                const double p012[2]  = { MIX(frac, p01,  p12)  };
                const double p123[2]  = { MIX(frac, p12,  p23)  };
                const double p0123[2] = { MIX(frac, p012, p123) };
#undef MIX

                // shorten the ajoining bezier handles appropriately
                _pt_data[cnt-1]._r1  *= frac;
                _pt_data[cnt  ]._r2  *= 1.0 - frac;

                // add new anchor with handles
                const double r1 = hypot(p123[0]-p0123[0], p123[1]-p0123[1]);
                const double r2 = hypot(p012[0]-p0123[0], p012[1]-p0123[1]);
                const double theta = atan2(p123[1]-p012[1], p123[0]-p012[0]);
                _pt_data.insert(pos, BezData(t, y, r1, r2, theta) );
                break;
            }
            cnt += 1;
        }
	return true;
    }
    else {
      return false;
    }
}

//////////////////////////////////////////////////////////////////////

const Anchor* BezCurve::
get_anchor(int index, bool &highlighted) const
{
    int N = int(_pt_data.size());
    if (index < N) {
        const Anchor* anchor = _pt_data[index].pos();
        highlighted = (anchor == _active_anchor);
        return anchor;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////

const Anchor* BezCurve::
get_handle(int index, bool &highlighted) const
{
    const Anchor* anchor = 0;

    int N = int(_pt_data.size());
    if (index/2 < N) {
        anchor = ((index%2 == 0)
                  ?  _pt_data[index/2].handle1()
                  :  _pt_data[index/2].handle2());
        highlighted = (anchor == _active_anchor);
    }

    return anchor;
}

bool BezCurve::
get_handle_line(int index, double pt1[2], double pt2[2]) const
{
    if (index/2  <  int(_pt_data.size())-1) {
        pt1[0] = _pt_data[(index+1)/2].x0();
        pt1[1] = _pt_data[(index+1)/2].y0();

        if (index%2 == 0) {
            pt2[0] = _pt_data[(index+1)/2].x1();
            pt2[1] = _pt_data[(index+1)/2].y1();
        }
        else {
            pt2[0] = _pt_data[(index+1)/2].x2();
            pt2[1] = _pt_data[(index+1)/2].y2();
        }

        return true;
    }

    // getting here means there are none left
    return false;
}

//////////////////////////////////////////////////////////////////////

void BezCurve::
remove_anchor(Anchor* anchor)
{
    printf("Remove\n");
    for (std::vector<BezData>::iterator pos = _pt_data.begin(); pos != _pt_data.end(); ++pos)
        if (pos->pos() == anchor) {
            _pt_data.erase(pos);
            break;
        }
}

//////////////////////////////////////////////////////////////////////

void BezCurve::
get_plot_points(std::vector<Gdk::Point> &points, int hei)
{
    const int N = _coef.size();
    points . push_back(Gdk::Point(get_x2pix(_coef[0].a[0]),
                                  get_y2pix(_coef[0].b[0])));

    for (int i=0; i<N-1; ++i) {
        static const int Divs = 64;
        for (int d=1; d<=Divs; ++d) {
            double t = d / double(Divs);
            double x = _coef[i].a[0] + t*(_coef[i].a[1] + t*(_coef[i].a[2] + t*_coef[i].a[3]));
            double y = _coef[i].b[0] + t*(_coef[i].b[1] + t*(_coef[i].b[2] + t*_coef[i].b[3]));

            int iy = get_y2pix(y);
            if (iy < 0)   iy = 0;
            if (iy > hei) iy = hei;

            points . push_back(Gdk::Point(get_x2pix(x),
                                          iy));
        }
    }

}

//////////////////////////////////////////////////////////////////////

void BezCurve::
get_deriv_points(std::vector<Gdk::Point> &points, int hei)
{
    const int N = _coef.size();
    {
      double x = _coef[0].a[0];
      double dx = _coef[0].a[1];
      double dy = _coef[0].b[1];
      double y = (dx != 0
		  ?  dy / dx
		  :  0.0);

      points . push_back(Gdk::Point(get_x2pix(x),
				    get_y2pix(y)));
    }

    for (int i=0; i<N-1; ++i) {
        static const int Divs = 64;
        for (int d=1; d<=Divs; ++d) {
            double t = d / double(Divs);
            double x = _coef[i].a[0] + t*(_coef[i].a[1] + t*(_coef[i].a[2] + t*_coef[i].a[3]));

            double dx = _coef[i].a[1] + t*(2.*_coef[i].a[2] + t*3.*_coef[i].a[3]);
	    double dy = _coef[i].b[1] + t*(2.*_coef[i].b[2] + t*3.*_coef[i].b[3]);
	    double y = (dx == 0
			?  ( dy == 0
                             ? hei / 2
                             : (dy > 0
                                ? hei
                                : 0))
			:  dy / dx);

            int iy = get_y2pix(y);
            if (iy < 0)   iy = 0;
            if (iy > hei) iy = hei;

            points . push_back(Gdk::Point(get_x2pix(x),
                                          iy));
        }
    }

}

//////////////////////////////////////////////////////////////////////
void calc_coef_internal(double a[],
                        double x0, double x1, double x2, double x3)
{
    a[0] = x0;
    a[1] = 3 * (x1 - x0);
    a[2] = 3 * (x0 - 2.* x1 + x2);
    a[3] = x3 - x0 + 3.*(x1 - x2);
}

void BezCurve::
calc_coef()
{
    int N = int(_pt_data.size());
    if (int(_coef.size()) != N)
        _coef . resize( N );

    for (int i=0; i<N-1; ++i) {
        _coef[i].t0 = _pt_data[i].x0();
        _coef[i].dt = _pt_data[i+1].x0() - _pt_data[i].x0();

        calc_coef_internal(_coef[i].a, _pt_data[i].x0(),_pt_data[i].x1(), _pt_data[i+1].x2(), _pt_data[i+1].x0());
        calc_coef_internal(_coef[i].b, _pt_data[i].y0(),_pt_data[i].y1(), _pt_data[i+1].y2(), _pt_data[i+1].y0());
    }
    if (N > 0) {
        _coef[N-1].t0 = _pt_data[N-1].x0();
        _coef[N-1].dt = 0;
        _coef[N-1].a[0] = _pt_data[N-1].x0();
        _coef[N-1].b[0] = _pt_data[N-1].y0();
    }
}

//////////////////////////////////////////////////////////////////////

bool BezCurve::
read(FILE* fd)
{
    //
    // needs to mirror write() below
    //
    fscanf(fd, "    BezCurve {\n");

    int n_segments;
    fscanf(fd, "        Bezier with %d segments {\n", &n_segments);
    assert(n_segments >= 2);

    _pt_data.resize(n_segments);

    for (int seg=0; seg<n_segments; ++seg) {
        int id;
        float x, y, r1, r2, theta;
        fscanf(fd, "            Pt %d: (%g, %g) (%g, %g) %g\n",
               &id, &x, &y, &r1, &r2, &theta);
        _pt_data[seg]._pos.x(x);
        _pt_data[seg]._pos.y(y);
        _pt_data[seg].r1(r1);
        _pt_data[seg].r2(r2);
        _pt_data[seg].theta(theta);
    }
    fscanf(fd, "        }  // end Bezier\n");
    fscanf(fd, "    } // end curve\n");

    //
    // use this new data
    calc_coef();

    return true;
}

//////////////////////////////////////////////////////////////////////

bool BezCurve::
write(FILE* fd) const
{
    //
    // needs to mirror read() above
    //
    fprintf(fd, "    BezCurve {\n");

    int N_Segments = int(_pt_data.size());
    fprintf(fd, "        Bezier with %d segments {\n", N_Segments);
    for (int seg=0; seg<N_Segments; ++seg)
        fprintf(fd, "            Pt %d: (%g, %g) (%g, %g) %g\n",
                seg,
                _pt_data[seg].x0(), _pt_data[seg].y0(),
                _pt_data[seg].r1(), _pt_data[seg].r2(),
                _pt_data[seg].theta());
    fprintf(fd, "        }  // end Bezier\n");

    fprintf(fd, "    } // end curve\n");

    return true;
}
