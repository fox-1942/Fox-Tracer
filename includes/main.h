//
// Created by fox1942 on 11/10/20.
//
#ifndef RAYTRACERBOROS_MAIN_H
#define RAYTRACERBOROS_MAIN_H

#include "shaderprogram.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "model.h"
#include "filesystem.h"
#include "bvhnode.h"
#include "flatbvhnode.h"
#include "stb_image.h"
#include "light.h"

vector<glm::vec4> hiddenPrimitives;
const vector<glm::vec4> &BBox::primitiveCoordinates(hiddenPrimitives);

int hiddenNumberOfPolygons;
const int &bvhnode::numberOfPolygonsInModel(hiddenNumberOfPolygons);

int hiddenMaxNumberOfPolyInALeaf=0;
int &bvhnode::numberOfPolyInTheLeafWithLargestNumberOfPoly(hiddenMaxNumberOfPolyInALeaf);

class Main {

    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 720;

    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    ShaderProgram shaderQuadProgram;
    Shader shaderQuadVertex;
    Shader shaderQuadFragment;

    Model mymodel;
    bvhnode *bvhNode;
    GLFWwindow *window;

    float fieldOfview = 45;
    glm::vec3 posCamera = glm::vec3(0, 0, 2);
    glm::vec3 upVector = glm::vec3(0, 1, 0);
    glm::vec3 viewPoint = glm::vec3(0, 0, 0);
    glm::vec3 connect = posCamera - viewPoint;
    glm::vec3 canvasX = cross(upVector, connect) * getLength(connect) * tanf(fieldOfview / 2);
    glm::vec3 canvasY = cross(connect, canvasX) * getLength(connect) * tanf(fieldOfview / 2);

    Light light = Light(glm::vec3(0.7, 0.5, 0.5), glm::vec3(0.7, 0.6, 0.6), glm::vec3(0.7f, 0.7f, 0.7f));

    void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

    void renderQuad();

    void sendVerticesIndices();

    void buildBvhTree();

    // The rotation around Y-axis works fine without any ratio distortion
    void rotateCamAroundY(float param);

    // Unfortunately, rotation around X-axis causes ratio distortion. So the model is stretched and the camera tends to do weird movements.
    void rotateCamAroundX(float param);

    void getInputFromKeyboard(GLFWwindow *window);


public:
    int init();

    void loop();

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};


#endif //RAYTRACERBOROS_MAIN_H
