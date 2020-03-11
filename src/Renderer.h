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
    static void resize_callback(GLFWwindow* window, int width, int height);

    GLFWwindow* window;
};