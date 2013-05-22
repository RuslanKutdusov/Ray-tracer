#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include "Color.hpp"

struct image_t{
    uint32_t* image;
    uint16_t width;
    uint16_t height;
    image_t(){
        image = NULL;
    }
    ~image_t(){
        if (image)
            delete image;
    }
};

int read_png(const std::string & file_name, image_t & image);
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
    Color pixel(const double & x, const double & y){
        size_t xx = x * image.width;
        size_t yy = y * image.height;
        xx = xx == image.width ? image.width - 1 : xx;
        yy = yy == image.height ? image.height - 1 : yy;
        uint32_t c = image.image[xx + yy * image.width];
        return Color (*RED(c) / 255, *GREEN(c) / 25, *BLUE(c) / 255);
    }
};

#endif // TEXTURE_HPP
