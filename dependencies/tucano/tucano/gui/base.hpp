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

#ifndef __GUIBASE__
#define __GUIBASE__

#include "tucano/camera.hpp"
#include "tucano/gui/guishaders.hpp"
#include "tucano/gui/button.hpp"
#include "tucano/gui/selectbutton.hpp"
#include "tucano/gui/slider.hpp"
#include "tucano/gui/label.hpp"
#include "tucano/gui/groupbox.hpp"
#include "tucano/gui/selectgroup.hpp"
#include <functional>

namespace Tucano
{

namespace GUI
{

/**
 * @brief Base class for GUI, contains all elements and handles viewport and rendering transformations
 */
class Base {

protected:

    /// Viewport size
    Eigen::Vector2i viewport_size;

    /// 2D camera
    Tucano::Camera camera_2d;

    /// Shader for rendering gui elements
    Tucano::Shader gui_shader;

    /// Vector containing gui elements
    vector < Tucano::GUI::Element* > elements;

    /// Last clicked element
    Tucano::GUI::Element* last_clicked = NULL;

    /// Last hovering element
    Tucano::GUI::Element* last_hovering = NULL;
public:

    /**
     * @brief Default constructor.
     */
    Base (void) 
    {
        gui_shader.setShaderName("gui_shader");
        gui_shader.initializeFromStrings(gui_vertex_code, gui_fragment_code);
    }

    /**
     * @brief Adds an element to the gui
     */
    void add (Element *el)
    {
        elements.push_back(el);
    }

    /**
     * @brief Set viewport size
     * @param vs Viewport size
     */
    void setViewportSize (const Eigen::Vector2i & vs)
    {
        camera_2d.reset();
        camera_2d.setOrthographicMatrix(0.0, (float)vs[0], (float)vs[1], 0.0, 0.0, 1.0);
    }

     /**
     * @brief Set viewport size
     * @param w Viewport width
     * @param h Viewport height
     */
    void setViewportSize (int w, int h)
    {
        setViewportSize (Eigen::Vector2i (w, h) );
    }

    /**
     * @brief renders the 2D gui interface
     */
    void render (void)
    {
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            elements[i]->render(camera_2d, gui_shader);
        }
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }
   
	/**
	 * @brief Check if any element in a given list is inside clicked coordinates
	 * @param x X clicked coordinate
	 * @param y Y clicked coordinate
	 * @param list Element list
	 * @return True if any element was clicked, false otherwise
	 */
	bool checkClicked (int x, int y, vector <Tucano::GUI::Element *> * list)
	{
	    for (unsigned int i = 0; i < list->size(); ++i)
        {
			Tucano::GUI::Element* element = list->at(i);
            if (element->isVisible() && element->isInside(x, y))
            {
                last_clicked = element;
				// if element was a groupbox, check elements inside group
				if (element->getType() == Tucano::GUI::GROUPBOX)
				{
					return checkClicked (x , y, ((Tucano::GUI::GroupBox*)element)->elementList());
				}

				// if element is a select group call internal routine for checking selection
				if (element->getType() == Tucano::GUI::SELECTGROUP)
				{				
					if ( static_cast <Tucano::GUI::SelectGroup*> (element)->checkClicked(x, y) == nullptr )
                        return false;
                    return true;
				}
 
				// clicked element is a button
                if (element->getType() == Tucano::GUI::BUTTON)
                {
                    ((Tucano::GUI::Button*)element)->clicked();
                }

				// clicked element is a select button
	            if (element->getType() == Tucano::GUI::SELECTBUTTON)
                {
					static_cast <Tucano::GUI::SelectButton*> (element)->clicked();
                }
	            return true;
            }
        }
		return false;
	}

	/**
	 * @brief Check if any element in a given list is inside mouse coordinates (hovering)
	 * @param x X clicked coordinate
	 * @param y Y clicked coordinate
	 * @param list Element list
	 * @return True if any element is on hover mode, false otherwise
	 */
	bool checkHovering (int x, int y, vector <Tucano::GUI::Element *> * list)
	{
	    for (unsigned int i = 0; i < list->size(); ++i)
        {
			Tucano::GUI::Element* element = list->at(i);
            if (element->isVisible() && element->isInside(x, y))
            {
                if (last_hovering == element)
                    return true;

                // else unhover last_hovering

                last_hovering = element;
				// if element was a groupbox, check elements inside group
				if (element->getType() == Tucano::GUI::GROUPBOX)
				{
					return checkHovering (x , y, ((Tucano::GUI::GroupBox*)element)->elementList());
				}

				// if element is a select group call internal routine for checking selection
				if (element->getType() == Tucano::GUI::SELECTGROUP)
				{				
					SelectButton* sb = static_cast <Tucano::GUI::SelectGroup*> (element)->checkHovering(x, y);

                    if (sb == nullptr)
                        return false;

//                    if (last_hovering != sb)
  //                      static_cast <Tucano::GUI::SelectButton*> (last_hovering)->toggleHover()

                    last_hovering = sb;
                    return true;
				}
 
				// clicked element is a button
                if (element->getType() == Tucano::GUI::BUTTON)
                {
                    ((Tucano::GUI::Button*)element)->hovering();
                }

				// clicked element is a select button
	            if (element->getType() == Tucano::GUI::SELECTBUTTON)
                {
					static_cast <Tucano::GUI::SelectButton*> (element)->hovering();
                }
	            return true;
            }
        }
		return false;
	}


    /**
     * @brief Treats mouse left click callback for all elements
     * @param x Mouse x position
     * @param y Mouse y position
     * @return True if any gui element was clicked, false otherwise
     */
    bool leftButtonPressed (int x, int y)
    {
        last_clicked = NULL;
		return checkClicked (x, y, &elements);
    }

    /**
     * @brief Treats mouse left release callback for last clicked element
     * @param x Mouse x position
     * @param y Mouse y position
     * @return True if any element is selected, false otherwise
     */
    bool leftButtonReleased (int x, int y)
    {
        if (last_clicked)
        {
            last_clicked->release();
            last_clicked = 0;
            return true;
        }
        return false;
    }

     /**
     * @brief Treats mouse mouvement
     * @param x Mouse x position
     * @param y Mouse y position
     * @return True if any element is selected, false otherwise
     */
    bool cursorMove (int x, int y)
    {
        if (last_clicked)
        {
            last_clicked->cursorMove(x,y);
            if (last_clicked->getType() == Tucano::GUI::SLIDER)
            {
                ((Tucano::GUI::Slider*)last_clicked)->valueChanged();
            }
            return true;
        }
        return false;
    }

};
}
}
#endif
