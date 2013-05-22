#include "Texture.hpp"


#include "Texture.hpp"
#include <png.h>
#include <stdio.h>

void PNGAPI error_function(png_structp png, png_const_charp dummy) {
  (void)dummy;  // remove variable-unused warning
  longjmp(png_jmpbuf(png), 1);
}

uint8_t * ALPHA(const uint32_t & argb)
{
        return (uint8_t*)(&argb) + 3;
}

uint8_t * RED(const uint32_t & argb)
{
        return (uint8_t*)(&argb) + 2;
}

uint8_t * GREEN(const uint32_t & argb)
{
        return (uint8_t*)(&argb) + 1;
}

uint8_t * BLUE(const uint32_t & argb)
{
        return (uint8_t*)(&argb);
}

 int read_png(const std::string & file_name, image_t & image) {
  png_structp png;
  png_infop info;
  int color_type, bit_depth, interlaced;
  int has_alpha;
  int num_passes;
  int p;
  int ok = 0;
  png_uint_32 width, height, y;
  int stride;
  uint8_t* rgb = NULL;

  FILE * fp;

    fp = fopen(file_name.c_str(), "rb");
    if (fp == NULL)
        return -1;

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (png == NULL) {
    goto End;
  }

  png_set_error_fn(png, 0, error_function, NULL);
  if (setjmp(png_jmpbuf(png))) {
 Error:
    png_destroy_read_struct(&png, NULL, NULL);
    free(rgb);
    goto End;
  }

  info = png_create_info_struct(png);
  if (info == NULL) goto Error;

  png_init_io(png, fp);
  png_read_info(png, info);
  if (!png_get_IHDR(png, info,
                    &width, &height, &bit_depth, &color_type, &interlaced,
                    NULL, NULL)) goto Error;

  png_set_strip_16(png);
  png_set_packing(png);
  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) {
      png_set_expand_gray_1_2_4_to_8(png);
    }
    png_set_gray_to_rgb(png);
  }
  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
    has_alpha = 1;
  } else {
    has_alpha = !!(color_type & PNG_COLOR_MASK_ALPHA);
  }


  num_passes = png_set_interlace_handling(png);
  png_read_update_info(png, info);
  stride = (has_alpha ? 4 : 3) * width * sizeof(*rgb);
  rgb = (uint8_t*)malloc(stride * height);
  if (rgb == NULL) goto Error;
  for (p = 0; p < num_passes; ++p) {
    for (y = 0; y < height; ++y) {
      png_bytep row = rgb + y * stride;
      png_read_rows(png, &row, NULL, 1);
    }
  }
  png_read_end(png, info);
  png_destroy_read_struct(&png, &info, NULL);

  image.width = width;
  image.height = height;
  image.image = new uint32_t[height * width];
  for(size_t y = 0; y < height; y++)
      for(size_t x = 0; x < width; x++){
          size_t i = y * width + x;
          if (has_alpha) {
              *ALPHA(image.image[i]) = rgb[stride * y + x * 4];
              *RED(image.image[i])   = rgb[stride * y + x * 4 + 1];
              *GREEN(image.image[i]) = rgb[stride * y + x * 4 + 2];
              *BLUE(image.image[i])  = rgb[stride * y + x * 4 + 3];
          }
          else{
              *ALPHA(image.image[i]) = 0xFF;
              *RED(image.image[i])   = rgb[stride * y + x * 3];
              *GREEN(image.image[i]) = rgb[stride * y + x * 3 + 1];
              *BLUE(image.image[i])  = rgb[stride * y + x * 3 + 2];
          }
      }
  free(rgb);

 End:
  fclose(fp);
  return ok;
}
