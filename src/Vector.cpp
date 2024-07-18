#include "Vector.h"

Vector::Vector() : e{0, 0, 0} {
}

Vector::Vector(const Vector& v) : e{v.e[0], v.e[1], v.e[2]} {
}

Vector::Vector(double x, double y, double z) : e{x, y, z} {
}

Vector& Vector::operator +=(const Vector& v) {
    this->e[0] += v.e[0];
    this->e[1] += v.e[1];
    this->e[2] += v.e[2];
    return *this;
}

Vector& Vector::operator -=(const Vector& v) {
    this->e[0] -= v.e[0];
    this->e[1] -= v.e[1];
    this->e[2] -= v.e[2];
    return *this;
}

Vector& Vector::operator *=(double n) {
    this->e[0] *= n;
    this->e[1] *= n;
    this->e[2] *= n;
    return *this;
}

Vector& Vector::operator /=(double n) {
    this->e[0] /= n;
    this->e[1] /= n;
    this->e[2] /= n;
    return *this;
}

Vector& Vector::operator =(const Vector& v) {
    this->e[0] = v.e[0];
    this->e[1] = v.e[1];
    this->e[2] = v.e[2];
    return *this;
}

Vector Vector::normalize() const {
    double length = this->length();
    return Vector(e[0] / length, e[1] / length, e[2] / length);
}

std::ostream& operator <<(std::ostream& out, const Vector& v) {
    return out << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ")"; 
}
