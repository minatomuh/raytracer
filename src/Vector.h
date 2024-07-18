#ifndef _VECTOR_H
#define _VECTOR_H

#include <iostream>
#include <cmath>

class Vector {
public:
    Vector();
    Vector(const Vector&);
    Vector(double, double, double);

    inline double x() const {return e[0];};
    inline double y() const {return e[1];};
    inline double z() const {return e[2];};

    inline Vector operator +(const Vector& v) const {return Vector(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]);}
    Vector& operator +=(const Vector&);

    inline Vector operator -(const Vector& v) const {return Vector(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]);}
    Vector& operator -=(const Vector&);

    inline Vector operator *(double n) const {return Vector(e[0] * n, e[1] * n, e[2] * n);}
    Vector& operator *=(double);

    inline Vector operator /(double n) const {return Vector(e[0] / n, e[1] / n, e[2] / n);}
    Vector& operator /=(double);

    
    //Cross product
    inline Vector operator *(const Vector&v ) const {return Vector(e[1] * v.e[2] - e[2] * v.e[1],
                                                                e[2] * v.e[0] - e[0] * v.e[2],
                                                                e[0] * v.e[1] - e[1] * v.e[0]);}
    //Dot product
    inline double dot(const Vector& v) const { return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];}
    inline double dot(const Vector& v, Vector& v2) {return v.e[0] * v2.e[0] + v.e[1] * v2.e[1] + v.e[2] * v2.e[2];}

    Vector& operator =(const Vector&);

    inline double length() const {return std::sqrt(length_sqr());}
    inline double length_sqr() const {return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];}

    Vector normalize() const;

    friend std::ostream& operator <<(std::ostream&, const Vector&);

    double e[3];
};

using Point = Vector;
using Color = Vector;

#endif