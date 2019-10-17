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

#ifndef __CYLINDER__
#define __CYLINDER__

#include "tucano/mesh.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

namespace Shapes
{

/// Default fragment shader for rendering cylinder 
const string cylinder_fragment_code = "\n"
        "#version 430\n"
        "in vec4 color;\n"
		"in vec3 normal;\n"
		"in vec4 vert;\n"
        "out vec4 out_Color;\n"
		"uniform mat4 lightViewMatrix;\n"
		"uniform mat4 viewMatrix;\n"
        "uniform int with_cap;\n"
        "void main(void)\n"
        "{\n"
        "	vec3 normvec = normalize(normal);\n"
		"   vec3 eyeDirection = normalize(-vert.xyz);\n"
        "   if (with_cap == 1 && dot(normvec, eyeDirection) < 0.0) discard;\n"
		"   vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;\n"
		"   lightDirection = normalize(lightDirection);\n"
		"   vec3 lightReflection = reflect(-lightDirection, normvec);\n"
		"   float shininess = 100.0;\n"
		"	vec4 ambientLight = color * 0.4;\n"
		"	vec4 diffuseLight = color * 0.6 * max(dot(lightDirection, normvec),0.0);\n"
		"	vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);\n"
		"	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, color.w);\n"
        "}\n";

/// Default vertex shader for rendering cylinder 
const string cylinder_vertex_code = "\n"
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
 * @brief A simple cylinder shape
 **/
class Cylinder : public Tucano::Mesh {

private:

	/// Shader to render cylinder 
	//Tucano::Shader cylinder_shader;

	// Create a shared pointer for the shader, in case multiple spheres
	// are created only one shader is instanced
	std::shared_ptr < Tucano::Shader > shader_sptr;

	/// Cylinder cylinder_height
	float cylinder_height = 1.0;

	/// Cylinder cylinder_radius
	float cylinder_radius = 1.0;
    
    /// flag to create or not caps
    bool with_cap = true;

public:

	/**
	* @brief Default Constructor
	*/
	Cylinder(float r = 1.0, float h = 1.0, int subs_xy = 32, int subs_z = 8, bool cap = true)
	{
        with_cap = cap;
		setSize (r, h);
		createGeometry (subs_xy, subs_z);

        setColor(Eigen::Vector4f(0.0, 0.48, 1.0, 1.0));

		shader_sptr = std::shared_ptr < Tucano::Shader > ( new Tucano::Shader () );
	
		shader_sptr->setShaderName("cylinderShader");
    	shader_sptr->initializeFromStrings(cylinder_vertex_code, cylinder_fragment_code);
    
        #ifdef TUCANODEBUG
        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "cylinder constructor"); 
        #endif
	}


	/**
	 * @brief Sets cylinder size by scaling the size matrix
	 */
    void setSize (float r, float h)
    {
        cylinder_radius = r;
        cylinder_height = h;
		resetShapeMatrix();
		shape_matrix.scale (Eigen::Vector3f(cylinder_radius, cylinder_radius, cylinder_height));		
    }

	/**
	* @brief Render cylinder
	*/
	void render (const Tucano::Camera& camera, const Tucano::Camera& light)
	{
		Eigen::Vector4f viewport = camera.getViewport();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		shader_sptr->bind();

       	shader_sptr->setUniform("modelMatrix", getShapeModelMatrix());
		shader_sptr->setUniform("viewMatrix", camera.getViewMatrix());
       	shader_sptr->setUniform("projectionMatrix", camera.getProjectionMatrix());
		shader_sptr->setUniform("lightViewMatrix", light.getViewMatrix());
       	shader_sptr->setUniform("in_Color", default_color);
        shader_sptr->setUniform("with_cap", (int)with_cap);

 		this->setAttributeLocation(shader_sptr.get());

		this->bindBuffers();
		this->renderElements();
		this->unbindBuffers();

       	shader_sptr->unbind();

		#ifdef TUCANODEBUG
		Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
		
	}

	/**
	* @brief Returns cylinder cylinder_height
	*/
	float getHeight (void)
	{
		return cylinder_height;
	}

	/**
	* @brief Returns cylinder cylinder_radius 
	*/
	float getRadius (void)
	{
		return cylinder_radius;
	}

