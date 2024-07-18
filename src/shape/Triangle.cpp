#include <array>
#include "Triangle.h"

Triangle::Triangle() : Object() {
}

Triangle::Triangle(int id, const std::array<Point, 3>& c) : Object(id), coords(c) {
}

Hit Triangle::intersect(const Ray &ray) const {
    Hit no_hit;
    no_hit.t = INF;

    double a, f, u, v;

    //Using Möller-Trumbore algorithm

    //Compute the plane of triangle
    Vector e1 = coords[1] - coords[0];
    Vector e2 = coords[2] - coords[0];

    Vector h = ray.direction() * e2;
    a = e1.dot(h);

    //If ray is parallel to the plane
    if(a > -EPSILON && a < EPSILON) {
        return no_hit;
    }

    f = 1.0/a;
    Vector s = ray.origin() - coords[0];
    u = f * s.dot(h);
    if(u < 0.0 || u > 1.0) {
        return no_hit;
    }

    Vector q = s * e1;
    v = f * ray.direction().dot(q);
    if(v < 0.0 || u + v > 1.0) {
        return no_hit;
    }

    double t = f * e2.dot(q);
    if(t > EPSILON) {
        Vector normal = e1 * e2;
        normal = normal.normalize();

        Hit hit;
        hit.t = t;
        hit.normal = normal;
        hit.hit_point = ray.origin() + ray.direction() * t;
        hit.material = this->material;
        return hit;
    }

    return no_hit;
}

std::string Triangle::getType() const {
    return "Triangle";
}
