// /////////////////////////////////////////////////////// //
//                                                         //
//    ()_()        Walt Disney Imagineering       ()_()    //
//     (_)      Research and Development, Inc      (_)     //
//                                                         //
//   Copyright (C) 2006 - 2007  The Walt Disney Company    //
//                                                         //
// /////////////////////////////////////////////////////// //

#ifndef ANCHOR_HH
#define ANCHOR_HH


//
//  An abstract base class for anchors
//

class Anchor {
public:
    virtual ~Anchor() {}

    virtual double x() const = 0;
    virtual double y() const = 0;

    virtual void x(double val) = 0;
    virtual void y(double val) = 0;

    virtual void set(double x, double y) = 0;
    virtual void move(double dx, double dy) = 0;
};


#if 0
enum AnchorType {
    StandardAnchor,
    HighlightedAnchor,
    SecondaryAnchor,
    HighlightedSecondaryAnchor
};

class Pos;

struct AnchorPtr {
    AnchorPtr() : pos(0), type(0) {}
    AnchorPtr(Pos* p, int t) : pos(p), type(t) {}

    bool operator!= (const AnchorPtr& a) {
        return (pos != a.pos) || (type != a.type);
    }

    Pos* pos;
    int type;
};
#endif

#endif  // ANCHOR_HH

