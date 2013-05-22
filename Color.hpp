#ifndef COLOR_HPP
#define COLOR_HPP
#include <QColor>

class Color{
public:
    double r;
    double g;
    double b;
    Color()
        : r(0), g(0), b(0)
    {}
    Color(const double & r_, const double & g_, const double & b_)
        : r(r_), g(g_), b(b_)
    {}
    virtual ~Color(){}
    void normalize(){
        if (r > 1)
            r = 1;
        if (g > 1)
            g = 1;
        if (b > 1)
            b = 1;
    }
    Color operator+(const Color & c) const{
        return Color(r + c.r, g + c.g, b + c.b);
    }
    Color operator-(const Color & c) const{
        return Color(r - c.r, g - c.g, b - c.b);
    }
    Color operator*(const Color & c) const{
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator*(const double & k) const{
        return Color(k * r, k*  g, k * b);
    }
    QColor convert(){
        normalize();
        return QColor(255 * r, 255 * g, 255 * b);
    }
    bool is_black() const{
        return r == 0 && g == 0 && b == 0;
    }
};

#endif // COLOR_HPP
