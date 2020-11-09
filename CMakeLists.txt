cmake_minimum_required(VERSION 3.17)
project(RayTracerBoros)

add_compile_options(-DGLEW_NO_GLU)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/stb_image.cpp
        src/BBox.cpp
        src/BvhNode.cpp
        includes/BvhNode.h
        src/flatBvhNode.cpp src/ShaderProgram.cpp src/Shader.cpp)

target_link_libraries(${PROJECT_NAME} GL glfw GLEW assimp)