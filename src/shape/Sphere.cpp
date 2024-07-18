#include "Sphere.h"

// Full Formula:
// (A + tb − C) . (A + tb − C) = r^2
// Analytic solution
Hit Sphere::intersect(const Ray &ray) const {
    Hit no_hit;
    no_hit.t = INF;

    // Solutions for t
    double t0, t1;

    Vector L = ray.origin() - center;

    double a = ray.direction().dot(ray.direction());
    double b = 2 * ray.direction().dot(L);
    double c = L.dot(L) - radius * radius;

    double discriminant = b*b - 4*a*c;
    if(discriminant < 0) {
        return no_hit;
    }
    if(discriminant == 0) {
        t0 = t1 = -0.5 * b / a;
    }
    else {
        double q = (b > 0) ?
            -0.5 * (b + sqrt(discriminant)) :
            -0.5 * (b - sqrt(discriminant));
        t0 = q / a;
        t1 = c / q;
    }
    if(t0 > t1) {
        std::swap(t0, t1);
    }

    if(t0 < 0) {
        t0 = t1;
        if(t0 < 0) {
            return no_hit;
        }
    }

    Hit hit;
    hit.material = this->material;
    hit.t = t0;
    hit.hit_point = ray.at(t0);
    hit.normal = (hit.hit_point - center).normalize();

    return hit;
}

std::string Sphere::getType() const {
    return "Sphere";
}
