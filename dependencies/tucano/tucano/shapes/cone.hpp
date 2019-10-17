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

#ifndef __CONE__
#define __CONE__

#include "tucano/mesh.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{

/// Default fragment shader for rendering cone 
const string cone_fragment_code = "\n"
        "#version 430\n"
        "in vec4 color;\n"
		"in vec3 normal;\n"
		"in vec4 vert;\n"
        "out vec4 out_Color;\n"
		"uniform mat4 lightViewMatrix;\n"
		"uniform mat4 viewMatrix;\n"
        "void main(void)\n"
        "{\n"
        "	vec3 normvec = normalize(normal);\n"
		"   vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;\n"
		"   lightDirection = normalize(lightDirection);\n"
		"   vec3 lightReflection = reflect(-lightDirection, normvec);\n"
		"   vec3 eyeDirection = -normalize(vert.xyz);\n"
		"   float shininess = 100.0;\n"
		"	vec4 ambientLight = color * 0.4;\n"
		"	vec4 diffuseLight = color * 0.6 * max(dot(lightDirection, normvec),0.0);\n"
		"	vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);\n"
		"	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, color.w);\n"
        "}\n";

/// Default vertex shader for rendering cone 
const string cone_vertex_code = "\n"
        "#version 430\n"
		"in vec4 in_Position;\n"
		"in vec4 in_Normal;\n"
        "out vec4 color;\n"
		"out vec3 normal;\n"
		"out vec4 vert;\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform vec4 in_Color;\n"
        "void main(void)\n"
        "{\n"
		"   mat4 modelViewMatrix = viewMatrix * modelMatrix;\n"
		"   mat4 normalMatrix = transpose(inverse(modelViewMatrix));\n"
		"   normal = normalize(vec3(normalMatrix * vec4(in_Normal.xyz,0.0)).xyz);\n"
		"   vert = modelViewMatrix * in_Position;\n"
        "   gl_Position = projectionMatrix * modelViewMatrix * in_Position;\n"
        "   color = in_Color;\n"
        "}\n";


/**
 * @brief A simple cone shape
 **/
class Cone : public Tucano::Mesh {

private:

	/// Shader to render cone 
	Tucano::Shader cone_shader;

	/// Cone color
	Eigen::Vector4f color;

	/// Cone cone_height
	float cone_height;

	/// Cone cone_radius
	float cone_radius;

public:

	/**
	* @brief Default Constructor
	*/
	Cone()
	{
		resetModelMatrix();
		create(0.1, 0.5);

		color << 0.0, 0.48, 1.0, 1.0;

		cone_shader.setShaderName("coneShader");
		cone_shader.initializeFromStrings(cone_vertex_code, cone_fragment_code);

	}

    ///Default destructor.
    ~Cone() 
	{}


	/**
	* @brief Sets the cone color
	* @param c New color
	*/
	void setColor (const Eigen::Vector4f &c)
	{
		color = c;
	}

	/**
	* @brief Render cone
	*/
	void render (const Tucano::Camera& camera, const Tucano::Camera& light)
	{
		Eigen::Vector4f viewport = camera.getViewport();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		cone_shader.bind();

       	cone_shader.setUniform("modelMatrix", model_matrix);
		cone_shader.setUniform("viewMatrix", camera.getViewMatrix());
       	cone_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
		cone_shader.setUniform("lightViewMatrix", light.getViewMatrix());
       	cone_shader.setUniform("in_Color", color);

 		this->setAttributeLocation(&cone_shader);

		glEnable(GL_DEPTH_TEST);
		this->bindBuffers();
		this->renderElements();
		this->unbindBuffers();
		glDisable(GL_DEPTH_TEST);

       	cone_shader.unbind();

		#ifdef TUCANODEBUG
		Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
		
	}

	/**
	* @brief Create cone with given parameters
	* @param r Radius
	* @param h Height
	* @param s Number of subdivisions
	*/
	void create (float r, float h, int s = 32)
	{
		cone_radius = r;
		cone_height = h;
		createGeometry(s);
	}

	/**
	* @brief Returns cone cone_height
	*/
	float getHeight (void)
	{
		return cone_height;
	}

	/**
	* @brief Returns cone cone_radius 
	*/
	float getRadius (void)
	{
		return cone_radius;
	}


private:


	/**
	* @brief Define cone geometry
	*
	* Cone is created by creating one disk (cap) and a vertex, and generating triangles
	* between them 
	*
	* @param subdivisions Number of subdivisons for cap and body
	*/
	void createGeometry (int subdivisions)
	{
		reset();

		vector< Eigen::Vector4f > vert;
		vector< Eigen::Vector3f > norm;
		vector< GLuint > faces;

		float x, y, theta;
		// create vertices for body
		for (int i = 0; i < subdivisions; ++i)
		{
			theta = 2.0*M_PI*i/(float)subdivisions;
			x = sin(theta)*cone_radius;
			y = cos(theta)*cone_radius;
			vert.push_back(Eigen::Vector4f(x, y, 0.0, 1.0));
			norm.push_back(Eigen::Vector3f(x, y, 0.0));
		}

		// apex vertex
		vert.push_back(Eigen::Vector4f(0.0, 0.0, cone_height, 1.0));
		norm.push_back(Eigen::Vector3f(0.0, 0.0, 1.0));

		// create a face with every two vertices and apex
		for (int i = 0; i < subdivisions; ++i)
		{
			faces.push_back(i);
			faces.push_back((i+1)%(subdivisions));
			faces.push_back(vert.size()-1);
		}

		// create cap
		vert.push_back(Eigen::Vector4f(0.0, 0.0, 0.0, 1.0));
		norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
		int center_index = vert.size()-1;
		int offset = vert.size();
		for (int i = 0; i < subdivisions; ++i)
		{
			theta = 2.0*M_PI*i/(float)subdivisions;
			x = sin(theta)*cone_radius;
			y = cos(theta)*cone_radius;
			vert.push_back(Eigen::Vector4f(x, y, 0.0, 1.0));
			norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
		}

		for (int i = 0; i < subdivisions; ++i)
		{
			faces.push_back(i+offset);
			faces.push_back((i+1)%(subdivisions) + offset);
			faces.push_back(center_index);
		}


		loadVertices(vert);
		loadNormals(norm);
		loadIndices(faces);

		setDefaultAttribLocations();
		
	}

};
}
}
#endif
