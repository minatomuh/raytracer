#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "../Vector.h"

struct Material {
    int id;
    Color ambient;
    Color diffuse;
    Color specular;
    double phong_exponent;
    Color mirror_reflectance;
};

#endif