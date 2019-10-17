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

#ifndef __WIREFRAME__
#define __WIREFRAME__

#include "tucano/effect.hpp"
#include "tucano/camera.hpp"
#include "tucano/mesh.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief Renders a mesh with wireframe edges and flat faces in a single pass
 */
class Wireframe : public Tucano::Effect
{

private:

    /// Phong Shader
    Tucano::Shader wireframe_shader;

	/// Default color
	Eigen::Vector4f line_color = Eigen::Vector4f (0.0, 0.0, 0.0, 1.0);

    /// Edge thickness
    float thickness = 0.05;

    /// Flag to draw faces
    bool draw_faces = true;

public:

    /**
     * @brief Default constructor.
     */
    Wireframe (void)
    {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files wireframeShader.(vert,frag,geom,comp)
        loadShader(wireframe_shader, "wireframe") ;
    }

	/**
	* @brief Sets the line color
	*/
	void setLineColor ( const Eigen::Vector4f& color )
	{
		line_color = color;
	}

    /**
    * @brief Set shininess exponent
    * @param New shininess coeff (alpha)
    */
    void setEdgeThickness (float value)
    {
        thickness = value;
    }


    /** 
     * @brief Render the mesh given a camera and light, using a Wireframe shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {

        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        wireframe_shader.bind();

        // sets all uniform variables for the wireframe shader
        wireframe_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        wireframe_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        wireframe_shader.setUniform("viewMatrix", camera.getViewMatrix());
        wireframe_shader.setUniform("lightViewMatrix", lightTrackball.getViewMatrix());
        wireframe_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));
		wireframe_shader.setUniform("default_color", mesh.getColor());
		wireframe_shader.setUniform("line_color", line_color);
        wireframe_shader.setUniform("thickness", thickness);

        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
        mesh.setAttributeLocation(wireframe_shader);

        mesh.render();

        wireframe_shader.unbind();
    }


};
}
}


#endif
