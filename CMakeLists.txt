cmake_minimum_required(VERSION 3.15)
project(RayTracerBoros)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
message(CMAKE_MODULE_PATH " ${CMAKE_MODULE_PATH}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 ")

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
        includes/vectorCalculator.h
        includes/BvhNode.h
        includes/errorChecking.h
        includes/flatBvhNode.h
        includes/BBox.h
        includes/flatBvhNode.h
        includes/stb_image.h
        includes/stb_image.cpp
        includes/light.h includes/main.h)

target_link_libraries(RayTracerBoros GL glfw GLEW assimp SOIL)