//
// Created by fox1942 on 11/9/20.
//

#include "../includes/Shader.h"

Shader::Shader() {}

Shader::~Shader() {}

std::string Shader::loader(const GLchar *path) {
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
    int success;
    GLchar *info = new GLchar;
    shaderCode = loader(PathToFile);
    shader_id = glCreateShader(shaderType);

    glShaderSource(shader_id, 1, reinterpret_cast<const GLchar *const *>(&shaderCode), NULL);
    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(shader_id, 100, NULL, info);

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



