#ifndef POINT_HPP
#define POINT_HPP

#include "cuda.hpp"
#include <math.h>

class Vector{
private:

public:
    float x;
    float y;
    float z;
    float w;

    HOST_DEVICE Vector()
    {

	}

    HOST_DEVICE Vector( const float & x_, const float & y_, const float & z_ )
    	: x( x_ ), y( y_ ), z( z_ ), w( 1.0f )
    {

    }

    HOST_DEVICE Vector( const float & x_, const float & y_, const float & z_, const float & w_ )
		: x( x_ ), y( y_ ), z( z_ ), w( w_ )
    {

	}

    HOST_DEVICE Vector( const Vector & v, float w_ )
    	: x( v.x ), y( v.y ), z( v.y ), w( w_ )
    {

    }

    HOST_DEVICE float dot( const Vector& v ) const
    {
    	return x * v.x + y * v.y + z * v.z;
    }

    HOST_DEVICE Vector operator+( const Vector& v ) const
    {
    	return Vector(x + v.x, y + v.y, z + v.z);
    }

    HOST_DEVICE Vector operator-( const Vector& v ) const
    {
    	return Vector(x - v.x, y - v.y, z - v.z);
    }

    HOST_DEVICE Vector operator*( const Vector& v ) const
    {
  		return Vector(y * v.z - z * v.y,
    			  	  z * v.x - x * v.z,
    				  x * v.y - y * v.x);
    }

    HOST_DEVICE Vector scalar( const float & s ) const
    {
    	return Vector(x * s, y * s, z * s);
    }

    HOST_DEVICE float length() const
    {
    	return sqrt(x * x + y * y + z * z);
    }


    HOST_DEVICE float distance( const Vector & v ) const
    {
    	return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
    }

    HOST_DEVICE Vector reflect( const Vector& normal ) const
    {
    	return *this - normal.scalar(2 * dot(normal));
    }

    HOST_DEVICE void normalize()
    {
    	float l = length();
    	x = x / l;
    	y = y / l;
    	z = z / l;
    }

    Vector move(const Vector & v) const
    {
    	return Vector(x + v.x, y + v.y, z + v.z);
    }
} __attribute__ ((aligned(16)));


