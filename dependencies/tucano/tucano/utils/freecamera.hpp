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

#ifndef __FREECAMERA__
#define __FREECAMERA__

#include "tucano/utils/flycamera.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

/**
 * @brief Free camera class for manipulating a camera.
 *
 * This class simulates a free camera which apply transformations based on camera's frame, instead of world's frame.
 **/
class Freecamera : public Tucano::Flycamera {
protected:
	
	float rotation_Z_axis;

public:

    /**
     * @brief Resets camera to initial position and orientation
     */
    void reset (void) override
    {
		start_mouse_pos = Eigen::Vector2f::Zero();
		rotation_matrix = Eigen::Matrix3f::Identity();
		default_translation = Eigen::Vector3f (0.0, 0.0, -2.0);
		translation_vector = default_translation;
        rotation_X_axis = 0.0;
        rotation_Y_axis = 0.0;
		rotation_Z_axis = 0.0;
		updateViewMatrix();
    }

	/**
	 * @brief Compose rotation and translation
	 */
	void updateViewMatrix() override
	{
		Eigen::Vector3f xAxis = rotation_matrix.row(0);
		Eigen::Vector3f yAxis = rotation_matrix.row(1);
		Eigen::Vector3f zAxis = rotation_matrix.row(2);
		
		if( rotation_Z_axis )
		{
			Eigen::AngleAxisf transfRotZ = Eigen::AngleAxisf(rotation_Z_axis, zAxis);
			
			// compute X axis restricted to a rotation around Z axis
			xAxis = transfRotZ * xAxis;
			xAxis.normalize();
			
			// compute Y axis restricted to a rotation around Z axis
			yAxis = transfRotZ * yAxis;
			yAxis.normalize();
			
			rotation_Z_axis = 0.0;
		}
		
		Eigen::AngleAxisf transfRotY = Eigen::AngleAxisf(rotation_Y_axis, yAxis);
		
		// compute X axis restricted to a rotation around Y axis
		Eigen::Vector3f rotX = transfRotY * xAxis;
		rotX.normalize();

		Eigen::AngleAxisf transfRotX = Eigen::AngleAxisf(rotation_X_axis, rotX);
		
		// rotate Z axis around Y axis, then rotate new Z axis around X new axis
		Eigen::Vector3f rotZ = transfRotY * zAxis;
		rotZ = transfRotX * rotZ;
		rotZ.normalize();

		// rotate Y axis around X new axis
		Eigen::Vector3f rotY = transfRotX * yAxis;
		rotY.normalize();

		rotation_matrix.row(0) = rotX;
		rotation_matrix.row(1) = rotY;
		rotation_matrix.row(2) = rotZ;
		
		resetViewMatrix();
		view_matrix.rotate (rotation_matrix);
		view_matrix.translate (translation_vector);
		
		rotation_X_axis = 0.0;
		rotation_Y_axis = 0.0;
	}

    /**
     * @brief Translates the view matrix to the left.
     */
    void strideLeft ( void ) override
    {
		translation_vector += rotation_matrix.row(0) * speed;
    }

    /**
     * @brief Translates the view matrix to the right.
     */
    void strideRight ( void ) override
    {
		translation_vector -= rotation_matrix.row(0) * speed;
    }

    /**
     * @brief Translates the view matrix back.
     */
    void moveBack ( void ) override
    {
		translation_vector -= rotation_matrix.row(2) * speed;
    }

    /**
     * @brief Translates the view matrix forward.
     */
    void moveForward ( void ) override
    {
		translation_vector += rotation_matrix.row(2) * speed;
    }

    /**
     * @brief Translates the view matrix down.
     */
    void moveDown ( void ) override
    {
		translation_vector += rotation_matrix.row(1) * speed;
    }

    /**
     * @brief Translates the view matrix up.
     */
    void moveUp ( void ) override
    {
		translation_vector -= rotation_matrix.row(1) * speed;
	}

	/**
	 * @brief Rotates the camera view direction around X and Y axes.
	 * @param new_mouse_pos New mouse position
	 */
	void rotate ( Eigen::Vector2f new_mouse_pos ) override
	{
		Eigen::Vector2f new_position = normalizePosition(new_mouse_pos);
		Eigen::Vector2f dir2d = new_position - start_mouse_pos;
		
		start_mouse_pos = new_position;

		rotation_X_axis = dir2d[1]*M_PI;
		rotation_Y_axis = -dir2d[0]*M_PI;

        if (rotation_X_axis > 2*M_PI)
            rotation_X_axis -= 2*M_PI;
        if (rotation_X_axis < 0)
            rotation_X_axis += 2*M_PI;
        if (rotation_Y_axis > 2*M_PI)
            rotation_Y_axis -= 2*M_PI;
        if (rotation_Y_axis < 0)
            rotation_Y_axis += 2*M_PI;
	}
	
	/**
	 * @brief Rotates the camera view direction around Z axis.
	 * @param new_mouse_pos New mouse position
	 */
	void rotateZ ( Eigen::Vector2f new_mouse_pos ) override
	{
		Eigen::Vector2f new_position = normalizePosition(new_mouse_pos);
		Eigen::Vector2f dir2d = new_position - start_mouse_pos;
		
		start_mouse_pos = new_position;
		
		rotation_Z_axis = dir2d[1]*M_PI;
		
		if (rotation_Z_axis > 2*M_PI)
            rotation_Z_axis -= 2*M_PI;
        if (rotation_Z_axis < 0)
			rotation_X_axis += 2*M_PI;
	}
	
};

}

#endif
