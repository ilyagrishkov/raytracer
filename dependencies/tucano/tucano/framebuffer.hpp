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

#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include "tucano/shader.hpp"
#include "tucano/texture.hpp"
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <GL/glew.h>

namespace Tucano
{

/**
 * @brief A wrapper class for creating and using FBOs.
 *
 * The Framebuffer class is responsible for framebuffer generation and storage.
 * It holds many shortcut methods to bind, draw to buffer, read buffer for debug, etc...
 */
class Framebuffer {

protected:

    /// The Framebuffer Object.
    GLuint fbo_id;

    /// The Depthbuffer Object.
    GLuint depthbuffer_id;

    /// Array of textures attachments.
    std::vector<Texture> fboTextures;

    /// Texture type for framebuffer attachments, default is GL_TEXTURE_2D.
    GLenum texture_type;

    /// Framebuffer dimensions.
    Eigen::Vector2i size;

    /// Internal format as defined by OpenGL (ex. GL_RGBA, GL_RGBA32F ...).
    GLenum internal_format;

    /// Pixel type (GL_FLOAT, GL_UNSIGNED_INT ...)
    GLenum pixel_type;

    /// Format as defined by OpenGL (ex. GL_RGBA, GL_RGBA_INTEGER ...)
    GLenum format;

    /// Number of samples (for multisampling)
    int num_samples = 1;

    /**
     * @brief Flag to indicate if buffer is binded or not
     *
     * In operations where the buffer needs to be binded (such as read, clear, etc)
     * it will be automatically binded and then unbided if flag is not currently binded,
     * otherwise, if it was already binded, it will be left binded.
     */
    bool is_binded;

    /// Shared pointer to fbo id
    std::shared_ptr < GLuint > fboID_sptr;

    /// Shared pointer to depth buffer id
    std::shared_ptr < GLuint > depthbufferID_sptr;

public:

    /**
     * @brief Framebuffer default constructor.
     *
     * Creates a framebuffer with the given size and number of buffers, as well as set all texture units as not used.
     * @param w: Width of the framebuffer texture object.
     * @param h: Height of the framebuffer texture object.
     * @param num_buffers: Number of textures that the framebuffer object will hold.
     * @param textype: Type of the framebuffer texture object.
     * @param int_frm Internal format (default is GL_RGBA32F)
     * @param frm Format (default is GL_RGBA)
     * @param pix_type Texture pixel type (default is GL_FLOAT)
     */
    Framebuffer (int w, int h, int num_buffers = 1, GLenum textype = GL_TEXTURE_2D, GLenum int_frm = GL_RGBA32F, GLenum frm = GL_RGBA, GLenum pix_type = GL_UNSIGNED_BYTE  ) :
        texture_type(textype), internal_format(int_frm), pixel_type(pix_type), format(frm)
    {
        is_binded = false;
        fboTextures.clear();
        create(w, h, num_buffers);
    }

    /**
     * @brief Framebuffer default empty constructor
     */
    Framebuffer (void) : texture_type(GL_TEXTURE_2D), internal_format(GL_RGBA32F), pixel_type(GL_UNSIGNED_BYTE), format(GL_RGBA)
    {
        is_binded = false;
        size = Eigen::Vector2i(0,0);

        glGenFramebuffers(1, &fbo_id);
        fboID_sptr = std::shared_ptr < GLuint > (
                new GLuint (fbo_id),
                [] (GLuint *p) {
                    glDeleteFramebuffers(1, p);
                    delete p;
                }
                );
 
        glGenRenderbuffers(1, &depthbuffer_id);
        depthbufferID_sptr = std::shared_ptr < GLuint > (
                new GLuint (depthbuffer_id),
                [] (GLuint *p) {
                    glDeleteFramebuffers(1, p);
                    delete p;
                }
                );
    }

    /**
     * @brief Creates the framebuffer with specified parameters.
     * @param w Width of FBO.
     * @param h Height of FBO.
     * @param num_attachs Number of texture attachments to be created.
     * @param num_samples Number of samples, if more than 1 creates multisample texture.
     */
    void create (int w, int h, int num_attachs = 1, int nsamples = 1)
    {
        num_samples = nsamples;
        size << w, h;
        int max_attachs;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_attachs);
        if (num_attachs > max_attachs)
        {
            cout << "WARNING : number of buffers > Max Color Attachments: " << max_attachs << endl;
        }

