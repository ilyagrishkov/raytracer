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

#ifndef __CAMERAREP__
#define __CAMERAREP__

#include "tucano/mesh.hpp"
#include "tucano/effects/phongshader.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{


/// Default fragment shader for rendering camerarep 
const string camerarep_fragment_code = "\n"
        "#version 150\n"
        "in vec4 color;\n"
		"in vec3 normal;\n"
        "out vec4 out_Color;\n"
		"uniform mat4 lightViewMatrix;\n"
		"uniform mat4 viewMatrix;\n"        
        "void main(void)\n"
        "{\n"
        "	vec3 normvec = normalize(normal);\n"
		"   vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;\n"
		"   lightDirection = normalize(lightDirection);\n"
		"	vec4 ambientLight = color * 0.2;\n"
		"	vec4 diffuseLight = color * max( dot(lightDirection, normvec), 0.0);\n"
		"	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz, color.w);\n"
        "}\n";

/// Default vertex shader for rendering camerarep 
const string camerarep_vertex_code = "\n"
        "#version 150\n"
		"in vec4 in_Position;\n"
		"in vec4 in_Normal;\n"
		"in vec4 in_Color;\n"
        "out vec4 color;\n"
		"out vec3 normal;\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"        
        "void main(void)\n"
        "{\n"
		"   mat4 modelViewMatrix = viewMatrix * modelMatrix;\n"
		"   mat4 normalMatrix = transpose(inverse(modelViewMatrix));\n"
		"   normal = normalize(vec3(normalMatrix * vec4(in_Normal.xyz,0.0)).xyz);\n"
        "   gl_Position = projectionMatrix * modelViewMatrix * in_Position;\n"
        "   color = in_Color;\n"
        "}\n";


/**
 * @brief Camera visual representation
 *
 * Renders a camera as a oriented frustum and x,y, and z axis
 **/
class CameraRep : public Tucano::Mesh {

private:

	Tucano::Shader camerarep_shader;

public:

	/**
	* @brief Default Constructor
	* @todo load shader from string and remove phong dependency
	*/
	CameraRep(bool front_closed=true)
	{

		resetModelMatrix();
		createGeometry(front_closed);

		initialize();
    }

    /**
     * @brief Initializes shaders
     * @param shader_dir Location of shader files
     */
    void initialize ()
    {
		camerarep_shader.setShaderName("camerarepShader");
		camerarep_shader.initializeFromStrings(camerarep_vertex_code, camerarep_fragment_code);
    }


	/**
	* @brief Render camera representation
	*/
	void render (const Tucano::Camera& camera, const Tucano::Camera& light)
	{
	    Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		camerarep_shader.bind();
        
		camerarep_shader.setUniform("viewMatrix", camera.getViewMatrix());
       	camerarep_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());      	
       	camerarep_shader.setUniform("modelMatrix", getShapeModelMatrix());
		camerarep_shader.setUniform("lightViewMatrix", light.getViewMatrix());

		setAttributeLocation(camerarep_shader);

		bindBuffers();
		renderElements();
		unbindBuffers();

       	camerarep_shader.unbind();
		
	}

private:

	/**
	* @brief Define camera geometry
	*
	* Camera is represented as a frustum and three axis
	*/
	void createGeometry (bool front_closed=true)
	{
		vector< Eigen::Vector4f > vert;
		vector< Eigen::Vector3f > norm;
		vector< Eigen::Vector4f > color;
		vector< GLuint > elementsVertices;
		
		// repeating vertices for each face to make life easier with normals
		// and other attributes

		Eigen::Vector3f normal = Eigen::Vector3f::Zero();
		Eigen::Vector3f vec1 = Eigen::Vector3f::Zero();
		Eigen::Vector3f vec2 = Eigen::Vector3f::Zero();

		// right face
		vert.push_back ( Eigen::Vector4f( 0.25, -0.25,  0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.50, -0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.50,  0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.25,  0.25,  0.5, 1.0) );
		vec1 = (vert[1] - vert[0]).head(3);
		vec2 = (vert[2] - vert[0]).head(3);
		normal = vec1.cross(vec2);		
		normal.normalize();
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		elementsVertices.push_back(0);
		elementsVertices.push_back(1);
		elementsVertices.push_back(2);
		elementsVertices.push_back(2);
		elementsVertices.push_back(3);
		elementsVertices.push_back(0);

		// left face
		vert.push_back ( Eigen::Vector4f(-0.25, -0.25,  0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.50, -0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.50,  0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.25,  0.25,  0.5, 1.0) );
		vec1 = (vert[7] - vert[4]).head(3);
		vec2 = (vert[6] - vert[4]).head(3);
		normal = vec1.cross(vec2);		
		normal.normalize();
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
		elementsVertices.push_back(4);
		elementsVertices.push_back(7);
		elementsVertices.push_back(6);
		elementsVertices.push_back(6);
		elementsVertices.push_back(5);
		elementsVertices.push_back(4);

		// bottom face
		vert.push_back ( Eigen::Vector4f( 0.25, -0.25,  0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.50, -0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.25, -0.25,  0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.50, -0.50, -0.5, 1.0) );
		vec1 = (vert[10] - vert[8]).head(3);
		vec2 = (vert[11] - vert[8]).head(3);
		normal = vec1.cross(vec2);		
		normal.normalize();
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		elementsVertices.push_back(8);
		elementsVertices.push_back(10);
		elementsVertices.push_back(11);
		elementsVertices.push_back(11);
		elementsVertices.push_back(9);
		elementsVertices.push_back(8);

		// top face
		vert.push_back ( Eigen::Vector4f( 0.25, 0.25,  0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.50, 0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.25, 0.25,  0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.50, 0.50, -0.5, 1.0) );
		vec1 = (vert[13] - vert[12]).head(3);
		vec2 = (vert[15] - vert[12]).head(3);
		normal = vec1.cross(vec2);		
		normal.normalize();
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		norm.push_back ( normal );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
		elementsVertices.push_back(12);
		elementsVertices.push_back(13);
		elementsVertices.push_back(15);
		elementsVertices.push_back(15);
		elementsVertices.push_back(14);
		elementsVertices.push_back(12);

		// front face
		vert.push_back ( Eigen::Vector4f( 0.5, -0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.5,  0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.5, -0.50, -0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.5,  0.50, -0.5, 1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, -1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, -1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, -1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, -1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		if (front_closed)
		{
			elementsVertices.push_back(16);
			elementsVertices.push_back(18);
			elementsVertices.push_back(19);
			elementsVertices.push_back(19);
			elementsVertices.push_back(17);
			elementsVertices.push_back(16);
		}

		// back face
		vert.push_back ( Eigen::Vector4f( 0.25, -0.25, 0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f( 0.25,  0.25, 0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.25, -0.25, 0.5, 1.0) );
		vert.push_back ( Eigen::Vector4f(-0.25,  0.25, 0.5, 1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, 1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, 1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, 1.0) );
		norm.push_back ( Eigen::Vector3f( 0.0, 0.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		color.push_back( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
		elementsVertices.push_back(20);
		elementsVertices.push_back(21);
		elementsVertices.push_back(23);
		elementsVertices.push_back(23);
		elementsVertices.push_back(22);
		elementsVertices.push_back(20);

		loadVertices(vert);
		loadNormals(norm);
		loadColors(color);
		loadIndices(elementsVertices);

		setDefaultAttribLocations();
		
	}

};
}
}
#endif
