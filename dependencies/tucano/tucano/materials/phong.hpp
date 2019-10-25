#ifndef __PHONGMATERIAL__
#define __PHONGMATERIAL__

#include <vector>
#include <Eigen/Dense>
#include "tucano/materials/mtl.hpp"

namespace Tucano
{


namespace Material
{
/**
 * 
 **/
class Phong {

protected:

    /// Ambient coefficient
    Eigen::Vector3f ka = Eigen::Vector3f(0.3, 0.3, 0.3);

    /// Diffuse coefficient
    Eigen::Vector3f kd = Eigen::Vector3f(0.5, 0.5, 0.5);
    
    /// Specular coefficient
    Eigen::Vector3f ks = Eigen::Vector3f(1.0, 1.0, 1.0);

    /// Shininess    
    float shininess = 10;

    /// Diffuse texture map
    Texture diffuse_tex;

public:

    /**
     * Default Constructor.
     */
    Phong(void)
    {
    }

    void setFromMtl (const Tucano::Material::Mtl& mtl)
    {
        ka = mtl.getAmbient();
        kd = mtl.getDiffuse();
        ks = mtl.getSpecular();
        shininess = mtl.getShininess();
        if (mtl.getDiffuseTextureFilename() != "")
        {
            std::cout << "load diffuse tx " << mtl.getDiffuseTextureFilename().c_str() << std::endl;
            Tucano::ImageImporter::loadPPMImage (mtl.getDiffuseTextureFilename(), &diffuse_tex);
        }
        
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

    void loadDiffuseTexture (string tex) {
        Tucano::ImageImporter::loadPPMImage (tex, &diffuse_tex);
    }

    /**
    * @brief Set shininess exponent
    * @param New shininess coeff (shininess)
    */
    void setShininess (float value)
    {
        shininess = value;
    }

    Eigen::Vector3f getDiffuse (void ) {return kd;}
    Eigen::Vector3f getAmbient (void ) {return ka;}
    Eigen::Vector3f getSpecular (void ) {return ks;}
    float getShininess (void ) {return shininess;}    
    Texture& getDiffuseTexture (void ) {return diffuse_tex;}

};

}
}
#endif
