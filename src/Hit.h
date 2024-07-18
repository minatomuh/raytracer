#ifndef _HIT_H
#define _HIT_H

#include "Vector.h"
#include "scene/Material.h"

static const double EPSILON = std::numeric_limits<double>::epsilon();
static const double INF = std::numeric_limits<double>::infinity();

struct Hit {
    inline bool is_hit() {return (t != INF);}
    double t;
    Point hit_point;
    Vector normal;
    Material material;
};

#endif