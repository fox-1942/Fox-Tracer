#include <iostream>
#include <string>
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>

#include "glm/detail/type_mat4x4.hpp"

using namespace std;

class Shader {

private:
    GLuint shader_id;
    std::string shaderCode;
    bool isLoaded;

    std::string loader(const GLchar *path);

public:
    Shader()=default;

    ~Shader();

    bool loadShaderFromFile(const GLchar *PathToFile, GLenum&& shaderType);

    bool getIsLoaded() const;

    GLuint getShader_id() const;
};


