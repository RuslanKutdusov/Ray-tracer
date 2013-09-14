#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include "Color.hpp"

struct image_t{
    Color* image;
    uint16_t width;
    uint16_t height;
    image_t()
    	: image( nullptr ), width( 0 ), height( 0 )
    {
    }
    ~image_t(){
        if( image )
            delete[] image;
    }
};

int read_png(const std::string & file_name, image_t & image);
int save_png(const std::string & file_name, const image_t & image);
uint8_t * ALPHA(const uint32_t & argb);
uint8_t * RED(const uint32_t & argb);
uint8_t * GREEN(const uint32_t & argb);
uint8_t * BLUE(const uint32_t & argb);

class Texture{
private:
    image_t image;
public:
    Texture(){

    }
    Texture(const std::string & filename){
        read_png(filename, image);
    }
    virtual ~Texture(){

    }
    Color pixel(const float & x, const float & y){
        if( !image.image )
            return Color(1, 1, 1);
        size_t xx = x * image.width;
        size_t yy = y * image.height;
        xx = xx == image.width ? image.width - 1 : xx;
        yy = yy == image.height ? image.height - 1 : yy;
        return image.image[xx + yy * image.width];
    }
};

#endif // TEXTURE_HPP