	/**
	* @brief Sets the cylinder origin and orientation
	* @param origin New cylinder origin (center of bottom face)
	* @param dir A normalized direction to set cylinder orientation
	*/
	void setOriginOrientation (const Eigen::Vector3f& origin, const Eigen::Vector3f& dir)
	{
		// default cylinder direction
		Eigen::Vector3f cylinder_dir = Eigen::Vector3f::UnitZ();

		// angle between default direction and desired direction
		float angle = acos(dir.dot(cylinder_dir));

		// rotation axis to take default direction to desired direction
		Eigen::Vector3f axis = cylinder_dir.cross(dir);
		axis.normalize();

		// place cylinder in ray position and direction
		resetModelMatrix();
		model_matrix.translate (origin);
		model_matrix.rotate(Eigen::AngleAxisf(angle, axis));
	}

private:


	/**
	* @brief Define cylinder geometry
	*
	* Cylinder is created by creating two disks (caps) and generating triangles between them
	*
	*/
	void createGeometry (int subs_xy, int subs_z)
	{

		vector< Eigen::Vector4f > vert;
		vector< Eigen::Vector3f > norm;
		vector< GLuint > faces;

		float x, y, theta;

		// create vertices for rings
        for (int j = 0; j <= subs_z + 1; ++j)
        {
            float z = j * (1.0 / (float)(subs_z+1));
            for (int i = 0; i < subs_xy; ++i)
            {
                theta = 2.0*M_PI*i/(float)subs_xy;
                x = sin(theta);
                y = cos(theta);
                vert.push_back(Eigen::Vector4f(x, y, z, 1.0));
                norm.push_back(Eigen::Vector3f(x, y, 0.0));
            }
        }

		// create a face with every three vertices between two rings
        // every vertice creates a tringle with two vertices from lower ring and top ring, and another with two vertices from top ring and one from lower
        for (int j = 0; j <= subs_z; ++j)
        {
		    for (int i = 0; i < subs_xy; ++i)
		    {
			    faces.push_back(i + j*subs_xy); // current vertex
                faces.push_back(i + (j+1)*subs_xy); // same vertex on lower ring
                faces.push_back(((i+1)%subs_xy) + (j+1)*subs_xy); // next vertex on lower ring
                faces.push_back((i+subs_xy-1)%subs_xy + j*subs_xy); // previous vertex on current ring
                faces.push_back(i + (j+1)*subs_xy); // same vertex on lower ring
			    faces.push_back(i + j*subs_xy); // current vertex
		    }
        }

        if (with_cap)
        {
            // create top cap
            vert.push_back(Eigen::Vector4f(0.0, 0.0, 1.0, 1.0));
            norm.push_back(Eigen::Vector3f(0.0, 0.0, 1.0));
            int center_index = vert.size()-1;
            int offset = vert.size();
            for (int i = 0; i < subs_xy; ++i)
            {
                theta = 2.0*M_PI*i/(float)subs_xy;
                x = sin(theta);
                y = cos(theta);
                vert.push_back(Eigen::Vector4f(x, y, 1.0, 1.0));
                norm.push_back(Eigen::Vector3f(0.0, 0.0,  1.0));
            }

            for (int i = 0; i < subs_xy; ++i)
            {
                faces.push_back(i+offset);
                faces.push_back((i+1)%(subs_xy) + offset);
                faces.push_back(center_index);
            }
            // create bottom cap
            vert.push_back(Eigen::Vector4f(0.0, 0.0, 0.0, 1.0));
            norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
            center_index = vert.size()-1;
            offset = vert.size();
            for (int i = 0; i < subs_xy; ++i)
            {
                theta = 2.0*M_PI*i/(float)subs_xy;
                x = sin(theta);
                y = cos(theta);
                vert.push_back(Eigen::Vector4f(x, y, 0.0, 1.0));
                norm.push_back(Eigen::Vector3f(0.0, 0.0, -1.0));
            }

            for (int i = 0; i < subs_xy; ++i)
            {
                faces.push_back(i+offset);
                faces.push_back(center_index);
                faces.push_back((i+1)%(subs_xy) + offset);
            }
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
