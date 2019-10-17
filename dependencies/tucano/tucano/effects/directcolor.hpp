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

#ifndef __DIRECTCOLOR__
#define __DIRECTCOLOR__

#include "tucano/tucano.hpp"
#include "tucano/camera.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief Renders a mesh without illumination, using directly vertex color.
 */
class DirectColor : public Tucano::Effect
{

private:

    /// Phong Shader
    Tucano::Shader directcolor_shader;

	/// Default color
	Eigen::Vector4f default_color = Eigen::Vector4f(0.7, 0.7, 0.7, 1.0);

public:

    /**
     * @brief Default constructor.
     */
    DirectColor (void)
    {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files directcolor.(vert,frag,geom,comp)
        loadShader(directcolor_shader, "directcolor") ;
    }

	/**
	* @brief Sets the default color, usually used for meshes without color attribute
	*/
	void setDefaultColor ( Eigen::Vector4f& color )
	{
		default_color = color;
	}

    /** * @brief Render the mesh given a camera 
     * @param mesh Given mesh
     * @param camera Given camera
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera)
    {

        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        directcolor_shader.bind();

        // sets all uniform variables for the phong shader
        directcolor_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        directcolor_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        directcolor_shader.setUniform("viewMatrix", camera.getViewMatrix());
        directcolor_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));
		directcolor_shader.setUniform("default_color", default_color);

        mesh.setAttributeLocation(directcolor_shader);

        glEnable(GL_DEPTH_TEST);
        mesh.render();

        directcolor_shader.unbind();
    }

};
}
}


#endif
