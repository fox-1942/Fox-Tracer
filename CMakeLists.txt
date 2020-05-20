cmake_minimum_required(VERSION 3.15)
project(RayTracerBoros)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
message(CMAKE_MODULE_PATH " ${CMAKE_MODULE_PATH}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 ")


#configure_file(includes/root_directory.h.in includes/root_directory.h)
#find_package(ASSIMP REQUIRED)
#message(STATUS "Found ASSIMP in ${ASSIMP_LIBRARY_DIR}")

find_package(GLEW REQUIRED)
message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")

link_directories(/usr/local/lib)
add_executable(RayTracerBoros
        src/main.cpp
        includes/Shader.h
        includes/ShaderProgram.h
        includes/filesystem.h
        includes/root_directory.h
        includes/ShaderProgram.h
        includes/framework.h
        includes/bvhtree.h
        includes/errorChecking.h
        includes/flatBvhNode.h
        includes/BBox.h
        includes/flatBvhNode.h
        includes/stb_image.h includes/stb_image.cpp)

target_link_libraries(RayTracerBoros GL glfw GLEW assimp SOIL)