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

#ifndef __PLANE__
#define __PLANE__

#include "tucano/mesh.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{

/// Default fragment shader for rendering plane
const string plane_fragment_code = "\n"
        "#version 430\n"
        "in vec4 color;\n"
		"in vec3 normal;\n"
		"in vec4 vert;\n"
        "out vec4 out_Color;\n"
		"uniform mat4 lightViewMatrix;\n"
		"uniform mat4 viewMatrix;\n"
        "void main(void)\n"
        "{\n"
        "   vec3 normvec = normalize(normal);\n"
		"   vec3 eyeDirection = -normalize(vert.xyz);\n"
		"   vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;\n"
		"   lightDirection = normalize(lightDirection);\n"
		"   vec3 lightReflection = reflect(-lightDirection, normvec);\n"
		"   float shininess = 100.0;\n"
		"	vec4 ambientLight = color * 0.4;\n"
		"	vec4 diffuseLight = color * 0.6 * max(dot(lightDirection, normvec),0.0);\n"
		"	vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);\n"
		"	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, color.w);\n"
        "}\n";

/// Default vertex shader for rendering plane
const string plane_vertex_code = "\n"
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
 * @brief A simple plane with bounded limits
 **/
class Plane : public Tucano::Mesh {

private:

	// Shader to render splane
	Tucano::Shader plane_shader;

    /// Plane width
    float width;

    /// Plane height
    float height;

public:

	/**
	* @brief Default Constructor
	*/
	Plane(float w = 1.0, float h = 1.0)
	{
		setSize(w,h);
		resetModelMatrix();
		createGeometry();

		default_color << 1.0, 0.48, 0.16, 1.0;

		plane_shader.setShaderName("planeShader");
		plane_shader.initializeFromStrings(plane_vertex_code, plane_fragment_code);

        #ifdef TUCANODEBUG
        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "plane constructor"); 
        #endif
	}

	/**
	 * @brief Sets cylinder size by scaling the size matrix
	 */
    void setSize (float w, float h)
    {
        width = w;
        height = h;
		resetShapeMatrix();
		shape_matrix.scale (Eigen::Vector3f(width, height, 1.0));
    }

	/**
	* @brief Render plane
	*/
	void render (const Tucano::Camera& camera, const Tucano::Camera& light)
	{
		Eigen::Vector4f viewport = camera.getViewport();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		plane_shader.bind();

       	plane_shader.setUniform("modelMatrix", getShapeModelMatrix());
		plane_shader.setUniform("viewMatrix", camera.getViewMatrix());
       	plane_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
		plane_shader.setUniform("lightViewMatrix", light.getViewMatrix());
       	plane_shader.setUniform("in_Color", default_color);

 		this->setAttributeLocation(&plane_shader);

		this->bindBuffers();
		this->renderElements();
		this->unbindBuffers();

       	plane_shader.unbind();

		#ifdef TUCANODEBUG
		Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
	}

private:


	/**
	* @brief Define plane geometry
	*
	*/
	void createGeometry (void)
	{
		vector< Eigen::Vector4f > vert;
		vector< Eigen::Vector3f > normals;
        vector<Eigen::Vector2f> texcoord;
		vector< GLuint > faces;
	
		vert.push_back ( Eigen::Vector4f( -0.5, -0.5, 0.0, 1.0) );
		vert.push_back ( Eigen::Vector4f(  0.5, -0.5, 0.0, 1.0) );
		vert.push_back ( Eigen::Vector4f(  0.5,  0.5, 0.0, 1.0) );
		vert.push_back ( Eigen::Vector4f( -0.5,  0.5, 0.0, 1.0) );
        
        texcoord.push_back ( Eigen::Vector2f(0.0, 0.0) );
        texcoord.push_back ( Eigen::Vector2f(1.0, 0.0) );
        texcoord.push_back ( Eigen::Vector2f(1.0, 1.0) );
        texcoord.push_back ( Eigen::Vector2f(0.0, 1.0) );

        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );

		int a[6] = { 0, 1, 2, 2, 3, 0};
		faces.insert(faces.end(), a, a+6);
	
		loadVertices(vert);
		loadNormals(normals);
        loadTexCoords(texcoord);
		loadIndices(faces);

		setDefaultAttribLocations();
		
	}

};
}
}
#endif