        if (num_samples > 1 && texture_type == GL_TEXTURE_2D)
        {
            texture_type = GL_TEXTURE_2D_MULTISAMPLE;
        }
        else if (num_samples == 1 && texture_type == GL_TEXTURE_2D_MULTISAMPLE)
        {
            texture_type = GL_TEXTURE_2D;
        }

        createFramebuffer(size[0], size[1], num_attachs);
    }

    /**
    * Overload of create method to use vector2i instead of width and height
    */
    void create (Eigen::Vector2i size, int num_attachs = 1, int nsamples = 1)
    {
        create(size[0], size[1], num_attachs, nsamples);
    }

    /**
     * @brief Returns the total number of data elements held by the framebuffer, in terms of red, green, blue and alpha components.
     * Use this method if you need to allocate a buffer large enough to copy the framebuffer's contents.
     * @return The total number of elements stored by the framebuffer
     */
    int bufferElements() const {
    	return size[0] * size[1] * 4;
    }

    /**
	 * @brief Returns the total number of depth data elements held by the framebuffer.
	 * Use this method if you need to allocate a buffer large enough to copy the framebuffer's depth contents.
	 * @return The total number of depth elements stored by the framebuffer
	 */
    int depthBufferElements() const {
		return size[0] * size[1];
	}

    /**
     * @brief Returns the id of the texture in given color attachment.
     * @param tex_id Position of the texture, as in the ith texture of the FBO.
     * @return ID of the texture, the OpenGL handle for the texture.
     */
    GLuint getTexID (int tex_id)
    {
        return fboTextures[tex_id].texID();
    }

    /**
     * @brief Returns a pointer to a texture (attachment).
     * @param tex_id Position of the texture, as in the ith texture of the FBO.
     * @return Pointer to the texture.
     */
    Texture* getTexture (int tex_id)
    {
        return &fboTextures[tex_id];
    }

    /**
     * @return The texture type of the texture attachments.
     */
    GLenum textureType (void)
    {
        return texture_type;
    }

    /**
     * @brief Fills a texture with given data in an array of Bytes.
     * @param attach_id Attachment of texture to be filled.
     * @param data Pointer to the data.
     */
    void fillTexture (int attach_id, GLubyte *data)
    {
        cout << "bytes\n";
        fboTextures[attach_id].update(data);
    }

    /**
     * @brief Fill a texture with given data in an array of Floats.
     * @param attach_id Attachment of texture to be filled.
     * @param data Pointer to the data.
     */
    void fillTexture (int attach_id, GLfloat *data)
    {
        cout << "floats\n";
        fboTextures[attach_id].update(data);
    }


