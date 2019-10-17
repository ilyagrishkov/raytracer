#ifndef __TEXTUREMANAGER__
#define __TEXTUREMANAGER__

/// Defines our unique instance of this singleton class.
#define texManager TextureManager::Instance()

#include <set>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "tucano/utils/misc.hpp"


using namespace std;

namespace Tucano
{

/**
 * @brief Singleton class that manages texture unit allocation.
 *
 * When a texture is binded, this singleton will search for the first free slot to allocate it.
 * This removes the burden of managing texture units inside the program.
 */
class TextureManager {



	//Still got to think about an interesting way to set texture parameters here. Besides. got to see a way to decrease the number of parameters for the create texture method.

	//One problem: framebuffer has it's method for storing the free units. What will I do? Use the texture manager in the framebuffer?

	//I need to test the texturemanager in an application that uses more than one texture unit at a time.
	public:

	/** 
     * @brief Returns the unique instance. If no instace exists, it will create one (only once).
     */
    static TextureManager &Instance (void)
    {
      static TextureManager _instance;
      return _instance;
    }

	/**
     * @brief Binds a texture to a unit given by the user.
     */
    void bindTexture (GLenum texType, GLuint texID, int texture_unit)
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(texType, texID);

        if (used_units[texture_unit] != -1)
        {
            cerr << "WARNING: Texture unit already used. Replacing bound texture..." << endl;
        }

        used_units[texture_unit] = texID;
    }

	/**
     * @brief Binds a texture to the first free texture unit and returns the allocated unit.
     * @return Allocated unit, or -1 if no free unit is available.
     */
    int bindTexture (GLenum texType, GLuint texID)
    {
        int free_unit = getAvailableUnit();

        if (free_unit != -1)
        {
            glActiveTexture(GL_TEXTURE0 + free_unit);
            glBindTexture(texType, texID);
            used_units[free_unit] = texID;
        }
        else
        {
            cerr << "WARNING: no free texture unit found!\n";
        }

        return free_unit;
    }

    /**
     * @brief Returns the first available texture unit.
     */
    int getAvailableUnit (void)
    {
        for (int i = 0; i < max_texture_units; ++i)
        {
          if (used_units[i] == -1)
          {
            return i;
          }
        }
        return -1;
    }

    /**
     * @brief Sets one unit as unavaiable. in case something besides the texture manager has bound a texture to it.
     */
	void setUnavailableUnit(int unit);

	///Unbinds the texture from the specific texture unit.
    void unbindTexture(GLenum texType, int texture_unit)
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(texType, 0);
        used_units[texture_unit] = -1;
    }

    /**
     * @brief Unbinds a texture with given ID
     * Searches texture unit vector for the texture and frees unit
     * @param texType Texture type
     * @param texID The ID handler to given texture
     */
    void unbindTextureID(GLenum texType, GLuint texID)
    {
       for (int i = 0; i < max_texture_units; ++i) {
         if (used_units[i] == (GLint)texID) {
             glActiveTexture(GL_TEXTURE0 + i);
             glBindTexture(texType, 0);
             used_units[i] = -1;
         }
       }
   }

	//THAT'S SOMETHING IMPORTANT. SHOULDN'T REALLY I DELETE THE INSTANCE? WON'T THERE BE A MEMORY LEAKING THERE?
	~TextureManager() {};

private:

	//I need to define the constructor. the copy constructor and the operator= as private in order to ensure that only one Texture Manager can be created. through the use of the Instance() method.

	///Default Constructor
	TextureManager() {
		// get maximum number of texture units. and set all as not used
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

		for (int i = 0; i < max_texture_units; ++i) {
			used_units.push_back(-1);
		}
	}

	///Copy Constructor
	TextureManager(TextureManager const&) {}

	///Assignment Operation
	TextureManager& operator=(TextureManager const&);

	///Pointer to the instance of the Texture Manager object.
    static TextureManager* pInstance;

	/// Maximum number of texture units
	int max_texture_units;

	/// Texture units in use. each slot holds the texture id or -1 if free
	std::vector<int> used_units;

};

//Not sure why it is here.
//http://www.yolinux.com/TUTORIALS/C++Singleton.html says: "Global static pointer used to ensure a single instance of the class."
//Need to understand it better.
//static TextureManager* TextureManager::pInstance = NULL;


}


#endif
