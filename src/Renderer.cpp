#include "Renderer.h"

#include <iostream>

const char* vertex_shader_source =
  "#version 330 core\n"
  "layout (location = 0) in vec3 pos;\n"
  "void main()\n"
  "{\n"
  "   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
  "}\0";

const char* fragment_shader_source =
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}\n\0";

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

    if (!generate_shaders()) {
        exit(1);
    }

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
    // clean all of the GLFW's resources
    glfwTerminate();
}

void Renderer::render() {
    // Clear screen
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Load shaders
    glUseProgram(shader);
    // Load vertex attribute configuration
    glBindVertexArray(vao);
    // Draw contents of VBO (bound from VAO)
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Renderer::generate_shaders() {
    int success;
    char log[512];
    // compile our custom vertex shader
    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, sizeof(log), NULL, log);
        std::cout << "Failed to compile vertex shader\n:" << log << std::endl;
        return false;
    }

    // compile our custom fragment shader
    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, sizeof(log), NULL, log);
        std::cout << "Failed to compile vertex shader:\n" << log << std::endl;
        return false;
    }

    // link our custom shaders
    shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        std::cout << "Failed to link shaders:\n" << log << std::endl;
        return false;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return true;
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