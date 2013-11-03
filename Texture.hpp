#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <stdint.h>
#include "Color.hpp"

struct image_t
{
    Color* 		image;
    uint16_t 	width;
    uint16_t 	height;

    image_t()
    	: image( NULL ), width( 0 ), height( 0 )
    {

    }

    ~image_t()
    {
        if( image )
            delete[] image;
    }
};

int read_png( const std::string & file_name, image_t & image, float gamma );
int save_png( const std::string & file_name, const image_t & image, float gamma );
uint8_t* ALPHA( const uint32_t & argb );
uint8_t* RED( const uint32_t & argb );
uint8_t* GREEN( const uint32_t & argb );
uint8_t* BLUE( const uint32_t & argb );
void InitTextureSystem( float gamma );

class Texture
{
private:
    image_t m_image;
public:
    Texture();
    Texture( const std::string& filename );
    ~Texture();
    Color pixel( const float& x, const float& y );
};

#endif // TEXTURE_HPP
