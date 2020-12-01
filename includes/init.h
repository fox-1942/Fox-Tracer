//
// Created by fox1942 on 11/10/20.
//
#ifndef RAYTRACERBOROS_INIT_H
#define RAYTRACERBOROS_INIT_H

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
#include "camera.h"

vector<glm::vec4> hiddenPrimitives;
const vector<glm::vec4> &BBox::primitiveCoordinates(hiddenPrimitives);

int hiddenNumberOfPolygons;
const int &BvhNode::numberOfPolygonsInModel(hiddenNumberOfPolygons);

int hiddenMaxNumberOfPolyInALeaf;
int &BvhNode::numberOfPolyInTheLeafWithLargestNumberOfPoly(hiddenMaxNumberOfPolyInALeaf);

class Init {

private:
    const pair<const unsigned int, const unsigned int> SCR_W_H;
    Camera camera;
    Light light;
    GLuint quadVAO;
    GLuint quadVBO;

    ShaderProgram shaderQuadProgram;
    Shader shaderQuadVertex;
    Shader shaderQuadFragment;

    Model mymodel;
    BvhNode *bvhNode;
    GLFWwindow *window;

    glm::vec3 connect;
    glm::vec3 canvasX;
    glm::vec3 canvasY;


    void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

    void renderQuad();

    void sendVerticesIndices();

    void buildBvhTree();

    // The rotation around Y-axis works fine without any ratio distortion
    void rotateCamAroundY(float param);

    // Unfortunately, rotation around X-axis causes ratio distortion. So the model is stretched and the camera tends to do weird movements.
    void rotateCamAroundX(float param);

    void getInputFromKeyboard(GLFWwindow *window);

    void updateCanvasSizes();

public:

    Init();

    int setup();

    void loop();

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};


#endif //RAYTRACERBOROS_INIT_H
