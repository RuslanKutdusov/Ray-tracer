#ifndef COLOR_HPP
#define COLOR_HPP

#include <math.h>

template<class T>
static T saturated( T a ){
	return a > 1.0 ? 1.0 : a < 0.0 ? 0.0 : a;
}

class Color
{
public:
    float r;
    float g;
    float b;
    float a;
    Color()
        : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f )
    {

    }
    Color( const float & r_, const float & g_, const float & b_ )
        : r( r_ ), g( g_ ), b( b_ ), a( 0.0f )
    {

    }
    Color( const float & c )
    	: r( c ), g( c ), b( c ), a( 0.0f )
    {

    }
    ~Color(){}
    void normalize()
    {
        if ( r > 1.0f )
            r = 1.0f;
        if ( g > 1.0f )
            g = 1.0f;
        if ( b > 1.0f )
            b = 1.0f;
    }
    Color operator+( const Color & c ) const
    {
        return Color( r + c.r, g + c.g, b + c.b );
    }
    Color operator-( const Color & c ) const
    {
        return Color( r - c.r, g - c.g, b - c.b );
    }
    Color operator*( const Color & c ) const
    {
        return Color( r * c.r, g * c.g, b * c.b );
    }
    Color operator*( const float & k ) const
    {
        return Color( k * r, k *  g, k * b );
    }
    Color operator/( const float & k ) const
    {
        return Color( r / k, g / k, b / k );
    }
    Color operator^( const float & k ) const
    {
        return Color( pow( r, k ), pow( g, k ), pow( b, k ) );
    }
    bool is_black() const
    {
        return r == 0.0f && g == 0.0f && b == 0.0f;
    }
    void saturate()
    {
    	r = saturated<float>( r );
    	g = saturated<float>( g );
    	b = saturated<float>( b );
    }
    float luminance()
    {
    	return 0.299f * r + 0.587f * g  + 0.114f * b;
    }
    void tone_mapping()
    {
    	float lum = luminance();
    	r /= lum + 1.0f;
    	g /= lum + 1.0f;
    	b /= lum + 1.0f;
    }
    void gamma_correction()
    {
    	float gamma = 1.0f / 2.2f;
    	r = pow( r, gamma );
    	g = pow( g, gamma );
    	b = pow( b, gamma );
    }
} __attribute__ ( ( aligned( 16 ) ) );;

#endif // COLOR_HPP