    /**
     * @brief Binds framebuffer object.
     */
    virtual void bind (void)
    {
        if (!is_binded)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, *fboID_sptr);
        }
        is_binded = true;
    }

    /**
     * @brief Bind framebuffer object and set render buffer to given attachment.
     * @param attachID The color attachment to be used for writing.
     */
    virtual void bindRenderBuffer (GLuint attachID)
    {
        bind();
        glDrawBuffer(GL_COLOR_ATTACHMENT0+attachID);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 2 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1)
    {
        bind();
        GLenum buffers[2] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1};
        glDrawBuffers(2, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 3 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2)
    {
        bind();
        GLenum buffers[3] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                             GL_COLOR_ATTACHMENT0+attachID2};
        glDrawBuffers(3, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 4 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     * @param attachID3 The fourth color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2, GLuint attachID3)
    {
        bind();
        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                             GL_COLOR_ATTACHMENT0+attachID2, GL_COLOR_ATTACHMENT0+attachID3};
        glDrawBuffers(4, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 5 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     * @param attachID3 The fourth color attachment to be used for writing.
     * @param attachID4 The fifth color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2, GLuint attachID3,
                                   GLuint attachID4)
    {
        bind();
        GLenum buffers[5] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                             GL_COLOR_ATTACHMENT0+attachID2, GL_COLOR_ATTACHMENT0+attachID3,
                             GL_COLOR_ATTACHMENT0+attachID4};
        glDrawBuffers(5, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 6 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     * @param attachID3 The fourth color attachment to be used for writing.
     * @param attachID4 The fifth color attachment to be used for writing.
     * @param attachID5 The sixth color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2, GLuint attachID3,
                                   GLuint attachID4, GLuint attachID5)
    {
        bind();
        GLenum buffers[6] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                             GL_COLOR_ATTACHMENT0+attachID2, GL_COLOR_ATTACHMENT0+attachID3,
                             GL_COLOR_ATTACHMENT0+attachID4, GL_COLOR_ATTACHMENT0+attachID5};
        glDrawBuffers(6, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 7 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     * @param attachID3 The fourth color attachment to be used for writing.
     * @param attachID4 The fifth color attachment to be used for writing.
     * @param attachID5 The sixth color attachment to be used for writing.
     * @param attachID6 The seventh color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2, GLuint attachID3,
                                   GLuint attachID4, GLuint attachID5, GLuint attachID6)
    {
        bind();
        GLenum buffers[7] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                             GL_COLOR_ATTACHMENT0+attachID2, GL_COLOR_ATTACHMENT0+attachID3,
                             GL_COLOR_ATTACHMENT0+attachID4, GL_COLOR_ATTACHMENT0+attachID5,
                             GL_COLOR_ATTACHMENT0+attachID6};
        glDrawBuffers(7, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to 8 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     * @param attachID3 The fourth color attachment to be used for writing.
     * @param attachID4 The fifth color attachment to be used for writing.
     * @param attachID5 The sixth color attachment to be used for writing.
     * @param attachID6 The seventh color attachment to be used for writing.
     * @param attachID7 The eigth color attachment to be used for writing.
     */
    virtual void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2, GLuint attachID3,
                                   GLuint attachID4, GLuint attachID5, GLuint attachID6, GLuint attachID7)
    {
        bind();
        GLenum buffers[8] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                             GL_COLOR_ATTACHMENT0+attachID2, GL_COLOR_ATTACHMENT0+attachID3,
                             GL_COLOR_ATTACHMENT0+attachID4, GL_COLOR_ATTACHMENT0+attachID5,
                             GL_COLOR_ATTACHMENT0+attachID6, GL_COLOR_ATTACHMENT0+attachID7};
        glDrawBuffers(8, buffers);
    }

    /**
     * @brief Bind framebuffer object and set render buffer to given array of buffers.
     * @param n Number of attachments to be used for writing.
     * @param buffers Array of attachments to be used for writing.
     */
    virtual void bindRenderBuffers (GLsizei n, GLuint* buffers)
    {
        bind();
        glDrawBuffers(n, buffers);
    }

    /**
     * @brief Unbinds framebuffer object.
     */
    virtual void unbindFBO (void)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        is_binded = false;
    }

    /**
     * @brief Unbinds fbo and all texture units in use.
     */
    void unbind (void)
    {
        unbindFBO();
        unbindAttachments();
        glDrawBuffer(GL_BACK);
    }

    /**
     * @brief Copy fbo attachment to another fbo with blit operation
     * @param copyfbo Pointer to destination FBO
     * @param source_attach Source attachment
     * @param dest_attach Destination attachment
     */
    void blitTo (Framebuffer& copyfbo, int source_attach = 0, int dest_attach = 0)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, copyfbo.getID());
        copyfbo.bindRenderBuffer(dest_attach);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_id);
        glReadBuffer (GL_COLOR_ATTACHMENT0 + source_attach); 

        glBlitFramebuffer(0, 0, size[0], size[1], 0, 0, copyfbo.getWidth(), copyfbo.getHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        copyfbo.unbind();
        unbind();
    }



    /**
     * @brief Returns the framebuffer's dimensions as a 2-component vector.
     * @return The framebuffer's dimensions.
     */
    Eigen::Vector2i getSize() const {
    	return size;
    }

    /**
     * @brief Clears all attachments with a given color.
     * @param clear_color Clear color (default is zero vector).
     */
    void clearAttachments (Eigen::Vector4f clear_color = Eigen::Vector4f::Zero())
    {
        bool was_binded = is_binded;

        bind();
        if (pixel_type == GL_RGBA32UI)
        {
            glClearColor(0, 0, 0, 0);
        }
        else
        {
            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        }
        for (unsigned int i = 0; i < fboTextures.size(); ++i)
        {
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        if (!was_binded)
        {
            unbindFBO();
        }
    }

    /**
     * @brief Clears a given attachments with a given color.
     * @param attachment Given texture attachment to be cleared.
     * @param clear_color Clear color (default is zero vector)
     */
    void clearAttachment (int attachment, Eigen::Vector4f clear_color = Eigen::Vector4f::Zero())
    {
        bool was_binded = is_binded;
        bind();
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachment);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!was_binded)
        {
            unbindFBO();
        }
    }

    /**
     * @brief Clears the FBO depthbuffer.
     */
    void clearDepth (void)
    {
        bool was_binded = is_binded;
        bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        if (!was_binded)
        {
            unbindFBO();
        }
    }


    /**
    * @brief Binds a texture attachment to a given texture unit.
    * @param attachment Number of color attachment.
    * @param texture_unit Number of unit to bind texture.
    */
    void bindAttachment (int attachment, int texture_unit)
    {
        fboTextures[attachment].bind(texture_unit);
    }

    /**
     * @brief Binds a texture attachment to the first free unit.
     * @param attachment Number of color attachment
     * @return Number of unit attached, or -1 if no unit available.
     */
    int bindAttachment (int attachment)
    {
        return fboTextures[attachment].bind();
    }

    /**
     * @brief Unbinds all texture attachments.
     */
    void unbindAttachments (void)
    {
        for (unsigned int i = 0; i < fboTextures.size(); ++i)
        {
            fboTextures[i].unbind();
        }
    }

    /**
     * @brief Sets the internal format of the FBO's texture.
     *
     * Default when created is GL_RGBA32F.
     * @param int_frm Internal format.
     */
    void setInternalFormat (GLenum int_frm)
    {
        internal_format = int_frm;
    }

    /**
     * @brief Sets format to read data to the FBO.
     *
     * Default when created is GL_RGBA32F.
     * @param in_format Given input data format.
     */
    void setInputFormat (GLenum in_format)
    {
        format = in_format;
    }

    /**
     * @brief Sets data type for reading pixels to the FBO.
     *
     * Default when created is GL_FLOAT.
     * @param in_type Given input data type.
     */
    void setInputType (GLenum in_type)
    {
        pixel_type = in_type;
    }

    /**
     * @brief Sets FBO texture type.
     *
     * Default when created is GL_TEXTURE_2D.
     * @param tex_type Given input data type.
     */
    void setTextureType (GLenum tex_type)
    {
        texture_type = tex_type;
    }

    /**
     * @brief Reads a pixel from a buffer and returns it as an Eigen vector.
     *
     * Assumes that a pixel has four GLfloat elements.
     * @param attach Buffer to be read, the id of the attachment.
     * @param pos Pixel position to be read.
     * @return Read pixel as a vector of four floats.
     */
    Eigen::Vector4f readPixel (int attach,  Eigen::Vector2i pos)
    {
        bool was_binded = is_binded;
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach);
        GLfloat pixel[4];
        glReadPixels(pos[0], pos[1], 1, 1, GL_RGBA, GL_FLOAT, &pixel[0]);
        if (!was_binded)
        {
            unbindFBO();
        }
        Eigen::Vector4f result (pixel[0], pixel[1], pixel[2], pixel[3]);
        return result;
    }

    /**
     * @brief Reads a GPU buffer and stores it in a CPU array of floats.
     *
     * Assumes each pixel has four GLfloat elements.
     * If `pixels` is not null, it is deallocated, then reallocated to ensure
     * the array as large enough to store the framebuffer's contents.
     *
     * @param attach_id Buffer to be read, the id of the attachment.
     * @param pixels Pointer to array of pixels.
     */
    void readBuffer (int attach_id,  GLfloat **pixels)
    {
        bool was_binded = is_binded;
        if (*pixels == NULL)
        {
        	*pixels = new GLfloat[bufferElements()];
        }
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach_id);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, *pixels);
        if (!was_binded)
        {
            unbindFBO();
        }
    }


    /**
    * @brief Reads a GPU buffer and stores it in a CPU array of unsigned bytes.
    *
    * Assumes each pixel has four GLBytes elements.
    * If `pixels` is not null, it is deallocated, then reallocated to ensure
    * the array as large enough to store the framebuffer's contents.
    *
    * @param attach_id Buffer to be read, the id of the attachment
    * @param pixels Pointer to array of pixels.
    */
    void readBuffer (int attach_id,  GLbyte ** pixels)
    {
        bool was_binded = is_binded;
        if (*pixels == NULL)
        {
        	*pixels = new GLbyte[bufferElements()];
        }
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach_id);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
        if (!was_binded)
        {
            unbindFBO();
        }
    }


    /**
     * @brief Reads a GPU buffer and stores it in a CPU array of unsigned char.
     *
     * Assumes each pixel has four unsigned char elements.
     * If `pixels` is not null, it is deallocated, then reallocated to ensure
     * the array as large enough to store the framebuffer's contents.
     *
     * @param attach_id Buffer to be read, the id of the attachment
     * @param pixels Pointer to array of pixels.
     */
    void readBuffer (int attach_id,  unsigned char ** pixels)
    {
        bool was_binded = is_binded;
        if (*pixels == NULL)
        {
        	*pixels = new unsigned char[bufferElements()];
        }
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach_id);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
        if (!was_binded)
        {
            unbindFBO();
        }
    }


    /**
     * @brief Reads a GPU buffer and stores it in a CPU vector of unsigned char.
     *
     * The vector is cleared prior to copying the framebuffer's contents.
     *
     * Assumes each pixel has four unsigned char elements.
     * @param attach_id Buffer to be read, the id of the attachment
     * @param pixels Vector of pixels.
     */
    void readBuffer (int attach_id, vector<unsigned char>& pixels)
    {
        bool was_binded = is_binded;
        pixels.clear();
        pixels.resize(bufferElements());
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach_id);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
        if (!was_binded)
        {
            unbindFBO();
        }
    }

    /**
     * @brief Reads a GPU buffer and stores it in a CPU vector of float.
     *
     * The vector is cleared prior to copying the framebuffer's contents.
     *
     * Assumes each pixel has four float elements.
     * @param attach_id Buffer to be read, the id of the attachment.
     * @param pixels Vector of float pixels.
     */
    void readBuffer (int attach_id, vector<float>& pixels)
    {
        bool was_binded = is_binded;
        pixels.clear();
        pixels.resize(bufferElements(), 0.0);
        bind();
        glFinish();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach_id);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, &pixels[0]);
        if (!was_binded)
        {
            unbindFBO();
        }
    }
