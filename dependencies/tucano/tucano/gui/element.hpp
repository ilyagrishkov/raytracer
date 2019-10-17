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

#ifndef __GUIELEMENT__
#define __GUIELEMENT__

#include "tucano/gui/guishaders.hpp"
#include "tucano/utils/imageIO.hpp"
#include "tucano/texture.hpp"

namespace Tucano
{

namespace GUI
{

enum ElementType : signed int
{
    NONE,
    BUTTON,
	SELECTBUTTON,
    SLIDER,
    LABEL,
    GROUPBOX,
	SELECTGROUP
};

/**
 * @brief Base class for all GUI elements (buttons, sliders ...)
 *
 * This class contain the basic callback interface
 */
class Element {

protected:

    /// Type of element
    int element_type = Tucano::GUI::NONE;

    /// Number of parameters for the callback
    int num_params = 0;

    /// Model Matrix for placing element
    Eigen::Affine3f model_matrix;

    /// Dimensions in pixels
    Eigen::Vector2i dimensions;

    /// Position of top-left corner in pixels
    Eigen::Vector2i position;

     /// Quad to hold texture
    Shapes::Quad quad;

    /// Element texture
    Tucano::Texture texture;

    /// Alternative texture (for example, on/off buttons)
    Tucano::Texture texture_alt;

    /// 2nd Alternative texture (for example, hover on)
    Tucano::Texture texture_alt2;

    /// Flag to define if element has alt texture
    bool has_alt_texture = 0;

    /// Flag to define if element has second alt texture
    bool has_alt_texture2 = 0;

    /// Color
    Eigen::Vector4f color = Eigen::Vector4f (1.0, 1.0, 1.0, 1.0);

    function< void() > callback_noparam;
    function< void(int) > callback_1i;
    function< void(float) > callback_1f;

	/// Flag to show/hide element
	bool visible = true;

    /// Flag to enable/disable hover on functionality
    bool hover = false;

public:

    /**
     * @brief Default constructor.
     */
    Element (void) 
    {
        model_matrix = Eigen::Affine3f::Identity();
        callback_noparam = NULL;
        callback_1i = NULL;
        callback_1f = NULL;
    }


	/**
	 * @brief Returns the element's dimensions
	 * @return Element's dimensions as a Vector2i
	 */
	Eigen::Vector2i getDimensions (void)
	{
		return dimensions;
	}

	/**
	 * @brief Returns coordinates of elements top left corner
	 * @return Element's position as Vector2i
	 */
	Eigen::Vector2i getPosition (void)
	{
		return position;
	}

     /**
     * @brief Sets the button dimensions
     * @param dim New button dimensions
     */
    void setDimensions (const Eigen::Vector2i& dim)
    {
        dimensions = dim;
        setModelMatrix();
    }

     /**
     * @brief Sets the button dimensions
     * @param w New button width
     * @param h New button height
     */
    void setDimensions (int w, int h)
    {
        setDimensions(Eigen::Vector2i(w,h));
    }

    /**
     * @brief Sets the width proportinal to given height
     * Uses texture dimensions as parameter, it must be already set
     * @param s Given height
     */
    virtual void setDimensionsFromHeight (int h)
    {
        if (texture.getWidth() != 0)
        {
            dimensions << texture.getWidth() * h/(float)texture.getHeight(), h;
            setModelMatrix();
        }
    }

    /**
     * @brief Loads the button texture file
     * @param file Texture path
     */
    virtual void setTexture (string file)
    {
        Tucano::ImageImporter::loadImage(file, &texture);
        texture.setTexParameters( GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR );
    }

    /**
     * @brief Loads the button alt texture file
     * @param file Texture path
     */
    virtual void setAltTexture (string file)
    {
        Tucano::ImageImporter::loadImage(file, &texture_alt);
        texture_alt.setTexParameters( GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR );
        has_alt_texture = true;
	    color = Eigen::Vector4f (1.0, 1.0, 1.0, 1.0); // wont modulate color, set it to white
    }

