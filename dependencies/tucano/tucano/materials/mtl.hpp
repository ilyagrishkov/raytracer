#ifndef __MTLMATERIAL__
#define __MTLMATERIAL__

#include <vector>
#include <Eigen/Dense>
#include "tucano/utils/ppmIO.hpp"

namespace Tucano
{


namespace Material
{
/**
 * 
 **/
class Mtl {

protected:

    /// Ambient coefficient
    Eigen::Vector3f ka = Eigen::Vector3f(0.3, 0.3, 0.3);

    /// Diffuse coefficient
    Eigen::Vector3f kd = Eigen::Vector3f(0.5, 0.5, 0.5);
    
    /// Specular coefficient
    Eigen::Vector3f ks = Eigen::Vector3f(1.0, 1.0, 1.0);

    /// Shininess    
    float shininess = 10;

    /// Optical density
    float optical_density = 0.0;

    /// Transparency factor
    float dissolve_factor = 1.0;

    /// illumination model
    int illumination_model = 0;

    /// texture for kd
    string diffuse_tex_filename;
    Texture diffuse_tex;

    /// material name
    string name;

public:

    /**
     * Default Constructor.
     */
    Mtl(void)
    {
    }

    /**
    * @brief Set ambient coefficient
    * @param value New ambient coeff (ka)
    */
    void setAmbient (Eigen::Vector3f value)
    {
        ka = value;
    }

    /**
    * @brief Set diffuse coefficient
    * @param value New diffuse coeff (kd)
    */
    void setDiffuse (Eigen::Vector3f value)
    {
        kd = value;
    }

    /**
    * @brief Set specular coefficient
    * @param New specular coeff (ks)
    */
    void setSpecular (Eigen::Vector3f value)
    {
        ks = value;
    }

    /**
    * @brief Set shininess exponent
    * @param New shininess coeff (shininess)
    */
    void setShininess (float value)
    {
        shininess = value;
    }

    /**
    * @brief Set optical density
    * @param New optical density
    */
    void setOpticalDensity (float value)
    {
        optical_density = value;
    }

    void setDissolveFactor (float value)
    {
        dissolve_factor = value;
    }

    void setIlluminationModel (int value) {illumination_model = value;}

    void setDiffuseTextureFilename (string tex) {
        diffuse_tex_filename = tex;
        //Tucano::ImageImporter::loadPPMImage (diffuse_tex_filename, &diffuse_tex);
    }

    void setName (string n) {name = n;}
    string getName (void) {return name;}

    Eigen::Vector3f getDiffuse (void ) const {return kd;}
    Eigen::Vector3f getAmbient (void ) const {return ka;}
    Eigen::Vector3f getSpecular (void ) const {return ks;}
    float getShininess (void ) const {return shininess;}
    float getOpticalDensity (void ) const {return optical_density;}
    float getDissolveFactor (void ) const {return dissolve_factor;}
    float getIlluminationModel (void ) const {return illumination_model;}
    string getDiffuseTextureFilename (void) const {return diffuse_tex_filename;}
    Texture& getDiffuseTexture (void ) {return diffuse_tex;}

};

}
}
#endif

