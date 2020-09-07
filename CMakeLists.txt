cmake_minimum_required(VERSION 3.17)
project(RayTracerBoros)

add_executable(RayTracerBoros
        src/main.cpp
        includes/stb_image.cpp
        )

target_link_libraries(RayTracerBoros GL glfw GLEW assimp SOIL)