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

#ifndef __PHONG__
#define __PHONG__

#include "tucano/effect.hpp"
#include "tucano/camera.hpp"
#include "tucano/mesh.hpp"
#include "tucano/texture.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief Renders a mesh using a Phong shader.
 */
class Phong : public Tucano::Effect
{

private:

    /// Phong Shader
    Tucano::Shader phong_shader;

	/// Default color
	Eigen::Vector4f default_color = Eigen::Vector4f (0.7, 0.7, 0.7, 1.0);

    /// Ambient coefficient
    Eigen::Vector3f ka = Eigen::Vector3f(0.3, 0.3, 0.3);

    /// Diffuse coefficient
    Eigen::Vector3f kd = Eigen::Vector3f(0.5, 0.5, 0.5);
    
    /// Specular coefficient
    Eigen::Vector3f ks = Eigen::Vector3f(1.0, 1.0, 1.0);

    /// Shininess    
    float shininess = 10;

    /// Texture
    Tucano::Texture texture;

public:

    /**
     * @brief Default constructor.
     */
    Phong (void)
    {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
        loadShader(phong_shader, "phongshader") ;
    }

	/**
	* @brief Sets the default color, usually used for meshes without color attribute
	*/
	void setDefaultColor ( const Eigen::Vector4f& color )
	{
		default_color = color;
	}

    /**
    * @brief Set ambient coefficient
    * @param value New ambient coeff (ka)
    */
    void setAmbientCoeff (Eigen::Vector3f value)
    {
        ka = value;
    }

    /**
    * @brief Set diffuse coefficient
    * @param value New diffuse coeff (kd)
    */
    void setDiffuseCoeff (Eigen::Vector3f value)
    {
        kd = value;
    }

    /**
    * @brief Set specular coefficient
    * @param New specular coeff (ks)
    */
    void setSpecularCoeff (Eigen::Vector3f value)
    {
        ks = value;
    }

    /**
    * @brief Set shininess exponent
    * @param New shininess coeff (shininess)
    */
    void setShininessCoeff (float value)
    {
        shininess = value;
    }

    /**
     * @brief Set a texture for the model
     * Note that the mesh must contain tex coords to work properly with texture
     * @param tex Given texture
     */
    void setTexture (const Tucano::Texture &tex)
    {
        texture = tex;
    }


    Eigen::Vector3f getDiffuseCoeff (void ) {return kd;}
    Eigen::Vector3f getAmbientCoeff (void ) {return ka;}
    Eigen::Vector3f getSpecularCoeff (void ) {return ks;}
    float getShininessCoeff (void ) {return shininess;}
    Tucano::Texture* getTexture (void) {return &texture;}

    /** 
     * @brief Render the mesh given a camera and light, using a Phong shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {

        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        phong_shader.bind();

        // sets all uniform variables for the phong shader
        phong_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        phong_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        phong_shader.setUniform("viewMatrix", camera.getViewMatrix());
        phong_shader.setUniform("lightViewMatrix", lightTrackball.getViewMatrix());
        phong_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));
		phong_shader.setUniform("default_color", mesh.getColor());
        phong_shader.setUniform("ka", ka);
        phong_shader.setUniform("kd", kd);
        phong_shader.setUniform("ks", ks);
        phong_shader.setUniform("shininess", shininess);

        bool has_texture = mesh.hasAttribute("in_TexCoords") && !texture.isEmpty();
 
        phong_shader.setUniform("has_texture", has_texture);
        if (has_texture)
            phong_shader.setUniform("model_texture", texture.bind());

        else
            phong_shader.setUniform("model_texture", 0);


        mesh.setAttributeLocation(phong_shader);

        mesh.render();

        phong_shader.unbind();
        if (has_texture)
            texture.unbind();
    }


};
}
}


#endif
