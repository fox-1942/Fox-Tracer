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
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "../Vendor/glm/detail/type_mat4x4.hpp"
#include "Light.h"

class Shader {

public:

    Shader();

    unsigned int ID;

    Shader(const GLchar *VS_Path, const GLchar *FS_Path); // érték vagy referencia szerint adjam át?
    ~Shader();

    void setUniform4f(const std::string &name, float v0, float v1, float f2, float f3);

    std::string fragment_shader_code;
    std::string vertex_shader_code;


    void use() const;

    void print_shader_codes() const;

    void CompileShader();

    void setValFragmentCode(char val);

    std::string loader(const GLchar *path);

    void setUniform1i(const std::string &name, int v0);

    void setUniformMat4f(const std::string &name, glm::mat4 &matrix);

    void setUniformVec3f(const std::string &name, glm::vec3 &matrix);

    unsigned int GetUniformLocation(const std::string &name);

    int getUniformLocation(const std::string &name);

    void Bind() const;

    void Unbind();


    void setUniformLightStruct(const char string[13], Light light);

};

#endif //OPENGL_SHADER_H
