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

#ifndef __BUFFEROBJECT__
#define __BUFFEROBJECT__

#include "tucano/shader.hpp"
#include "tucano/texture.hpp"
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <limits>
#include <GL/glew.h>

namespace Tucano
{

/**
 * @brief A buffer object (i.e. ShaderStorageBuffer).
 *
 * The Buffer Object class is responsible for buffer generation and storage.
 * A BO can be of many types: atomic, storage, transform feedback etc ..
 */
template <class T> class BufferObject {

protected:

    /**
     * @brief Creates the Buffer Object.
     */
    virtual void create(void)
    {
        // declare and generate a buffer object name
        glGenBuffers(1, &buffer_id);
        bind();
        // define its initial storage capacity
        glBufferData(buffer_type, sizeof(T) * size, NULL, GL_DYNAMIC_DRAW);
        unbind();
    }

    /// The handle of the Buffer Object.
    GLuint buffer_id;

    /// Type of buffer.
    GLenum buffer_type;

    /// Buffer dimension (number of elements).
    int size;

    /// Binding point for this buffer
    int binding_point;

public:

    /**
     * Buffer Object constructor.
     * @param s Size of buffer
     * @param buftype Type of buffer object.
     */
    BufferObject (int s, GLenum buftype) : buffer_id(0), buffer_type(buftype), size(s)
    {
        create();
        clear();
    }

    /**
     * @brief Default Destructor.
     */
    virtual ~BufferObject (void)
    {}

    /**
     * @brief Returns The id of the buffer (handle).
     * @return ID of the buffer
     */
    GLuint getBufferID (void) {
        return buffer_id;
    }

    /**
     * @brief Binds buffer object.
     */
    virtual void bind (void)
    {
        glBindBuffer(buffer_type, buffer_id);
    }

    /**
     * @brief Binds buffer to a specific binding point.
     * @param index Binding point.
     */
    void bindBase (int index)
    {
        binding_point = index;
        glBindBufferBase(buffer_type, binding_point, buffer_id);
    }

    /**
     * @brief Unbinds buffer from binding point.
     */
    void unbindBase (void) {
        glBindBufferBase(buffer_type, binding_point, 0);
        binding_point = -1;
    }

    /**
     * @brief Unbinds the buffer object.
     */
    virtual void unbind(void)
    {
        glBindBuffer(buffer_type, 0);
    }

    /**
     * @brief Clears all values (sets to zero).
     */
    virtual void clear (void)
    {
        // declare a pointer to hold the values in the buffer
        T *buffer_data;
        bind();
        // map the buffer, userCounters will point to the buffers data

        buffer_data = (T*)glMapBufferRange(buffer_type,
                                                 0 ,
                                                 sizeof(T) * size,
                                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
                                                 );

        // set the memory to zeros, resetting the values in the buffer
        memset(buffer_data, 0, sizeof(T) * size );
        // unmap the buffer
        glUnmapBuffer(buffer_type);
        unbind();

    }

    /**
     * @brief Reads a GPU buffer and stores it in a CPU array.
     *
     * Assumes each element is of Template Type T.
     * @param return_values Pointer to array of values.
     */
    virtual void readBuffer (T **return_values)
    {
        T *values = new T[size];
        T *buffer_data;
        bind();
        // again we map the buffer to userCounters, but this time for read-only access
        buffer_data = (T*)glMapBufferRange(buffer_type,
                                                 0,
                                                 sizeof(T) * size,
                                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
                                                );

        // copy the values to other variables because...
        for (int i = 0; i < size; ++i) {
            values[i] = buffer_data[i];
        }
        *return_values = values;

        // ... as soon as we unmap the buffer the pointer buffer becomes invalid.
        glUnmapBuffer(buffer_type);
        unbind();
    }

    /**
     * @brief Prints the content of a GPU. Usually used for debugging.
     */
    void printBuffer (void) {
        T *values;
        readBuffer(&values);
        for (int i = 0; i < size; ++i) {
            cout << i << " : " << values[i] << endl;
        }
        delete [] values;
    }

    /**
     * @brief Returns the size of the buffer, number of elements.
     * @return The size of the storage buffer
     */
    int getSize (void)
    {
        return size;
    }



};

/**
 * @brief An Atomic Buffer object (inherited from BufferObject).
 */
class AtomicBuffer : public BufferObject <GLuint>
{

public:
    /**
     * @brief Atomic Buffer constructor.
     * @param s Size of buffer.
     */
    AtomicBuffer (int s) : BufferObject<GLuint>(s, GL_ATOMIC_COUNTER_BUFFER) {}
};

/**
 * @brief The buffer object of thype ShaderStorageBuffer with Float elements
 */
class ShaderStorageBufferFloat: public BufferObject <GLfloat>
{

public:
    /**
     * @brief Float Shader Storage Buffer constructor.
     * @param s Size of buffer.
     */
    ShaderStorageBufferFloat (int s) : BufferObject<GLfloat>(s, GL_SHADER_STORAGE_BUFFER) {}
};

/**
 * @brief The buffer object of thype ShaderStorageBuffer with Integer elements
 */
class ShaderStorageBufferInt: public BufferObject <GLint>
{

public:
    /**
     * @brief Integer Shader Storage Buffer constructor.
     * @param s Size of buffer.
     */
    ShaderStorageBufferInt (int s) : BufferObject<GLint>(s, GL_SHADER_STORAGE_BUFFER) {}
};

}

#endif
