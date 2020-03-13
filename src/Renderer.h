#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "shader.h"

#include <string>

typedef unsigned int gl_obj_t;

class Renderer {
  public:
    // Initialize window named name, with dimensions, width x height
    Renderer(std::string name, int width, int height);
    // Clean up GLFW allocation
    ~Renderer(void);

    // Loads the texture in the file at path to the slot given by slot
    bool load_texture(const char* path, int slot);
    // Render loop. Will block until exit condition
    void loop(void);

  private:
    // Rendering pipeline
    void render(void);
    // Setters for transforms on GPU
    inline void set_model(glm::mat4& model);
    inline void set_view(glm::mat4& view);
    inline void set_projection(glm::mat4& projection);

    // GLFW callbacks
    static void resize_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Screen dimensions
    int width;
    int height;
    // Render window
    GLFWwindow* window;
    // Shader
    Shader* shader;
    // IDs reserved for storing data to GPU
    gl_obj_t vao, vbo, ebo, texture;
};