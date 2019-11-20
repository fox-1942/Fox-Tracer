//
// Created by fox on 11/8/19.
//

#ifndef OPENGL_SHADER_H  //include guards
#define OPENGL_SHADER_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>

class Shader {

public:
    Shader(const GLchar *VS_Path, const GLchar *FS_Path);

    GLuint FragShader;
    GLuint VertShader;
    GLuint ID;

    std::string fragment_shader_code;
    std::string vertex_shader_code;

    void use();

    void print_shader_codes();

    void compile();

    void setValFragmentCode(char val);

    std::string loader(const GLchar *path);
};

#endif //OPENGL_SHADER_H