//    void readBuffer (int attach_id, vector<float>& pixels, GLenum type = GL_RGBA)
//    {
//        bool was_binded = is_binded;
//        pixels.clear();
//        pixels.resize(bufferElements(), 0.0);
//        bind();
//        glFinish();
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//        glReadBuffer(GL_COLOR_ATTACHMENT0 + attach_id);
//        glReadPixels(0, 0, size[0], size[1], type, GL_FLOAT, &pixels[0]);
//        if (!was_binded)
//        {
//            unbindFBO();
//        }
//    }
    /**
	 * @brief Reads the depth buffer and stores it in a CPU vector of GLbyte.
	 * @param depth_values Vector of GLbyte pixels to receive depth values.
	 */
	void readDepthBuffer (vector<GLbyte> &depth_values)
	{
		depth_values.clear();
		depth_values.resize(depthBufferElements());
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindRenderbuffer(GL_RENDERBUFFER, *depthbufferID_sptr);
		glReadPixels(0, 0, size[0], size[1], GL_DEPTH_COMPONENT, GL_BYTE, &depth_values[0]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

    /**
     * @brief Reads the depth buffer and stores it in a CPU vector of float.
     * @param depth_values Vector of float pixels to receive depth values.
     */
    void readDepthBuffer (vector<float> & depth_values)
    {
        depth_values.clear();
        depth_values.resize(depthBufferElements());
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindRenderbuffer(GL_RENDERBUFFER, *depthbufferID_sptr);
        glReadPixels(0, 0, size[0], size[1], GL_DEPTH_COMPONENT, GL_FLOAT, &depth_values[0]);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

	/**
	 * @brief Saves the buffer to a PPM image
	 * @param filename Output ppm filename
	 * @param attach FBO attachment to save as image (default is 0)
	 */
	void saveAsPPM (string filename, int attach = 0)
	{
		ofstream out_stream;
		out_stream.open(filename.c_str());
		out_stream << "P3\n";
		out_stream << size[0] << " " << size[1] << "\n";
		out_stream << "255\n";

        bool was_binded = is_binded;

        GLfloat * pixels = new GLfloat[(int)(size[0]*size[1]*4)];
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0+attach);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, pixels);

		int pos;
	    for (int j = size[1]-1; j >= 0; --j)
        {
            for (int i = 0 ; i < size[0]; ++i)
            {
                pos = (i + size[0]*j)*4;
				out_stream << min(255, (int)(255*pixels[pos+0])) << " " << min(255, (int)(255*pixels[pos+1])) << " " << min(255, (int)(255*pixels[pos+2])) << " ";
			}
			out_stream << "\n";
		}
		out_stream.close();

	    if (!was_binded)
        {
            unbindFBO();
        }
		delete [] pixels;

	}

    /**
     * @brief Prints the content of a GPU. Usually used for debugging.
     *
     * Assumes each pixel has four GLfloat elements.
     * @param attach Number of buffer to be read
     * @param exception Pixel value that should not be printed, usually the background.
     */
    void printBuffer (int attach, Eigen::Vector4f exception = Eigen::Vector4f(0.0,0.0,0.0,0.0) )
    {
        bool was_binded = is_binded;

        GLfloat * pixels = new GLfloat[(int)(size[0]*size[1]*4)];
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0+attach);
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, pixels);

        int count = 0;
        Eigen::Vector4f min = Eigen::Vector4f::Constant(numeric_limits<float>::max());
        Eigen::Vector4f max = Eigen::Vector4f::Constant (numeric_limits<float>::min());

        int pos;
        Eigen::Vector4f pixel;
        for (int j = 0; j < size[1]; ++j)
        {
            for (int i = 0 ; i < size[0]; ++i)
            {
                pos = (i + size[0]*j)*4;
                pixel << pixels[pos+0], pixels[pos+1], pixels[pos+2], pixels[pos+3];
                if (pixel != exception)
                {
                    cout << "(" << i << "," << j << ") = [ " << pixel.transpose() << " ]" << endl;
                    count++;

                    for (int k = 0; k < 4; ++k)
                    {
                        if (pixel[k] < min[k])
                        {
                            min[k] = pixel[k];
                        }
                        if (pixel[k] > max[k])
                        {
                            max[k] = pixel[k];
                        }
                    }
                }
            }
        }
        delete [] pixels;

        if (!was_binded)
        {
            unbindFBO();
        }

        cout << endl << "info : " << endl;
        cout << "num valid pixels : " << count << endl;
        cout << "min values : " << min.transpose() << endl;
        cout << "max values : " << max.transpose() << endl;
        cout << "tex id : " << attach << endl;
    }


    /**
     * @brief Returns the fbo ID
     * @return FBO ID
     */
    GLuint getID (void)
    {
        return fbo_id;
    }

    /**
     * @brief Returns the dimensions of the FBO.
     * @return The dimensions of the FBO
     */
    Eigen::Vector2i getDimensions (void)
    {
        return size;
    }

    /**
     * @brief Returns the width of the FBO.
     * @return The width of the FBO
     */
    int getWidth (void)
    {
        return size[0];
    }

    /**
     * @brief Returns the height of the FBO.
     * @return The height of the FBO
     */
    int getHeight (void) {
        return size[1];
    }

    /**
     * @brief Returns the number of attachments.
     * @return Numbre of attachments.
     */
    int getNumAttachments (void) {
        return fboTextures.size();
    }

    /**
     * @brief Returns wether the FBO is currently binded or not.
     * @return True if FBO is binded, false otherwise.
     */
    bool isBinded (void)
    {
        return is_binded;
    }

