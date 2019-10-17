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

#ifndef __MEANFILTER__
#define __MEANFILTER__

#include "tucano/tucano.hpp"


namespace Tucano
{

namespace Effects
{

/**
 * @brief A simple mean filter for image processing.
 *
 * No weights are taken in consideration, averages all neighbors equally.
 **/
class MeanFilter : public Tucano::Effect
{
	
private:

  /// The mean filter shader.
  Tucano::Shader shader;

  /// The size of the mean filter kernel (window size to apply convolution)
  int kernelsize = 3;

  /// A quad to be rendered forcing one call of the fragment shader per image pixel
  Tucano::Mesh quad;

public:
  /**
   * @brief Default Constructor.
   *
   * Default kernel size is 3.
   **/
  MeanFilter (void) {}

  /**
   * @brief Initializes the effect, creating and loading the shader.
   */
  virtual void initialize()
  {
      loadShader(shader, "meanfilter");
      quad.createQuad();
  }

  /**
   * @brief Applies the mean filter to an image.
   **/
  void renderTexture(Tucano::Texture& tex, Eigen::Vector2i viewport)
  {
      glViewport(0, 0, viewport[0], viewport[1]);
      shader.bind();

      shader.setUniform("imageTexture", tex.bind());
      shader.setUniform("kernelsize", kernelsize);

	  quad.setAttributeLocation(shader);
      quad.render();

      shader.unbind();
      tex.unbind();
  }

  /**
   * @brief Set kernel size.
   * @param kernel Given kernel size.
   */
  void setKernel (int kernel)
  {
      kernelsize = kernel;
  }

  /**
   * @brief Returns the current size of the kernel
   * @return size of kernel
   */
  int getKernelSize (void)
  {
    return kernelsize;
  }

};
}

}

#endif
