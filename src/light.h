#pragma once
#include "model.h"
#include "shader.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define LIGHT_DIR_UP 1
#define LIGHT_DIR_DOWN -1

class PointLight {
public:
  glm::vec3 pos;
  glm::vec3 color;
  float intensity;
  int dir;
  float speed;
  PointLight(const glm::vec3 pos, const glm::vec3 color, float intensity, int dir, float speed);
  void draw(const glm::mat4& projection, const glm::mat4& view);

private:
  static unsigned int vao, vbo, ebo;
  static void setupLight();
  static Shader* shader;
};