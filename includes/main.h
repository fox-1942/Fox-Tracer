//
// Created by fox-1942 on 5/28/20.
//

#ifndef RAYTRACERBOROS_MAIN_H
#define RAYTRACERBOROS_MAIN_H

#include "../includes/errorChecking.h"
#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../Vendor/glm/gtc/type_ptr.hpp"
#include "../includes/ShaderProgram.h"
#include "../includes/model.h"
#include "../includes/filesystem.h"
#include "../includes/BvhNode.h"
#include "../includes/flatBvhNode.h"
#include "../includes/stb_image.h"
#include "../includes/light.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int quadVAO = 0;
unsigned int quadVBO;

ShaderProgram shaderQuadProgram;
Shader shaderQuadVertex;
Shader shaderQuadFragment;

Model mymodel;
BvhNode *bvhNode;
GLFWwindow *window;

float fieldOfview = 45;
glm::vec3 posCamera = glm::vec3(0, 0, 2);
glm::vec3 upVector = glm::vec3(0, 1, 0);
glm::vec3 viewPoint = glm::vec3(0, 0, 0);
glm::vec3 connect = posCamera - viewPoint;
glm::vec3 canvasX = cross(upVector, connect) * getLength(connect) * tanf(fieldOfview / 2);
glm::vec3 canvasY = cross(connect, canvasX) * getLength(connect) * tanf(fieldOfview / 2);

static vector<glm::vec4> hiddenPrimitives;
const vector<glm::vec4> &BBox::primitiveCoordinates(hiddenPrimitives);

static int hiddenNumberOfPolyInLeaf;
const int &BvhNode::numberOfPolyInLeaf(hiddenNumberOfPolyInLeaf);

Light light = Light(glm::vec3(0.7, 0.5, 0.5), glm::vec3(0.7, 0.6, 0.6), glm::vec3(0.7f, 0.7f, 0.7f));

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void getInputFromKeyboard(GLFWwindow *window);

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

void sendVerticesIndices();

void buildBvhTree();

#endif //RAYTRACERBOROS_MAIN_H
