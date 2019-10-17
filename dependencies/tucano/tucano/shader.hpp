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

#ifndef __TUCANOSHADER__
#define __TUCANOSHADER__

#include "tucano/utils/misc.hpp"

#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include <memory>

using namespace std;

#define TUCANODEBUG

namespace Tucano
{

/**
 * @brief A Shader object represents one GLSL program.
 *
 * One object can store either the standard rendering pipeline shaders (vertex, geometry, fragment...) or a group of
 * compute shaders. For convenience, it also stores a user defined name, making it easier to access the shaders from within the main program.
 * The shader's name is the same as the shaders filenames, without the extensions.
 */
class Shader {

private:

    /// Stores an user mainteined identification for the shader. If the shader is created with the autoloader, the name is equal to the filename, without extensions.
    string shaderName;

    /// Stores the path to the vertex shader file.
    string vertexShaderPath;

    /// Stores the path to the tessellation control shader file.
    string tessellationControlShaderPath;

    /// Stores the path to the tessellation evaluation shader file.
    string tessellationEvaluationShaderPath;

    /// Stores the path to the geometry shader file.
    string geometryShaderPath;

    /// Stores the path to the fragment shader file.
    string fragmentShaderPath;

    /// Stores the paths to the compute shaders files.
    string computeShaderPath;

    /// Vertex shader code
    string vertex_code;

    /// Fragment shader code
    string fragment_code;

    /// Geometry shader code
    string geometry_code;
    
    /// Tesselation evaluation code
    string tessellation_evaluation_code;
    
    /// Tesselation control code
    string tessellation_control_code;

    /// Compute shader code
    string compute_shader_code;

    /// Compute shader identifications.
    GLuint computeShader = 0;

    /// Vertex Shader identification.
    GLuint vertexShader = 0;

    /// Tessellation Control Shader identification.
    GLuint tessellationControlShader = 0;

    /// Tessellation Evaluation Shader identification.
    GLuint tessellationEvaluationShader = 0;

    /// Geometry Shader identification.
    GLuint geometryShader = 0;

    /// Fragment Shader identification.
    GLuint fragmentShader = 0;

    /// Shader program identification.
    GLuint shaderProgram = 0;

    /// Debug level for outputing warnings and messages
    int debug_level = 1;

    /// Shared pointer for program ID
    std::shared_ptr < GLuint > programID_sptr = 0;
    std::shared_ptr < GLuint > vertexID_sptr = 0;
    std::shared_ptr < GLuint > fragID_sptr = 0;
    std::shared_ptr < GLuint > geomID_sptr = 0;
    std::shared_ptr < GLuint > tessEvalID_sptr = 0;
    std::shared_ptr < GLuint > tessContID_sptr = 0;
    std::shared_ptr < GLuint > computeID_sptr = 0;

public:

    /**
     * @brief Copy Contructor
     * Copies the shader codes and recompiles to generate new program
     */
    /*Shader (const Shader& other)
    {
        shaderName = other.shaderName;
        vertexShaderPath = other.vertexShaderPath;
        fragmentShaderPath = other.fragmentShaderPath;
        geometryShaderPath = other.geometryShaderPath;
        tessellationControlShaderPath = other.tessellationControlShaderPath;
        tessellationEvaluationShaderPath = other.tessellationEvaluationShaderPath;
        computeShaderPaths = other.computeShaderPaths;

        vertex_code = other.vertex_code;
        fragment_code = other.fragment_code;
        geometry_code = other.geometry_code;
        tessellation_evaluation_code = other.tessellation_evaluation_code;
        tessellation_control_code = other.tessellation_control_code;

        debug_level = other.debug_level;

        initializeFromStrings(vertex_code, fragment_code, geometry_code, tessellation_evaluation_code, tessellation_control_code);
        
        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif
    }*/
    
    /**
     * @brief Creates a program ID and sets a shared pointer to monitor it
     */
    void createProgramID (void)
    {

        shaderProgram = glCreateProgram();
        programID_sptr = std::shared_ptr < GLuint > ( 
                    new GLuint (shaderProgram),
                    [] (GLuint *p) {
                        glDeleteProgram(*p);
                        delete p;
                    }
                    );

    }

    /**
     * @brief Create the necessary shaders
     */
    void createShaders (void)
    {
     
        if (!(fragmentShaderPath.empty() && fragment_code.empty()) && fragID_sptr == 0)
        {   
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            fragID_sptr = std::shared_ptr < GLuint > ( 
                        new GLuint (fragmentShader),
                        [] (GLuint *p) {
                            glDeleteShader(*p);
                            delete p;
                        }
                        );
        }

        if (!(vertexShaderPath.empty() && vertex_code.empty()) && vertexID_sptr == 0)
        {   
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            vertexID_sptr = std::shared_ptr < GLuint > ( 
                        new GLuint (vertexShader),
                        [] (GLuint *p) {
                            glDeleteShader(*p);
                            delete p;
                        }
                        );
        }

        if (!(geometryShaderPath.empty() && geometry_code.empty()) && geomID_sptr == 0)
        {   
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            geomID_sptr = std::shared_ptr < GLuint > ( 
                        new GLuint (geometryShader),
                        [] (GLuint *p) {
                            glDeleteShader(*p);
                            delete p;
                        }
                        );
        }


        if (!(tessellationControlShaderPath.empty() && tessellation_control_code.empty()) && tessContID_sptr == 0)
        {   
            tessellationControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
            tessContID_sptr = std::shared_ptr < GLuint > ( 
                        new GLuint (tessellationControlShader),
                        [] (GLuint *p) {
                            glDeleteShader(*p);
                            delete p;
                        }
                    );
        }

        if (!(tessellationEvaluationShaderPath.empty() && tessellation_evaluation_code.empty()) && tessEvalID_sptr == 0)
        {   
            tessellationEvaluationShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
            tessEvalID_sptr = std::shared_ptr < GLuint > ( 
                        new GLuint (tessellationEvaluationShader),
                        [] (GLuint *p) {
                            glDeleteShader(*p);
                            delete p;
                        }
                    );
        }
        
        
        if (!(computeShaderPath.empty() && compute_shader_code.empty()) && computeID_sptr == 0)
        {   
            computeShader = glCreateShader(GL_COMPUTE_SHADER);
            computeID_sptr = std::shared_ptr < GLuint > ( 
                        new GLuint (computeShader),
                        [] (GLuint *p) {
                            glDeleteShader(*p);
                            delete p;
                        }
                    );
        }


        #ifdef TUCANODEBUG
        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "mesh constructor");
        #endif

    }

