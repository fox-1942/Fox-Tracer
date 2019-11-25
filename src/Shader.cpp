//
// Created by fox on 11/8/19.
//

#include <fstream>
#include <iostream>
#include <cstring>
#include "Shader.h"

using namespace std;

Shader::Shader(const GLchar *VS_Path, const GLchar *FS_Path) {
    cout << "VS_Path: " << VS_Path << endl;
    cout << "FS_Path: " << FS_Path << endl;

    vertex_shader_code = loader(VS_Path);
    fragment_shader_code = loader(FS_Path);
}

void Shader::compile() {
    std::cout << "" << std::endl;
    print_shader_codes();

    unsigned int vertex, fragment;
    int success = 1;
    GLchar *info = new GLchar;

    //----------------------------------------------------------------------------------
    //for vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, reinterpret_cast<const GLchar *const *>(&vertex_shader_code), NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    cout << "success: " << success << endl;

    glGetShaderInfoLog(vertex, 600, NULL, info);

    cout << "info: " << info << endl;

    //----------------------------------------------------------------------------------
    //for fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, reinterpret_cast<const GLchar *const *>(&fragment_shader_code), NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    cout << "success: " << success << endl;

    glGetShaderInfoLog(fragment, 400, NULL, info);

    cout << "info: " << info << endl;

    //for shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ID, 400, NULL, info);
        cout << "ERROR DURING LINKING SHADER PROGRAM" << info << endl;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::print_shader_codes() {
    cout << "Fragment shader code:\n" << fragment_shader_code << endl;
    cout << "Vertex shader code:\n" << vertex_shader_code << endl;
}

void Shader::use() {
    glUseProgram(this->ID);
}

string Shader::loader(const GLchar *path) {


    std::string content;
    std::ifstream fileStream(path, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << path << ". File does not exist." << std::endl;
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();

    return content;
}