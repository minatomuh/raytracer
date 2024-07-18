#ifndef _SPHERE_H
#define _SPHERE_H

#include "../Ray.h"
#include "../Hit.h"
#include "Object.h"

class Sphere : public Object {
public:
    Sphere() : Object() {}
    Sphere(int id, double _radius) : Object(id), radius(_radius) {}
    virtual Hit intersect(const Ray& ray) const;
    std::string getType() const override;

    Point center;
    double radius;
};

#endif