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

#ifndef __MTLIMPORTER__
#define __MTLIMPORTER__

#include "tucano/materials/mtl.hpp"

namespace Tucano
{

namespace MaterialImporter
{

static bool loadMTL (vector<Tucano::Material::Mtl>& materials, string filename);

static string getPathName(const string& s)
{
   size_t found = s.find_last_of("/\\");
   return s.substr(0, found+1);
}


/**
 * @brief Loads a MTL file (material for OBJ)
 *
 * @param filename Given filename of the PPM file.
 * @return True if loaded successfully, false otherwise
 */
static bool loadMTL (vector<Tucano::Material::Mtl>& materials, string filename)
{    
    string path = getPathName(filename);

    ifstream in(filename.c_str(),ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename.c_str() << std::endl; 
        return false;
    }
    else
    {
        std::cout << "Reading:" << filename.c_str() << std::endl; 
        for( std::string line; getline( in, line ); )
        {
            std::stringstream ss(line);

            if (ss.str().empty())
                continue;

            std::string s;
            vector < std::string > tokens;
            while (std::getline(ss, s, ' '))
            {
                tokens.push_back (s);
            }
            if (tokens[0].compare("#") == 0)
            {
                continue;
            }
            if (tokens[0].compare("newmtl") == 0)
            {            
                Tucano::Material::Mtl newmtl;
                materials.push_back(newmtl);
                materials.back().setName (tokens[1]);
            }
            else if (tokens[0].compare("Ns") == 0)
            {
                materials.back().setShininess(atof(tokens[1].c_str()));
            }
            else if (tokens[0].compare("Ka") == 0)
            {
                Eigen::Vector3f ka (atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));            
                materials.back().setAmbient(ka);
            }
            else if (tokens[0].compare("Kd") == 0)
            {
                Eigen::Vector3f kd (atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));            
                materials.back().setDiffuse(kd);
            }
            else if (tokens[0].compare("Ks") == 0)
            {
                Eigen::Vector3f ks (atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));            
                materials.back().setSpecular(ks);
            }
            else if (tokens[0].compare("Ni") == 0)
            {
                materials.back().setOpticalDensity( atof(tokens[1].c_str()) );
            }
            else if (tokens[0].compare("d") == 0)
            {
                materials.back().setDissolveFactor( atof(tokens[1].c_str()) );
            }
            else if (tokens[0].compare("illum") == 0)
            {
                materials.back().setIlluminationModel( atoi(tokens[1].c_str()) );
            }
            else if (tokens[0].compare("map_kd") == 0)
            {
                materials.back().setDiffuseTextureFilename( path + tokens[1] );
            }
        }
    }

    // if no mtllib then just create a default material
    if (materials.empty())
    {
        Tucano::Material::Mtl mtl;
        materials.push_back(mtl);
    }

    #ifdef TUCANODEBUG
    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
    #endif
    return true;
}


}
}
#endif
