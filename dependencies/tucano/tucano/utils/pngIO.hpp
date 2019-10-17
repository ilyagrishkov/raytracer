/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PNGIO__
#define __PNGIO__

#include "tucano/framebuffer.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3

#include <png.h>


namespace Tucano
{

namespace ImageImporter
{

static bool loadPNGImage (string filename, Tucano::Texture *tex);
//static bool writePNGImage (string filename, Tucano::Framebuffer* fbo, int attach = 0);

static void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}


/* @brief Loads a texture from a PNG file using pnglib.
 * The texture receives data in the range [0,1] to create a FLOAT texture
 *
 * @param filename Given filename of the PNG file.
 * @param tex Pointer to the texture
 * @return True if loaded successfully, false otherwise
 */
static bool loadPNGImage (string filename, Tucano::Texture *tex)
{
    unsigned char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp)
            abort_("[read_png_file] File %s could not be opened for reading", filename);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
            abort_("[read_png_file] File %s is not recognized as a PNG file", filename);


    /* initialize stuff */
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
            abort_("[read_png_file] png_create_read_struct failed");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
            abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    //png_byte color_type = 
    png_get_color_type(png_ptr, info_ptr);
    //png_byte bit_depth = 
    png_get_bit_depth(png_ptr, info_ptr);

    //int number_of_passes = 
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    //auto screen_gamma = PNG_DEFAULT_sRGB;
    //png_set_gamma(png_ptr, screen_gamma, PNG_DEFAULT_sRGB);

   /*int intent;

   auto screen_gamma = PNG_DEFAULT_sRGB;
   if (png_get_sRGB(png_ptr, info_ptr, &intent) != 0)
      png_set_gamma(png_ptr, screen_gamma, PNG_DEFAULT_sRGB);
   else
   {
      double image_gamma;
      if (png_get_gAMA(png_ptr, info_ptr, &image_gamma) != 0)
         png_set_gamma(png_ptr, screen_gamma, image_gamma);
      else
         png_set_gamma(png_ptr, screen_gamma, 0.45455);
   }*/


    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during read_image");

    auto row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
            row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

    fclose(fp);  

    int d = 3;
    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
        d = 3;
    else if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGBA)
        d = 4;    

    vector<float> data;
    //for (unsigned int y = 0; y < height; y++) {
    for (int j = height-1; j >= 0; j--)
    {
        png_byte* row = row_pointers[j];
        for (int i = 0; i < width; i++)
        {

            png_byte* ptr = &(row[i*d]);
            //printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n", i, j, ptr[0], ptr[1], ptr[2], ptr[3]);
            //std::cout << i << " " << j << ": " << (int)ptr[0]/255.0 << " " << (int)ptr[1]/255.0 << " " << (int)ptr[2]/255.0 << std::endl;
            data.push_back((int)ptr[0]/255.0);
            data.push_back((int)ptr[1]/255.0);
            data.push_back((int)ptr[2]/255.0);
            if (d == 4)
                data.push_back((int)ptr[3]/255.0);
        }
    }

    if (d == 3)
        tex->create (GL_TEXTURE_2D, GL_RGBA32F, width, height, GL_RGB, GL_FLOAT, &data[0], 0);
    else if (d == 4)
        tex->create (GL_TEXTURE_2D, GL_RGBA32F, width, height, GL_RGBA, GL_FLOAT, &data[0], 0);

    #ifdef TUCANODEBUG
    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "load PAM");
    #endif

    return true;
}



/**
 * @brief Saves a framebuffer attachment to an image file
 * Note that since the input is a float image, the max value is arbitrary
 * For now we support only 1 byte per value
 * We are also assuming writing to an RGBA format
 *
 * @param filename Image file to save fbo attach
 * @param fbo Pointer to the Framebuffer
 * @param attach Attchment number to be saved
 */
// static bool writePNGImage (string filename, Tucano::Framebuffer* fbo, int attach)
// {

// }


}
}
#endif
