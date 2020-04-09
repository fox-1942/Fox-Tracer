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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path);

void createComputeProgram(const GLchar *CS1_Path, const GLchar *CS2_Path, const GLchar *CS3_Path);

void initComputeProgram();

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
        float quadVertices[] = {
                // positions
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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


    mymodel = Model("../model/nanosuit/nanosuit.obj");

    createQuadShaderProg("../Shaders/vertexQuad.shader", "../Shaders/fragmentQuad.shader");
   // create3DShaderProg("../Shaders/vertex.shader", "../Shaders/fragment.shader");


    mymodel.fillAllPositionVertices();

    unsigned int primitives;
    glGenBuffers(1, &primitives);
    glBindBuffer(GL_UNIFORM_BUFFER, primitives);
    glBufferData(GL_UNIFORM_BUFFER,3430* sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // define the range of the buffer that links to a uniform binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, primitives, 0,3430* sizeof(glm::vec3));

    glBindBuffer(GL_UNIFORM_BUFFER, primitives);

    glBufferSubData(GL_UNIFORM_BUFFER, 0,  sizeof(glm::vec3), glm::value_ptr(mymodel.allPositionVertices.at(0)));
    glBufferSubData(GL_UNIFORM_BUFFER, 4,  sizeof(glm::vec3), glm::value_ptr(mymodel.allPositionVertices.at(1)));

    for(int i=2;i<=3430;i++) {
    glBufferSubData(GL_UNIFORM_BUFFER, pow(2,i),  sizeof(glm::vec3), glm::value_ptr(mymodel.allPositionVertices.at(i)));
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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

        GLuint valami;
        glGetUniformiv(shaderProgram.getShaderProgram_id(), GL_MAX_UNIFORM_BLOCK_SIZE, reinterpret_cast<GLint *>(&valami));

        //cout<<"sdsdsdsd: "<< valami <<endl;
*/




        // First pipeline - Ortho --------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDepthMask(0);

        shaderQuadProgram.useProgram();

        model = glm::scale(model, glm::vec3(500.0f, 500.0f, 0.0f));
        glm::mat4 orthoMatrix = glm::ortho(0.0f, float(SCR_WIDTH), 0.0f, float(SCR_HEIGHT));
        shaderQuadProgram.setUniformMat4f("modelMatrix", model);
        shaderQuadProgram.setUniformMat4f("projectionMatrix", orthoMatrix);

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


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime+1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime+1);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime+1);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime+1);
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