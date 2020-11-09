//
// Created by fox-1942 on 5/28/20.
//

#ifndef RAYTRACERBOROS_MAIN_H
#define RAYTRACERBOROS_MAIN_H

#include "errorChecking.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "ShaderProgram.h"
#include "model.h"
#include "filesystem.h"
#include "BvhNode.h"
#include "flatBvhNode.h"
#include "stb_image.h"
#include "light.h"

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

static int hiddenNumberOfPolygons;
const int &BvhNode::numberOfPolygonsInModel(hiddenNumberOfPolygons);

static int hiddenMaxNumberOfPolyInALeaf=0;
int &BvhNode::numberOfPolyInTheLeafWithLargestNumberOfPoly(hiddenMaxNumberOfPolyInALeaf);

Light light = Light(glm::vec3(0.7, 0.5, 0.5), glm::vec3(0.7, 0.6, 0.6), glm::vec3(0.7f, 0.7f, 0.7f));

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void getInputFromKeyboard(GLFWwindow *window);

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

void sendVerticesIndices();

void buildBvhTree();

#endif //RAYTRACERBOROS_MAIN_H
