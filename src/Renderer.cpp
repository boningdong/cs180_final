#include "renderer.h"

#include <iostream>

#define VERTEX_SHADER_PATH "shaders/vertex.vs"
#define FRAGMENT_SHADER_PATH "shaders/fragment.fs"

float vertices[] = {
    0.5f,  0.5f,  0.0f, // top right
    0.5f,  -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f  // top left
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

Renderer::Renderer(std::string name, int width, int height) {
    glfwInit();
    // make sure the opengl version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // generate window
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        exit(1);
    }
    glfwMakeContextCurrent(window);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    shader = new Shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    // generate gl object for vertex array (attributes)
    glGenVertexArrays(1, &vao);
    // generate gl object for vertex buffer (data)
    glGenBuffers(1, &vbo);
    // generate gl object for element buffer (unique indices)
    glGenBuffers(1, &ebo);
    // bind vertex array object (to save vertex configuration)
    glBindVertexArray(vao);
    // bind vertex buffer object (buffer for data)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // bind element buffer object (buffer for unique indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // copy data from vbo buffer to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // copy data from ebo buffer to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Configure vertex attributes (will be saved to vao)
    // * location 0
    // * vertex structure contains 3 items (x, y, z)
    // * vertex structure uses floating point
    // * unnormalized (only needed for integer type)
    // * size of vertex structure
    // * offset 0 bytes into buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // enable location 0 vertex attributes
    glEnableVertexAttribArray(0);

    // register the callback functions
    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);
}

void Renderer::loop() {
    while (!glfwWindowShouldClose(window)) {
        render();
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Renderer::~Renderer() {
    delete shader;
    // clean all of the GLFW's resources
    glfwTerminate();
}

void Renderer::render() {
    // Clear screen
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set globals
    shader->set_vec4f("global_color", 0, 1, 0, 1);
    // Load shaders
    shader->use();

    // Load vertex attribute configuration
    glBindVertexArray(vao);
    // Draw contents of VBO (bound from VAO)
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}