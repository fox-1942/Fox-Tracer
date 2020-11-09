//
// Created by fox1942 on 11/9/20.
//
#include "../includes/ShaderProgram.h"


void ShaderProgram::GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

void ShaderProgram::GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error]: " << error << std::endl;
    }
}

GLuint shaderProgram_id;
bool isLinked = false;

void ShaderProgram::CreateShaderProgram() {
    shaderProgram_id = glCreateProgram();

}

bool ShaderProgram::addShaderToProgram(const Shader &shader) {

    if (!shader.getIsLoaded())
        return false;

    GLClearError();
    glAttachShader(shaderProgram_id, shader.getShader_id());
    GLCheckError();
    cout << "Shader attached, Shader_ID: " << shader.getShader_id() << " to ShaderProgram:" << shaderProgram_id << endl;

    glDeleteShader(shader.getShader_id());
    return true;
}

bool ShaderProgram::linkShaderProgram() {
    glLinkProgram(shaderProgram_id);
    int linkStatus;
    glGetProgramiv(shaderProgram_id, GL_LINK_STATUS, &linkStatus);
    isLinked = linkStatus == GL_TRUE;

    if (!isLinked) {
        GLchar *info;
        int logLength;
        glGetProgramInfoLog(shaderProgram_id, 2048, &logLength, info);
        std::cout << "Error! Shader program wasn't linked! The linker returned: " << info << std::endl;
        return false;
    }

    std::cout << "shaderProgram id: " << shaderProgram_id << " | Linking was successfull." << std::endl;
    std::cout << "-------------------------------------------------------------------------\n" << std::endl;
    return isLinked;
}

void ShaderProgram::useProgram() {
    if (isLinked) {
        glUseProgram(shaderProgram_id);
    }
}

void ShaderProgram::deleteProgram() {
    if (!isLinked) {
        return;
    }

    glDeleteProgram(shaderProgram_id);
    isLinked = false;
}

int ShaderProgram::getShaderProgram_id() const {
    return shaderProgram_id;
}

void ShaderProgram::setUniform1i(const std::string &name, int v0) {
    glUniform1i(getUniformLocation(name), v0);
}

void ShaderProgram::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

int ShaderProgram::getUniformLocation(const std::string &name) {
    int location = glGetUniformLocation(shaderProgram_id, name.c_str());
    return location;
}

void ShaderProgram::setUniformMat4f(const std::string &name, glm::mat4 &matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::setUniformVec3f(const std::string &name, glm::vec3 &vec3) {
    glUniform3fv(getUniformLocation(name), 1, (const GLfloat *) &vec3);
}