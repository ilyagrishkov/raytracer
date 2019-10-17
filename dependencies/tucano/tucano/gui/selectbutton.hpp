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

#ifndef __SELECTBUTTON__
#define __SELECTBUTTON__

#include "tucano/shapes/quad.hpp"
#include "tucano/gui/element.hpp"
#include <Eigen/Dense>

namespace Tucano
{

namespace GUI
{

class SelectGroup;

/**
 * @brief The button class draws a clickable rectangle on the screen
 * to be used as a callback interface
 * The select button toggles between pressed and not pressed when clicked
 *
 * The alt textures are used for Selection (alt) and Hovering (alt2)
 */
class SelectButton : public Element {

friend SelectGroup;

protected:

	/// Flag to select (pressed) / unselect button
	bool is_selected = false;

    /// Flag to indicate is button is being hovered
    bool is_hovering = false;

    /// Clicked callback without parameters
    function< void() > click_callback;

    /// Hovering callback without parameters
    function< void() > hover_callback;
public:

    /**
     * @brief Default constructor.
     */
    SelectButton (void)
    {
        SelectButton(10, 10, 0, 0);
    }

    /**
     * @brief Overload Constructor.
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     */
    SelectButton (int w, int h, int x, int y)
    {
        dimensions << w, h;
        position << x, y;
        setModelMatrix();
        element_type = Tucano::GUI::BUTTON;
        num_params = 0;
		color = Eigen::Vector4f (0.1, 0.1, 0.1, 1.0);
    }

    /**
     * @brief Overload constructor that receives dimensions and texture file
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     * @param texture Path to texture file
     */
    SelectButton (int w, int h, int x, int y, string texture)
    {
        SelectButton (w, h, x, y);
        setTexture (texture);
    }

    /**
     * @brief Overload constructor that receives dimensions and texture file
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     * @param texture Path to texture file
     */
    SelectButton (int w, int h, int x, int y, string texture, string alt_texture)
    {
        SelectButton (w, h, x, y);
        setTexture (texture);
        setAltTexture (alt_texture);
    }

    /**
     * @brief Sets the texture to render when button is selected
     * @param file Texture path
     */
    void setSelectedTexture (string file)
    {
        setAltTexture(file);
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
        return Tucano::GUI::SELECTBUTTON;
    }


    /**
     * brief Sets the button callback with no parameters
     * @param f Callback method
     */
    void onClick (function<void()> f)
    {
        click_callback = f;  
    }

	/**
	 * @brief Click callback
	 */
    void clicked (void)
    {
		toggleSelected();
        click_callback();
    }

    /**
     * @brief sets the button hover callback with no parameters
     */
    void onHover (function<void()> f)
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

	/**
	 * @brief Toggles button state selected/unselected
	 */
	void toggleSelected (void)
	{
		is_selected = !is_selected;

        // if no alternate texture, modulate the color
        if (!has_alt_texture)
        {
    		if (is_selected)
	    		color = Eigen::Vector4f (1.0, 1.0, 1.0, 1.0);
		    else
			    color = Eigen::Vector4f (0.1, 0.1, 0.1, 1.0);
        }
	}

	/**
	 * @brief Renders button
	 * If has alternate texture uses the one corresponding to current state,
	 * otherwise uses always main texture.
	 * @param camera_2d The 2D GUI camera
	 * @param shader A shader for rendering the button
	 */
  	void render (Camera &camera_2d, Shader &shader) 
	{
		if (!visible)
			return;

        shader.bind();

        shader.setUniform("modelMatrix", model_matrix);
        shader.setUniform("viewMatrix", camera_2d.getViewMatrix());
        shader.setUniform("projectionMatrix", camera_2d.getProjectionMatrix());
        shader.setUniform("in_Color", color);
        if (is_selected && has_alt_texture)
            shader.setUniform("shapetex", texture_alt.bind());
        else if (is_hovering && has_alt_texture2)
            shader.setUniform("shapetex", texture_alt2.bind());
        else
            shader.setUniform("shapetex", texture.bind());

        quad.setAttributeLocation(shader);

        quad.bindBuffers();
        quad.renderElements();
        quad.unbindBuffers();

        shader.unbind(); 
        if (is_selected && has_alt_texture)
            texture_alt.unbind();
        else if (is_hovering && has_alt_texture2)
            texture_alt2.unbind();
        else
            texture.unbind();
	}


};
}
}
#endif
