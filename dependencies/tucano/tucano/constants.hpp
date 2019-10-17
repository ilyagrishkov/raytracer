#ifndef _TUCANO_CONSTANTS_HPP
#define _TUCANO_CONSTANTS_HPP

#include <string>

const char TUCANO_DEFAULT_RESOURCE_DIR[] = "resources/";

inline std::string tucano_shader_dir(std::string const &resource_dir)
{
    return resource_dir + "shaders/";
}

#endif