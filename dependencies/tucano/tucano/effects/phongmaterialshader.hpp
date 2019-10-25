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

#ifndef __PHONGMATERIALSHADER__
#define __PHONGMATERIALSHADER__

#include "tucano/effect.hpp"
#include "../constants.hpp"
#include "tucano/camera.hpp"
#include "tucano/mesh.hpp"
#include "tucano/texture.hpp"
#include "tucano/materials/phong.hpp"
#include "tucano/materials/mtl.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief Renders a mesh using a Phong shader.
 */
class PhongMaterial : public Tucano::Effect
{

private:

    /// Phong Shader
    Tucano::Shader phong_shader;

    /// Phong Model Material
    vector<Tucano::Material::Phong> phongmaterials;

    /// Texture
    Tucano::Texture texture;

public:

    /**
     * @brief Default constructor.
     */
    PhongMaterial (string resource_dir = TUCANO_DEFAULT_RESOURCE_DIR) : Effect(resource_dir)
    {
    }

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
        loadShader(phong_shader, "phongmaterialshader") ;
    }

    void addMaterial (Tucano::Material::Phong& mat)
    {
        phongmaterials.push_back(mat);
    }

    void addMaterial (Tucano::Material::Mtl& mat)
    {
        Tucano::Material::Phong pmat;
        pmat.setFromMtl(mat);
        addMaterial(pmat);
    }


    Tucano::Material::Phong getMaterial (int id) {return phongmaterials[id];}

    /**
     * @brief Set a texture for the model
     * Note that the mesh must contain tex coords to work properly with texture
     * @param tex Given texture
     */
    void setTexture (const Tucano::Texture &tex)
    {
        texture = tex;
    }

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

        for (int i = 0; i < mesh.numberIndexBuffers(); ++i)
        {
            int matid = mesh.getMaterialId (i);

            bool has_texture = mesh.hasAttribute("in_TexCoords") && !phongmaterials[matid].getDiffuseTexture().isEmpty();
     
            phong_shader.setUniform("has_texture", has_texture);
            if (has_texture)
                phong_shader.setUniform("model_texture", phongmaterials[matid].getDiffuseTexture().bind());
            else
                phong_shader.setUniform("model_texture", 0);

            

            if (matid >= 0 && matid < phongmaterials.size())
            {
                phong_shader.setUniform("ka", phongmaterials[matid].getAmbient());
                phong_shader.setUniform("kd", phongmaterials[matid].getDiffuse());
                phong_shader.setUniform("ks", phongmaterials[matid].getSpecular());
                phong_shader.setUniform("shininess", phongmaterials[matid].getShininess());
            }
            else //some default material
            {
                phong_shader.setUniform("ka", Eigen::Vector3f(0.3, 0.3, 0.3));
                phong_shader.setUniform("kd", Eigen::Vector3f(0.8, 0.5, 0.1));
                phong_shader.setUniform("ks", Eigen::Vector3f(1.0, 1.0, 1.0));
                phong_shader.setUniform("shininess", 10.0);
            }

            mesh.setAttributeLocation(phong_shader);
            mesh.renderIndexBuffer(i);
            if (has_texture)
               phongmaterials[matid].getDiffuseTexture().unbind();
        }

        phong_shader.unbind();

    }


};
}
}


#endif