    /**
     * @brief Constructor that receives the path for each shader separately.
     *
     * It must receive a vertex and a fragment shader, and optionally can receive a geometry shader.
     * @param name The string to be set as shader identification. User mantained.
     * @param vertex_shader_path String giving the path to the external file containing the vertex shader.
     * @param fragment_shader_path String giving the path to the external file containing the fragment shader.
     * @param geometry_shader_path String giving the path to the external file containing the geometry shader.
     * @param tessellation_evaluation_shader_path String giving the path to the external file containing the tessellation evaluation shader.
     * @param tessellation_control_shader_path String giving the path to the external file containing the tessellation control shader.
     */
    Shader (string name, string vertex_shader_path, string fragment_shader_path, string geometry_shader_path = "", string tessellation_evaluation_shader_path = "", string tessellation_control_shader_path = "")
    {
        shaderName = name;
        vertexShaderPath = vertex_shader_path;
        tessellationControlShaderPath = tessellation_control_shader_path;
        tessellationEvaluationShaderPath = tessellation_evaluation_shader_path;
        geometryShaderPath = geometry_shader_path;
        fragmentShaderPath = fragment_shader_path;

        createProgramID();
    }

    /**
     * @brief Constructors that searches a given directory for shaders with given name.
     *
     * Receives a directory and a shader name, searches for files with the same name and extesions vert, frag, geom and comp to auto load shaders.
     * @param shader_dir Directory containing shaders.
     * @param name Shader name, must be the same as the files name without extensions.
     */
    Shader (string name, string shader_dir)
	{
		load (name, shader_dir);
        createProgramID();
    }


    /**
     * @brief Empty constructor.
     */
    Shader() 
    {
        createProgramID();
    }

    /**
     * This constructor receives the path to the compute shader 
     * @param name The string as the shader name 
     * @param compute_shader_path String with the path to the external files containing the compute shader.
     */
    /*Shader (string name, string compute_shader_path)
    {
        shaderName = name;
        computeShaderPath = compute_shader_path;
        createProgramID();
    }*/

    /**
     * Default destructor.
     */
    ~Shader (void)
    {
        //deleteShaders();

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "Shader destructor");
        #endif
    }

	/**
	* @brief Sets the shader name, very useful for debugging
	* @param name Shader name
	*/
	void setShaderName (string name)
	{
		shaderName = name;
	}

    /**
     * @brief Returns a string with the shader name.
     *
     * If the shader was created with the auto-loader, the name is the same as the shader file name. If not, it's user manteined.
     * @return The shader name.
     */
    string getShaderName (void)
    {
        return shaderName;
    }

    /**
     * @brief Returns the program identification handle.
     * @return Identification handle.
     */
    GLuint getShaderProgram (void)
    {
        if (programID_sptr)
            return *programID_sptr;
        return 0;
    }

    /**
     * @brief Returns a handle to the fragment shader.
     * @return The Fragment Shader identification handle.
     */
    GLuint getFragmentShader (void)
    {
        if (fragID_sptr)
            return *fragID_sptr;
        return 0;
    }

    /**
     * @brief Returns a handle to the vertex shader.
     * @return The Vertex Shader identification handle.
     */
    GLuint getVertexShader (void)
    {
        if (vertexID_sptr)
            return *vertexID_sptr;
        return 0;
    }

    /**
     * @brief Returns a handle to the geometry shader.
     * @return Return the Geometry Shader identification handle.
     */
    GLuint getGeometryShader (void)
    {
        if (geomID_sptr)
            return *geomID_sptr;
        return 0;
    }

    /**
     * @brief Returns a handle to the tessellation control shader.
     * @return Return the Tessellation Control Shader identification handle.
     */
    GLuint getTessellationControlShader (void)
    {
        if (tessContID_sptr)
            return *tessContID_sptr;
        return 0;
    }


    /**
     * @brief Returns a handle to the tessellation evaluation shader.
     * @return Return the Tessellation Evaluation Shader identification handle.
     */
    GLuint getTessellationEvaluationShader (void)
    {
        if (tessEvalID_sptr)
            return *tessEvalID_sptr;
        return 0;
    }



    /**
   * @return Return the Compute Shader identification handle.
   **/
    GLuint getComputeShader (void) 
    {
        if (computeID_sptr)
            return *computeID_sptr;
        return 0;
    }


