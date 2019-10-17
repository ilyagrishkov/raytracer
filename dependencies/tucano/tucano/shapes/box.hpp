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

#ifndef __BOX__
#define __BOX__

#include "tucano/mesh.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{

/// Default fragment shader for rendering box
const string box_fragment_code = "\n"
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
        "   if (dot(normal, eyeDirection) < 0.0) discard;\n"
		"   vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;\n"
		"   lightDirection = normalize(lightDirection);\n"
		"   vec3 lightReflection = reflect(-lightDirection, normvec);\n"
		"   float shininess = 100.0;\n"
		"	vec4 ambientLight = color * 0.4;\n"
		"	vec4 diffuseLight = color * 0.6 * max(dot(lightDirection, normvec),0.0);\n"
		"	vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);\n"
		"	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, color.w);\n"
        "}\n";

/// Default vertex shader for rendering box
const string box_vertex_code = "\n"
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
 * @brief A simple box with bounded limits
 **/
class Box : public Tucano::Mesh {

private:

	// Shader to render sbox
	Tucano::Shader box_shader;

    /// Box width
    float width;

    /// Box height
    float height;

    /// Box depth
    float depth;

public:

	vector<Eigen::Vector4f> vertices;
	/**
	* @brief Default Constructor
	*/
	Box(float w = 1.0, float h = 1.0, float d = 1.0) : width(w), height(h), depth(d)
	{
		resetModelMatrix();
		createGeometry();

		default_color << 1.0, 0.48, 0.16, 1.0;

		box_shader.setShaderName("boxShader");
		box_shader.initializeFromStrings(box_vertex_code, box_fragment_code);

        #ifdef TUCANODEBUG
        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "box constructor"); 
        #endif
	}

	/**
	* @brief Render box
	*/
	void render (const Tucano::Camera& camera, const Tucano::Camera& light)
	{
		Eigen::Vector4f viewport = camera.getViewport();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		box_shader.bind();

       	box_shader.setUniform("modelMatrix", model_matrix);
		box_shader.setUniform("viewMatrix", camera.getViewMatrix());
       	box_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
		box_shader.setUniform("lightViewMatrix", light.getViewMatrix());
       	box_shader.setUniform("in_Color", default_color);

 		this->setAttributeLocation(&box_shader);

		this->bindBuffers();
		this->renderElements();
		this->unbindBuffers();

       	box_shader.unbind();

		#ifdef TUCANODEBUG
		Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
	}

private:


	/**
	* @brief Define box geometry
	*
	*/
	void createGeometry (void)
	{
		vector< Eigen::Vector4f > vert;
		vector< Eigen::Vector3f > normals;
		vector< GLuint > faces;
	

		vertices.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f(  width*0.5,  height*0.5, -depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f( -width*0.5,  height*0.5, -depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5, depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5, depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f(  width*0.5,  height*0.5, depth*0.5, 1.0) );
		vertices.push_back ( Eigen::Vector4f( -width*0.5,  height*0.5, depth*0.5, 1.0) );

        // front
		vert.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5,  height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5,  height*0.5, -depth*0.5, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, -1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, -1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, -1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, -1.0) );

        // back
		vert.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5, depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5, depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5,  height*0.5, depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5,  height*0.5, depth*0.5, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 0.0, 1.0) );

        // bottom
		vert.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5,  depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5,  depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5, -depth*0.5, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, -1.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (0.0, -1.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (0.0, -1.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (0.0, -1.0, 0.0) );

        // top
		vert.push_back ( Eigen::Vector4f( -width*0.5, height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, height*0.5,  depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5, height*0.5,  depth*0.5, 1.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 1.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 1.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 1.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (0.0, 1.0, 0.0) );

        // right
		vert.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5, -height*0.5,  depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5,  height*0.5,  depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(  width*0.5,  height*0.5, -depth*0.5, 1.0) );
        normals.push_back ( Eigen::Vector3f (1.0, 0.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (1.0, 0.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (1.0, 0.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (1.0, 0.0, 0.0) );

        // left
		vert.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5,  height*0.5, -depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5,  height*0.5,  depth*0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( -width*0.5, -height*0.5,  depth*0.5, 1.0) );
        normals.push_back ( Eigen::Vector3f (-1.0, 0.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (-1.0, 0.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (-1.0, 0.0, 0.0) );
        normals.push_back ( Eigen::Vector3f (-1.0, 0.0, 0.0) );

		int a[36] = { 0, 1, 2, 2, 3, 0,
                     4, 5, 6, 6, 7, 4,
                     8, 9, 10, 10, 11, 8,
                    12, 13, 14, 14, 15, 12,
                    16, 17, 18, 18, 19, 16,
                    20, 21, 22, 22, 23, 20};
        faces.insert(faces.end(), a, a+36);
	
		loadVertices(vert);
		loadNormals(normals);
		loadIndices(faces);

		setDefaultAttribLocations();
		
	}

};
}
}
#endif
