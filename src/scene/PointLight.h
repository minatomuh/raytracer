#ifndef _POINTLIGHT_H
#define _POINTLIGHT_H

#include "../Vector.h"

struct PointLight {
    int id;
    Vector position, intensity;
};

#endif