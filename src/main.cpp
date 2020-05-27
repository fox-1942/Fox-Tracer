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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

void sendVerticesIndices();

void buildBvhTree();

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 camera(glm::vec3(0.0f, 0.0f, 0.0f));

unsigned int quadVAO = 0;
unsigned int quadVBO;

ShaderProgram shaderQuadProgram;
Shader shaderQuadVertex;
Shader shaderQuadFragment;

Model mymodel;
BvhNode *bvhNode;
GLFWwindow *window;

float fov = 45;
glm::vec3 eye = glm::vec3(0, 0, 2);
glm::vec3 vup = glm::vec3(0, 1, 0);
glm::vec3 lookat = glm::vec3(0, 0, 0);
glm::vec3 w = eye - lookat;
float f = length(w);
glm::vec3 right1 = normalize(cross(vup, w)) * f * tanf(fov / 2);
glm::vec3 up = normalize(cross(w, right1)) * f * tanf(fov / 2);


static vector<glm::vec4> hiddenPrimitives;
const vector<glm::vec4> &BBox::primitiveCoordinates(hiddenPrimitives);

static int hiddenNumberOfPolyInLeaf;
const int &BvhNode::numberOfPolyInLeaf(hiddenNumberOfPolyInLeaf);


Light light = Light(glm::vec3(0.7, 0.5, 0.5), glm::vec3(0.7, 0.6, 0.6), glm::vec3(0.7f, 0.7f, 0.7f));

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path) {

    shaderQuadVertex = Shader();
    shaderQuadFragment = Shader();
    shaderQuadVertex.loadShaderFromFile(VS_Path, GL_VERTEX_SHADER);
    shaderQuadFragment.loadShaderFromFile(FS_Path, GL_FRAGMENT_SHADER);

    shaderQuadProgram.CreateShaderProgram();

    shaderQuadProgram.addShaderToProgram(shaderQuadVertex);
    shaderQuadProgram.addShaderToProgram(shaderQuadFragment);

    shaderQuadProgram.linkShaderProgram();
}

void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] =
                {-1, -1, 1, -1, 1, 1, -1, 1};

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void sendVerticesIndices() {
    mymodel.fillAllPositionVertices();

    unsigned int primitives;
    glGenBuffers(1, &primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitives);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mymodel.allPositionVertices.size() * sizeof(glm::vec4),
                 glm::value_ptr(mymodel.allPositionVertices.front()), GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, primitives, 0,
                      mymodel.allPositionVertices.size() * sizeof(glm::vec4));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    unsigned int materials;
    glGenBuffers(1, &materials);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materials);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mymodel.materials.size() * sizeof(Material),
                  &mymodel.materials.front(), GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, materials, 0,
                      mymodel.materials.size() * sizeof(Material));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



}

void buildBvhTree() {
    hiddenPrimitives = mymodel.allPositionVertices;
    hiddenNumberOfPolyInLeaf=mymodel.indicesPerFaces.size();

    bvhNode = new BvhNode();
    bvhNode->buildTree(mymodel.indicesPerFaces, 0);
    bvhNode->makeBvHTreeComplete();

    bvhNode->InfoAboutNode();

    vector<FlatBvhNode> *nodeArrays = putNodeIntoArray(bvhNode);

    unsigned int nodesArraytoSendtoShader;
    glGenBuffers(1, &nodesArraytoSendtoShader);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesArraytoSendtoShader);

    glBufferData(GL_SHADER_STORAGE_BUFFER, nodeArrays->size() * sizeof(FlatBvhNode), nodeArrays->data(),
                 GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, nodesArraytoSendtoShader, 0,
                      nodeArrays->size() * sizeof(FlatBvhNode));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


int init() {

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FOX1942/OpenGL-Mesh-Tracer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    std::cout << "glewInit: " << glewInit << std::endl;
    std::cout << "OpenGl Version: " << glGetString(GL_VERSION) << "\n" << std::endl;
    mymodel  = Model("../model/CornellBox-Original.obj");
    createQuadShaderProg("../Shaders/vertexQuad.shader", "../Shaders/fragmentQuad.shader");


    sendVerticesIndices();
    buildBvhTree();

//-----------------------------------------------------------------------------
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(FileSystem::getPath("model/concrete.jpg").c_str(), &width, &height, &nrChannels,
                                    0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
        std::cout << "ERROR: FAILED to load texture." << std::endl;
    }
    stbi_image_free(data);

//-----------------------------------------------------------------------------
    return 0;
}

void loop() {
    while (!glfwWindowShouldClose(window)) {


        processInput(window);

        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model = glm::mat4(1.0f);

        shaderQuadProgram.useProgram();

        glUniform1i(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "texture1"), 0);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "viewPoint"), 1, &lookat.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "x"), 1, &right1.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "y"), 1, &vup.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "camera"), 1, &eye.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].Le"), 1, &light.Le.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].La"), 1, &light.La.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].direction"), 1,
                     &light.direction.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].position"), 1,
                     &light.position.x);

        renderQuad();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main() {
    if (init() == -1) {
        return -1;
    }

    loop();
    glfwTerminate();

    return 0;

}

void setCamera(float param) {

    eye = glm::vec3(eye.x * cos(param) + eye.z * sin(param), eye.y, -eye.x * sin(param) + eye.z * cos(param)) + lookat;
    w = eye - lookat;
    float f = length(w);
    right1 = normalize(cross(vup, w)) * f * tanf(fov / 2);
    up = normalize(cross(w, right1)) * f * tanf(fov / 2);
}

void setCameraY(float param) {

    eye = glm::vec3(eye.x, eye.y + param, eye.z) ;
    w = eye - lookat;
    f = length(w);
    right1 = normalize(cross(vup, w)) * f * tanf(fov / 2);
    up = normalize(cross(w, right1)) * f * tanf(fov / 2);

}

void setCameraZ(float param) {

    eye = glm::vec3(eye.x, eye.y , eye.z+ param) ;
    w = eye - lookat;
    f = length(w);
    right1 = normalize(cross(vup, w)) * f * tanf(fov / 2);
    up = normalize(cross(w, right1)) * f * tanf(fov / 2);

}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        setCamera(0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        setCamera(-0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        setCameraY(+0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        setCameraY(-0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        setCameraZ(+0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        setCameraZ(-0.1);
    }


}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

