#include "Ray.h"

Ray::Ray() {
}

Ray::Ray(const Point& origin, const Vector& direction) : orig(origin), dir(direction.normalize()) {
}

Point Ray::origin() const {
    return orig;
}

Vector Ray::direction() const {
    return dir;
}

//P(t) = A + tb
Point Ray::at(double t) const {
    return orig + dir * t;
}
