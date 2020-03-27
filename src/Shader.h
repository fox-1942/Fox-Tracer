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
    GLuint shader_id;
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
        int success;
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

        cout<<"Shader id: " <<shader_id<< " | Shader compilation was succesfull." << info <<"\n" <<endl;

        isLoaded = true;
        return true;

    }

    bool getIsLoaded() const {
        return this->isLoaded;
    }

    GLuint getShader_id() const {
        return this->shader_id;
    }

};