    /**
     * @brief Loads the button second alt texture file
     * @param file Texture path
     */
    virtual void setAltTexture2 (string file)
    {
        Tucano::ImageImporter::loadImage(file, &texture_alt2);
        texture_alt.setTexParameters( GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR );
        has_alt_texture2 = true;
	    color = Eigen::Vector4f (1.0, 1.0, 1.0, 1.0); // wont modulate color, set it to white
    }



   /**
     * @brief Sets the element model matrix
     */
    virtual void setModelMatrix (void)
    {
        model_matrix = Eigen::Affine3f::Identity();
        
        // translates to position
        model_matrix.translate (Eigen::Vector3f ((float)position[0], (float)position[1], 0.0) );
        // scales to fit required dimensions, invert y axis to get appropriate texcoords
        model_matrix.scale (Eigen::Vector3f ((float)dimensions[0], (float)dimensions[1], 1.0));
        // sets the quad to range (0, 1) instead of (-0.5, 0.5)
        model_matrix.translate (Eigen::Vector3f (0.5, 0.5, 0.0) );
    }



    /**
     * @brief Sets the button top left position
     * @param pos New button position
     */
    void setPosition (const Eigen::Vector2i& pos)
    {
        position = pos;
        setModelMatrix();
    }

    /**
     * @brief Sets the button top left position
     * @param x New button x position
     * @param y New button y position
     */
    void setPosition (int x, int y)
    {
        setPosition (Eigen::Vector2i(x,y));
    }
   /**
     * @brief Overloads the isInside method. Queries if a point is inside the button.
     * @param x X position of query point
     * @param y Y position of query point
     */
    bool isInside (int x, int y)
    {
        return isInside (Eigen::Vector2i(x,y));
    }

    /**
     * @brief Returns if a point is inside the element controller
     * @param pos Screen position
     */
    virtual bool isInside (const Eigen::Vector2i& pos)
    {
        if (pos[0] >= position[0] && pos[0] <= position[0] + dimensions[0] &&
            pos[1] >= position[1] && pos[1] <= position[1] + dimensions[1])
            return true;
        return false;
    }

    /**
     * @brief Set behavior when mouse is released after clicking this element
     */
    virtual void release (void) {}

    /**
     * @brief Set behavior when mouse is released after clicking this element
     * @param x X position of cursor
     * @param y Y position of cursor
     */
    virtual void cursorMove (int x, int y) {}

    /**
     * @brief Returns the element type
     */
    virtual int getType (void)
    {
        return element_type;
    }

    virtual void render (Camera &camera_2d, Shader &shader) 
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
	}



    virtual void setCallback (function<void()> f)
    {
        callback_noparam = f;  
    }

    /**
     * @brief Sets the callback from an method
     */
    virtual void setCallback1i (function< void(int) > f)
    {
        callback_1i = f;
    }

    /**
     * @brief Sets the callback from an method
     */
    virtual void setCallback1f (function< void(float) > f)
    {
        callback_1f = f;
    }

    /**
     * @brief Callback with no parameters
     */
    virtual void callback (void)
    {
        callback();
    }

    /**
    * @brief Callback with 1 integer
    */
    virtual void callback (int p)
    {
        callback_1i(p);
    }

    /**
    * @brief Callback with 1 integer
    */
    virtual void callback (float p)
    {
        callback_1f(p);
    }

	/**
	 * @brief Shows element
	 */
	void show(void) { visible = true; }

	/**
	 * @brief Hides element
	 */
	void hide(void) { visible = false; }

	/**
	 * @brief Toggles show/hide flag
	 */
	void toggleDisplay(void) { visible = !visible;}

	/**
	 * @brief Return whether element is visible or not
	 * @return True if visible, false otherwise
	 */
	bool isVisible (void) { return visible; }

};
}
}
#endif
