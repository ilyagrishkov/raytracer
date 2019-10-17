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

#ifndef __RENDERTEXTURE__
#define __RENDERTEXTURE__

#include "tucano/effect.hpp"
#include "tucano/texture.hpp"
#include "tucano/mesh.hpp"

namespace Tucano
{
namespace Effects
{

/**
 * @brief A simple effect to render a texture.
 **/
class RenderTexture : public Tucano::Effect
{
    
private:

    /// The mean filter shader.
    Tucano::Shader shader;

    /// A quad to be rendered forcing one call of the fragment shader per image pixel (its just a proxy geometry)
    Tucano::Mesh quad;

    /// Transparency
    float alpha = 1.0;

public:
    /**
     * @brief Default Constructor.
     */
    RenderTexture (void) {}

    /**
     * @brief Initializes the effect, creating and loading the shader.
     */
    virtual void initialize()
    {
		loadShader(shader, "rendertexture");
        quad.createQuad();
    }

    void setTransparency (float a)
    {
        alpha = a;
    }
    
    /**
     * @brief Renders the given texture.
     *
     * Renders the given texture using a proxy geometry, a quad the viewport
     * to hold the texture.
     */
    void renderTexture (Tucano::Texture& tex, const Eigen::Vector4i& viewport)
    {
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        shader.bind();
        shader.setUniform("imageTexture", tex.bind());
        shader.setUniform("viewport", viewport);
        shader.setUniform("alpha", alpha);

		quad.setAttributeLocation(shader);
        quad.render();

        shader.unbind();
        tex.unbind();
    }

    /**
     * @brief Renders the given texture.
     *
     * Renders the given texture using a proxy geometry, a quad the size of the viewport
     * to hold the texture.
     */
    void renderTexture (Tucano::Texture& tex, const Eigen::Vector2i& viewport)
    {
        renderTexture (tex, Eigen::Vector4i (0, 0, viewport[0], viewport[1]));
    }
};
}
}

#endif
