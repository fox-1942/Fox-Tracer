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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

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

ShaderProgram shaderProgram;
Shader shaderVertex;
Shader shaderFragment;

Model mymodel;

GLFWwindow *window;

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
        position = position_;
    }
};

Light light1 = Light(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.3f, 0.3f, 0.3f),
                     glm::vec3(0.5, 0.5, 0.5));

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

void create3DShaderProg(const GLchar *VS_Path, const GLchar *FS_Path) {

    shaderVertex = Shader();
    shaderFragment = Shader();

    shaderVertex.loadShaderFromFile(VS_Path, GL_VERTEX_SHADER);
    shaderFragment.loadShaderFromFile(FS_Path, GL_FRAGMENT_SHADER);

    shaderProgram.CreateShaderProgram();

    shaderProgram.addShaderToProgram(shaderVertex);
    shaderProgram.addShaderToProgram(shaderFragment);

    shaderProgram.linkShaderProgram();
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


    mymodel = Model("../model/model.obj");

    createQuadShaderProg("../Shaders/vertexQuad.shader", "../Shaders/fragmentQuad.shader");
    // create3DShaderProg("../Shaders/vertex.shader", "../Shaders/fragment.shader");


    cout<<"eredeti"<<endl;
    cout <<eye.x<<" "<<eye.y<<" "<<eye.z<<endl;
    cout <<lookat.x<<" "<<lookat.y<<" "<<lookat.z<<endl;
    cout <<right1.x<<", "<<right1.y<<" ,"<<right1.z<<","<<endl;
    cout <<up.x<<", "<<up.y<<" ,"<<up.z<<","<<endl;
    cout <<"---------------------------------------------------------------"<<endl;
    sendVerticesIndices();

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




/*
        // Second pipeline - Perspective---------------------------------------------

        model=glm::translate(model, glm::vec3(0.0,0.0,0.0));
        model=glm::scale(model, glm::vec3(0.1,0.1,0.1));

        shaderProgram.useProgram();


        unsigned int uboMatrices;
        glGenBuffers(1, &uboMatrices);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,1000.0f);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shaderProgram.setUniformMat4f("modelMatrix", model);

        mymodel.Draw(shaderProgram);
*/




        // First pipeline - Ortho --------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDepthMask(0);

        shaderQuadProgram.useProgram();
/*
        model = glm::scale(model, glm::vec3(500.0f, 500.0f, 0.0f));
        glm::mat4 orthoMatrix = glm::ortho(0.0f, float(SCR_WIDTH), 0.0f, float(SCR_HEIGHT));
        shaderQuadProgram.setUniformMat4f("modelMatrix", model);
        shaderQuadProgram.setUniformMat4f("projectionMatrix", orthoMatrix);*/

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wLookAt"), 1, &lookat.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wRight"), 1, &right1.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wUp"), 1, &vup.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "wEye"), 1, &eye.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights.Le"), 1, &light1.Le.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights[0].La"), 1, &light1.La.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights.direction"), 1,
                     &light1.direction.x);

        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "lights.position"), 1,
                     &light1.position.x);

        renderQuad();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(1);
        // ------------------------------------------------------------------------

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

    cout<<"set"<<endl;
    cout <<right1.x<<", "<<right1.y<<" ,"<<right1.z<<","<<endl;
}

void setCameraY(float param) {

    eye = glm::vec3(eye.x, eye.y+param,eye.z) + lookat;
    w = eye - lookat;
    float f = length(w);
    right1 = normalize(cross(vup, w)) * f * tanf(fov / 2);
    up = normalize(cross(w, right1)) * f * tanf(fov / 2);

    cout<<"set"<<endl;
    cout <<right1.x<<", "<<right1.y<<" ,"<<right1.z<<","<<endl;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
       setCamera(0.5);
    }


    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        setCamera(-0.5);
    }


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        setCameraY(+0.5);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        setCameraY(-0.5);
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