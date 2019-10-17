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

#ifndef __IMAGEIO__
#define __IMAGEIO__

#include "tucano/utils/pamIO.hpp"
#include "tucano/utils/ppmIO.hpp"

namespace Tucano
{

namespace ImageImporter
{

static bool loadImage (string filename, Tucano::Texture* tex);
static bool writeImage (string filename, Tucano::Framebuffer* tex, int attach = 0);
static bool searchAlternativeExtension (string filename, string ext);


/**
 * @brief If file format not supported searches for an alternative supported file (same name)
 * @param filename Unsupported filename without extension
 * @param ext Supported extension to search file
 */
static bool searchAlternativeExtension (string filename, string ext)
{
    string supported_file = filename + ext;
    std::ifstream infile(supported_file);
    if (infile.good())
    {        
        return true;
    }
    return false;
}


/* @brief Loads a texture from a supported file format.
 * Checks file extension, if format is supported reads file and loads texture
 *
 * @param filename Given filename.
 * @param tex Pointer to the texture
 * @return True if loaded successfully, false otherwise
 */
static bool loadImage (string filename, Tucano::Texture *tex)
{
    string ext = filename.substr(filename.find_last_of(".") + 1);

    if( ext.compare("pam") == 0)
    {
        return loadPAMImage (filename, tex);
    } 
    else if (ext.compare("ppm") == 0)
    {
        return loadPPMImage (filename, tex);
    }
    // if extesion not valid, search for same filename with same extension
    else    
    {
        std::cerr << "format not supported : " << ext.c_str() << std::endl;
        string file_without_ext = filename.substr(0, filename.find_last_of(".") + 1);
        
        if (searchAlternativeExtension (file_without_ext, "pam"))
        {
            std::cout << "using equivalent PAM file" << std::endl;
            return loadPAMImage (file_without_ext + "pam", tex);
        }
        if (searchAlternativeExtension (file_without_ext, "ppm"))
        {
            std::cout << "using equivalent PPM file" << std::endl;
            return loadPAMImage (file_without_ext + "ppm", tex);
        }
    }


    return false;

}

/**
 * @brief Saves a framebuffer attachment to an image file
 *
 * @param filename Image file to save fbo attach
 * @param fbo Pointer to the Framebuffer
 * @param attach Attchment number to be saved
 */
static bool writeImage (string filename, Tucano::Framebuffer* fbo, int attach)
{
    string ext = filename.substr(filename.find_last_of(".") + 1);
    if( ext.compare("pam") == 0)
    {
        return writePAMImage (filename, fbo, attach);
    } 
    else if (ext.compare("ppm") == 0)
    {
        return false;//writePPMImage (filename, fbo, attach);
    }
    std::cerr << "format not supported : " << ext.c_str() << std::endl;
    return false;
}


}
}
#endif
