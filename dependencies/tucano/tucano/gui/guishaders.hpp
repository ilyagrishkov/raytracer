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

#ifndef __GUISHADERS__
#define __GUISHADERS__


namespace Tucano
{

namespace GUI
{


/// Default fragment shader for rendering gui elments
const string gui_fragment_code = "\n"
        "#version 430\n"
        "in vec4 color;\n"
        "in vec4 texcoords;\n"
        "out vec4 out_Color;\n"
        "uniform sampler2D shapetex;\n"
        "void main(void)\n"
        "{\n"
        "   vec4 tex = texture(shapetex, texcoords.xy).xyzw;\n"
        "   if (tex.w == 0.0)\n"
        "       discard;\n"
        "   out_Color = tex*color;\n"
        "}\n";

/// Default vertex shader for rendering gui elements
const string gui_vertex_code = "\n"
        "#version 430\n"
        "in vec4 in_Position;\n"
        "in vec4 in_TexCoords;\n"
        "out vec4 color;\n"
        "out vec4 texcoords;\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform vec4 in_Color;\n"
        "void main(void)\n"
        "{\n"
        "   vec4 v = projectionMatrix * viewMatrix * modelMatrix * in_Position;\n"
        "   gl_Position = v;\n"
        "   texcoords = in_TexCoords;\n"
        "   texcoords.y = 1.0 - texcoords.y;\n"
        "   color = in_Color;\n"
        "}\n";

}
}
#endif