protected:

    /**
     * @brief Creates the framebuffer and the depthbuffer.
     *
     * The viewport width and height are needed in order to create the FBO. The number of texture attachments may be greater than one.
     * Note, however, that all textures of the FBO must have the same size (unless it is a MipMap) and parameters.
     * @param viewportWidth The current viewport width.
     * @param viewportHeight The current viewport height.
     * @param numberOfTextures Number of output attachments
     */
    virtual void createFramebuffer (int viewportWidth, int viewportHeight, int numberOfTextures = 1)
    {
        is_binded = false;
        // update dimensions
        size << viewportWidth, viewportHeight;

        bind();

        // clearing textures that already exist
        fboTextures.clear();

        //Creating texture:
        fboTextures.resize( numberOfTextures );
        for (int i = 0; i < numberOfTextures; ++i)
        {
            createTexture(i);
        }

        //Depth Buffer Generation:
        glBindRenderbuffer(GL_RENDERBUFFER, *depthbufferID_sptr);
        if (num_samples == 1)
        {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size[0], size[1]);
        }
        else
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, num_samples, GL_DEPTH_COMPONENT, size[0], size[1]);
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depthbufferID_sptr);

        GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "FBO ERROR : " << status << std::endl;
        }

        unbind();

        //errorCheckFunc(__FILE__, __LINE__);
    }

    /**
     * @brief Generate the ith FBO texture as the ith color attachment.
     * @param attach_id Attachment holding the texture.
     */
    virtual void createTexture (int attach_id)
    {
        fboTextures[attach_id].setNumSamples(num_samples);
        
        fboTextures[attach_id].create(texture_type, internal_format, size[0], size[1], format, pixel_type);

        glBindTexture(texture_type, fboTextures[attach_id].texID());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attach_id, texture_type, fboTextures[attach_id].texID() , 0);
        glBindTexture(texture_type, 0);
    }

};




}
#endif
