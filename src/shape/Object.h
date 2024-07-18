#ifndef _OBJECT_H
#define _OBJECT_H

#include <limits>
#include "../Ray.h"
#include "../Hit.h"

class Object {
public:
    Object();
    Object(int);
    
    virtual ~Object() {}
    virtual Hit intersect(const Ray& ray) const = 0;
    virtual std::string getType() const = 0;

    int id;
    Material material;
};

#endif