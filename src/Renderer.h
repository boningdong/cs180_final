#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "shader.h"
#include "mesh.h"
#include "scene.h"

#include <string>

class Renderer {
  protected:
  public:
    static Renderer* get_instance();
    // Render loop. Will block until exit condition
    void loop(void);
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
    // deferred shading
    void init_deferred_engine(void);
    void render_geometry();
    void render_lighting();

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

    // IDs for deferred shading
    unsigned int gBuffer; 
    unsigned int gPosition, gNormal, gColorSpec;



    // scene
    Scene scene;

};