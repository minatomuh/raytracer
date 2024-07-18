#ifndef _RAY_H
#define _RAY_H

#include "Vector.h"

class Ray {
public:
    Ray();
    Ray(const Point&, const Vector&);

    Point origin() const;
    Vector direction() const;
    Point at(double) const;

private:
    Point orig;
    Vector dir;
};

#endif