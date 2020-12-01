/* Copyright:
 *
 * LearnOpenGl.com Joey de Vries
 * https://learnopengl.com/Model-Loading/Mesh
 * https://twitter.com/JoeyDeVriez.
 *
 * Changes were made by Tamás Boros.
 *
 * Attribution-NonCommercial 4.0 International (CC BY-NC 4.0), Creative Commons
*/
#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <assimp/types.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaderprogram.h"

using namespace std;

struct Vertex {
    // Position
    glm::vec4 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

struct Material {
    //Material color lighting
    glm::vec4 Ka;
    //Diffuse reflection
    glm::vec4 Kd;
    //Mirror reflection
    glm::vec4 Ks;
    float Ni;
    float shadingModel;
    float shininess;
    float dummy;
};

class Mesh {

private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    Material mats;
    unsigned int uniformBlockIndex;

    /*  Functions  */
    // Constructor

    Mesh()=default;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, Material mat);

    // Render the mesh
    void Draw(ShaderProgram shader);

    // Initializes all the buffer objects/arrays
    void setupMesh();
};

#endif