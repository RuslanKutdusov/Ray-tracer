#ifndef POINT_HPP
#define POINT_HPP

#include <math.h>

#define SSE 1

#if SSE
#include <xmmintrin.h>
#endif

class Vector{
private:

public:
    float x;
    float y;
    float z;
    float w;
    Vector();
#if SSE
    Vector( const __m128 & v );
#endif
    Vector(const float & x_, const float & y_, const float & z_);
    float dot(const Vector& v) const;
    Vector operator+(const Vector& v) const;
    Vector operator-(const Vector& v) const;
    Vector operator*(const Vector& v) const;
    Vector scalar(const float & s) const;
    float length() const;
    float distance(const Vector & v)const;
    Vector reflect(const Vector& normal) const;
    void normalize();
    Vector move(const Vector & v) const;
    Vector rotate_x(const float & a) const{
            return Vector(x, y * cos(a) + z * sin(a),
                             -y * sin(a) + z * cos(a));
    }
    Vector rotate_y(const float & a) const{
            return Vector(  x * cos(a) - z * sin(a),
                            y,
                            x * sin(a) + z * cos(a));
    }
    Vector rotate_z(const float & a) const{
            return Vector(  x * cos(a) + y * sin(a),
                            -x * sin(a) + y * cos(a),
                            z);
    }
}
#if SSE
__attribute__ ((aligned(16)));
#else
;
#endif


class Matrix{
private:
    float m[5][5] = { { 0 } };
    void identity(){
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
        m[3][3] = 1.0f;
        m[4][4] = 1.0f;
    }

public:
    Matrix()
    {
        identity();
    }
    static Matrix TranslateMatrix(const float & x, const float & y, const float & z) {
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
    static Matrix RotateX(const float & angle) {
        Matrix ret;
        ret.m[2][2] = cos(angle);
        ret.m[2][3] = sin(angle);
        ret.m[3][2] = -sin(angle);
        ret.m[3][3] = cos(angle);
        return ret;
    }
    static Matrix RotateY(const float & angle) {
        Matrix ret;
        ret.m[1][1] = cos(angle);
        ret.m[1][3] = -sin(angle);
        ret.m[3][1] = sin(angle);
        ret.m[3][3] = cos(angle);
        return ret;
    }
    static Matrix RotateZ(const float & angle) {
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
    float det() const{
        return m[1][1] * ( m[2][2] * m[3][3] - m[2][3] * m[3][2] ) -
                         m[1][2] * ( m[2][1] * m[3][3] - m[2][3] * m[3][1] ) +
                         m[1][3] * ( m[2][1] * m[3][2] - m[2][2] * m[3][1] );
    }
    Matrix inverse() const{
        float DetInv = 1 / det();
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
    float x;
    float y;
    float z;
    float w;
     Vector4(){}
    Vector4(const float & x_, const float & y_, const float & z_, const float & w_ = 1)
        : x(x_), y(y_), z(z_), w(w_)
    {
    }
    float operator*(const Vector& v){
        return x * v.x + y * v.y + z * v.z;
    }
    void normalize(){
        float l = sqrt(x * x + y * y + z * z + w * w);
        x = x / l;
        y = y / l;
        z = z / l;
        w = w / l;
    }
};

class Viewport{
public:
    Vector m_p1;
    Vector m_p2;
    Vector m_p3;
    Vector m_p4;
    Viewport() = default;
    Viewport(const Vector & p1, const Vector & p2, const Vector & p3, const Vector & p4)
        : m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4)
    {
    }
};

typedef Viewport Rect;

#endif