public:

 
	/**
	 * @brief Loads a shader given a directory and a name. Searches for all shader
	 * extensions in directory.
	 *
     * Receives a directory and a shader name, searches for files with the same name and extesions vert, frag, geom and comp to auto load shaders.
     * @param shader_dir Directory containing shaders.
     * @param name Shader name, must be the same as the files name without extensions.
	 */
	void load (string name, string shader_dir = "")
	{
	    shaderName = name;

        bool found = false;

        //Vertex:
        string vs_name = shader_dir + name + ".vert";
        ifstream vertex_file(vs_name.c_str());
        if (vertex_file.good())
        {
            vertexShaderPath = vs_name;
            found = true;
        }

        //Tessellation Control:
        string tesc_name = shader_dir + name + ".tesc";
        ifstream tesc_file(tesc_name.c_str());
        if (tesc_file.good())
        {
            tessellationControlShaderPath = tesc_name;
            found = true;
        }

        //Tessellation Evaluation:
        string tese_name = shader_dir + name + ".tese";
        ifstream tese_file(tese_name.c_str());
        if (tese_file.good())
        {
            tessellationEvaluationShaderPath = tese_name;
            found = true;
        }

        //Geometry:
        string gs_name = shader_dir + name + ".geom";
        ifstream geom_file(gs_name.c_str());
        if (geom_file.good())
        {
            geometryShaderPath = gs_name;
            found = true;
        }

        //Fragment:
        string fs_name = shader_dir + name + ".frag";
        ifstream fragment_file(fs_name.c_str());
        if (fragment_file.good())
        {
            fragmentShaderPath = fs_name;
            found = true;
        }

        //Compute:
        string cs_name = shader_dir + name + ".comp";
        ifstream comp_file(cs_name.c_str());
        if (comp_file.good())
        {
            computeShaderPath = cs_name;
            found = true;
        }

        // if no shader was found, emit an warning
        if (!found)
        {
            std::cerr << "Warning: no shader " << name.c_str() << " file found in directory : " << shader_dir.c_str() << std::endl;
        }

	}


   /**
     * @brief Link shader program and check for link errors.
     */
    void linkProgram (void)
    {

        glLinkProgram(*programID_sptr);

        GLint result = GL_FALSE;
        glGetProgramiv(*programID_sptr, GL_LINK_STATUS, &result);
        if (result != GL_TRUE)
        {
            std::cerr << "Error linking program : " << shaderName << std::endl;
            GLchar errorLog[1024] = {0};
            glGetProgramInfoLog(*programID_sptr, 1024, NULL, errorLog);
            fprintf(stdout, "%s", &errorLog[0]);
            std::cerr << std::endl;
        }

        #ifdef TUCANODEBUG
        else
        {
            std::cout << " Successfully linked : " << shaderName << std::endl << std::endl;
        }
        #endif
    }


    /**
     * @brief Initializes shader and prepares it to use Transform Feedback.
     *
     * To use Transform Feedbacks somes parameters must be set before linking the program.
     * These parameters are the number and list of varyings to be set by the TF, and the type of buffer.
     * @param size Number of Transform Feedback varyings.
     * @param varlist List of varyings for TF.
     * @param buffer_mode Can be interleaved attributes or separate attributes (default is GL_INTERLEAVED_ATTRIBS).
     */
    void initializeTF (int size, const char** varlist, GLenum buffer_mode = GL_INTERLEAVED_ATTRIBS)
    {
        createShaders();
        if(!vertexShaderPath.empty())
        {
            readVertexCode();
            // tessellation control shader needs a vertex shader
            if (!tessellationControlShaderPath.empty())
            {
                readTessellationControlCode();
            }
            // tessellation evaluation shader needs a vertex shader
            if (!tessellationEvaluationShaderPath.empty())
            {
                readTessellationEvaluationCode();
            }
            // geom shader needs a vertex shader
            if(!geometryShaderPath.empty())
            {
                readGeometryCode();
            }
        }
        if(!fragmentShaderPath.empty())
        {
            readFragmentCode();
        }
        if(!computeShaderPath.empty())
        {
            readComputeShaderCode();
        }

        glTransformFeedbackVaryings(*programID_sptr, size, varlist, buffer_mode);

        linkProgram();

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif
    }

    /**
     * @brief Initializes shader directly from string, no files.
     * @param vertex_code String containing vertex code.
     * @param fragment_code String containing fragment code.
     * @param geometry_code String containing geometry code, this is optional.
     * @param tessellation_evaluation_code String containing tessellation evaluation code, this is optional.
     * @param tessellation_control_code String containing tessellation control code, this is optional.
     */
    void initializeFromStrings (string in_vertex_code, string in_fragment_code, string in_geometry_code = "", string in_tessellation_evaluation_code = "", string in_tessellation_control_code = "")
    {
        vertex_code = in_vertex_code;
        fragment_code = in_fragment_code;
        geometry_code = in_geometry_code;
        tessellation_evaluation_code = in_tessellation_evaluation_code;
        tessellation_control_code = in_tessellation_control_code;

        createShaders();

        if (vertex_code.empty())
        {
            std::cerr << "warning: " << shaderName.c_str() << " : empty vertex string code!" << std::endl;
        }
        else
        {
            setVertexShader(vertex_code);
            // tessellation control shader is optional, but needs needs a vertex shader
            if (!tessellationControlShaderPath.empty())
            {
                setTessellationControlShader(tessellation_control_code);
            }
            // tessellation evaluation shader is optional, but needs a vertex shader
            if (!tessellationEvaluationShaderPath.empty())
            {
                setTessellationEvaluationShader(tessellation_evaluation_code);
            }
            // geometry shader is optional, but need a vertex shader if is set
            if (!geometry_code.empty())
            {
                setGeometryShader(geometry_code);
            }
        }
        if (fragment_code.empty())
        {
            std::cerr << "warning: " << shaderName.c_str() << " : empty fragment string code!" << std::endl;
        }
        else
        {
            setFragmentShader(fragment_code);
        }

        linkProgram();

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif
    }

    /**
     * @brief Calls all the functions related to the shader initialization, i.e., creates, loads the shaders from the external files and links the shader program.
     */
    void initialize (void)
    {
        createShaders();
        if(!vertexShaderPath.empty())
        {
            readVertexCode();
            // tessellation control shader needs a vertex shader
            if (!tessellationControlShaderPath.empty())
            {
                readTessellationControlCode();
            }
            // tessellation evaluation shader needs a vertex shader
            if (!tessellationEvaluationShaderPath.empty())
            {
                readTessellationEvaluationCode();
            }
            // geom shader needs a vertex shader
            if(!geometryShaderPath.empty()) {
                readGeometryCode();
            }
        }
        if(!fragmentShaderPath.empty())
        {
            readFragmentCode();
        }
        if(!computeShaderPath.empty())
        {
            readComputeShaderCode();
        }        

        linkProgram();

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif
    }

    /**
     * @brief Loads vertex code into shader program.
     * @param vertexShaderCode String containing code
     */
    void setVertexShader (string &vertexShaderCode)
    {
        GLint result = GL_FALSE;
        int infoLogLength;

        char const * vertexSourcePointer = vertexShaderCode.c_str();
        glShaderSource(*vertexID_sptr, 1, &vertexSourcePointer , NULL);
        glCompileShader(*vertexID_sptr);

        // Check Vertex Shader
        glGetShaderiv(*vertexID_sptr, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE)
        {
            // if an error is found, print the log (even if not in debug mode)
            std::cerr << "Erro compiling vertex shader: " << vertexShaderPath << std::endl;
            glGetShaderiv(*vertexID_sptr, GL_INFO_LOG_LENGTH, &infoLogLength);
            char * vertexShaderErrorMessage = new char[infoLogLength];
            glGetShaderInfoLog(*vertexID_sptr, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
            fprintf(stdout, "\n%s", &vertexShaderErrorMessage[0]);
            delete [] vertexShaderErrorMessage;
        }
        // #ifdef TUCANODEBUG
        // else
        // {
        //     if (vertexShaderPath.empty())
        //     {
        //         std::cout << "Compiled vertex shader from string without errors : " << shaderName.c_str() << std::endl;
        //     }
        //     else
        //     {
        //         std::cout << "Compiled vertex shader without errors : " << vertexShaderPath << std::endl;
        //     }
        // }
        // #endif

        glAttachShader(*programID_sptr, *vertexID_sptr);

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error loading vertex shader code");
        #endif

    }

    /**
     * @brief Reads the external file containing the vertex shader and loads it into the shader program.
     */
    void readVertexCode (void)
    {
        // Read the Vertex Shader code from the file
        string vertexShaderCode;

        ifstream vertexShaderStream(vertexShaderPath.c_str(), std::ios::in);

        if(vertexShaderStream.is_open())
        {
            string line = "";
            while(getline(vertexShaderStream, line))
            {
                vertexShaderCode += "\n" + line;
            }
            vertexShaderStream.close();
        }
        else
        {
            std::cout << "warning: no vertex shader file found : " << vertexShaderPath << std::endl;
        }

        setVertexShader(vertexShaderCode);
        vertex_code = vertexShaderCode;
    }

    /**
     * @brief Loads tessellation control code into shader program.
     * @param tessellationControlShaderCode String containing code
     */
    void setTessellationControlShader(string &tessellationControlCode)
    {
        GLint result = GL_FALSE;
        int infoLogLength;

        char const * tessellationControlSourcePointer = tessellationControlCode.c_str();
        glShaderSource(*tessContID_sptr, 1, &tessellationControlSourcePointer , NULL);
        glCompileShader(*tessContID_sptr);

        // Check Vertex Shader
        glGetShaderiv(*tessContID_sptr, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE)
        {
            // if an error is found, print the log (even if not in debug mode)
            std::cerr << "Erro compiling tessellation evaluation shader: " << tessellationControlShaderPath << std::endl;
            glGetShaderiv(*tessContID_sptr, GL_INFO_LOG_LENGTH, &infoLogLength);
            char * tessellationControlShaderErrorMessage = new char[infoLogLength];
            glGetShaderInfoLog(*tessContID_sptr, infoLogLength, NULL, &tessellationControlShaderErrorMessage[0]);
            fprintf(stdout, "\n%s", &tessellationControlShaderErrorMessage[0]);
            delete [] tessellationControlShaderErrorMessage;
        }
        // #ifdef TUCANODEBUG
        // else
        // {
        //     if (tessellationControlShaderPath.empty())
        //     {
        //         std::cout << "Compiled tessellation control shader from string without errors : " << shaderName.c_str() << std::endl;
        //     }
        //     else
        //     {
        //         std::cout << "Compiled tessellation control shader without errors : " << tessellationControlShaderPath << std::endl;
        //     }
        // }
        // #endif

        glAttachShader(*programID_sptr, *tessContID_sptr);

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error loading tessellation control shader code");
        #endif
    }

    /**
     * @brief Reads the external file containing the tessellation control shader and loads it into the shader program.
     */
    void readTessellationControlCode(void)
    {
        // Read the tessellation control Shader code from the file
        string tessellationControlShaderCode;

        ifstream tessellationControlShaderStream(tessellationControlShaderPath.c_str(), std::ios::in);
        if(tessellationControlShaderStream.is_open())
        {
            string line = "";
            while(getline(tessellationControlShaderStream, line))
            {
                tessellationControlShaderCode += "\n" + line;
            }
            tessellationControlShaderStream.close();
        }
        else
        {
            std::cout << "warning: no tessellation control shader file found : " << tessellationControlShaderPath << std::endl;
        }

        setTessellationControlShader(tessellationControlShaderCode);
        tessellation_control_code =  tessellationControlShaderCode;
    }

    /**
     * @brief Loads tessellation evaluation code into shader program.
     * @param tessellationEvaluationShaderCode String containing code
     */
    void setTessellationEvaluationShader(string &tessellationEvaluationCode)
    {
        GLint result = GL_FALSE;
        int infoLogLength;

        char const * tessellationEvaluationSourcePointer = tessellationEvaluationCode.c_str();
        glShaderSource(*tessEvalID_sptr, 1, &tessellationEvaluationSourcePointer , NULL);
        glCompileShader(*tessEvalID_sptr);

        // Check Vertex Shader
        glGetShaderiv(*tessEvalID_sptr, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE)
        {
            // if an error is found, print the log (even if not in debug mode)
            std::cerr << "Erro compiling tessellation evaluation shader: " << tessellationEvaluationShaderPath << std::endl;
            glGetShaderiv(*tessEvalID_sptr, GL_INFO_LOG_LENGTH, &infoLogLength);
            char * tessellationEvaluationShaderErrorMessage = new char[infoLogLength];
            glGetShaderInfoLog(*tessEvalID_sptr, infoLogLength, NULL, &tessellationEvaluationShaderErrorMessage[0]);
            fprintf(stdout, "\n%s", &tessellationEvaluationShaderErrorMessage[0]);
            delete [] tessellationEvaluationShaderErrorMessage;
        }
        // #ifdef TUCANODEBUG
        // else
        // {
        //     if (tessellationEvaluationShaderPath.empty())
        //     {
        //         std::cout << "Compiled tessellation evaluation shader from string without errors : " << shaderName.c_str() << std::endl;
        //     }
        //     else
        //     {
        //         std::cout << "Compiled tessellation evaluation shader without errors : " << tessellationEvaluationShaderPath << std::endl;
        //     }
        // }
        // #endif

        glAttachShader(*programID_sptr, *tessEvalID_sptr);

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error loading tessellation evaluation shader code");
        #endif
    }

    /**
     * @brief Reads the external file containing the tessellation evaluation shader and loads it into the shader program.
     */
    void readTessellationEvaluationCode(void)
    {
        // Read the Vertex Shader code from the file
        string tessellationEvaluationShaderCode;

        ifstream tessellationEvaluationShaderStream(tessellationEvaluationShaderPath.c_str(), std::ios::in);

        if(tessellationEvaluationShaderStream.is_open())
        {
            string line = "";
            while(getline(tessellationEvaluationShaderStream, line))
            {
                tessellationEvaluationShaderCode += "\n" + line;
            }
            tessellationEvaluationShaderStream.close();
        }
        else
        {
            std::cout << "warning: no tessellation evaluation shader file found : " << tessellationEvaluationShaderPath << std::endl;
        }

        setTessellationEvaluationShader(tessellationEvaluationShaderCode);
        tessellation_evaluation_code = tessellationEvaluationShaderCode;
    }

    /**
     * @brief Loads geometry code into shader program.
     * @param geometryShaderCode String containing geometry code.
     */
    void setGeometryShader(string &geometryShaderCode)
    {
        GLint result = GL_FALSE;
        int infoLogLength;

        char const * geometrySourcePointer = geometryShaderCode.c_str();
        glShaderSource(*geomID_sptr, 1, &geometrySourcePointer , NULL);
        glCompileShader(geometryShader);

        // Check Geometry Shader
        glGetShaderiv(*geomID_sptr, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE)
        {
            // if an error is found, print the log (even if not in debug mode)
            std::cerr << "Erro compiling geometry shader: " << geometryShaderPath << std::endl;
            glGetShaderiv(*geomID_sptr, GL_INFO_LOG_LENGTH, &infoLogLength);
            char * geometryShaderErrorMessage = new char[infoLogLength];
            glGetShaderInfoLog(*geomID_sptr, infoLogLength, NULL, &geometryShaderErrorMessage[0]);
            fprintf(stdout, "\n%s", &geometryShaderErrorMessage[0]);
            delete [] geometryShaderErrorMessage;
        }
        // #ifdef TUCANODEBUG
        // else
        // {
        //     if (geometryShaderPath.empty())
        //     {
        //         std::cout << "Compiled geometry shader from string without errors : " << shaderName.c_str() << std::endl;
        //     }
        //     else
        //     {
        //         std::cout << "Compiled geometry shader without errors : " << geometryShaderPath << std::endl;
        //     }
        // }
        // #endif

        glAttachShader(*programID_sptr, *geomID_sptr);

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error loading geometry shader code");
        #endif

    }

    /**
     * @brief Reads the external file containing the geometry shader and loads it into the shader program.
     */
    void readGeometryCode (void)
    {
        // Read the Geometry Shader code from the file
        string geometryShaderCode;

        ifstream geometryShaderStream(geometryShaderPath.c_str(), std::ios::in);

        if(geometryShaderStream.is_open())
        {
            string line = "";
            while(getline(geometryShaderStream, line))
            {
                geometryShaderCode += "\n" + line;
            }
            geometryShaderStream.close();
        }
        else
        {
            std::cerr << "warning: no geom shader found : " << geometryShaderPath << std::endl;
        }

        setGeometryShader(geometryShaderCode);
        geometry_code = geometryShaderCode;
    }

    /**
     * @brief Loads fragment code into shader program.
     * @param fragmentShaderCode String containing fragment code.
     */
    void setFragmentShader (string& fragmentShaderCode)
    {
        GLint result = GL_FALSE;
        int infoLogLength;

        char const * fragmentSourcePointer = fragmentShaderCode.c_str();
        glShaderSource(*fragID_sptr, 1, &fragmentSourcePointer , NULL);
        glCompileShader(fragmentShader);

        // Check Fragment Shader
        glGetShaderiv(*fragID_sptr, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE)
        {
            std::cerr << "Erro compiling fragment shader: " << fragmentShaderPath << std::endl;
            glGetShaderiv(*fragID_sptr, GL_INFO_LOG_LENGTH, &infoLogLength);
            char * fragmentShaderErrorMessage = new char[infoLogLength];
            glGetShaderInfoLog(*fragID_sptr, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
            fprintf(stdout, "\n%s", &fragmentShaderErrorMessage[0]);
            delete [] fragmentShaderErrorMessage;
        }
        // #ifdef TUCANODEBUG
        // else
        // {
        //     if (fragmentShaderPath.empty())
        //     {
        //         std::cout << "Compiled fragment shader from string without errors : " << shaderName.c_str() << std::endl;
        //     }
        //     else
        //     {
        //         std::cout << "Compiled fragment shader without errors : " << fragmentShaderPath << std::endl;
        //     }
        // }
        // #endif

        glAttachShader(*programID_sptr, *fragID_sptr);

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error loading fragment shader code");
        #endif

    }

    /**
     * @brief Reads the external file containing the fragment shader and loads it into the shader program.
     */
    void readFragmentCode (void)
    {
        // Read the Fragment Shader code from the file
        string fragmentShaderCode;
        ifstream fragmentShaderStream(fragmentShaderPath.c_str(), std::ios::in);
        if(fragmentShaderStream.is_open())
        {
            string line = "";
            while(getline(fragmentShaderStream, line))
            {
                fragmentShaderCode += "\n" + line;
            }
            fragmentShaderStream.close();
        }
        else
        {
            std::cerr << "warning: no fragment shader found : " << fragmentShaderPath << std::endl;
        }

        setFragmentShader(fragmentShaderCode);
        fragment_code = fragmentShaderCode;
    }


    /**
     * @brief Loads compute code into shader program
     * @param computeShaderCode String containing compute shader code
     */
    void setComputeShader (string& computeShaderCode)
    {
        GLint result = GL_FALSE;
        int infoLogLength;

        char const * computeSourcePointer = computeShaderCode.c_str();
        glShaderSource(*computeID_sptr, 1, &computeSourcePointer , NULL);
        glCompileShader(*computeID_sptr);

        // Check Compute Shader
        glGetShaderiv(*computeID_sptr, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE)
        {
            std::cerr << "Erro compiling compute shader: " << computeShaderPath << std::endl;
            glGetShaderiv(*computeID_sptr, GL_INFO_LOG_LENGTH, &infoLogLength);
            char * computeShaderErrorMessage = new char[infoLogLength];
            glGetShaderInfoLog(*computeID_sptr, infoLogLength, NULL, &computeShaderErrorMessage[0]);
            fprintf(stdout, "\n%s", &computeShaderErrorMessage[0]);
            delete [] computeShaderErrorMessage;
        }
        // #ifdef TUCANODEBUG
        // else
        // {
        //     if (computeShaderPath.empty())
        //     {
        //         std::cout << "Compiled compute shader from string without errors : " << shaderName.c_str() << std::endl;
        //     }
        //     else
        //     {
        //         std::cout << "Compiled compute shader without errors : " << computeShaderPath << std::endl;
        //     }
        // }
        // #endif

        glAttachShader(*programID_sptr, *computeID_sptr);

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error loading compute shader code");
        #endif
    }

    /**
     * @brief Reads the external file containing the compute shader code
     */
    void readComputeShaderCode (void)
    {
        string computeShaderCode;
        ifstream computeShaderStream(computeShaderPath.c_str(), std::ios::in);
        if(computeShaderStream.is_open())
        {
            string line = "";
            while(getline(computeShaderStream, line))
            {
                computeShaderCode += "\n" + line;
            }
            computeShaderStream.close();
        }
        else
        {
            std::cerr << "warning: no compute shader found : " << computeShaderPath << std::endl;
        }

        setComputeShader(computeShaderCode);
        compute_shader_code = computeShaderCode;

    }


    /**
     * @brief Reloads all shaders by reading the files again.
     *
     * This feature enables runtime editing of the shader codes.
     * After saving the text file after editing, the reload applies changes immediately.
     */
    void reloadShaders (void)
    {

        #ifdef TUCANODEBUG
        std::cout << "reloading shaders" << std::endl;
        #endif

        if(vertexID_sptr)
        {
            glDetachShader(*programID_sptr, *vertexID_sptr);
            readVertexCode();
        }
        if (tessContID_sptr)
        {
            glDetachShader(*programID_sptr, *tessContID_sptr);
            readTessellationControlCode();
        }
        if (tessEvalID_sptr)
        {
            glDetachShader(*programID_sptr, *tessEvalID_sptr);
            readTessellationEvaluationCode();
        }
        if(geomID_sptr)
        {
            glDetachShader(*programID_sptr, *geomID_sptr);
            readGeometryCode();
        }
        if(fragID_sptr)
        {
            glDetachShader(*programID_sptr, *fragID_sptr);
            readFragmentCode();
        }
        if(computeID_sptr)
        {
            glDetachShader(*programID_sptr, *computeID_sptr);
            readComputeShaderCode();
        }

        linkProgram();

        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif
    }

    /**
     * @brief Enables the shader program for usage.
     *
     * After enabling a shader any OpenGL draw call will use it for rendering.
     */
    void bind (void)
    {
        glUseProgram(*programID_sptr);
        #ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__, "error binding shader");
        #endif
    }

    /**
     * @brief Disables the shader program.
     */
    void unbind (void)
    {
        glUseProgram(0);
    }

    /**
     * @brief Detaches and deletes the shaders and the shader program.
     */
    /*void deleteShaders (void)
    {
        glDetachShader(*programID_sptr, fragmentShader);
        glDetachShader(*programID_sptr, vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteProgram(*programID_sptr);
    }*/

	/**
	* @brief Generates a list with all active attributes
	* @param attribs Vector of strings to hold attributes names
	*/
	void getActiveAttributes( vector< string > &attribs )
	{
		int maxlength = 0;
		int numattribs = 0;

		glGetProgramiv (*programID_sptr, GL_ACTIVE_ATTRIBUTES, &numattribs);
		glGetProgramiv (*programID_sptr, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxlength);

		int length = 0;
		int size = 0;
		GLuint type = 0;
		char* name = new char[maxlength];
		for (int i = 0; i < numattribs; ++i)
		{
			glGetActiveAttrib(*programID_sptr, i, maxlength, &length, &size, &type, name);
			attribs.push_back(name);
		}
        delete [] name;
	}

    /**
     * Given the name of a uniform used inside the shader, returns it's location.
     * @param name Name of the uniform variable in shader.
     * @return The uniform location.
     */
    GLint getUniformLocation (const GLchar* name) const
    {
        return glGetUniformLocation(*programID_sptr, name);
    }

    /**
     * Returns the location of an attribute, such as a vertex attribute
     * @param name Name of the attribute variable in the shader.
     * @return The attribute location, or -1 if the attribute was not found or has an invalid name.
     */
    GLint getAttributeLocation (const GLchar* name) const
    {
        return glGetAttribLocation(*programID_sptr, name);
    }

    //============================Uniforms Setters==========================================================


    //============================ Integer ==========================================================

    /**
     * @brief Sets an uniform integer 4D vector (ivec4) given a location and the vector values.
     * @param location Location handle of uniform variable.
     * @param a First value of the ivec4.
     * @param b Second value of the ivec4.
     * @param c Third value of the ivec4.
     * @param d Fourth value of the ivec4.
     */
    void setUniform (GLint location, GLint a, GLint b, GLint c, GLint d)
    {
        glUniform4i(location, a, b, c, d);
    }

    /**
     * @brief Sets an uniform integer 3D vector (ivec3) given a location and the vector values.
     * @param location Location handle of uniform variable.
     * @param a First value of the ivec3.
     * @param b Second value of the ivec3.
     * @param c Third value of the ivec3.
     */
    void setUniform (GLint location, GLint a, GLint b, GLint c)
    {
        glUniform3i(location, a, b, c);
    }

    /**
     * @brief Sets an uniform integer 2D vector (ivec2) given a location and the vector values.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec2.
     * @param b Second value of the vec2.
     */
    void setUniform (GLint location, GLint a, GLint b)
    {
        glUniform2i(location, a, b);
    }

    /**
     * @brief Sets an uniform integer given a location and the integer value.
     * @param location Location handle of uniform variable.
     * @param a Integer value.
     */
    void setUniform (GLint location, GLint a)
    {
        glUniform1i(location, a);
    }

    /**
     * @brief Sets an uniform integer 4D vector (ivec4) given its name in the shader and the vector values.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the ivec4.
     * @param b Second value of the ivec4.
     * @param c Third value of the ivec4.
     * @param d Fourth value of the ivec4.
     */
    void setUniform (const GLchar* name, GLint a, GLint b, GLint c, GLint d)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b, c, d);
    }

    /**
     * @brief Sets an uniform integer 3D vector (ivec3) given its name in the shader and the vector values.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the ivec3.
     * @param b Second value of the ivec3.
     * @param c Third value of the ivec3.
     */
    void setUniform (const GLchar* name, GLint a, GLint b, GLint c)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b, c);
    }

    /**
     * @brief Sets an uniform integer 2D vector (ivec2) given its name in the shader and the vector values.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the ivec2.
     * @param b Second value of the ivec2.
     */
    void setUniform (const GLchar* name, GLint a, GLint b)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b);
    }

    /**
     * @brief Sets an uniform integer given its name in the shader and the integer value.
     * @param name Name of uniform variable in the shader code.
     * @param a Integer value.
     */
    void setUniform (const GLchar* name, GLint a)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a);
    }

    /**
     * @brief Sets an uniform integer 4D vector (ivec4) given a location and the vector with values.
     * @param location Location handle of uniform variable.
     * @param vec 4D integer vector.
     */
    void setUniform (GLint location, const Eigen::Vector4i &vec)
    {
        glUniform4i(location, vec[0], vec[1], vec[2], vec[3]);
    }

    /**
     * @brief Sets an uniform integer 3D vector (ivec3) given a location and the vector with values.
     * @param location Location handle of uniform variable.
     * @param vec 3D integer vector.
     */
    void setUniform (GLint location, const Eigen::Vector3i &vec)
    {
        glUniform3i(location, vec[0], vec[1], vec[2]);
    }

    /**
     * @brief Sets an uniform integer 2D vector (ivec2) given a location and the vector with values.
     * @param location Location handle of uniform variable.
     * @param vec 2D integer vector.
     */
    void setUniform (GLint location, const Eigen::Vector2i &vec)
    {
        glUniform2i(location, vec[0], vec[1]);
    }

    /**
     * @brief Sets an uniform integer 4D vector (ivec4) given its name in the shader and the vector with values.
     * @param name Name of uniform variable in the shader code.
     * @param vec 4D integer vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector4i &vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    /**
     * @brief Sets an uniform integer 3D vector (ivec3) given its name in the shader and the vector with values.
     * @param name Name of uniform variable in the shader code.
     * @param vec 3D integer vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector3i &vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    /**
     * @brief Sets an uniform integer 2D vector (ivec2) given its name in the shader and the vector with values.
     * @param name Name of uniform variable in the shader code.
     * @param vec 2D integer vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector2i &vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    //============================ Float ==========================================================


    /**
     * @brief Sets an uniform float 4D vector (vec4) given a location and the vector values.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec4.
     * @param b Second value of the vec4.
     * @param c Third value of the vec4.
     * @param d Fourth value of the vec4.
     */
    void setUniform (GLint location, GLfloat a, GLfloat b, GLfloat c, GLfloat d)
    {
        glUniform4f(location, a, b, c, d);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given a location and the vector values.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec3.
     * @param b Second value of the vec3.
     * @param c Third value of the vec3.
     */
    void setUniform (GLint location, GLfloat a, GLfloat b, GLfloat c)
    {
        glUniform3f(location, a, b, c);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given a location and the vector values.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec2.
     * @param b Second value of the vec2.
     */
    void setUniform (GLint location, GLfloat a, GLfloat b)
    {
        glUniform2f(location, a, b);
    }

    /**
     * @brief Sets an uniform float given a location and the float value.
     * @param location Location handle of uniform variable.
     * @param a Float value.
     */
    void setUniform (GLint location, GLfloat a)
    {
        glUniform1f(location, a);
    }

    /**
     * @brief Sets an uniform float 4D vector (vec4) given its name in the shader and the vector values.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the vec4.
     * @param b Second value of the vec4.
     * @param c Third value of the vec4.
     * @param d Fourth value of the vec4.
     */
    void setUniform (const GLchar* name, GLfloat a, GLfloat b, GLfloat c, GLfloat d)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b, c, d);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given its name in the shader and the vector values.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the vec3.
     * @param b Second value of the vec3.
     * @param c Third value of the vec3.
     */
    void setUniform (const GLchar* name, GLfloat a, GLfloat b, GLfloat c)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b, c);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec4) given its name in the shader and the vector values.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the vec2.
     * @param b Second value of the vec2.
     */
    void setUniform (const GLchar* name, GLfloat a, GLfloat b)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b);
    }

    /**
     * @brief Sets an uniform float given its name in the shader and the float value.
     * @param name Name of uniform variable in the shader code.
     * @param a Float value.
     */
    void setUniform (const GLchar* name, GLfloat a)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a);
    }

    /**
     * @brief Sets an uniform float 4D vector (vec4) given a location and the vector with values.
     * @param location Location handle of uniform variable.
     * @param vec 4D float vector.
     */
    void setUniform (GLint location, const Eigen::Vector4f &vec)
    {
        glUniform4f(location, vec[0], vec[1], vec[2], vec[3]);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given a location and the vector with values.
     * @param location Location handle of uniform variable.
     * @param vec 3D float vector.
     */
    void setUniform (GLint location, const Eigen::Vector3f &vec)
    {
        glUniform3f(location, vec[0], vec[1], vec[2]);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given a location and the vector with values.
     * @param location Location handle of uniform variable.
     * @param vec 2D float vector.
     */
    void setUniform (GLint location, const Eigen::Vector2f &vec)
    {
        glUniform2f(location, vec[0], vec[1]);
    }

    /**
     * @brief Sets an uniform float 4D vector (vec4) given its name in the shader and the vector with values.
     * @param name Name of uniform variable in the shader code.
     * @param vec 4D float vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector4f &vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given its name in the shader and the vector with values.
     * @param name Name of uniform variable in the shader code.
     * @param vec 3D float vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector3f &vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given its name in the shader and the vector with values.
     * @param name Name of uniform variable in the shader code.
     * @param vec 2D float vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector2f &vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    //============================ Double ==========================================================

    /**
     * @brief Sets an uniform float 4D vector (vec4) given a location and the vector values as Double.
     * The double values are converted to float.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec4.
     * @param b Second value of the vec4.
     * @param c Third value of the vec4.
     * @param d Fourth value of the vec4.
     */
    void setUniform (GLint location, GLdouble a, GLdouble b, GLdouble c, GLdouble d)
    {
        glUniform4f(location, (GLfloat)a, (GLfloat)b, (GLfloat)c, (GLfloat)d);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given a location and the vector values as Double.
     * The double values are converted to float.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec3.
     * @param b Second value of the vec3.
     * @param c Third value of the vec3.
     */
    void setUniform (GLint location, GLdouble a, GLdouble b, GLdouble c)
    {
        glUniform3f(location, (GLfloat)a, (GLfloat)b, (GLfloat)c);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given a location and the vector values as Double.
     * The double values are converted to float.
     * @param location Location handle of uniform variable.
     * @param a First value of the vec2.
     * @param b Second value of the vec2.
     */
    void setUniform (GLint location, GLdouble a, GLdouble b)
    {
        glUniform2f(location, (GLfloat)a, (GLfloat)b);
    }

    /**
     * @brief Sets an uniform float given a location and a double value that is converted to float.
     * @param location Location handle of uniform variable.
     * @param a Double value.
     */
    void setUniform (GLint location, GLdouble a)
    {
        glUniform1f(location, (GLfloat)a);
    }

    /**
     * @brief Sets an uniform float 4D vector (vec4) given its name in the shader and the vector values as Double.
     *
     * The double values are converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the vec4.
     * @param b Second value of the vec4.
     * @param c Third value of the vec4.
     * @param d Fourth value of the vec4.
     */
    void setUniform (const GLchar* name, GLdouble a, GLdouble b, GLdouble c, GLdouble d)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b, c, d);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given its name in the shader and the vector values as Double.
     *
     * The double values are converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the vec3.
     * @param b Second value of the vec3.
     * @param c Third value of the vec3.
     */
    void setUniform (const GLchar* name, GLdouble a, GLdouble b, GLdouble c)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b, c);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given its name in the shader and the vector values as Double.
     *
     * The double values are converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param a First value of the vec2.
     * @param b Second value of the vec2.
     */
    void setUniform (const GLchar* name, GLdouble a, GLdouble b)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a, b);
    }
    /**
     * @brief Sets an uniform float given its name in the shader and a double value that is converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param a Double value.
     */
    void setUniform (const GLchar* name, GLdouble a)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, a);
    }


    /**
     * @brief Sets an uniform float 4D vector (vec4) given a location and the double vector that is converted to float.
     * @param location Location handle of uniform variable.
     * @param vec 4D double vector.
     */
    void setUniform (GLint location, const Eigen::Vector4d vec)
    {
        glUniform4f(location, (GLfloat)vec[0], (GLfloat)vec[1], (GLfloat)vec[2], (GLfloat)vec[3]);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given a location and the double vector that is converted to float.
     * @param location Location handle of uniform variable.
     * @param vec 3D double vector.
     */
    void setUniform (GLint location, const Eigen::Vector3d vec)
    {
        glUniform3f(location, (GLfloat)vec[0], (GLfloat)vec[1], (GLfloat)vec[2]);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given a location and the double vector that is converted to float.
     * @param location Location handle of uniform variable.
     * @param vec 2D double vector.
     */
    void setUniform (GLint location, const Eigen::Vector2d vec)
    {
        glUniform2f(location, (GLfloat)vec[0], (GLfloat)vec[1]);
    }

    /**
     * @brief Sets an uniform float 4D vector (vec4) given its name in the shader and the double vector that is converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param vec 4D double vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector4d vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    /**
     * @brief Sets an uniform float 3D vector (vec3) given its name in the shader and the double vector that is converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param vec 3D double vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector3d vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }

    /**
     * @brief Sets an uniform float 2D vector (vec2) given its name in the shader and the double vector that is converted to float.
     * @param name Name of uniform variable in the shader code.
     * @param vec 2D double vector.
     */
    void setUniform (const GLchar* name, const Eigen::Vector2d vec)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, vec);
    }


    //============================ Vector from Array ==========================================================

    /**
     * @brief Sets a integer uniform vector value given its location and an array with the values.
     *
     * The vector will be passed as int, ivec2, ivec3, or ivec4 depending on the number of values.
     * @param location Location handle of uniform variable.
     * @param v Integer array holding the values.
     * @param nvalues Number of elements per vector.
     * @param count Number of vectors.
     */
    void setUniform (GLint location, const GLint* v, GLuint nvalues, GLsizei count = 1)
    {
        switch (nvalues)
        {
            case 1: glUniform1iv(location, count, v); break;
            case 2: glUniform2iv(location, count, v); break;
            case 3: glUniform3iv(location, count, v); break;
            case 4: glUniform4iv(location, count, v); break;
        }
    }
    /**
     * @brief Sets a float uniform vector value given its location and an array with the values.
     *
     * The vector will be passed as float, vec2, vec3, or vec4 depending on the number of values.
     * @param location Location handle of uniform variable.
     * @param v Float array holding the values.
     * @param nvalues Number of values in the vector v.
     * @param nvalues Number of elements per vector.
     * @param count Number of vectors.
     */
    void setUniform (GLint location, const GLfloat* v, GLuint nvalues, GLsizei count = 1)
    {
        switch (nvalues)
        {
            case 1: glUniform1fv(location, count, v); break;
            case 2: glUniform2fv(location, count, v); break;
            case 3: glUniform3fv(location, count, v); break;
            case 4: glUniform4fv(location, count, v); break;
        }
    }

    /**
     * @brief Sets a integer uniform vector value given its name in shader and an array with the values.
     *
     * The vector will be passed as int, ivec2, ivec3, or ivec4 depending on the number of values.
     * @param name Name of uniform variable in the shader code.
     * @param v Integer array holding the values.
     * @param nvalues Number of elements per vector.
     * @param count Number of vectors.
     */
    void setUniform (const GLchar* name, const GLint* v, GLuint nvalues, GLsizei count = 1)
    {
        GLint location = getUniformLocation(name);
        setUniform(location,v,nvalues,count);
    }

    /**
     * @brief Sets a float uniform vector value given its name in shader and an array with the values.
     *
     * The vector will be passed as float, vec2, vec3, or vec4 depending on the number of values.
     * @param name Name of uniform variable in the shader code.
     * @param v Float array holding the values.
     * @param nvalues Number of values in the vector v.
     * @param nvalues Number of elements per vector.
     * @param count Number of vectors.
     */
    void setUniform (const GLchar* name, const GLfloat* v, GLuint nvalues, GLsizei count = 1)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, v, nvalues, count);
    }

    void setUniform (const GLchar* name, vector<GLfloat> &v)
    {
        GLint location = getUniformLocation(name);
        glUniform1fv(location, v.size(), &v[0]);

    }


    void setUniform (const GLchar* name, vector<GLint> &v)
    {
        GLint location = getUniformLocation(name);
        glUniform1iv(location, v.size(), &v[0]);

    }


    //============================ Matrix ==========================================================

    /**
     * @brief Sets a uniform float matrix value given its location.
     *
     * The uniform matrix is assumed to be a mat2, mat3, or mat4 depending on the given dimension.
     * @param location  Location handle of uniform variable.
     * @param m Matrix value with dim^2 values
     * @param dim Dimension of the matrix m.
     * @param transpose If transpose is GL_FALSE, each matrix is assumed to be supplied in
     *					in column major order, otherwise is in row major order.
     * @param count  Number of elements of the uniform matrix array to be modified.
     *				a count of 1 should be used if modifying the value of a single matrix.
     */
    void setUniform (GLint location, const GLfloat* m, GLuint dim, GLboolean transpose = GL_FALSE, GLsizei count = 1)
    {
        switch(dim)
        {
            case 2: glUniformMatrix2fv(location, count, transpose, m); break;
            case 3: glUniformMatrix3fv(location, count, transpose, m); break;
            case 4: glUniformMatrix4fv(location, count, transpose, m); break;
        }
    }

    /**
     * @brief Sets a uniform float matrix value given its name in shader code.
     *
     * The uniform matrix is assumed to be a mat2, mat3, or mat4 depending on the given dimension.
     * @param name Name of uniform variable in the shader code.
     * @param m Matrix value with dim^2 values
     * @param dim Dimension of the matrix m.
     * @param transpose If transpose is GL_FALSE, each matrix is assumed to be supplied in
     *					in column major order, otherwise is in row major order.
     * @param count  Number of elements of the uniform matrix array to be modified.
     *				a count of 1 should be used if modifying the value of a single matrix.
     */
    void setUniform (const GLchar* name, const GLfloat* m, GLuint dim, GLboolean transpose = GL_FALSE, GLsizei count = 1)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, m, dim, transpose, count);
    }

    /**
     * @brief Sets a uniform 4x4 float matrix value given its location and eigen 4x4 matrix.
     * @param location  Location handle of uniform variable.
     * @param matrix 4x4 float matrix.
     */
    void setUniform (GLint location, const Eigen::Matrix4f &matrix)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix.data());
    }

    /**
     * @brief Sets a uniform 3x3 float matrix value given its location and eigen 3x3 matrix.
     * @param location  Location handle of uniform variable.
     * @param matrix 3x3 float matrix.
     */
    void setUniform (GLint location, const Eigen::Matrix3f &matrix)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, matrix.data());
    }

    /**
     * @brief Sets a uniform 2x2 float matrix value given its location and eigen 2x2 matrix.
     * @param location  Location handle of uniform variable.
     * @param matrix 2x2 float matrix.
     */
    void setUniform (GLint location, const Eigen::Matrix2f &matrix)
    {
        glUniformMatrix2fv(location, 1, GL_FALSE, matrix.data());
    }

    /**
     * @brief Sets a uniform 4x4 float matrix value given its name in shader and eigen 4x4 matrix.
     * @param name Name of uniform variable in the shader code.
     * @param matrix 4x4 float matrix.
     */
    void setUniform (const GLchar* name, const Eigen::Matrix4f &matrix)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, matrix);
    }

    /**
     * @brief Sets a uniform 3x3 float matrix value given its name in shader and eigen 3x3 matrix.
     * @param name Name of uniform variable in the shader code.
     * @param matrix 3x3 float matrix.
     */
    void setUniform (const GLchar* name, const Eigen::Matrix3f &matrix)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, matrix);
    }

    /**
     * @brief Sets a uniform 2x2 float matrix value given its name in shader and eigen 2x2 matrix.
     * @param name Name of uniform variable in the shader code.
     * @param matrix 2x2 float matrix.
     */
    void setUniform (const GLchar* name, const Eigen::Matrix2f &matrix)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, matrix);
    }

    /**
     * @brief Sets a uniform 4x4 float matrix value given its location and eigen 3x3 affine matrix.
     * @param location Location handle of uniform variable.
     * @param affine_matrix 3x3 float affine matrix.
     */
    void setUniform (GLint location, const Eigen::Affine3f &affine_matrix)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, affine_matrix.matrix().data());
    }

    /**
     * @brief Sets a uniform 3x3 float matrix value given its location and eigen 2x2 affine matrix.
     * @param location Location handle of uniform variable.
     * @param affine_matrix 2x2 float affine matrix.
     */
    void setUniform (GLint location, const Eigen::Affine2f &affine_matrix)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, affine_matrix.matrix().data());
    }

    /**
     * @brief Sets a uniform 4x4 float matrix value given its name in shader and eigen 3x3 affine matrix.
     * @param name Name of uniform variable in the shader code.
     * @param affine_matrix 3x3 float affine matrix.
     */
    void setUniform (const GLchar* name, const Eigen::Affine3f &affine_matrix)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, affine_matrix);
    }

    /**
     * @brief Sets a uniform 3x3 float matrix value given its name in shader and eigen 2x2 affine matrix.
     * @param name Name of uniform variable in the shader code.
     * @param affine_matrix 2x2 float affine matrix.
     */
    void setUniform (const GLchar* name, const Eigen::Affine2f &affine_matrix)
    {
        GLint location = getUniformLocation(name);
        setUniform(location, affine_matrix);
    }


};

}
#endif
