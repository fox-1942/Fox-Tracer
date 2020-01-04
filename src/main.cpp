#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include "Shader.h"
#include "Renderer.h"
#include "../includes/camera.h"
#include "../includes/model.h"

#include "../includes/filesystem.h"

struct Light {
//---------------------------
  glm::vec3 direction;
  glm::vec3 Le, La;
  Light(glm::vec3 _direction, glm::vec3 _Le, glm::vec3 _La) {
    direction = normalize(_direction);
    Le = _Le; La = _La;
  }
};


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void setUniform(glm::vec3 vec, const char string[16]);
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, -4.0f, 3.0f));
float lastX ;
float lastY ;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error]: " << error << std::endl;
    }
}

static bool GLlogCall(const char *function, const char *file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL Error: " << error << " " << function <<
                  " " << file << " " << line << std::endl;
        return false;
    }
    return true;
}

void setUniformLight(Light* light) {
  setUniform(light->La, "light.La");
  setUniform(light->Le, "light.Le");
  setUniform(light->direction, "light.direction");
}
void setUniform(glm::vec3 vec, const char string[16]) {}

int main() {
    GLFWwindow *window;

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

    //glfwSwapInterval(5);

    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    std::cout << "glewInit: " << glewInit << std::endl;
    std::cout << "OpenGl Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

    //glEnable(GL_DEPTH_TEST);

    Shader shader = Shader("../Shaders/vertex.shader","../Shaders/fragment.shader");

    Model mymodel(FileSystem::getPath("model/nanosuit/nanosuit.obj"));


    // draw in wireframe
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


/*
    float positions[] = {
            100.0f, 100.0f, 0.0f, 0.0f, // 0
            200.0f, 100.0f, 1.0f, 0.0f, // 1
            200.0f, 200.0f, 1.0f, 1.0f, // 2
            100.0f, 200.0f, 0.0f, 1.0f  // 3
    };

    unsigned int indices[6] = {
            0, 1, 2,
            2, 3, 0
    };
*/
    /*VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.AddFloat(2);
    layout.AddFloat(2);

    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);
    */




    Renderer renderer;

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glEnable(GL_TEXTURE_2D);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.CompileShader();
        shader.use();

        Light lightToSendAsUniform= Light(glm::vec3(1, 1, 1), glm::vec3(3, 3, 3), glm::vec3(0.4f, 0.3f, 0.3f));





        // Setting uniformSetting uniforms for fragment shader
        shader.setUniformVec3f("cameraPosition", camera.Position);  //the position of the camera in word-space
        glm::vec3 cameraLookAt=camera.Position+camera.Front; // where the camera is looking at



        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        glm::mat4 u_MVP =  projection * view * model ;
        shader.setUniformMat4f("u_MVP", u_MVP);

        mymodel.Draw(shader);


        // The background around the model
        //model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));

/*
        Texture texture("../textures/image.png");
        texture.Bind(0);
        shader.setUniform1i("u_Texture", 0);
*/

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}