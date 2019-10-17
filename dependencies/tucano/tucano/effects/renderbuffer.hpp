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

#ifndef __RENDERBUFFER__
#define __RENDERBUFFER__

#include "tucano/tucano.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief A simple effect to render a buffer without any interpolation.
 **/
class RenderBuffer : public Effect
{

private:

    /// The mean filter shader.
    Tucano::Shader shader;

    /// A quad to be rendered forcing one call of the fragment shader per image pixel (its just a proxy geometry)
    Tucano::Mesh quad;

    /// use gaussian filter as antialiasing
    bool use_aa_filter = false;
    
public:
    /**
     * @brief Default Constructor.
     */
    RenderBuffer (void) {}

    /**
     * @brief Initializes the effect, creating and loading the shader.
     */
    virtual void initialize()
    {
		loadShader(shader, "renderbuffer");
        quad.createQuad();
    }

    void applyAAFilter (bool a)
    {
        use_aa_filter = a;
    }

    /**
     * @brief Renders the given FBO attachment.
     *
     * Renders the given attachment of an FBO.
	 * @param fbo Given fbo
	 * @param attach_id Id of the FBO attach to render
     */
    void render (Tucano::Framebuffer& fbo, GLint attach_id = 0)
    {
        glViewport(0, 0, fbo.getWidth(), fbo.getHeight());

        shader.bind();
        shader.setUniform("tex", fbo.bindAttachment(attach_id));
        shader.setUniform("use_aa_filter", (int)use_aa_filter);
		quad.setAttributeLocation(shader);
        quad.render();

        shader.unbind();
        fbo.unbindAttachments();
    }
};
}

}

#endif
