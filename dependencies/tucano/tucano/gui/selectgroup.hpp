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

#ifndef __SELECTGROUP__
#define __SELECTGROUP__

#include "tucano/gui/element.hpp"
#include <Eigen/Dense>

namespace Tucano
{

namespace GUI
{

/**
 * @brief The gui select group is a placeholder for many selection buttons, so only one can be active at a time
 * Note, to visually group the buttons, you can use a groupbox and insert the selectgroup into the groupbox
 */
class SelectGroup : public Tucano::GUI::Element {

protected:

    /// Vector containing select buttons inside box
    vector < Tucano::GUI::SelectButton* > buttons;

	/// ID of currently selected button, -1 if no button is selected
	int selected = -1;

	// position of bottom right corner
	Eigen::Vector2i end_position;

public:

    /**
     * @brief Default constructor.
     */
    SelectGroup (void)
    {
        element_type = Tucano::GUI::SELECTGROUP;
        dimensions << 0, 0;
        position << 0, 0;
        end_position << 0, 0;
    }

    /**
     * @brief Adds an element to the gui
	 * The dimensions of the select group adjusts to fit all included select buttons
     */
    virtual void add (SelectButton *el)
    {
        buttons.push_back(el);

		// redimension select group to contain all buttons
		if (el->position[0] < position[0] || buttons.size() == 1)
			position[0] = el->position[0];
		if (el->position[1] < position[1] || buttons.size() == 1)
			position[1] = el->position[1];

		if (el->position[0] + el->dimensions[0] > end_position[0])
			end_position[0] = el->position[0] + el->dimensions[0];
		if (el->position[1] + el->dimensions[1] > end_position[1])
			end_position[1] = el->position[1] + el->dimensions[1];

		dimensions = end_position - position;
	}

	/**
	 * @brief Returns this element type, SELECTGROUP
	 */
    int getType (void)
    {
        return Tucano::GUI::SELECTGROUP;
    }

	/**
	 * @brief Select first element of group and makes sure all others are unselected
	 */
	void resetSelection (void)
	{
		if (buttons.empty())
			return;

	    for (unsigned int i = 1; i < buttons.size(); ++i)
		{
			if (buttons[i]->is_selected)
			{
				buttons[i]->clicked();
			}
		}
		if (!buttons[0]->is_selected)
			buttons[0]->clicked();

		selected = 0;
	}
 
	/**
	 * @brief Check if any element inside select group was clicked
	 * @param x X clicked coordinate
	 * @param y Y clicked coordinate
     * @return Pointer to clicked button
	 */
	SelectButton* checkClicked (int x, int y)
	{
	    for (unsigned int i = 0; i < buttons.size(); ++i)
        {
            if (buttons[i]->isVisible() && buttons[i]->isInside(x, y))
            {
				if (selected != -1)
				{
					buttons[selected]->clicked();
				}
				buttons[i]->clicked();
				selected = i;
				return buttons[i];
			}
        }
		return nullptr;
	}

	/**
	 * @brief Check if any element inside select group is on hover mode
	 * @param x X clicked coordinate
	 * @param y Y clicked coordinate
	 * @return Pointer to element in hovering mode
	 */
    SelectButton* checkHovering (int x, int y)
	{
	    for (unsigned int i = 0; i < buttons.size(); ++i)
        {
            if (buttons[i]->isVisible() && buttons[i]->isInside(x, y))
            {
				buttons[i]->hovering();
				return buttons[i];
			}
        }
		return nullptr;
	}


	/**
	 * @brief Renders all elements inside group
	 * @param camera_2d GUI 2D camera
	 * @param shader A custom shader for renderin groupbox and elements
	 */
    void render ( Tucano::Camera &camera_2d, Shader &shader )
    {        
		for (unsigned int i = 0; i < buttons.size(); ++i)
        {
            buttons[i]->render(camera_2d, shader);
        }
 
    }    

};
}
}
#endif
