#include "../includes/init.h"
#include <string>

void Init::createQuadShaderProg(const GLchar *VS_Path, const GLchar *FS_Path) {
    shaderQuadVertex = Shader();
    shaderQuadFragment = Shader();
    shaderQuadVertex.loadShaderFromFile(VS_Path, GL_VERTEX_SHADER);
    shaderQuadFragment.loadShaderFromFile(FS_Path, GL_FRAGMENT_SHADER);

    shaderQuadProgram.CreateShaderProgram();

    shaderQuadProgram.addShaderToProgram(shaderQuadVertex);
    shaderQuadProgram.addShaderToProgram(shaderQuadFragment);

    shaderQuadProgram.linkShaderProgram();
}

void Init::renderQuad() {
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

void Init::sendVerticesIndices() {

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

void Init::buildBvhTree() {

    hiddenPrimitives = mymodel.allPositionVertices;
    hiddenNumberOfPolygons = mymodel.indicesInModel.size();

    bvhNode = new BvhNode();
    bvhNode->buildTree(mymodel.indicesInModel, 0);
    bvhNode->makeBvHTreeComplete();
    bvhNode->InfoAboutNode();

    vector<FlatBvhNode> *nodeArrays = FlatBvhNode::putNodeIntoArray(bvhNode);
    delete bvhNode;

    unsigned int nodesArraytoSendtoShader;
    glGenBuffers(1, &nodesArraytoSendtoShader);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesArraytoSendtoShader);

    glBufferData(GL_SHADER_STORAGE_BUFFER, nodeArrays->size() * sizeof(FlatBvhNode), nodeArrays->data(),
                 GL_STATIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, nodesArraytoSendtoShader, 0,
                      nodeArrays->size() * sizeof(FlatBvhNode));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void Init::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


int Init::setup() {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    mymodel = Model("../model/CornellBox-Original.obj");

    /* If throwing an instance of 'std::out_of_range', increase the number of 'indices' array size in struct 'FlatBvhNode'
    *  and the 'indices' array size in fragmentQuad.shader to the largest number of triangles in a leaf (info in console during runtime).
    */
    createQuadShaderProg("../Shaders/vertexQuad.shader", "../Shaders/fragmentQuad.shader");

    sendVerticesIndices();
    buildBvhTree();

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
    unsigned char *data = stbi_load(File::getPath("model/wood.png").c_str(), &width, &height, &nrChannels,
                                    0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
        std::cout << "ERROR: FAILED to load texture." << std::endl;
    }
    stbi_image_free(data);

    return 0;
}

void Init::loop() {

    while (!glfwWindowShouldClose(window)) {

        getInputFromKeyboard(window);
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        shaderQuadProgram.useProgram();

        glUniform1i(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "texture1"), 0);
        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "viewPoint"), 1,
                     &camera.getViewPoint().x);
        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "canvasX"), 1, &canvasX.x);
        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "canvasY"), 1,
                     &camera.getUpVector().x);
        glUniform3fv(glGetUniformLocation(shaderQuadProgram.getShaderProgram_id(), "camera"), 1,
                     &camera.getPosCamera().x);
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

void Init::updateCanvasSizes() {
    connect = camera.getPosCamera() - camera.getViewPoint();

    float aspect = (float)SCR_W_H.first/(float)SCR_W_H.second;
    float length = tanf(camera.getFieldOfview() / 2);

    canvasX = glm::normalize(glm::cross(camera.upVector, connect)) / length / aspect;
    canvasY = glm::normalize(glm::cross(connect, canvasX)) / length;

}

// The rotation around Y-axis works fine without any ratio distortion
void Init::rotateCamAroundY(float param) {
    camera.setPosCamera(glm::vec3(
            camera.getPosCamera().x * cos(param) + camera.getPosCamera().z * sin(param) + camera.getViewPoint().x,
            camera.getPosCamera().y,
            -camera.getPosCamera().x * sin(param) + camera.getPosCamera().z * cos(param) + camera.getViewPoint().z));
    updateCanvasSizes();
}

// Unfortunately, rotation around X-axis causes ratio distortion. So the model is stretched and the camera tends to do weird movements.
void Init::rotateCamAroundX(float param) {
    camera.posCamera = glm::vec3(camera.posCamera.x,
               (camera.posCamera.y - camera.viewPoint.y) * cos(param) + (camera.posCamera.z - camera.viewPoint.z) * sin(param) + camera.viewPoint.y,
               -(camera.posCamera.y - camera.viewPoint.y) * sin(param) + (camera.posCamera.z - camera.viewPoint.z) * cos(param) + camera.viewPoint.z);

    updateCanvasSizes();
}


void Init::getInputFromKeyboard(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        rotateCamAroundY(0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        rotateCamAroundY(-0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        rotateCamAroundX(+0.1);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        rotateCamAroundX(-0.1);
    }
}

Init::Init()
        : SCR_W_H(1280.0f, 720.0f),
          camera(45 * (float)M_PI / 180, glm::vec3(0, 2, 24), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0)),
          light(glm::vec3(0.7, 0.5, 0.5), glm::vec3(0.7, 0.6, 0.6),
                glm::vec3(0.7f, 0.7f, 0.7f)),
          quadVAO(0),
          quadVBO(0),
          shaderQuadProgram(),
          shaderQuadVertex(),
          shaderQuadFragment(),
          mymodel(),
          bvhNode() {
    glfwInit();
    window = glfwCreateWindow(SCR_W_H.first, SCR_W_H.second, "FoxTracer", nullptr, nullptr);
    updateCanvasSizes();
}

int main() {
    Init init;
    if (init.setup() == -1) {
        return -1;
    }

    init.loop();
    glfwTerminate();

    return 0;
}
