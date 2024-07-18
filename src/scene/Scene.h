#ifndef _SCENE_H
#define _SCENE_H

#include <vector>
#include <memory>

#include "Camera.h"
#include "PointLight.h"
#include "Material.h"
#include "../RGB.h"
#include "../Vector.h"
#include "../shape/Object.h"
#include "../shape/Mesh.h"

struct Scene {
    unsigned int max_raytracedepth;
    RGB background_color;
    std::vector<Camera> cameras;
    RGB ambient_light;
    std::vector<PointLight> lights;
    std::vector<Material> materials;
    std::vector<Point> vertexdata;
    std::vector<Object*> objects;
};

#endif