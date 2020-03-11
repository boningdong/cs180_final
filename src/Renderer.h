#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <string>

class Renderer {
  public:
    Renderer(std::string name, int width, int height);
    ~Renderer(void);

    void loop(void);

  private:
    void render(void);
    static void resize_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* window;
};