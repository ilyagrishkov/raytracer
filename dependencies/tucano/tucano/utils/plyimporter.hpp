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

#ifndef __PLYIMPORTER__
#define __PLYIMPORTER__

#include "tucano/mesh.hpp"
#include "tucano/utils/rply.hpp"


namespace Tucano
{
namespace MeshImporter
{
//#if _WIN32  //define something for Windows (32-bit and 64-bit, this part is common)
//    #pragma warning(disable:4996)
//#else
    // avoid warnings of unused function
	static bool loadPlyFile (Mesh* mesh, string filename);
    static void faceToVertexTexCoords (vector<unsigned int> &indices, vector<float> &face_tex_coords, std::vector<Eigen::Vector4f> &vertices, std::vector<Eigen::Vector3f> &normals, std::vector<Eigen::Vector4f> &colors, std::vector<Eigen::Vector2f> &tex_coords);
    static string getPlyTextureFile (string filename);
//#endif



    static int normal_cb( p_ply_argument argument )
    {
        static Eigen::Vector3f v;

        void* data;
        long coord;

        ply_get_argument_user_data( argument, &data, &coord );

        vector<Eigen::Vector3f>* vec = static_cast< vector<Eigen::Vector3f>* >( data );

        switch( coord )
        {
            case 0:
            case 1:
                v[coord] = ply_get_argument_value( argument );
                break;

            case 2:
                v[2] = ply_get_argument_value( argument );
                vec->push_back( v );
                break;
        }

        return 1;
    }

    static int color_cb( p_ply_argument argument )
    {
        static Eigen::Vector4f c;

        void* data;
        long coord;

        ply_get_argument_user_data( argument, &data, &coord );

        float channel = ply_get_argument_value( argument );
        if (channel > 1.0)
            channel /= 255.0;

        switch( coord )
        {
            case 0:
            case 1:
                c[coord] = channel;
                break;

            case 2:
                c[2] = channel;
                c[3] = 1.0;
                (static_cast< vector<Eigen::Vector4f>* >( data ))->push_back( c );
                break;
        }

        return 1;
    }

    static int vertex_cb( p_ply_argument argument )
    {
        static Eigen::Vector4f v;
        void* data;
        long coord;

        ply_get_argument_user_data( argument, &data, &coord );

        switch( coord )
        {
            case 0:
            case 1:
                v[coord] = ply_get_argument_value( argument );
                break;

            case 2:
                v[2] = ply_get_argument_value( argument );
                v[3] = 1.0;
                (static_cast< vector<Eigen::Vector4f>* >( data ))->push_back( v );
                break;
        }

        return 1;
    }


    static int face_cb( p_ply_argument argument )
    {
        long value_index;
        void* data;

        // 3rd parameter is the list length, which is always 3 for triangle meshes
        // if necessary it can be retrieved passing a reference to a long var
        ply_get_argument_property( argument, NULL, NULL, &value_index);
        ply_get_argument_user_data( argument, &data, NULL );

        if (value_index >= 0 && value_index < 3)
        {
            (static_cast< vector<unsigned int>* >(data))->push_back(ply_get_argument_value(argument));
        }

        return 1;
    }

    static int face_texcoords_cb( p_ply_argument argument )
    {
        long value_index;
        void* data;

        // 3rd parameter is the list length, which is always 6 for triangle meshes
        // if necessary it can be retrieved passing a reference to a long var
        ply_get_argument_property( argument, NULL, NULL, &value_index);
        ply_get_argument_user_data( argument, &data, NULL );

        if (value_index >= 0 && value_index < 6)
        {
            (static_cast< vector<float>* >(data))->push_back(ply_get_argument_value(argument));
        }

        return 1;
    }

    /**
     * @brief Gets the texture filename (if any) from the ply header
     *
     * @return string with texture filename if exists, empty string otherwise
     */ 
    static string getPlyTextureFile (string filename)
    {
        std::string texture_file;

        p_ply ply = ply_open( filename.c_str(), NULL, 0, NULL );
        if( !ply || !ply_read_header( ply ) )
        {
            std::cerr << "Cannot open " << filename.c_str() << std::endl;
            return texture_file;
        }

        const char * comment = nullptr;
        std::string tex_text ("TextureFile");
        // check all ply comments for TextureFile
        do 
        {
            comment = ply_get_next_comment( ply, comment);
            if (comment != nullptr)
            {
                std::string com (comment);
                // check if comment has TextureFile
                std::size_t found = com.find(tex_text);
                if (found != std::string::npos)
                {
                    texture_file = com.substr (tex_text.length() + 1);
                    #ifdef TUCANODEBUG
                    std::cout << "Texture file : " << texture_file.c_str() << std::endl;
                    #endif
                }
            }
        }        
        while (comment != nullptr);
        ply_close( ply );

        return texture_file;    
    }


