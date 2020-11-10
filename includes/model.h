/* Copyright:
 *
 * LearnOpenGl.com Joey de Vries
 * https://learnopengl.com/Model-Loading/Model
 * https://twitter.com/JoeyDeVriez.
 *
 * Changes were made by Tamás Boros.
 *
 * Attribution-NonCommercial 4.0 International (CC BY-NC 4.0), Creative Commons
*/
#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "mesh.h"
#include "shaderprogram.h"

using namespace std;

GLint TextureFromFile(const char *path, string directory);

class Model {

public:
    const aiScene *scene;
    unsigned int offset = 0;
    /*  Model Data  */
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;    // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    Material mat;

    vector<glm::vec4> allPositionVertices;
    vector<glm::vec4> indicesInModel;
    vector<Material> materials;

    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model();

    Model(string path);

    // Draws the model, and thus all its meshes
    void Draw(ShaderProgram shader);

    void getInfoAboutModel();

private:

    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path);

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

    GLint TextureFromFile(const char *path, string directory);
};


#endif