class Matrix
{
private:
    float m[ 4 ][ 4 ];
public:
    HOST_DEVICE void identity(){
    		memset( m, 0, 16 * sizeof( float ) );
            m[ 0 ][ 0 ] = 1.0f;
            m[ 0 ][ 1 ] = 0.0f;
            m[ 0 ][ 2 ] = 0.0f;
            m[ 0 ][ 3 ] = 0.0f;

            m[ 1 ][ 0 ] = 0.0f;
            m[ 1 ][ 1 ] = 1.0f;
            m[ 1 ][ 2 ] = 0.0f;
            m[ 1 ][ 3 ] = 0.0f;

            m[ 2 ][ 0 ] = 0.0f;
            m[ 2 ][ 1 ] = 0.0f;
            m[ 2 ][ 2 ] = 1.0f;
            m[ 2 ][ 3 ] = 0.0f;

            m[ 3 ][ 0 ] = 0.0f;
            m[ 3 ][ 1 ] = 0.0f;
            m[ 3 ][ 2 ] = 0.0f;
            m[ 3 ][ 3 ] = 1.0f;
    }
    HOST_DEVICE Matrix()
    {
    }
    HOST_DEVICE static Matrix IdentityMatrix()
    {
    	Matrix ret;
    	ret.identity();
    	return ret;
    }
    HOST_DEVICE static Matrix TranslateMatrix(const float & x, const float & y, const float & z) {
        Matrix ret;
        ret.identity();
        ret.m[ 3 ][ 0 ] = x;
        ret.m[ 3 ][ 1 ] = y;
        ret.m[ 3 ][ 2 ] = z;
        return ret;
    }
    HOST_DEVICE static Matrix TranslateMatrix(const Vector & pos) {
        Matrix ret;
        ret.identity();
        ret.m[ 3 ][ 0 ] = pos.x;
        ret.m[ 3 ][ 1 ] = pos.y;
        ret.m[ 3 ][ 2 ] = pos.z;
        return ret;
    }
    HOST_DEVICE static Matrix RotateX(const float & angle) {
        Matrix ret;
        ret.identity();
        ret.m[ 1 ][ 1 ] = cos(angle);
        ret.m[ 1 ][ 2 ] = sin(angle);
        ret.m[ 2 ][ 1 ] = -sin(angle);
        ret.m[ 2 ][ 2 ] = cos(angle);
        return ret;
    }
    HOST_DEVICE static Matrix RotateY(const float & angle) {
        Matrix ret;
        ret.identity();
        ret.m[ 0 ][ 0 ] = cos(angle);
        ret.m[ 0 ][ 2 ] = -sin(angle);
        ret.m[ 2 ][ 0 ] = sin(angle);
        ret.m[ 2 ][ 2 ] = cos(angle);
        return ret;
    }
    HOST_DEVICE static Matrix RotateZ(const float & angle) {
        Matrix ret;
        ret.identity();
        ret.m[ 0 ][ 0 ] = cos(angle);
        ret.m[ 0 ][ 1 ] = sin(angle);
        ret.m[ 1 ][ 0 ] = -sin(angle);
        ret.m[ 1 ][ 1 ] = cos(angle);
        return ret;
    }
    HOST_DEVICE Matrix operator*( const Matrix & v ) const {
        Matrix ret;
        for( unsigned char i = 0; i < 4; i++ )
            for( unsigned char j = 0; j < 4; j++ )
            {
                ret.m[ i ][ j ] = 0;
                for( unsigned char k = 0; k < 4; k++ )
                    ret.m[ i ][ j ] = ret.m[ i ][ j ] + ( m[ i ][ k ] * v.m[ k ][ j ] );
            }
        return ret;
    }
    HOST_DEVICE Matrix operator=( const Matrix & v ){
    	if( this != &v )
    	{
    		for( unsigned char i = 0; i < 4; i++ )
				for( unsigned char j = 0; j < 4; j++ )
					m[i][j] = v.m[i][j];
    	}
    	return *this;
    }
    HOST_DEVICE Vector mul(const Vector & v) const{
        return Vector(m[ 0 ][ 0 ] * v.x + m[ 1 ][ 0 ] * v.y + m[ 2 ][ 0 ] * v.z + m[ 3 ][ 0 ],
                      m[ 0 ][ 1 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 2 ][ 1 ] * v.z + m[ 3 ][ 1 ],
                      m[ 0 ][ 2 ] * v.x + m[ 1 ][ 2 ] * v.y + m[ 2 ][ 2 ] * v.z + m[ 3 ][ 2 ]);
    }
    HOST_DEVICE float det() const{
    			return   m[ 0 ][ 0 ] * ( m[ 1 ][ 1 ] * m[ 2 ][ 2 ] - m[ 1 ][ 2 ] * m[ 2 ][ 1 ] ) -
                         m[ 0 ][ 1 ] * ( m[ 1 ][ 0 ] * m[ 2 ][ 2 ] - m[ 1 ][ 2 ] * m[ 2 ][ 0 ] ) +
                         m[ 0 ][ 2 ] * ( m[ 1 ][ 0 ] * m[ 2 ][ 1 ] - m[ 1 ][ 1 ] * m[ 2 ][ 0 ] );
    }
    HOST_DEVICE Matrix inverse() const{
        float DetInv = 1 / det();
        Matrix ret;
        ret.IdentityMatrix();

          ret.m[ 0 ][ 0 ] =  DetInv * ( m[ 1 ][ 1 ] * m[ 2 ][ 2 ] - m[ 1 ][ 2 ] * m[ 2 ][ 1 ] );
          ret.m[ 0 ][ 1 ] = -DetInv * ( m[ 0 ][ 1 ] * m[ 2 ][ 2 ] - m[ 0 ][ 2 ] * m[ 2 ][ 1 ] );
          ret.m[ 0 ][ 2 ] =  DetInv * ( m[ 0 ][ 1 ] * m[ 1 ][ 2 ] - m[ 0 ][ 2 ] * m[ 1 ][ 1 ] );
          ret.m[ 0 ][ 3 ] = 0;

          ret.m[ 1 ][ 0 ] = -DetInv * ( m[ 1 ][ 0 ] * m[ 2 ][ 2 ] - m[ 1 ][ 2 ] * m[ 2 ][ 0 ] );
          ret.m[ 1 ][ 1 ] =  DetInv * ( m[ 0 ][ 0 ] * m[ 2 ][ 2 ] - m[ 0 ][ 2 ] * m[ 2 ][ 0 ] );
          ret.m[ 1 ][ 2 ] = -DetInv * ( m[ 0 ][ 0 ] * m[ 1 ][ 2 ] - m[ 0 ][ 2 ] * m[ 1 ][ 0 ] );
          ret.m[ 1 ][ 3 ] = 0;

          ret.m[ 2 ][ 0 ] =  DetInv * ( m[ 1 ][ 0 ] * m[ 2 ][ 1 ] - m[ 1 ][ 1 ] * m[ 2 ][ 0 ] );
          ret.m[ 2 ][ 1 ] = -DetInv * ( m[ 0 ][ 0 ] * m[ 2 ][ 1 ] - m[ 0 ][ 1 ] * m[ 2 ][ 0 ] );
          ret.m[ 2 ][ 2 ] =  DetInv * ( m[ 0 ][ 0 ] * m[ 1 ][ 1 ] - m[ 0 ][ 1 ] * m[ 1 ][ 0 ] );
          ret.m[ 2 ][ 3 ] = 0;

          ret.m[ 3 ][ 0 ] = -( m[ 3 ][ 0 ] * ret.m[ 0 ][ 0 ] + m[ 3 ][ 1 ] * ret.m[ 1 ][ 0 ] + m[ 3 ][ 2 ] * ret.m[ 2 ][ 0 ] );
          ret.m[ 3 ][ 1 ] = -( m[ 3 ][ 0 ] * ret.m[ 0 ][ 1 ] + m[ 3 ][ 1 ] * ret.m[ 1 ][ 1 ] + m[ 3 ][ 2 ] * ret.m[ 2 ][ 1 ] );
          ret.m[ 3 ][ 2 ] = -( m[ 3 ][ 0 ] * ret.m[ 0 ][ 2 ] + m[ 3 ][ 1 ] * ret.m[ 1 ][ 2 ] + m[ 3 ][ 2 ] * ret.m[ 2 ][ 2 ] );
          ret.m[ 3 ][ 3 ] = 1;
          return ret;
    }
};


class Viewport{
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

#endif