    /**
     * @brief Loads a mesh from an PLY file.
     *
     * @param mesh Pointer to mesh instance to load file.
     * @param filename Given filename of the PLY file.
     */
    static bool loadPlyFile (Mesh *mesh, string filename)
    {
        p_ply ply = ply_open( filename.c_str(), NULL, 0, NULL );
        if( !ply || !ply_read_header( ply ) )
        {
            std::cerr << "Cannot open " << filename.c_str() << std::endl;
            return false;
        }

        #ifdef TUCANODEBUG
        std::cout << "Opening Stanford ply file " << filename.c_str() << std::endl << std::endl;
        #endif

        std::vector<Eigen::Vector4f> vertices;
        std::vector<Eigen::Vector3f> normals;
        std::vector<Eigen::Vector4f> colors;
        std::vector<Eigen::Vector2f> tex_coords;
        std::vector<unsigned int> indices;
        std::vector<float> face_tex_coords;

        ply_set_read_cb( ply, "vertex", "x", vertex_cb, ( void* )&vertices, 0 );
        ply_set_read_cb( ply, "vertex", "y", vertex_cb, ( void* )&vertices, 1 );
        ply_set_read_cb( ply, "vertex", "z", vertex_cb, ( void* )&vertices, 2 );

        ply_set_read_cb( ply, "vertex", "red", color_cb, ( void* )&colors, 0 );
        ply_set_read_cb( ply, "vertex", "green", color_cb, ( void* )&colors, 1 );
        ply_set_read_cb( ply, "vertex", "blue", color_cb, ( void* )&colors, 2 );

        ply_set_read_cb( ply, "vertex", "ny", normal_cb, ( void* )&normals, 1 );
        ply_set_read_cb( ply, "vertex", "nx", normal_cb, ( void* )&normals, 0 );
        ply_set_read_cb( ply, "vertex", "nz", normal_cb, ( void* )&normals, 2 );

        ply_set_read_cb(ply, "face", "vertex_indices", face_cb, &indices, 0);

        ply_set_read_cb(ply, "face", "texcoord", face_texcoords_cb, &face_tex_coords, 0);

        if( !ply_read( ply ) )
        {
            return false;
        }

        ply_close( ply );

        // convert from face tex coords to vertex face coords by replicating vertices
        if (face_tex_coords.size() > 0)
        {
            faceToVertexTexCoords (indices, face_tex_coords, vertices, normals, colors, tex_coords);
        }
   
        // load attributes found in file
        if (vertices.size() > 0)
            mesh->loadVertices(vertices);
        if (normals.size() > 0)
            mesh->loadNormals(normals);
        if (tex_coords.size() > 0)
            mesh->loadTexCoords(tex_coords);
        if (colors.size() > 0)
            mesh->loadColors(colors);
        if (indices.size() > 0)
            mesh->loadIndices(indices);


        // sets the default locations for accesing attributes in shaders
        mesh->setDefaultAttribLocations();


        #ifdef TUCANODEBUG
        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif

        return true;
    }

    /**
     * @brief Convert texture coordinates per face to texture coordinates per vertex
     * Vertices are repeated for each incident face
     * WARNING: load all vertex attributes before converting, since they will also be replicated
     * @param face_tex_coords Float vector containing tex coords per face
     */
    static void faceToVertexTexCoords (vector<unsigned int> &indices, vector<float> &face_tex_coords, std::vector<Eigen::Vector4f> &vertices, std::vector<Eigen::Vector3f> &normals, std::vector<Eigen::Vector4f> &colors, std::vector<Eigen::Vector2f> &tex_coords)
    {    
        std::vector <Eigen::Vector4f> vertices_tmp = vertices;
        std::vector <Eigen::Vector3f> normals_tmp = normals;
        std::vector <Eigen::Vector4f> colors_tmp = colors;
        std::vector <unsigned int> indices_tmp = indices;
        vertices.clear();
        normals.clear();
        colors.clear();
        indices.clear();
        tex_coords.clear();

        Eigen::Vector2f coord;
        // for each triangle (every three indices) replicate vertices and attributes
        for (unsigned int i = 0, j = 0; i < indices_tmp.size(); i=i+3, j=j+6)
        {   
            vertices.push_back (vertices_tmp[indices_tmp[i+0]]);
            vertices.push_back (vertices_tmp[indices_tmp[i+1]]);
            vertices.push_back (vertices_tmp[indices_tmp[i+2]]);
            if (normals_tmp.size() > 0)
            {
                normals.push_back (normals_tmp[indices_tmp[i+0]]);
                normals.push_back (normals_tmp[indices_tmp[i+1]]);
                normals.push_back (normals_tmp[indices_tmp[i+2]]);
            }
            if (colors_tmp.size() > 0)
            {
                colors.push_back (colors_tmp[indices_tmp[i+0]]);
                colors.push_back (colors_tmp[indices_tmp[i+1]]);
                colors.push_back (colors_tmp[indices_tmp[i+2]]);
            }
            coord << face_tex_coords[j+0], face_tex_coords[j+1];
            tex_coords.push_back (coord);
            coord << face_tex_coords[j+2], face_tex_coords[j+3];
            tex_coords.push_back (coord);
            coord << face_tex_coords[j+4], face_tex_coords[j+5];
            tex_coords.push_back (coord);
            indices.push_back(i+0);
            indices.push_back(i+1);
            indices.push_back(i+2);
        }

    }


}
}
#endif
