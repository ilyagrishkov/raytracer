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

#ifndef __PPMIMPORTER__
#define __PPMIMPORTER__

#include "tucano/texture.hpp"

namespace Tucano
{

namespace ImageImporter
{

static bool loadPPMImage (string filename, Tucano::Texture* tex);

/**
 * @brief Loads a texture from a PPM file.
 * The texture receives data in the range [0,1] to create a FLOAT texture
 * To convert to PPM in Linux use for example:
 * convert filename.png -compress none filename_out.ppm
 *
 * @param tex Pointer to the texture
 * @param filename Given filename of the PPM file.
 * @return True if loaded successfully, false otherwise
 */
static bool loadPPMImage (string filename, Tucano::Texture *tex)
{


    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());
    filename.erase(std::remove(filename.begin(), filename.end(), '\r'), filename.end());

    ifstream in(filename.c_str(), ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename.c_str() << std::endl; 
        return false;
    }

    vector<float> data;
    string header;
    in >> header;
    int w, h;
    in >> w >> h;
    float max_value;
    in >> max_value;

    float value;
    while (in >> value)
    {
        data.push_back(value/max_value);
    }

    if(in.is_open())
    {
        in.close();
    }

    // flip texture since it will be upside down (invertex y axis)
    vector<float> flipped;
    for (int j = h-1; j >= 0; j--)
    {
        for (int i = 0; i < w; i++)
        {            
            flipped.push_back( data[(j*w + i)*3 + 0]);
            flipped.push_back( data[(j*w + i)*3 + 1]);
            flipped.push_back( data[(j*w + i)*3 + 2]);
        }
    }

    tex->create (GL_TEXTURE_2D, GL_RGBA32F, w, h, GL_RGB, GL_FLOAT, &flipped[0], 0);

    #ifdef TUCANODEBUG
    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
    #endif
    return true;
}


/**
 * @brief Writes a PPM image from given data
 * @param filename Output ppm filename
 * @param width Width of image (must match data width size)
 * @param height Height of image (must match data height size)
 * @param data Vector with RGBA float values [0,1]
 */
static void writePPMImage (string filename, int width, int height, const vector<float>& data)
{
    if (data.size() != width*height*4)
    {
        std::cerr << "data does not match width and height! Data expects (width*height*4) values." << std::endl;
        return;
    }

    ofstream out_stream;
    out_stream.open(filename.c_str());
    out_stream << "P3\n";
    out_stream << width << " " << height << "\n";
    out_stream << "255\n";

    int pos;
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)        
        {
            pos = (i + width*j)*4;
            out_stream << min(255, (int)(255*data[pos+0])) << " " << min(255, (int)(255*data[pos+1])) << " " << min(255, (int)(255*data[pos+2])) << " ";
        }
        out_stream << "\n";
    }
    out_stream.close();

}

static void writePPMImage (string filename, const vector< vector<Eigen::Vector3f> >& data)
{
    int width = data[0].size();
    int height = data.size();

    ofstream out_stream;
    out_stream.open(filename.c_str());
    out_stream << "P3\n";
    out_stream << width << " " << height << "\n";
    out_stream << "255\n";

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)        
        {
            out_stream << min(255, (int)(255*data[j][i][0])) << " " << min(255, (int)(255*data[j][i][1])) << " " << min(255, (int)(255*data[j][i][2])) << " ";
        }
        out_stream << "\n";
    }
    out_stream.close();

}


/*
static bool writePPMImage (string filename, Tucano::Framebuffer* fbo, int attach = 0)
{   
    // compute max value
    float max_value = 0;
    int pos;
    for (int j = size[1]-1; j >= 0; --j)
     {
        for (int i = 0 ; i < size[0]; ++i)
        {
            pos = (i + size[0]*j)*4;
            for (int k = 0; k < 3; ++k)
            {
                max_value = max(max_value, pixels[pos+k]);
            }
        }
        out_stream << "\n";
    }
 
    ofstream out_stream;
    out_stream.open(filename.c_str());
    out_stream << "P3\n";
    out_stream << size[0] << " " << size[1] << "\n";
    out_stream << "255\n";

    bool was_binded = is_binded;

    GLfloat * pixels = new GLfloat[(int)(size[0]*size[1]*4)];
    bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0+attach);
    glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, pixels);

    for (int j = size[1]-1; j >= 0; --j)
    {
        for (int i = 0 ; i < size[0]; ++i)
        {
            pos = (i + size[0]*j)*4;
            out_stream << (int)(255*pixels[pos+0]) << " " << (int)(255*pixels[pos+1]) << " " << (int)(255*pixels[pos+2]) << " ";
        }
        out_stream << "\n";
    }
    out_stream.close();

    if (!was_binded)
    {
        unbindFBO();
    }
    delete [] pixels;
}
*/
}
}
#endif
