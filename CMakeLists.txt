cmake_minimum_required(VERSION 3.17)
project(RayTracerBoros)
set (CMAKE_CXX_STANDARD 17)


add_compile_options(-DGLEW_NO_GLU )

add_executable(${PROJECT_NAME}
        src/init.cpp
        src/stb_image.cpp
        src/bbox.cpp
        src/bvhnode.cpp
        src/flatbvhnode.cpp
        src/shaderprogram.cpp
        src/shader.cpp
        src/model.cpp
        src/mesh.cpp
        src/camera.cpp includes/camera.h)

target_include_directories(${PROJECT_NAME} PUBLIC includes)

target_link_libraries(${PROJECT_NAME} GL glfw GLEW assimp)