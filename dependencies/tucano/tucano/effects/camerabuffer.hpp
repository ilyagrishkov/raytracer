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

#ifndef __CAMERABUFFER__
#define __CAMERABUFFER__

#include "tucano/tucano.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * Renders a buffer with camera coordinates
 * The FBO may be set to multiple render targets, so many meshes can be rendered in different targets if necessary
 * without creating more effects or FBOs
**/
class CameraBuffer: public Tucano::Effect {

protected:
    /// Framebuffer to store the camera coordiantes map, this is using multisampling
    Tucano::Framebuffer fbo;

    /// A auxiliary fbo to blit multisampled fbo, this is without multisampling
    Tucano::Framebuffer aa_fbo;

    /// Creates the camera buffer by rendering from given camera
    Tucano::Shader camerabuffer_shader;

    /// Shader to render the camera coordinates buffer
    Tucano::Shader drawbuffer_shader;

    /// Quad for rendering buffer, mostly for debug
    Tucano::Mesh quad;

    /// Number of multisamples, if 1, no multisamping
    int num_samples = 1;

    /// Numer of render targets
    int num_attachs = 1;


public:

    /**
     * @brief Default constructor.
     *
	**/
    CameraBuffer (void)
    {
        quad.createQuad();
	}

    /**
     * @brief Set number of render targets
     * @param nt Number of rendertargets for FBO
     */
    void setNumAttachs (int nt)
    {
        num_attachs = nt;
    }


    /**
     * @brief Initializes the CameraBuffer effects,
     *
     */
    virtual void initialize (void)
    {
		loadShader(camerabuffer_shader, "cameracoords");
        loadShader(drawbuffer_shader, "renderbuffer");
    }

	void clearBuffer (void)
	{
		fbo.clearAttachments();
	}

    /**
     * @brief Returns a pointer to the camera coordinates map with single sampling
     * Note that if multisapling is used (num samples > 1) a blit operation
     * is performed, and the single sampled map is returned
     * @return Pointer to the camera buffer with single sampling
     */
    Tucano::Framebuffer* getFbo (void)
	{
        if (num_samples == 1)
            return &fbo;
		return &aa_fbo;
	}

    /**
     * @brief Returns a pointer to the multisampled camera coords map
     * @return Pointer to multisample buffer
     */
    Tucano::Framebuffer* getFboMultisample (void)
    {
        return &fbo;   
    }

    /**
    * @brief Set the number of samples for multisampling
    * @param n Given number of samples
    */
    void setNumSamples (int n)
    {
        num_samples = n;
    }

    /**
     * @brief Renders the buffers, usually useful for visual debugging
     * @param camera The camera to render from
     */
    void renderBuffer (const Tucano::Camera& camera, int attach = 0)
    {
        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        drawbuffer_shader.bind();
        if (num_samples == 1)
            drawbuffer_shader.setUniform("tex", fbo.bindAttachment(attach));
        else
            drawbuffer_shader.setUniform("tex", aa_fbo.bindAttachment(attach));
        drawbuffer_shader.setUniform("use_aa_filter", 0);
        quad.setAttributeLocation(drawbuffer_shader);
        quad.render();
        drawbuffer_shader.unbind();
        fbo.unbindAttachments();
        
    }

    /**
     * @brief Creates the camera coordinates buffer
     * @param mesh Mesh to be rendered.
     * @param camera A pointer to the camera object.
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, int target = 0, const Eigen::Vector4f& buffer_viewport = Eigen::Vector4f(0,0,0,0))
	{
        glEnable(GL_DEPTH_TEST);
        
        Eigen::Vector4f viewport = buffer_viewport;
        Eigen::Vector2i viewport_size;
        viewport_size << buffer_viewport[2] - buffer_viewport[0], buffer_viewport[3] - buffer_viewport[1];

        if (viewport.norm() == 0.0)
        {
            viewport = camera.getViewport();
            viewport_size = camera.getViewportSize();
        }

        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // check if viewport was modified, if so, regenerate fbo
        if (fbo.getWidth() != viewport_size[0] || fbo.getHeight() != viewport_size[1])
        {
            aa_fbo.create(viewport_size[0], viewport_size[1], num_attachs, 1); // single sampling
            fbo.create(viewport_size[0], viewport_size[1], num_attachs, num_samples); // multi sampling (actually used for generatin map)
            for (int i = 0; i < num_attachs; ++i)
            {
                fbo.getTexture(i)->setTexParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

                // if using sampler2DShadow in shaders the following parameter must be set, but usually we do the check manually
                fbo.getTexture(i)->bind();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);   
                fbo.getTexture(i)->unbind();
            }
        }

        // Bind buffer to store coord
        fbo.bindRenderBuffer(target);

        camerabuffer_shader.bind();
        camerabuffer_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        camerabuffer_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        camerabuffer_shader.setUniform("viewMatrix", camera.getViewMatrix());

        mesh.setAttributeLocation(camerabuffer_shader);
        mesh.render();

        camerabuffer_shader.unbind();
        fbo.unbind();

        if (num_samples > 1) // if multisampling prepare Anti Aliased single sampled buffer
        {
            for (int i = 0; i < num_attachs; ++i)
                fbo.blitTo(aa_fbo, i, i);
        }
    }

};
}
}
#endif
