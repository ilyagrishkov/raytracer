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

#ifndef __BUTTON__
#define __BUTTON__

#include "tucano/shapes/quad.hpp"
#include "tucano/gui/element.hpp"
#include <Eigen/Dense>

namespace Tucano
{

namespace GUI
{

/**
 * @brief The button class draws a clickable rectangle on the screen
 * to be used as a callback interface
 */
class Button : public Element {

protected:

    /// Clicked callback without parameters
    function< void() > click_callback;

    /// Hovering callback without parameters
    function< void() > hover_callback;

public:

    /**
     * @brief Default constructor.
     */
    Button (void)
    {
        Button(10, 10, 0, 0);
    }

    /**
     * @brief Overload Constructor.
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     */
    Button (int w, int h, int x, int y)
    {
        dimensions << w, h;
        position << x, y;
        setModelMatrix();
        element_type = Tucano::GUI::BUTTON;
        num_params = 0;
    }

    /**
     * @brief Overload constructor that receives dimensions and texture file
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     * @param texture Path to texture file
     */
    Button (int w, int h, int x, int y, string texture)
    {
        Button (w, h, x, y);
        setTexture (texture);
    }

    /**
     * @brief Sets the texture to render when button is hovered
     * @param file Texture path
     */
    void setHoverTexture (string file)
    {
        setAltTexture2(file);
    }


    int getType (void)
    {
        return Tucano::GUI::BUTTON;
    }

    /**
     * @brief Sets the button callback with no parameters
     * @param f Callback method
     */
    void onClick (function<void()> f)
    {
        click_callback = f;  
    }

    /**
     * @brief Calls clicked callback
     */
    void clicked (void)
    {
        click_callback();
    }

    /**
     * @brief sets the button hover callback with no parameters
     */
    void onhover (function<void()> f)
    {
        hover_callback = f;
    }

    /**
     * @brief calls hovering callback
     */
    void hovering (void)
    {
        hover_callback();
    }

    /**
     * @brief Sets the button color.
     * @param c New button color.
     */
    void setColor (const Eigen::Vector4f& c)
    {
        color = c;
    }

};
}
}
#endif
