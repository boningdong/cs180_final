#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "shader.h"

#include <string>

class Renderer {
  protected:
  public:
    static Renderer* get_instance();
    // Loads the texture in the file at path to the slot given by slot
    bool load_texture(const char* path, int slot);
    // Render loop. Will block until exit condition
    void loop(void);
    // Light render
    void init_objects();
    void init_light();
    void load_model();
    // GLFW callbacks
    void _resize(int width, int height);
    void _handle_mouse(int xpos, int ypos);
    void _handle_scroll(double offset);

  private:
    // Singleton instance
    static Renderer* instance;
    // Initialize window named name, with dimensions, width x height
    Renderer();
    // Clean up GLFW allocation
    ~Renderer(void);
    // Rendering pipeline
    void render(void);
    // Handle keyboard input
    void handle_keyboard(void);
    // Setters for transforms on GPU
    inline void set_model(glm::mat4& model);
    inline void set_view(glm::mat4& view);
    inline void set_projection(glm::mat4& projection);

    // Screen dimensions
    int width;
    int height;
    // Camera position/direction in world-space
    glm::vec3 camera_pos, camera_dir;
    // Rotational position
    float pitch, yaw;
    // Field of view (degrees)
    float fov;
    // Render window
    GLFWwindow* window;
    // Shader
    Shader* shader;
    Shader* lightShader;
    // IDs reserved for storing data to GPU
    unsigned int vao, vbo, ebo, texture;
    unsigned int lvao;
};