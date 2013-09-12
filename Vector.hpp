#ifndef POINT_HPP
#define POINT_HPP
#include <xmmintrin.h>
#include <math.h>

class Vector{
private:

public:
    double x;
    double y;
    double z;
     Vector(){}
    Vector(const double & x_, const double & y_, const double & z_)
        : x(x_), y(y_), z(z_)
    {
    }
    double dot(const Vector& v) const{
        return x * v.x + y * v.y + z * v.z;
    }
    Vector operator+(const Vector& v) const{
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) const{
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    Vector operator*(const Vector& v) const{
        return Vector(y * v.z - z * v.y,
                      z * v.x - x * v.z,
                      x * v.y - y * v.x);
    }
    Vector scalar(const double & s) const {
        return Vector(x * s, y * s, z * s);
    }
    double length() const{
        return sqrt(x * x + y * y + z * z);
    }
    double distance(const Vector & v)const{
        return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
    }
    Vector reflect(const Vector& normal) const{
        return *this - normal.scalar(2 * dot(normal));
    }
    void normalize(){
        double l = length();
        x = x / l;
        y = y / l;
        z = z / l;
    }
    Vector move(const Vector & v) const{
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector rotate_x(const double & a) const{
            return Vector(x, y * cos(a) + z * sin(a),
                             -y * sin(a) + z * cos(a));
    }
    Vector rotate_y(const double & a) const{
            return Vector(  x * cos(a) - z * sin(a),
                            y,
                            x * sin(a) + z * cos(a));
    }
    Vector rotate_z(const double & a) const{
            return Vector(  x * cos(a) + y * sin(a),
                            -x * sin(a) + y * cos(a),
                            z);
    }
};

class Matrix{
private:
    double m[5][5] = { { 0 } };
    void identity(){
        m[1][1] = 1;
        m[2][2] = 1;
        m[3][3] = 1;
        m[4][4] = 1;
    }

public:
    Matrix()
    {
        identity();
    }
    static Matrix TranslateMatrix(const double & x, const double & y, const double & z) {
        Matrix ret;
        ret.m[4][1] = x;
        ret.m[4][2] = y;
        ret.m[4][3] = z;
        return ret;
    }
    static Matrix TranslateMatrix(const Vector & pos) {
        Matrix ret;
        ret.m[4][1] = pos.x;
        ret.m[4][2] = pos.y;
        ret.m[4][3] = pos.z;
        return ret;
    }
    static Matrix RotateX(const double & angle) {
        Matrix ret;
        ret.m[2][2] = cos(angle);
        ret.m[2][3] = sin(angle);
        ret.m[3][2] = -sin(angle);
        ret.m[3][3] = cos(angle);
        return ret;
    }
    static Matrix RotateY(const double & angle) {
        Matrix ret;
        ret.m[1][1] = cos(angle);
        ret.m[1][3] = -sin(angle);
        ret.m[3][1] = sin(angle);
        ret.m[3][3] = cos(angle);
        return ret;
    }
    static Matrix RotateZ(const double & angle) {
        Matrix ret;
        ret.m[1][1] = cos(angle);
        ret.m[1][2] = sin(angle);
        ret.m[2][1] = -sin(angle);
        ret.m[2][2] = cos(angle);
        return ret;
    }
    Matrix operator*(const Matrix & v) const {
        Matrix ret;
        for(size_t i = 1; i <= 4; i++)
            for(size_t j = 1; j <= 4; j++){
                ret.m[i][j] = 0;
                for(size_t k = 1; k <= 4; k++)
                    ret.m[i][j] = ret.m[i][j] + (m[i][k] * v.m[k][j]);
            }
        return ret;
    }
    Vector mul(const Vector & v) const{
        return Vector(m[1][1] * v.x + m[2][1] * v.y + m[3][1] * v.z + m[4][1],
                      m[1][2] * v.x + m[2][2] * v.y + m[3][2] * v.z + m[4][2],
                      m[1][3] * v.x + m[2][3] * v.y + m[3][3] * v.z + m[4][3]);
    }
    double det() const{
        return m[1][1] * ( m[2][2] * m[3][3] - m[2][3] * m[3][2] ) -
                         m[1][2] * ( m[2][1] * m[3][3] - m[2][3] * m[3][1] ) +
                         m[1][3] * ( m[2][1] * m[3][2] - m[2][2] * m[3][1] );
    }
    Matrix inverse() const{
        double DetInv = 1 / det();
        Matrix ret;

          ret.m[1][1] =  DetInv * ( m[2][2] * m[3][3] - m[2][3] * m[3][2] );
          ret.m[1][2] = -DetInv * ( m[1][2] * m[3][3] - m[1][3] * m[3][2] );
          ret.m[1][3] =  DetInv * ( m[1][2] * m[2][3] - m[1][3] * m[2][2] );
          ret.m[1][4] = 0;

          ret.m[2][1] = -DetInv * ( m[2][1] * m[3][3] - m[2][3] * m[3][1] );
          ret.m[2][2] =  DetInv * ( m[1][1] * m[3][3] - m[1][3] * m[3][1] );
          ret.m[2][3] = -DetInv * ( m[1][1] * m[2][3] - m[1][3] * m[2][1] );
          ret.m[2][4] = 0;

          ret.m[3][1] =  DetInv * ( m[2][1] * m[3][2] - m[2][2] * m[3][1] );
          ret.m[3][2] = -DetInv * ( m[1][1] * m[3][2] - m[1][2] * m[3][1] );
          ret.m[3][3] =  DetInv * ( m[1][1] * m[2][2] - m[1][2] * m[2][1] );
          ret.m[3][4] = 0;

          ret.m[4][1] = -( m[4][1] * ret.m[1][1] + m[4][2] * ret.m[2][1] + m[4][3] * ret.m[3][1] );
          ret.m[4][2] = -( m[4][1] * ret.m[1][2] + m[4][2] * ret.m[2][2] + m[4][3] * ret.m[3][2] );
          ret.m[4][3] = -( m[4][1] * ret.m[1][3] + m[4][2] * ret.m[2][3] + m[4][3] * ret.m[3][3] );
          ret.m[4][4] = 1;
          return ret;
    }
};

class Vector4{
private:
public:
    double x;
    double y;
    double z;
    double w;
     Vector4(){}
    Vector4(const double & x_, const double & y_, const double & z_, const double & w_ = 1)
        : x(x_), y(y_), z(z_), w(w_)
    {
    }
    double operator*(const Vector& v){
        return x * v.x + y * v.y + z * v.z;
    }
    void normalize(){
        double l = sqrt(x * x + y * y + z * z + w * w);
        x = x / l;
        y = y / l;
        z = z / l;
        w = w / l;
    }
};

class Viewport{
private:
    Viewport() = default;
public:
    Vector m_p1;
    Vector m_p2;
    Vector m_p3;
    Vector m_p4;
    Viewport(const Vector & p1, const Vector & p2, const Vector & p3, const Vector & p4)
        : m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4)
    {
    }
};

typedef Viewport Rect;

#endif // POINT_HPP
