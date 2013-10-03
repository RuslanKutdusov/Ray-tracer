#ifndef COLOR_HPP
#define COLOR_HPP

#include "cuda.hpp"
#include <math.h>

template<class T>
static T saturated(T a){
	return a > 1.0 ? 1.0 : a < 0.0 ? 0.0 : a;
}

class Color{
public:
    float r;
    float g;
    float b;
    float a;
    HOST_DEVICE Color()
        //: r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f )
    {}
    HOST_DEVICE Color(const float & r_, const float & g_, const float & b_)
        : r(r_), g(g_), b(b_), a( 0.0f )
    {}
    HOST_DEVICE Color( const float & c)
    	: r( c ), g( c ), b( c ), a( 0.0f )
    {}
    HOST_DEVICE ~Color(){}
    HOST_DEVICE Color operator+(const Color & c) const{
        return Color(r + c.r, g + c.g, b + c.b);
    }
    HOST_DEVICE Color operator-(const Color & c) const{
        return Color(r - c.r, g - c.g, b - c.b);
    }
    HOST_DEVICE Color operator*(const Color & c) const{
        return Color(r * c.r, g * c.g, b * c.b);
    }
    HOST_DEVICE Color operator*(const float & k) const{
        return Color(k * r, k *  g, k * b);
    }
    HOST_DEVICE Color operator/(const float & k) const{
        return Color(r / k, g / k, b / k);
    }
    HOST_DEVICE Color operator^(const float & k) const{
        return Color( pow( r, k ), pow( g, k ), pow( b, k ) );
    }
    HOST_DEVICE bool is_black() const{
        return r == 0.0f && g == 0.0f && b == 0.0f;
    }
    HOST_DEVICE void saturate(){
    	r = saturated<float>( r );
    	g = saturated<float>( g );
    	b = saturated<float>( b );
    }
    HOST_DEVICE float luminance(){
    	return 0.299f * r + 0.587f * g  + 0.114f * b;
    }
    HOST_DEVICE void tone_mapping(){
    	float lum = luminance();
    	r /= lum + 1.0f;
    	g /= lum + 1.0f;
    	b /= lum + 1.0f;
    }
    HOST_DEVICE void gamma_correction(){
    	float gamma = 1.0f / 2.2f;
    	r = powf( r, gamma );
    	g = powf( g, gamma );
    	b = powf( b, gamma );
    }
} __attribute__ ((aligned(16)));;

#endif // COLOR_HPP
