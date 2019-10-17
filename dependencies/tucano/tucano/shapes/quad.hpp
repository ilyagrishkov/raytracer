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

#ifndef __QUAD__
#define __QUAD__

#include "tucano/mesh.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{

/// Default fragment shader for rendering quad
const string quad_fragment_code = "\n"
        "#version 430\n"
        "in vec4 color;\n"
        "out vec4 out_Color;\n"
        "void main(void)\n"
        "{\n"
		"	out_Color = color;\n"
        "}\n";

/// Default vertex shader for rendering quad
const string quad_vertex_code = "\n"
        "#version 430\n"
		"in vec4 in_Position;\n"
        "out vec4 color;\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform vec4 in_Color;\n"
        "void main(void)\n"
        "{\n"
		"   mat4 modelViewMatrix = viewMatrix * modelMatrix;\n"
        "   gl_Position = projectionMatrix * modelViewMatrix * in_Position;\n"
        "   color = in_Color;\n"
        "}\n";


/**
 * @brief A simple unitary quad
 **/
class Quad : public Tucano::Mesh {

private:

	/// Shader to render sphere
	Tucano::Shader quad_shader;

	/// Sphere color
	Eigen::Vector4f color;


public:

	/**
	* @brief Default Constructor
	*/
	Quad(void)
	{
		resetModelMatrix();
		createGeometry();

		color << 1.0, 0.48, 0.16, 1.0;

		quad_shader.setShaderName("quadShader");
		quad_shader.initializeFromStrings(quad_vertex_code, quad_fragment_code);

	}

    ///Default destructor.
    ~Quad() 
	{}

	/**
	* @brief Sets the quad color
	* @param c New color
	*/
	void setColor (const Eigen::Vector4f &c)
	{
		color = c;
	}

	/**
	* @brief Render quad
	*/
	void render (const Tucano::Camera& camera, const Tucano::Camera& light)
	{
		Eigen::Vector4f viewport = camera.getViewport();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		quad_shader.bind();

       	quad_shader.setUniform("modelMatrix", model_matrix);
		quad_shader.setUniform("viewMatrix", camera.getViewMatrix());
       	quad_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
		quad_shader.setUniform("lightViewMatrix", light.getViewMatrix());
       	quad_shader.setUniform("in_Color", color);

		vector <string> attribs;
		quad_shader.getActiveAttributes(attribs);

 		this->setAttributeLocation(&quad_shader);

		glEnable(GL_DEPTH_TEST);
		this->bindBuffers();
		this->renderElements();
		this->unbindBuffers();
		glDisable(GL_DEPTH_TEST);

       	quad_shader.unbind();

		#ifdef TUCANODEBUG
		Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
		
	}

private:

    /**
     * @brief Creates the quad geometry with associated tex coords
     */
    void createGeometry (void)
    {
        vector<Eigen::Vector4f> vert;
        vector<Eigen::Vector2f> texCoord;
        vector<GLuint> elementsVertices;

        vert.push_back ( Eigen::Vector4f( -0.5, -0.5, 0.0, 1.0) );
        vert.push_back ( Eigen::Vector4f(  0.5, -0.5, 0.0, 1.0) );
        vert.push_back ( Eigen::Vector4f(  0.5,  0.5, 0.0, 1.0) );
        vert.push_back ( Eigen::Vector4f( -0.5,  0.5, 0.0, 1.0) );

        elementsVertices.push_back(0);
        elementsVertices.push_back(1);
        elementsVertices.push_back(2);
        elementsVertices.push_back(2);
        elementsVertices.push_back(3);
        elementsVertices.push_back(0);

        texCoord.push_back ( Eigen::Vector2f(0.0, 0.0) );
        texCoord.push_back ( Eigen::Vector2f(1.0, 0.0) );
        texCoord.push_back ( Eigen::Vector2f(1.0, 1.0) );
        texCoord.push_back ( Eigen::Vector2f(0.0, 1.0) );

        loadVertices(vert);
        loadTexCoords(texCoord);
        loadIndices(elementsVertices);

        setDefaultAttribLocations();
    }

};
}
}
#endif
