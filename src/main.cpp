#include "../includes/errorChecking.h"
#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../Vendor/glm/gtc/type_ptr.hpp"
#include "../includes/ShaderProgram.h"
#include "../includes/camera.h"
#include "../includes/model.h"
#include "../includes/filesystem.h"
#include "../includes/framework.h"
#include "../includes/bvhtree.h"
#include "../includes/BBox.h"
#include "../includes/flatBvhNode.h"
#include "../includes/stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

void sendVerticesIndices();

void buildBvhTree();


const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX;
float lastY;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
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

// hidden variable for BvhNode::primitiveCoordinates
static vector<glm::vec3> hiddenPrimitives;
// Initialize BvhNode::primitiveCoordinates to reference the hidden variable
const vector<glm::vec3> &BBox::primitiveCoordinates(hiddenPrimitives);


struct Light {
    glm::vec3 Le;
    glm::vec3 La;
    glm::vec3 direction;
    glm::vec3 position;

    Light(glm::vec3 direction_, glm::vec3 Le_, glm::vec3 La_, glm::vec3 position_) {
        direction = normalize(direction_);
        Le = Le_;
        La = La_;
        position = position_;
    }
};


Light light1 = Light(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.6, 0.6, 0.6), glm::vec3(0.7f, 0.7f, 0.7f),
                     glm::vec3(0.5, 0.5, 0.5));


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
                // positions
                {-1, -1, 1, -1, 1, 1, -1, 1};
        // setup plane VAO
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

vector<glm::vec4> indicesPerFacesVec4;
vector<glm::vec4> primitiveCoordVec4;

void sendVerticesIndices() {
    mymodel.fillAllPositionVertices();

    for (int i = 0; i < mymodel.allPositionVertices.size(); i++) {
        primitiveCoordVec4.push_back(
                glm::vec4(mymodel.allPositionVertices.at(i).x, mymodel.allPositionVertices.at(i).y,
                          mymodel.allPositionVertices.at(i).z, 1));
    }


    cout << "Number of faces: " << mymodel.indicesPerFaces.size() << endl;

    for (int i = 0; i < mymodel.indicesPerFaces.size(); i++) {
        indicesPerFacesVec4.push_back(glm::vec4(mymodel.indicesPerFaces.at(i).x, mymodel.indicesPerFaces.at(i).y,
                                                mymodel.indicesPerFaces.at(i).z, 1));
    }

    unsigned int primitives;
    glGenBuffers(1, &primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitives);
    glBufferData(GL_SHADER_STORAGE_BUFFER, primitiveCoordVec4.size() * sizeof(glm::vec4),
                 glm::value_ptr(primitiveCoordVec4.front()), GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, primitives, 0,
                      primitiveCoordVec4.size() * sizeof(glm::vec4));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void buildBvhTree() {
    hiddenPrimitives = mymodel.allPositionVertices;
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
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, nodesArraytoSendtoShader, 0,
                      nodeArrays->size() * sizeof(FlatBvhNode));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


int init() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RaytracerBoros", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    std::cout << "glewInit: " << glewInit << std::endl;
    std::cout << "OpenGl Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

    //  mymodel = Model("../model/model2.obj");

    mymodel  = Model("../model/cubeplane.obj");


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
    unsigned char *data = stbi_load(FileSystem::getPath("model/gold.jpg").c_str(), &width, &height, &nrChannels,
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

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model = glm::mat4(1.0f);

        shaderQuadProgram.useProgram();

        glUniform1i(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "texture1"), 0);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wLookAt"), 1, &lookat.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wRight"), 1, &right1.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wUp"), 1, &vup.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wEye"), 1, &eye.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].Le"), 1, &light1.Le.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].La"), 1, &light1.La.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].direction"), 1,
                     &light1.direction.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].position"), 1,
                     &light1.position.x);

        renderQuad();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}