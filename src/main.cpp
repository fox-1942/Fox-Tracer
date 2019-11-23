#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "Shader.h"


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


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    std::cout << "glewInit: " << glewInit << std::endl;
    std::cout << "OpenGl Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

    float position[6] = {
            -0.5f, -0.5f,
            0.5f, 0.5f,
            0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


    Shader shader = Shader("../Shaders/shader.vs", "../Shaders/shader.fs");

    shader.compile();
    shader.use();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {


        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        GLCheckError();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;

}