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

#ifndef __ARROW__
#define __ARROW__

#include <tucano/mesh.hpp>
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{

/// Default fragment shader for rendering arrow 
const string arrow_fragment_code = "\n"
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
		"	vec4 specularLight = vec4(1.0) *  pow( max( dot(lightReflection, eyeDirection), 0.0), shininess);\n"
		"	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, color.w);\n"
        "}\n";

/// Default vertex shader for rendering arrow
const string arrow_vertex_code = "\n"
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
 * @brief A rounded arrow shape defined by a arrow and a cone
 *
 **/
class Arrow : public Tucano::Mesh {

private:

	/// Shader to render arrow 
	Tucano::Shader arrow_shader;

	/// Arrow color
	Eigen::Vector4f color;

	/// Height of body
	float body_height;
	
	/// Height of head
	float head_height;

	/// Body radius
	float body_radius;

	/// Head radius
	float head_radius;

public:

	/**
	* @brief Default Constructor
	*/
	Arrow(float rcyl = 0.05, float hcyl = 1.0, float rcon = 0.1, float hcon = 0.15, int subs = 32 )
	{
		resetModelMatrix();

		body_height = hcyl;
		head_height = hcon;
		body_radius = rcyl;
		head_radius = rcon;
		createGeometry(subs);

		setColor(Eigen::Vector4f(0.0, 0.7, 0.7, 1.0));

		arrow_shader.setShaderName("arrowShader");
		arrow_shader.initializeFromStrings(arrow_vertex_code, arrow_fragment_code);
	}

	/**
	 * @brief Sets the size of the arrow by scaling the shape matrix
	 */
    void setSize (float s)
    {
		resetShapeMatrix();
		shape_matrix.scale (s);
    }

	/**
	* @brief Sets the arrow color
	*/
	void setColor (const Eigen::Vector4f c)
	{
		color = c;
	}

	/**
	* @brief Render arrow
	*/
	void render (const Tucano::Camera &camera, const Tucano::Camera &light)
	{
		Eigen::Vector4f viewport = camera.getViewport();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		arrow_shader.bind();

       	arrow_shader.setUniform("modelMatrix", model_matrix);
		arrow_shader.setUniform("viewMatrix", camera.getViewMatrix());
       	arrow_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
		arrow_shader.setUniform("lightViewMatrix", light.getViewMatrix());
       	arrow_shader.setUniform("in_Color", color);

 		this->setAttributeLocation(&arrow_shader);

		glEnable(GL_DEPTH_TEST);
		this->bindBuffers();
		this->renderElements();
		this->unbindBuffers();

       	arrow_shader.unbind();

		#ifdef TUCANODEBUG
		Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
	}

private:

	/**
	* @brief Define arrow geometry
	*
	* @param subdivisions Number of subdivisons for cap and body
	*/
	void createGeometry (int subdivisions)
	{
		vector< Eigen::Vector4f > vert;
		vector< Eigen::Vector3f > norm;
		vector< GLuint > faces;

		float x, y, theta;
		// create vertices for top and bottom caps
		for (int i = 0; i < subdivisions; ++i)
		{
			theta = 2.0*M_PI*i/(float)subdivisions;
			x = sin(theta)*body_radius;
			y = cos(theta)*body_radius;
			vert.push_back(Eigen::Vector4f(x, y, body_height, 1.0) );
			vert.push_back(Eigen::Vector4f(x, y, 0.0, 1.0));
			norm.push_back(Eigen::Vector3f(x, y, 0.0));
			norm.push_back(Eigen::Vector3f(x, y, 0.0));
		}
		
		// create a face with every three vertices for arrow body
		for (int i = 0; i < subdivisions*2; ++i)
		{
			faces.push_back(i);
			faces.push_back((i+1)%(subdivisions*2));
			faces.push_back((i+2)%(subdivisions*2));
		}

		// create bottom cap
		vert.push_back(Eigen::Vector4f(0.0, 0.0, 0.0, 1.0));
		norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
		int center_index = vert.size()-1;
		int offset = vert.size();
		for (int i = 0; i < subdivisions; ++i)
		{
			theta = 2.0*M_PI*i/(float)subdivisions;
			x = sin(theta)*body_radius;
			y = cos(theta)*body_radius;
			vert.push_back(Eigen::Vector4f(x, y, 0.0, 1.0));
			norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
		}

		for (int i = 0; i < subdivisions; ++i)
		{
			faces.push_back(i+offset);
			faces.push_back((i+1)%(subdivisions) + offset);
			faces.push_back(center_index);
		}

		// create vertices for arrow head 
		offset = vert.size();
		for (int i = 0; i < subdivisions; ++i)
		{
			theta = 2.0*M_PI*i/(float)subdivisions;
			x = sin(theta)*head_radius;
			y = cos(theta)*head_radius;
			vert.push_back(Eigen::Vector4f(x, y, body_height, 1.0));
			norm.push_back(Eigen::Vector3f(x, y, 0.0));
            }

		// apex vertex
		vert.push_back(Eigen::Vector4f(0.0, 0.0, body_height+head_height, 1.0));
		norm.push_back(Eigen::Vector3f(0.0, 0.0, 1.0));

	
		// create a face with every two vertices and apex
		for (int i = 0; i < subdivisions; ++i)
		{
			faces.push_back(i+offset);
			faces.push_back((i+1)%(subdivisions) + offset);
			faces.push_back(vert.size()-1);
		}

		// create head cap
		vert.push_back(Eigen::Vector4f(0.0, 0.0, body_height, 1.0));
		norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
		center_index = vert.size()-1;
		offset = vert.size();
		for (int i = 0; i < subdivisions; ++i)
		{
			theta = 2.0*M_PI*i/(float)subdivisions;
			x = sin(theta)*head_radius;
			y = cos(theta)*head_radius;
			vert.push_back(Eigen::Vector4f(x, y, body_height, 1.0));
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
