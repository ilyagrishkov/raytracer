cmake_minimum_required(VERSION 3.10)

set (PROJECT_NAME "raytracing")

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories(${PROJECT_DIR})

# Tucano directories.
set(TUCANO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/dependencies/tucano/")

# find the necessary libraries (GLEW, OPENGL, EIGEN, GLFW)
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
  message(SEND_ERROR "GLEW not found on your system.")
endif()

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
  message(SEND_ERROR "OpenGL not found on your system.")
endif()

# Eigen 3 and glfw
# pkg-config is used so we don't hard-code the location of eigen.
find_package(PkgConfig)
if(NOT PKG_CONFIG_FOUND)
    set( EIGEN3_INCLUDE_DIRS "$ENV{EIGEN3_INCLUDE_DIR}" )
    if( NOT EIGEN3_INCLUDE_DIRS )
    message( SEND_ERROR "pkg-config not found on your system. Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
endif()
else()
    pkg_check_modules(EIGEN3 REQUIRED eigen3)
    pkg_check_modules(GLFW REQUIRED glfw3)
endif()

# turn on DEBUG in TUCANO (comment to turn it off)
#add_definitions(-DTUCANODEBUG)

# set the directory with your files as SHADING_SOURCES
set(
  PROJECT_SOURCES
  ${PROJECT_DIR}/main.cpp
  ${PROJECT_DIR}/flyscene.cpp
  #${PROJECT_DIR}/raytracing.cpp  
  )

# add all the source files for compilation (your files + tucano files)
add_executable(
  ${PROJECT_NAME}
  ${PROJECT_SOURCES}
  )

# directories with include files
include_directories(
  ${EIGEN3_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
  ${TUCANO_INCLUDE_DIRS}
  )

# link the program with external libraries (glew, glfw, opengl)
target_link_libraries(
  ${PROJECT_NAME}
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
