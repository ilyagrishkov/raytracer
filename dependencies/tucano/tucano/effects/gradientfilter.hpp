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

#ifndef __GRADIENT__
#define __GRADIENT__

#include "tucano/tucano.hpp"

namespace Tucano
{
namespace Effects
{


/**
 * @brief A simple Sobel filter for image processing.
 *
 **/
class GradientFilter : public Tucano::Effect
{

private:

  /// The mean filter shader.
  Tucano::Shader shader;

  /// A quad to be rendered forcing one call of the fragment shader per image pixel
  Tucano::Mesh quad;

  /// Apply gradient in horizontal direction.
  bool horizontal = true;

  /// Apply gradient in vertical direction.
  bool vertical = true;

public:

  /**
   * @brief Default Constructor.
   **/
  GradientFilter (void) {}

  /**
   * @brief Initializes the effect, creating and loading the shader.
   */
  virtual void initialize()
  {
      loadShader(shader, "gradientfilter");
      quad.createQuad();
  }

  /**
   * @brief Applies the gradient filter to an image.
   **/
  void renderTexture(Tucano::Texture& tex, Eigen::Vector2i viewport)
  {
      glViewport(0, 0, viewport[0], viewport[1]);

      shader.bind();
      shader.setUniform("imageTexture", tex.bind());
      shader.setUniform("hdir", horizontal);
      shader.setUniform("vdir", vertical);

	  quad.setAttributeLocation(shader);
      quad.render();

      shader.unbind();
      tex.unbind();
  }

  /**
   * @brief Sets flags for computing gradient in horizontal and vertical directions.
   * @param hgrad Gradient in horizontal direction.
   * @param vgrad Gradient in vertical direction.
   */
  void setDirections (bool hgrad, bool vgrad)
  {
      horizontal = hgrad;
      vertical = vgrad;
  }

};
}

}
#endif
