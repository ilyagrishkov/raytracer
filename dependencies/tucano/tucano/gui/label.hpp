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

#ifndef __LABEL__
#define __LABEL__

#include "tucano/shapes/quad.hpp"
#include "tucano/gui/element.hpp"
#include <Eigen/Dense>

namespace Tucano
{

namespace GUI
{

/**
 * @brief A simple label to hold text or similar
 * it has no callbacks, it is just a placeholder for rendering a texture
 */
class Label : public Element {

protected:

    /// Color
    Eigen::Vector4f color;

 public:

    /**
     * @brief Default constructor.
     */
    Label (void) 
    { 
        element_type = Tucano::GUI::LABEL;
        num_params = 0;
    }

    /**
     * @brief Overload constructor that receives position and texture file
     * @param x Label top left corner x coordinate in pixels
     * @param y Label top left corner y coordinate in pixels
     * @param texture Path to texture file
     */
    Label (int x, int y, string texture)
    {
        Label();
        position << x, y;
        setTexture (texture);
    }


    int getType (void)
    {
        return Tucano::GUI::LABEL;
    }

    /**
     * @brief Loads the label texture file
     * @param file Texture path
     */
    void setTexture (string file)
    {
        Tucano::ImageImporter::loadImage(file, &texture);
        texture.setTexParameters( GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR );

        dimensions << texture.getWidth(), texture.getHeight();
        setModelMatrix();
    }

     /**
     * @brief Sets the label color.
     * @param c New label color.
     */
    void setColor (const Eigen::Vector4f& c)
    {
        color = c;
    }

};
}
}
#endif
