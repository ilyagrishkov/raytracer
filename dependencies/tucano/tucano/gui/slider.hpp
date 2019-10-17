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

#ifndef __SLIDER__
#define __SLIDER__

#include "tucano/shapes/quad.hpp"
#include "tucano/gui/element.hpp"
#include <Eigen/Dense>

namespace Tucano
{

namespace GUI
{

/**
 * @brief The slide class draws a dragable cursor over a bar
 */
class Slider : public Element {

protected:

    /// Slider texture
    Tucano::Texture slider_texture;

    /// Slider has one callback with the slider position
    function< void(float) > callback;

    /// Slider current value
    float value = 0.5;

    /// Min max values
    float min_value = 0.0;
    float max_value = 1.0;

    /// Model matrix for the slider (not constant)
    Eigen::Affine3f slider_model_matrix = Eigen::Affine3f::Identity();

    /// is holding slider
    bool sliding = false;

    /// position of slider
    Eigen::Vector2i slider_pos = Eigen::Vector2i::Zero();

    /// slider dimensions
    Eigen::Vector2i slider_dim = Eigen::Vector2i::Zero();

public:

    /**
     * @brief Default constructor.
     */
    Slider (void)
    {
        Slider(10, 100, 0, 0);
    }

    /**
     * @brief Overload Constructor.
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     */
    Slider (int w, int h, int x, int y)
    {
        dimensions << w, h;
        position << x, y;
        setValue(value);
        element_type = Tucano::GUI::SLIDER;
        num_params = 1;
    }

    /**
     * @brief Overload constructor that receives dimensions and texture file
     * @param w Button width in pixels
     * @param h Button height in pixels
     * @param x Button top left corner x coordinate in pixels
     * @param y Button top left corner y coordinate in pixels
     * @param texture Path to texture file
     */
    Slider (int w, int h, int x, int y, string bar_tex, string slider_tex)
    {
        Slider (w, h, x, y);
        setTexture (bar_tex, slider_tex);
    }

    /**
     * @brief Sets min max values for the slide
     * All operations are treated in range [0,1] and only when
     * responding the callback mapped to [min, max]
     * @param minv Min value
     * @param maxv Max value
     */
    void setMinMaxValues (float minv, float maxv)
    {
        min_value = minv;
        max_value = maxv;
    }


    /**
     * @brief Move slider to relative position on bar [min,max]
     * @param v Value in range [min,max]
     */
    void moveSlider (float v)
    {
        setValue ((v - min_value)/(max_value - min_value));    
    }

    /**
     * @brief Move slider to relative normalized position on bar [0,1]
     * @param normalized_pos Value in range [0,1]
     */
    void setValue (float normalized_pos)
    {
         // clamp in range [0,1]
        value = min( max (0.0f, normalized_pos), 1.0f);
        slider_pos << position[0] + dimensions[0]*value, position[1];
        setSliderModelMatrix();
    }

    /**
     * @brief Returns if clicked on slider handle
     * If clicked on handle start slidigin while button is pressed
     * If clicked on bar but on handle returns true but does not start sliding
     * @param pos Screen position of the mouse click
     * @return True if clicked on bar or handle, false otherwise
     */
    bool isInside (const Eigen::Vector2i& pos)
    {
        // check if clicked on bar
        if (Tucano::GUI::Element::isInside(pos))
        {
            float relative_pos = (pos[0] - position[0])/(float)dimensions[0];
            // check if clicked on slider handle, we leave an extra margin to facilita clicking
            if (fabs(relative_pos - value) <= 1.5*slider_dim[0]/(float)dimensions[0]) 
            { 
                setValue(relative_pos);
                sliding = true;
            }
            return true;
        }
        sliding = false;
        return false;
    }

    /**
     * @brief Set behavior when mouse is released after clicking this element
     * @param x X position of cursor
     * @param y Y position of cursor
     */
    void cursorMove (int x, int y)
    {
        if (sliding)
        {
            setValue((x - position[0])/(float)dimensions[0]);
        }
    }


    /**
     * @brief Stop sliding when mouse is released
     */
    void release (void)
    {
        sliding = false;
    }


    int getType (void)
    {
        return Tucano::GUI::SLIDER;
    }

    /**
     * @brief Loads the slider texture file
     * @param file Texture path
     */
    void setTexture (string bar_file, string slider_file)
    {
        Tucano::ImageImporter::loadImage(bar_file, &texture);
        texture.setTexParameters( GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR );
        Tucano::ImageImporter::loadImage(slider_file, &slider_texture);
        slider_texture.setTexParameters( GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR );

        slider_dim << slider_texture.getWidth() * (dimensions[1] / (float) texture.getHeight()), dimensions[1];

        setValue(value);
    }

    /**
     * brief Sets the slider callback with one parameters
     * @param f Callback method
     */
    void onValueChanged (function<void(float)> f)
    {
        callback = f;  
    }


    /**
     * @brief Returns the value in range [min, max]
     * @return Slider value
     */
    float getValue (void)
    {
        return (value)*(max_value-min_value) + min_value;
    }


    /**
     * @brief Callback when value changes
     */
    void valueChanged (void)
    {
        callback(getValue());
    }

    /**
     * @brief Sets the slider color.
     * @param c New slider color.
     */
    void setColor (const Eigen::Vector4f& c)
    {
        color = c;
    }

    /**
     * @brief Sets the slider model matrix
     */
    void setModelMatrix (void)
    {
        Element::setModelMatrix();
        setSliderModelMatrix();
    }

    /**
     * @brief Sets the model matrix for the slider with the respective position
     */
    void setSliderModelMatrix (void)
    {
        slider_model_matrix = Eigen::Affine3f::Identity();
        // place the slider at the horizontal center of the value, and a little below the bar
        slider_model_matrix.translate (Eigen::Vector3f ((float)slider_pos[0], (float)slider_pos[1], 0.0));
        // scale slider to same scale as bar (in pixels slider should have twice the height of the bar)
        slider_model_matrix.scale (Eigen::Vector3f( (float)slider_dim[0], (float)slider_dim[1], 1.0) );
        // set y origin to top corner
        slider_model_matrix.translate (Eigen::Vector3f (0.0, 0.5, 0.0) );
    }

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
        texture.unbind();

        shader.setUniform("modelMatrix", slider_model_matrix);
        shader.setUniform("shapetex", slider_texture.bind());

        quad.setAttributeLocation(shader);
        quad.bindBuffers();
        quad.renderElements();

        quad.unbindBuffers();
        slider_texture.unbind();
        shader.unbind(); 
    }    

};
}
}
#endif
