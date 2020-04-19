#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../Vendor/glm/gtc/type_ptr.hpp"

#include <iostream>
#include "ShaderProgram.h"
#include "../includes/camera.h"
#include "../includes/model.h"
#include "../includes/filesystem.h"
#include "../includes/framework.h"




void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void mouse_buttonCallback(GLFWwindow *window, int button, int action, int mods);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void createInitQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

void createComputeProgram(const GLchar *CS1_Path, const GLchar *CS2_Path, const GLchar *CS3_Path);

void initComputeProgram();

void sendVerticesIndices();


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

ShaderProgram shaderComputeProgram;
Shader shaderCompute;


Model mymodel;

GLFWwindow *window;


GLuint outputTex;
GLuint sampler;
int tex;
GLuint vao;
int workgroupSizeX;
int workgroupSizeY;
int eyeuniform;
int ray00Uniform;
int ray01Uniform;
int ray10Uniform;
int ray11Uniform;
int timeUniform;
int blendFactorUniform;
int bounceCountUniform;

float fov = 45;
glm::vec3 eye = glm::vec3(0, 0, 2);
glm::vec3 vup = glm::vec3(0, 1, 0);
glm::vec3 lookat = glm::vec3(0, 0, 0);
glm::vec3 w = eye - lookat;
float f = length(w);
glm::vec3 right1 = normalize(cross(vup, w)) * f * tanf(fov / 2);
glm::vec3 up = normalize(cross(w, right1)) * f * tanf(fov / 2);


struct Light {
    glm::vec3 Le;
    glm::vec3 La;
    glm::vec3 direction;
    glm::vec3 position;

    Light(glm::vec3 direction_, glm::vec3 Le_, glm::vec3 La_, glm::vec3 position_) {
        direction = normalize(direction_);
        Le = Le_;
        La = La_;
        position=position_;
    }
};

Light light1 = Light(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.5, 0.5, 0.5));

int framebufferImageBinding;

float currRotationAboutY;
float currRotationAboutX;


float mouseX;

float mouseDownX;

bool mouseDown;

float rotationAboutY;

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] ";
        switch (error) {
            case GL_INVALID_ENUM :
                std::cout << "GL_INVALID_ENUM : An unacceptable value is specified for an enumerated argument.";
                break;
            case GL_INVALID_VALUE :
                std::cout << "GL_INVALID_OPERATION : A numeric argument is out of range.";
                break;
            case GL_INVALID_OPERATION :
                std::cout << "GL_INVALID_OPERATION : The specified operation is not allowed in the current state.";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION :
                std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION : The framebuffer object is not complete.";
                break;
            case GL_OUT_OF_MEMORY :
                std::cout << "GL_OUT_OF_MEMORY : There is not enough memory left to execute the command.";
                break;
            case GL_STACK_UNDERFLOW :
                std::cout
                        << "GL_STACK_UNDERFLOW : An attempt has been made to perform an operation that would cause an internal stack to underflow.";
                break;
            case GL_STACK_OVERFLOW :
                std::cout
                        << "GL_STACK_OVERFLOW : An attempt has been made to perform an operation that would cause an internal stack to overflow.";
                break;
            default :
                std::cout << "Unrecognized error" << error;
        }
        std::cout << std::endl;
        return false;
    }
    return true;
}


void createInitQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path) {

    shaderQuadVertex = Shader();
    shaderQuadFragment = Shader();

    shaderQuadVertex.loadShaderFromFile(VS_Path, GL_VERTEX_SHADER);
    shaderQuadFragment.loadShaderFromFile(FS_Path, GL_FRAGMENT_SHADER);

    shaderQuadProgram.CreateShaderProgram();

    shaderQuadProgram.addShaderToProgram(shaderQuadVertex);
    shaderQuadProgram.addShaderToProgram(shaderQuadFragment);

    shaderQuadProgram.linkShaderProgram();

    shaderQuadProgram.useProgram();

    int texUniform = glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "tex");
    glUniform1i(texUniform, 0);
    glUseProgram(0);
}

void createInitComputeProg(const GLchar *Path) {

    shaderCompute = Shader();

    shaderCompute.loadShaderFromFile(Path, GL_COMPUTE_SHADER);

    shaderComputeProgram.CreateShaderProgram();

    shaderComputeProgram.addShaderToProgram(shaderCompute);

    shaderComputeProgram.linkShaderProgram();

    shaderComputeProgram.useProgram();

    int workgroupSize[2];
    glGetProgramiv(shaderComputeProgram.getShaderProgram_id(),GL_COMPUTE_WORK_GROUP_SIZE, workgroupSize);
    workgroupSizeX = workgroupSize[0];
    workgroupSizeY = workgroupSize[1];

    cout<<workgroupSizeX <<" "<< workgroupSizeY << endl;

    eyeuniform = glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "eye");
    ray00Uniform = glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "ray00");
    ray10Uniform = glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "ray10");
    ray01Uniform = glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "ray01");
    ray11Uniform = glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "ray11");

    int loc = glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "framebufferImage");
    int param;
    glGetUniformiv(shaderComputeProgram.getShaderProgram_id(), loc, &param);
    framebufferImageBinding = param;

    glUseProgram(0);

}

