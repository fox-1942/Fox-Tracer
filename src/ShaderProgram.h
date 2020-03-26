//
// Created by fox-1942 on 3/26/20.
//

#include <GL/glew.h>
#include "Shader.h"

#ifndef OPENGL_SHADERPROGRAM_H
#define OPENGL_SHADERPROGRAM_H

#endif //OPENGL_SHADERPROGRAM_H

class ShaderProgram {

public:
    GLuint shaderProgram_id;
    bool isLinked = false;

    void CreateShaderProgram() {
        shaderProgram_id = glCreateProgram();
    }

    bool addShaderToProgram(const Shader &shader) {
        if (!shader.getIsLoaded())
            return false;

        glAttachShader(shaderProgram_id, shader.getShader_id());
        return true;
    }

    bool linkShaderProgram() {
        glLinkProgram(shaderProgram_id);
        int linkStatus;
        glGetProgramiv(shaderProgram_id, GL_LINK_STATUS, &linkStatus);
        isLinked = linkStatus == GL_TRUE;

        if (!isLinked == GL_TRUE) {
            char infoLogBuffer[2048];
            int logLength;
            glGetProgramInfoLog(shaderProgram_id, 2048, &logLength, infoLogBuffer);

            std::cout << "Error! Shader program wasn't linked! The linker returned: " << infoLogBuffer << std::endl;

            return false;

        }
        return isLinked;
    }

    void useProgram() {
        if (isLinked) {
            glUseProgram(shaderProgram_id);
        }
    }

    void deleteProgram() {
        if (!isLinked) {
            return;
        }

        glDeleteProgram(shaderProgram_id);
        isLinked = false;
    }

    int getShaderProgram_id() const
    {
        return shaderProgram_id;
    }

    void setUniform1i(const std::string &name, int v0) {
        glUniform1i(getUniformLocation(name), v0);
    }

    void setUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
        glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
    }

    int getUniformLocation(const std::string &name) {
        int location = glGetUniformLocation(shaderProgram_id, name.c_str());
        return location;
    }

    void setUniformMat4f(const std::string &name, glm::mat4 &matrix) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
    }

    void setUniformVec3f(const std::string &name, glm::vec3 &vec3) {
        glUniform3fv(getUniformLocation(name), 1, (const GLfloat *) &vec3);
    }
};