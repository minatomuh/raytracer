#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include <array>
#include "../Ray.h"
#include "../Hit.h"
#include "Object.h"

class Triangle : public Object {
public:
    Triangle();
    Triangle(int, const std::array<Point, 3>&);
    virtual Hit intersect(const Ray& ray) const;
    std::string getType() const override;
    
    std::array<Point, 3> coords;
};

#endif