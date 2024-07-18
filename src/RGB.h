#ifndef _RGB_H
#define _RGB_H

#include <iostream>

struct RGB {
    RGB();
    RGB(short, short, short);

    //Better to use uint8_t but input stream treats as char
    short r, g, b;

    inline RGB operator +(const RGB& c) {return RGB(r + c.r, g + c.g, b + c.b);}
    inline RGB operator /(const int n) {return RGB(r / n, g / n, b / n);}

    friend std::ostream& operator <<(std::ostream& out, const RGB& pixel);
};

#endif