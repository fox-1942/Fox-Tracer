cmake_minimum_required(VERSION 3.17)
project(RayTracerBoros)

add_compile_options(-DGLEW_NO_GLU)

add_executable(${PROJECT_NAME}
        src/main.cpp
        includes/stb_image.cpp
        includes/matrixCalculator.h)

target_link_libraries(${PROJECT_NAME} GL glfw GLEW assimp)