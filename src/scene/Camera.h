#ifndef _CAMERA_H
#define _CAMERA_H

#include "../Vector.h"

struct Camera {
    int id;
    Vector position;
    Vector gaze;
    Vector up;
    double left, right, bottom, top;
    double near_distance;
    unsigned int h_res, v_res;
    std::string image_name;
};

#endif