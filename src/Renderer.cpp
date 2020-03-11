#include "Renderer.h"

#include <iostream>

Renderer::Renderer(std::string name, int width, int height) {
    glfwInit();
    // make sure the opengl version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    // set the lower left corner and the upper right corner
    // opengl transforms the 2d coords to coordinates on the screen.
    glViewport(0, 0, width, height);

    // register the callback function when resize
    glfwSetFramebufferSizeCallback(window, resize_callback);
}

void Renderer::loop() {
    while (!glfwWindowShouldClose(this->window)) {
        // input handling here

        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // check and call events and swap the buffers
        glfwSwapBuffers(this->window);
        glfwPollEvents();
    }
}

Renderer::~Renderer() {
    // clean all of the GLFW's resources
    glfwTerminate();
}

void Renderer::resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}