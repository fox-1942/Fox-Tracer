cmake_minimum_required(VERSION 3.17)
project(RayTracerBoros)
set (CMAKE_CXX_STANDARD 17)


add_compile_options(-DGLEW_NO_GLU -DGLEW_STATIC
  -DBUILD_SHARED_LIBS=OFF 
  -DASSIMP_BUILD_ZLIB=ON 
  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF 
  -DASSIMP_BUILD_TESTS=OFF)

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

target_link_directories(${PROJECT_NAME} PRIVATE libs)
target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glfw3 glew32s assimp-vc145-mtd zlibstaticd)