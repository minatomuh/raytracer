#include "RGB.h"

RGB::RGB() : r(0), g(0), b(0) {
}

RGB::RGB(short _r, short _g, short _b) : r(_r), g(_g), b(_b)  {
}

std::ostream& operator <<(std::ostream& out, const RGB& pixel) {
    out << pixel.r << " " << pixel.g << " " << pixel.b << "\n";
    return out;
}