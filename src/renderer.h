#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>

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
  // deferred shading
  void init_deferred_engine(void);
  void render_geometry(const glm::mat4& projection, const glm::mat4& view);
  void render_lighting();
  void render_quad();

  // Camera position/direction in world-space
  glm::vec3 camera_pos, camera_dir;
  // Rotational position
  float pitch, yaw;
  // Field of view (degrees)
  float fov;
  // Render window
  GLFWwindow* window;
  // Shaders
  Shader* forward_shader;
  Shader* deferred_geometry_shader;
  Shader* deferred_light_shader;

  // IDs for deferred shading
  unsigned int gBuffer;
  unsigned int gPosition, gNormal, gColorSpec;
  // ID for depth buffer
  unsigned int rbo_depth;
  // IDs for quad
  unsigned int vao = 0;
  unsigned int vbo;

  // scene
  Scene scene;
};