/*
void initComputeProgram() {
    shaderCompute.use();
    int workgroupSize[3];
    glGetProgramiv(shaderCompute.ID, GL_COMPUTE_WORK_GROUP_SIZE, workgroupSize);
    workgroupSizeX = workgroupSize[0];
    workgroupSizeY = workgroupSize[1];

    eyeuniform = glGetUniformLocation(shaderCompute.ID, "eye");
    ray00Uniform = glGetUniformLocation(shaderCompute.ID, "ray00");
    ray01Uniform = glGetUniformLocation(shaderCompute.ID, "ray01");
    ray10Uniform = glGetUniformLocation(shaderCompute.ID, "ray10");
    ray11Uniform = glGetUniformLocation(shaderCompute.ID, "ray11");
    timeUniform = glGetUniformLocation(shaderCompute.ID, "time");
    blendFactorUniform = glGetUniformLocation(shaderCompute.ID, "blendFactor");
    bounceCountUniform = glGetUniformLocation(shaderCompute.ID, "bounceCount");
}*/

/*void present() {
    glUseProgram(quadProgram);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindSampler(0, this.sampler);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    glBindSampler(0, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}*/

void sendVerticesIndices() {

    mymodel.fillAllPositionVertices();

    unsigned int primitives;
    glGenBuffers(1, &primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitives);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mymodel.allPositionVertices.size() * sizeof(glm::vec4),
                 glm::value_ptr(mymodel.allPositionVertices.front()), GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, primitives, 0,
                      mymodel.allPositionVertices.size() * sizeof(glm::vec4));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    unsigned int indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mymodel.indicesPerFaces.size() * sizeof(glm::vec4),
                 glm::value_ptr(mymodel.indicesPerFaces.front()), GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, indices, 0, mymodel.indicesPerFaces.size() * sizeof(glm::vec4));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void createOutputTexture(){

    glGenTextures(1,&outputTex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void createSampler(){
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
    glfwSetMouseButtonCallback(window,mouse_buttonCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    std::cout << "glewInit: " << glewInit << std::endl;
    std::cout << "OpenGl Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

    createOutputTexture();
    createSampler();

    glGenVertexArrays(1,&vao);
    createInitComputeProg("../Shaders/compute.shader");
    createInitQuadShaderProg("../Shaders/vertexQuad.shader","../Shader/fragmentQuad.shader");

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


        if (mouseDown) {
            currRotationAboutY = rotationAboutY + (mouseX - mouseDownX) * 0.01f;
        }
        else
            {
            currRotationAboutY = rotationAboutY;
        }

        eye=glm::vec3((float) sin(-currRotationAboutY) * 3.0f,2,(float) cos(-currRotationAboutY) * 3.0f);

        glm::mat4 viewMatrix = glm::lookAt(eye, lookat, up);
        glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 projViewMatrix = projectionMatrix * viewMatrix;
        glm::mat4 InvProjViewMatrix = glm::inverse(projViewMatrix);
        glm::mat4 inverseProjViewMatrix=glm::inverse(glm::matrixCompMult(projectionMatrix,viewMatrix));

        shaderComputeProgram.useProgram();
        glUniform3fv(glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "eye"), 1, &eye.x);

        glm::vec3 frustrumVec=glm::vec3(-1, -1, 0);
        frustrumVec= mulProject(frustrumVec, inverseProjViewMatrix) - eye;
        glUniform3fv(glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "rayTopLeft"), 1, &frustrumVec.x);

        frustrumVec= mulProject(glm::vec3(-1, 1, 0), inverseProjViewMatrix) - eye;
        glUniform3fv(glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "rayTopRight"), 1, &frustrumVec.x);

        frustrumVec= mulProject(glm::vec3(1, -1, 0), inverseProjViewMatrix) - eye;
        glUniform3fv(glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "rayBottomLeft"), 1, &frustrumVec.x);

        frustrumVec= mulProject(glm::vec3(1, 1, 0), inverseProjViewMatrix) - eye;
        glUniform3fv(glGetUniformLocation(shaderComputeProgram.getShaderProgram_id(), "rayBottomRight"), 1, &frustrumVec.x);

        glBindImageTexture(framebufferImageBinding,tex,0,false,0, GL_WRITE_ONLY, GL_RGBA32F);

        int numGroupsX = (int) ceil((double)SCR_WIDTH / workgroupSizeX);
        int numGroupsY = (int) ceil((double)SCR_HEIGHT / workgroupSizeY);

        /* Invoke the compute shader. */
        glDispatchCompute(numGroupsX, numGroupsY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindImageTexture(framebufferImageBinding, 0, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glUseProgram(0);

        glUseProgram(shaderQuadProgram.getShaderProgram_id());
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBindSampler(0,sampler);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindSampler(0, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
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

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime + 1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime + 1);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime + 1);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime + 1);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  mouseX=(float) xpos;
}

void mouse_buttonCallback(GLFWwindow *window, int button, int action, int mods){
    if(action==GLFW_PRESS){
        mouseDownX=mouseX;
        mouseDown=true;

    } else if(action==GLFW_RELEASE){
        mouseDown=false;
        rotationAboutY=currRotationAboutY;
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}