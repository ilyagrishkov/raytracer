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

#ifndef __OBJIMPORTER__
#define __OBJIMPORTER__

#include "tucano/mesh.hpp"
#include "tucano/utils/mtlIO.hpp"

using namespace std;

namespace Tucano
{

namespace MeshImporter
{

#if _WIN32  //define something for Windows (32-bit and 64-bit, this part is common)
    #pragma warning(disable:4996)
#else
// avoid warnings of unused function
static void loadObjFile (vector<Tucano::Mesh>& meshes, vector<Tucano::Material::Mtl>& mtls, string filename);
#endif

static string getPathName(const string& s)
{
   size_t found = s.find_last_of("/\\");
   return s.substr(0, found+1);
}

static void computeNormals(const vector<Eigen::Vector4f>& vertices, const vector< vector<GLuint> > indices, vector<Eigen::Vector3f>& normals)
{
    for (int i = 0; i < vertices.size(); ++i)
        normals.push_back (Eigen::Vector3f::Zero());

    
    for (int id = 0; id < indices.size(); ++id)
    {
        for (int i = 0; i < indices[id].size(); i=i+3)
        {
            Eigen::Vector3f v1 = vertices[indices[id][i+2]].head(3) - vertices[indices[id][i]].head(3);
            Eigen::Vector3f v0 = vertices[indices[id][i+1]].head(3) - vertices[indices[id][i]].head(3);
            v0.normalize();
            v1.normalize();
            Eigen::Vector3f n = v0.cross(v1);
            n.normalize();
            normals[indices[id][i]] += n;
            normals[indices[id][i+1]] += n;
            normals[indices[id][i+2]] += n;
        }
    }

    for (int i = 0; i < normals.size(); ++i)
        normals[i].normalize();
}

/**
 * @brief Loads a mesh from an OBJ file.
 *
 * Loads vertex coordinates and normals, texcoords and color when available.
 * @param mesh Pointer to mesh instance to load file.
 * @param filename Given filename of the OBJ file.
 */
static void loadObjFile (Tucano::Mesh& mesh, vector<Tucano::Material::Mtl>& mtls, string filename)
{ 
    string path = getPathName(filename);

    vector<Eigen::Vector4f> vert;
    vector<Eigen::Vector3f> norm;
    vector<Eigen::Vector2f> texCoord;
    vector<Eigen::Vector4f> color;
    vector<int> elements_material_id;
    // array of indices (every material has its own id)
    vector < vector<GLuint> > elementsVertices;
    vector < vector<GLuint> > elementsNormals;
    vector < vector<GLuint> > elementsTexIDs;

    //Opening file:
    #ifdef TUCANODEBUG
    cout << "Opening Wavefront obj file " << filename.c_str() << endl << endl;
    #endif

    ifstream in(filename.c_str(),ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename.c_str() << endl; exit(1);
    }


    // create first indices array (we do not know if file uses materials or not yet)     
    elementsVertices.push_back( vector<GLuint>() );
    elementsNormals.push_back( vector<GLuint>() );
    elementsTexIDs.push_back( vector<GLuint>() );
    elements_material_id.push_back (-1);


    //Reading file:
    string line;
    int current_mat = -1;
    
    while(getline(in,line))
    {
        if(line.substr(0, 6) == "mtllib")
        {
            istringstream s(line.substr(7));
            
            //Tucano::MaterialImporter::loadMTL(mtls, path + line.substr(7)); 
            string mtlfn = path + line.substr(7);
            // remove newline or carriage return characters from the end
            mtlfn.erase(std::remove(mtlfn.begin(), mtlfn.end(), '\n'), mtlfn.end());
            mtlfn.erase(std::remove(mtlfn.begin(), mtlfn.end(), '\r'), mtlfn.end());
            Tucano::MaterialImporter::loadMTL(mtls, mtlfn);
        }
        else if(line.substr(0, 6) == "usemtl")
        {
            // check if last index array is still empty, otherwise create new vecs
            if (!elementsVertices.back().empty())
            {
                elementsVertices.push_back( vector<GLuint>() );
                elementsNormals.push_back( vector<GLuint>() );
                elementsTexIDs.push_back( vector<GLuint>() );
                elements_material_id.push_back (-1);
            }

            istringstream s(line.substr(7));
            for (int i = 0; i < mtls.size(); ++i)
            {
                if (mtls[i].getName().compare(line.substr(7)) == 0)
                {
                    current_mat = i;
                }
            } 
            elements_material_id.back() = current_mat;
                   
        }

        //Vertices reading:
        else if(line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            Eigen::Vector4f v;
            s >> v[0]; s >> v[1]; s >> v[2]; v[3] = 1.0f;
            vert.push_back(v);

            if(s.rdbuf()->in_avail())
            {
                Eigen::Vector4f c;
                s >> c[0]; s >> c[1]; s >> c[2]; c[3] = 1.0f;
                color.push_back(c);
            }
        }

        //Normals reading:
        else if(line.substr(0,2) == "vn")
        {
            istringstream s(line.substr(3));
            Eigen::Vector3f vn;
            s >> vn[0]; s >> vn[1]; s >> vn[2];            
            norm.push_back(vn);
        }

        //Texture Coordinates reading:
        else if(line.substr(0,2) == "vt")
        {
            istringstream s(line.substr(2));
            Eigen::Vector2f vt;
            s >> vt[0]; s >> vt[1];
            texCoord.push_back(vt);
        }

        //Elements reading: Elements are given through a string: "f vertexID/TextureID/NormalID". If no texture is given, then the string will be: "vertexID//NormalID".
        else if(line.substr(0,2) == "f ")
        {
            int vertexId, normalId, textureId;
            std::stringstream liness(line.substr(2));
            std::string buf, element;

            std::vector<std::string> face_element; // Create vector to hold our face elements
            while (liness >> buf)
                face_element.push_back(buf);

            // for each face element parse the vertex, texture and normal ids
            for (int i = 0; i < face_element.size(); ++i)
            {
                std::stringstream elementss (face_element[i]);
                std::getline(elementss, element, '/');
                vertexId = stoi(element);
                elementsVertices.back().push_back(vertexId-1);

                if (std::getline(elementss, element, '/'))
                
                if (element.compare("") != 0)
                {
                    textureId = stoi(element);
                    elementsTexIDs.back().push_back(textureId-1);
                }
                if (std::getline(elementss, element, '/'))
                if (element.compare("") != 0)
                {
                    normalId = stoi(element);
                    elementsNormals.back().push_back(normalId-1);
                }
            }
        }

        //Ignoring comment lines:
        else if(line[0] == '#') { }

        //Ignoring any other lines:
        else {};
    }
    if(in.is_open())
    {
        in.close();
    }    

    computeNormals(vert, elementsVertices, norm);

    // load attributes found in file in GL buffers
    // also store the original data
    if (vert.size() > 0)
    {
        mesh.loadVertices(vert);
        mesh.storeVertexData (vert);
    }
    if (norm.size() > 0)
    {
        mesh.loadNormals(norm);
        mesh.storeNormalData (norm);
    }

    if (texCoord.size() > 0)
    {
        mesh.loadTexCoords(texCoord);
        mesh.storeTexCoordData (texCoord);
    }
    if (color.size() > 0)
    {
        mesh.loadColors(color);
        mesh.storeColorData (color);
    }

    for (int i = 0; i < elementsVertices.size(); ++i)
    {        
        if (elementsVertices[i].size() > 0)
        {
            mesh.loadIndices(elementsVertices[i], elements_material_id[i]);
            mesh.storeVertexIdsData(elementsVertices[i]);
        }
    }

    mesh.createFaces();

    // sets the default locations for accesing attributes in shaders
    mesh.setDefaultAttribLocations();

    std::cout << "OBJ info:" << std::endl;
    std::cout << "number vertices : " << mesh.getNumberOfVertices() << std::endl;
    std::cout << "number faces : " << mesh.getNumberOfElements() << std::endl;
    std::cout << "number materials : " << mesh.getNumberOfMaterials() << std::endl;

    #ifdef TUCANODEBUG
    Misc::errorCheckFunc(__FILE__, __LINE__);
    #endif
}

}
}
#endif
