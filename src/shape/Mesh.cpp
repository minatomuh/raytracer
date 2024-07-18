#include <array>
#include "Mesh.h"
#include "Triangle.h"

Mesh::Mesh() : Object() {
}

Mesh::Mesh(int id) : Object(id) {
}

std::string Mesh::getType() const {
    return "Mesh";
}

Hit Mesh::intersect(const Ray &ray) const {
    Hit closest_hit;
    closest_hit.t = INF;

    for(const auto& face : faces) {
        Triangle tri(this->id, face);
        Hit tri_hit = tri.intersect(ray);
        if(tri_hit.t < closest_hit.t) {
            closest_hit = tri_hit;
        }
    }

    closest_hit.material = this->material;

    return closest_hit;
}
