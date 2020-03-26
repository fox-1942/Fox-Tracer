#include <iostream>
#include <string>
#include <GL/glew.h>
#include <sstream>
#include "../Vendor/glm/detail/type_mat4x4.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

class Shader {

private:
    unsigned int shader_id;
    std::string shaderCode;
    bool isLoaded;

    string loader(const GLchar *path) {
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

public:
    Shader() {}

    ~Shader() {}

    bool loadShaderFromFile(const GLchar *PathToFile, GLenum shaderType) {
        int success = -4;
        GLchar *info = new GLchar;
        shaderCode = loader(PathToFile);
        shader_id = glCreateShader(shaderType);

        cout << "Path to Shader: " << PathToFile << endl;
        glShaderSource(shader_id, 1, reinterpret_cast<const GLchar *const *>(&shaderCode), NULL);
        glCompileShader(shader_id);

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(shader_id, 100, NULL, info);

        if (!success) {
            cout << "Shader compilation problem: " << info << endl;
            return false;
        }
        isLoaded = true;
        return true;

    }

    bool getIsLoaded() const {
        return this->isLoaded;
    }

    bool getShader_id() const {
        return this->shader_id;
    }

};


