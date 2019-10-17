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

#ifndef __GROUPBOX__
#define __GROUPBOX__

#include "tucano/shapes/quad.hpp"
#include "tucano/gui/element.hpp"
#include <Eigen/Dense>

namespace Tucano
{

namespace GUI
{

/**
 * @brief The gui group box is just a placeholder for other gui elements.
 * It can be used to group elements, so they can be moved or hidden together for example
 * Visually it is also a protected clickable area, to avoid mis-clicking the elements
 */
class GroupBox : public Tucano::GUI::Element {

protected:

    /// Color
    Eigen::Vector4f color;

    /// Vector containing gui elements inside box
    vector < Tucano::GUI::Element* > elements;

public:

    /**
     * @brief Default constructor.
     */
    GroupBox (void)
    {
        GroupBox(10, 10, 0, 0);
    }

    /**
     * @brief Overload Constructor.
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     */
    GroupBox (int w, int h, int x, int y)
    {
        color << 0.0, 0.8, 0.8, 1.0;
        dimensions << w, h;
        position << x, y;
        setModelMatrix();
        element_type = Tucano::GUI::GROUPBOX;
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
    GroupBox (int w, int h, int x, int y, string texture)
    {
        GroupBox (w, h, x, y);
        setTexture (texture);
    }

    /**
     * @brief Adds an element to the gui
     */
    void add (Element *el)
    {
        elements.push_back(el);
    }

	vector < Tucano::GUI::Element *> * elementList (void)
	{
		return &elements;
	}

    int getType (void)
    {
        return Tucano::GUI::GROUPBOX;
    }

   /**
     * @brief Sets the button color.
     * @param c New button color.
     */
    void setColor (const Eigen::Vector4f& c)
    {
        color = c;
    }

	/**
	 * @brief Renders the groupbox and all elements inside
	 * @param camera_2d GUI 2D camera
	 * @param shader A custom shader for renderin groupbox and elements
	 */
    void render ( Tucano::Camera &camera_2d, Shader &shader )
    {        
		if (!visible)
			return;

        shader.bind();

        shader.setUniform("modelMatrix", model_matrix);
        shader.setUniform("viewMatrix", camera_2d.getViewMatrix());
        shader.setUniform("projectionMatrix", camera_2d.getProjectionMatrix());
        shader.setUniform("in_Color", color);
        shader.setUniform("shapetex", texture.bind());

        quad.setAttributeLocation(shader);

        quad.bindBuffers();
        quad.renderElements();
        quad.unbindBuffers();

        shader.unbind(); 
        texture.unbind();

		for (unsigned int i = 0; i < elements.size(); ++i)
        {
            elements[i]->render(camera_2d, shader);
        }
 
    }    

};
}
}
#endif
