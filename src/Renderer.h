#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <string>

class Renderer {
  public:
    // Initialize window named name, with dimensions, width x height
    Renderer(std::string name, int width, int height);
    // Clean up GLFW allocation
    ~Renderer(void);

    // Render loop. Will block until exit condition
    void loop(void);

  private:
    // Rendering pipeline, called once per render loop
    void render(void);
    // Compiles and generates shaders for initialization
    bool generate_shaders(void);

    // GLFW callbacks
    static void resize_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Render window
    GLFWwindow* window;
    // IDs reserved for storing data to GPU
    unsigned int vao, vbo, ebo;
    // ID for shader program object
    int shader;
};