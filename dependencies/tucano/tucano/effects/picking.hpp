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

#ifndef __PICKING__
#define __PICKING___

#include "tucano/tucano.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief Picks a 3D position from screen position
 */
class Picking : public Tucano::Effect
{

public:

    /**
     * @brief Default constructor.
     */
    Picking (void) {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        loadShader(worldcoords_shader, "worldcoords");
    }

    /**
     * @brief Render the mesh given a camera trackball
     * @param mesh Given mesh
     * @param camera Given camera trackball     
     */
    virtual void render (Tucano::Mesh& mesh, const Tucano::Camera& camera)
    {
        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);


        if (fbo.getWidth() != (viewport[2]-viewport[0]) || fbo.getHeight() != (viewport[3]-viewport[1]))
        {
            fbo.create(viewport[2]-viewport[0], viewport[3]-viewport[1], 1);
        }

        // sets the FBO first (and only) attachment as output
        fbo.clearAttachments();
        fbo.bindRenderBuffer(0);

        worldcoords_shader.bind();

        // sets all uniform variables for the phong shader
        worldcoords_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        worldcoords_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        worldcoords_shader.setUniform("viewMatrix", camera.getViewMatrix());

        mesh.setAttributeLocation(worldcoords_shader);

        glPointSize(5.0);
        glEnable(GL_DEPTH_TEST);
        mesh.render();
        glPointSize(1.0);

        worldcoords_shader.unbind();

        fbo.unbind(); // automatically returns the draw buffer to GL_BACK
    }

    /**
    * @brief Returns a pointer to the framebuffer object containg rendered world coords
    * @return Pointer to FBO
    */
    Tucano::Framebuffer* getFbo (void)
    {
        return &fbo;
    }

    /**
    * @brief Returns the world coordinates of point projected on given pixel
    * @param pos Screen position
    * @return world coordinates of projected point in given position
    */
    Eigen::Vector4f pick (const Eigen::Vector2i &pos)
    {
        return fbo.readPixel(0, pos);
    }

private:

    /// shader to write world coords to projected pixel positions
    Tucano::Shader worldcoords_shader;

    /// Buffer to store projected coords
    Tucano::Framebuffer fbo;
};
}

}



#endif
