cmake_minimum_required(VERSION 3.15)
project(opengl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
message(CMAKE_MODULE_PATH " ${CMAKE_MODULE_PATH}")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++11 ")


add_executable(RayTracerBoros
        src/main.cpp
        src/Shader.cpp
        src/Shader.h
        src/VertexBuffer.cpp
        src/VertexBuffer.h
        src/Renderer.cpp
        src/Renderer.h
        src/IndexBuffer.cpp
        src/IndexBuffer.h
        src/VertexArray.cpp src/VertexArray.h src/VertexBufferLayout.h Vendor/stb_image/stb_image.h Vendor/stb_image/stb_image.cpp src/Texture.cpp src/Texture.h)
target_link_libraries(RayTracerBoros GL glfw GLEW)


#-----------------------------------------------------------------------------------------------------------------------


#cmake_minimum_required(VERSION 3.15)
#project(opengl)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#message(CMAKE_MODULE_PATH " ${CMAKE_MODULE_PATH}")
#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++11 ")


#GLEW
#find_package(GLEW REQUIRED)
#message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")
#include_directories(${GLEW_INCLUDE_DIR})
#set(LIBS ${LIBS} )

#GLFW
#find_package(glfw3 REQUIRED)
#message(STATUS "Found GLFW3 in ${GLFW_INCLUDE_DIR}")
#include_directories(${GLFW3_INCLUDE_DIR})


# OpenGL
#find_package(OpenGL REQUIRED)

#add_executable(exampleProgram main.cpp)
#target_include_directories(exampleProgram  PUBLIC  ${OPENGL_INCLUDE_DIRS} ${LIBS})
#target_link_libraries(exampleProgram glfw ${OPENGL_GL_LIBRARY})