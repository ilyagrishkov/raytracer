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

#ifndef __COORDINATEAXES__
#define __COORDINATEAXES__

#include "tucano/mesh.hpp"
#include "tucano/shapes/arrow.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{


/**
 * @brief Visual representation of a 3D coordinate axes 
 *
 * The actual geometry is handled by the arrow meshes	
 *
 **/
class CoordinateAxes : public Tucano::Model {

private:

	Tucano::Shapes::Arrow arrow;

public:

	/**
	* @brief Default Constructor
	*/
	CoordinateAxes()
	{
		resetModelMatrix();
        arrow = Arrow(0.05, 0.8, 0.12, 0.2);
	}

	/**
	* @brief Render camera representation
    * @todo create 3D representation of axes (tubes) and use light
	*/
	void render (const Tucano::Camera &camera, const Tucano::Camera &light)
	{
        glEnable(GL_DEPTH_TEST);

		arrow.resetModelMatrix();
		arrow.modelMatrix()->scale(0.2);
		arrow.modelMatrix()->rotate(this->modelMatrix()->rotation());

		arrow.setColor(Eigen::Vector4f(0.0, 0.0, 1.0, 1.0));
		arrow.render(camera, light);

		arrow.modelMatrix()->rotate(Eigen::AngleAxisf(-M_PI*0.5, Eigen::Vector3f::UnitX()));
		arrow.setColor(Eigen::Vector4f(0.0, 1.0, 0.0, 1.0));
		arrow.render(camera, light);

		arrow.modelMatrix()->rotate(Eigen::AngleAxisf(M_PI*0.5, Eigen::Vector3f::UnitX()));
		arrow.modelMatrix()->rotate(Eigen::AngleAxisf(M_PI*0.5, Eigen::Vector3f::UnitY()));
		arrow.setColor(Eigen::Vector4f(1.0, 0.0, 0.0, 1.0));
		arrow.render(camera, light);
	}

};
}
}
#endif
