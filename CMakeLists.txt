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
        src/Shader.h
        src/ShaderProgram.h
        includes/filesystem.h
        includes/root_directory.h
        src/ShaderProgram.h includes/framework.h)

target_link_libraries(RayTracerBoros GL glfw GLEW assimp SOIL)