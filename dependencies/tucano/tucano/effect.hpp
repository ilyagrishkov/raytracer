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

#ifndef __EFFECT__
#define __EFFECT__

#include <Eigen/Dense>
#include <vector>
#include "constants.hpp"
#include "tucano/shader.hpp"

namespace Tucano
{

/**
 * @brief The Effect class is a holder for Shaders.
 * It is completely optional, but is contains a few methods for convenience, and is extremely useful
 * to organize shaders belonging to a single effect (such as multipass strategies).
 *
 * This class should be extended in order to hold a full GLSL effect, it does not necessarily needs
 * to be a render effect, but any GPU application.
 * By loading the Shaders trough the loadShader method, the effect class also mantains the list of Shaders
 * and can reload all shaders with a single call.
 */
class Effect {

public:

    /**
     * @brief Default constructor.
     * @param shadersDir The directory containing the shader files.
     */
    Effect (string resource_dir = TUCANO_DEFAULT_RESOURCE_DIR) : shaders_dir(resource_dir + "shaders/")
    {
    }

    /**
    * @brief Initializes all effect's funcionality.
    *
    * This method must be implemented in the extended class. It will typically hold the shader loading, however,
    * if the application uses other parameters, their initialization can also be done in this method.
    */
    virtual void initialize (void) = 0;

    /**
     * @brief Loads a shader by filename, initializes it, and inserts in shaders list.
     *
     * The name should be passed without extensions, the method will automatically search the shader directory
     * for shader extensions (vert, frag, geom, comp).
     * @param shader_name String with filename without extensions.
     */
    virtual Shader* loadShader (string shader_name)
    {
        Shader* shader_ptr = new Shader(shader_name, shaders_dir);
        shader_ptr->initialize();
        shaders_list.push_back(shader_ptr);
        return shader_ptr;
    }

    virtual void loadShader (Shader& shader, string shader_name)
    {
		shader.load(shader_name, shaders_dir);
        shader.initialize();
        shaders_list.push_back(&shader);
    }

    /**
     * @brief Loads a shader by complete filenames (with extensions), initializes it, and inserts in shaders list.
     * @param shader_name String with name of the shader
     * @param vertex_name Vertex shader filename
     * @param frag_name Fragment shader filename
     * @param geom_name Geometry shader filename
     */
    virtual Shader* loadShader (string shader_name, string vertex_name, string frag_name, string geom_name)
    {
        Shader* shader_ptr = new Shader(shader_name, vertex_name, frag_name, geom_name);
        shader_ptr->initialize();
        shaders_list.push_back(shader_ptr);
        return shader_ptr;
    }

    /**
     * @brief Reloads all shaders needed for effect usage.
     *
     * This method allows the user to change the shaders codes and see the results in real time, sparing him the need to restart the application.
     */
    virtual void reloadShaders (void)
    {
        for (unsigned int i = 0; i < shaders_list.size(); ++i)
        {
            shaders_list[i]->reloadShaders();
        }
    }



protected:


    /// Vector of pointers to shaders used in this effect, in case the user needs multiple pass rendering.
    std::vector< Shader* > shaders_list;

    /// Directory in which the shader files are stored.
    string shaders_dir;

};
}
#endif
