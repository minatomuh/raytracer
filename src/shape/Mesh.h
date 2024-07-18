#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <array>
#include "../Ray.h"
#include "../Hit.h"
#include "Object.h"

class Mesh : public Object {
public:
    Mesh();
    Mesh(int);

    std::string getType() const override;
    virtual Hit intersect(const Ray& ray) const;

    std::vector<std::array<Point, 3>> faces;
};


#endif