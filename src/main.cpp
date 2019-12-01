#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "../includes/camera.h"
#include "../includes/model.h"
#include "../includes/mesh.h"

#include "../Vendor/stb_image/stb_image.h"
#include "Texture.h"



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


int main() {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(5);

    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    std::cout << "glewInit: " << glewInit << std::endl;
    std::cout << "OpenGl Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

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


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // defining vertex array object
    //unsigned int vao;

    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);

    // vertex buffer
    /* unsigned int buffer;
       glGenBuffers(1, &buffer);
       glBindBuffer(GL_ARRAY_BUFFER, buffer);
       glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), positions, GL_STATIC_DRAW);*/


    // positions attribute

    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    //glEnableVertexAttribArray(0);

    // index buffer
    /*unsigned int indexBuffer;
      glGenBuffers(1, &indexBuffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
      */

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.AddFloat(2);
    layout.AddFloat(2);

    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Camera camera(glm::vec3(0.0f, 10.0f, 0.0f)); //Creating camera

    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 720;

    //MVP MVP MVP MVP MVP MVP MVP MVP MVP


    Renderer renderer;

    // int location = glGetUniformLocation(shader.ID, "u_Color");

    //glUniform4f(location, 0.2f,0.5f,0.7f,0.7f); //calling the location and setting the data

    /* glUseProgram(0);
     glBindVertexArray(0);
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

    Shader shader = Shader("../Shaders/shader.vs", "../Shaders/shader.fs");

    int location = glGetUniformLocation(shader.ID, "u_Color");
    GLfloat r = 0.0f;
    GLfloat increment;
    /*  Loop until the user closes the window */



    while (!glfwWindowShouldClose(window)) {


        shader.CompileShader();
        shader.use();

        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                          50.0f);

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 100, 0));

        glm::mat4 u_MVP = proj * view * model;

        shader.setUniformMat4f("u_MVP", u_MVP);

        // The background around the model
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));


        Texture texture("../textures/image.png");
        texture.Bind(0);
        shader.setUniform1i("u_Texture", 0);


        glUniform4f(location, 0.2f, 0.5f, r, 0.7f); //calling the location and setting the data
        renderer.Draw(va, ib, shader);

        if (r > 1.0f) {
            increment = -0.05f;
        } else if (r <= 0.0f) {
            increment = 0.05f;
        }

        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}