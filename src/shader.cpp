//
// Created by fox1942 on 11/9/20.
//

#include "../includes/shader.h"
#include <filesystem>

Shader::~Shader() {}

std::string Shader::loader(const GLchar* path) {
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

bool Shader::loadShaderFromFile(const GLchar *PathToFile, GLenum shaderType) {
    int success = 0;
    GLchar info[2048]{};
    shaderCode = loader(PathToFile);
    shader_id = glCreateShader(shaderType);

    const GLchar* src = shaderCode.c_str();
    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(shader_id, sizeof(info), NULL, info);

    if (!success) {
        cout << "Shader compilation problem: " << info << endl;
        return false;
    }

    cout << "Shader id: " << shader_id << " | Shader compilation was succesfull." << info << "\n" << endl;

    isLoaded = true;
    return true;
}

bool Shader::getIsLoaded() const {
    return this->isLoaded;
}

GLuint Shader::getShader_id() const {
    return this->shader_id;
}



