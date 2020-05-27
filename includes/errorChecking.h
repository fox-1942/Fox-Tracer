//
// Created by fox-1942 on 5/8/20.
//

#ifndef RAYTRACERBOROS_ERRORCHECKING_H
#define RAYTRACERBOROS_ERRORCHECKING_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError() {
    while (GLenum error = glGetError()) {

        std::cout << "[OpenGL Error] ";
        switch (error) {
            case GL_INVALID_ENUM :
                std::cout << "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE :
                std::cout << "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_OPERATION :
                std::cout << "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION :
                std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY :
                std::cout << "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW :
                std::cout
                        << "GL_STACK_UNDERFLOW :";
                break;
            case GL_STACK_OVERFLOW :
                std::cout
                        << "GL_STACK_OVERFLOW";
                break;
            default :
                std::cout << "Unrecognized error" << error;
        }
        std::cout << std::endl;
        return false;
    }
    return true;
}

#endif //RAYTRACERBOROS_ERRORCHECKING_H
